const RECENT_FILE_NAME = "RecentList.json";
const SETTINGS_FILE_NAME = "UserSettings.json";
const USERS_DIR_NAME = ".users";
const MAX_RECENT_VIEWS_PER_USER = 20;

var path = require("path");
var fs = require('fs');
var util = require('util');

//Sql and promise settings
var mysql = require('sync-mysql');
var Promise = require('bluebird');
var sha256 = require('js-sha256');

//sdt file reading configuration
var shell = require('shelljs');
var parse = require('xml-parser');
var xpath = require('xpath');
var dom = require('xmldom').DOMParser;

let AWS = require('aws-sdk');

let mySqlConnection;

const authenticateUser = function(data) {
	
	var returnObj = {
		sessionObj : {},	
		loginResult: {
			status: 'failure',
			userInformation: {}
		}
	};
	let UserAccountsResult, LicenseResult;
	// Check the userAccounts table for such user.
	let loginQuery = "SELECT * FROM UserAccounts WHERE UserAccounts.Email= '" + data.username + "'";
	UserAccountsResult = mySqlConnection.query(loginQuery);

	//authentication fail or pass relay
	if(UserAccountsResult.length){
		let licenseQuery = "SELECT * FROM UsageLicenses WHERE UsageLicenses.UserID= '" + UserAccountsResult[0].ID + "'";
		LicenseResult = mySqlConnection.query(licenseQuery);
	} else {
		return returnObj;
	}

	//Check for Administrator status
	// let adminQuery =  "SELECT Admin FROM UserAccounts WHERE UserAccounts.Email= '" + data.username + "'";
	// const [AdminResult] = mySqlConnection.query(adminQuery);

	// console.log(AdminResult, '=========Admin Result ==========', typeof AdminResult.Admin);

	var hash = sha256(data.password);

	var licensePortions = LicenseResult[0].Key.split("-");
	var days = (parseInt(licensePortions[4], 16)) / 86400;
	var currentDay = Math.round((new Date()).getTime() / 86400000);

	if (days - currentDay <= 0) {
		returnObj.loginResult.status = "expired";
	}
	// console.log(AdminResult.Admin === 'number');
	// if (typeof AdminResult.Admin === 'number' && AdminResult.Admin === 1) {
	// 	returnObj.loginResult.admin = 1;
	// console.log(returnObj.loginResult, '----------------------');
	// }

	// Check whether the passwords match
	if (days - currentDay >= 0 && UserAccountsResult && UserAccountsResult != "" && UserAccountsResult != null && (hash == UserAccountsResult[0].Password || data.password == UserAccountsResult[0].Password)) {
		var vizAccessPathList = [];

		// Update the user login count and last login timestamp.
		var update = "UPDATE UserAccounts SET " +
						"LoginCount = " + (parseInt(UserAccountsResult[0].LoginCount,10) + 1) + ", " +
						"LastLogin = '" + (new Date().toISOString().replace('T',' ').replace('Z','')) + "' " +
						"WHERE ID = " + UserAccountsResult[0].ID;
		
		mySqlConnection.query(update);
		
		// Delete the password from the return object
		delete UserAccountsResult[0].Password;
		returnObj.loginResult.status = 'success';
		returnObj.loginResult.userInformation = UserAccountsResult[0];
		console.log(UserAccountsResult, '=============');
		
		// Pull data from AvailableGroups joined SecurityGroups to get the S3 Directory
		var securityGroupQuery = "SELECT * FROM SecurityGroups LEFT JOIN AvailableGroups ON AvailableGroups.Group = SecurityGroups.ID WHERE AvailableGroups.UserID = '" 
									+ UserAccountsResult[0].ID + "'";
		//console.log(securityGroupQuery);

		const securityGroupResult = mySqlConnection.query(securityGroupQuery);
		
		var vizAccessQuery = "SELECT * from Visualizations V LEFT JOIN VisualizationGroups VG ON V.ID = VG.VizID WHERE VG.Group = " + securityGroupResult[0].ID;
		//console.log(vizAccessQuery);

		const vizAccessResult = mySqlConnection.query(vizAccessQuery);
		
		for (var i = 0; i < vizAccessResult.length; i++) {
			vizAccessPathList.push(vizAccessResult[i].Path);
		}
		
		// Add the information to the session object
		returnObj.sessionObj = {
			isUserLoggedIn: true,
			userInformation: returnObj.loginResult.userInformation,
			institutionDir: "./Resources/Institutions/" + securityGroupResult[0].S3Directory + "/",
			vizAccessPathList: vizAccessPathList,
			funnelInfo: {}
		}
		// console.log(returnObj)
		
		// Computing Funnel values visible to the user.
		var xmlObj = parseXML(returnObj.sessionObj.institutionDir + "syncedvisualizations.xml");
		var funnelObj = {};

		for (var outerIndex = 0; outerIndex < xmlObj.root.children.length; outerIndex++) {
			var temp = xmlObj.root.children[outerIndex];
			var childrenList = [];
			
			for (var innerIndex = 0; innerIndex < temp.children.length; innerIndex++) {
				childrenList.push([	temp.children[innerIndex].attributes.name, temp.children[innerIndex].content ]);
			}
			
			funnelObj[temp.attributes.name] = childrenList.length > 0 ? childrenList : false;
		}

		returnObj.loginResult.funnelInfo = funnelObj;
		returnObj.sessionObj.funnelInfo = funnelObj;
		returnObj.loginResult.userInformation.institutionDir = returnObj.sessionObj.institutionDir;
	}
	
	return returnObj;
}

const authenticateUserNew = function(data) {
	
	var returnObj = {
		sessionObj : {},	
		loginResult: {
			status: 'failure',
			userInformation: {}
		}
	};
	let UserAccountsResult, LicenseResult;
	// Check the userAccounts table for such user.
	let loginQuery = "SELECT * FROM UserAccounts WHERE UserAccounts.Email= '" + data.username + "'";
	UserAccountsResult = mySqlConnection.query(loginQuery);

	//authentication fail or pass relay
	if(UserAccountsResult.length){
		let licenseQuery = "SELECT * FROM UsageLicenses WHERE UsageLicenses.UserID= '" + UserAccountsResult[0].ID + "'";
		LicenseResult = mySqlConnection.query(licenseQuery);
	} else {
		return returnObj;
	}

	//Check for Administrator status
	// let adminQuery =  "SELECT Admin FROM UserAccounts WHERE UserAccounts.Email= '" + data.username + "'";
	// const [AdminResult] = mySqlConnection.query(adminQuery);

	// console.log(AdminResult, '=========Admin Result ==========', typeof AdminResult.Admin);

	// var hash = sha256(data.password);

	var licensePortions = LicenseResult[0].Key.split("-");
	var days = (parseInt(licensePortions[4], 16)) / 86400;
	var currentDay = Math.round((new Date()).getTime() / 86400000);

	if (days - currentDay <= 0) {
		returnObj.loginResult.status = "expired";
	}
	// console.log(AdminResult.Admin === 'number');
	// if (typeof AdminResult.Admin === 'number' && AdminResult.Admin === 1) {
	// 	returnObj.loginResult.admin = 1;
	// console.log(returnObj.loginResult, '----------------------');
	// }

	// Check whether the passwords match; not any more
	if (days - currentDay >= 0 && UserAccountsResult && UserAccountsResult != "" && UserAccountsResult != null 
		/* && (hash == UserAccountsResult[0].Password || data.password == UserAccountsResult[0].Password) */) {
		var vizAccessPathList = [];

		// Update the user login count and last login timestamp.
		var update = "UPDATE UserAccounts SET " +
						"LoginCount = " + (parseInt(UserAccountsResult[0].LoginCount,10) + 1) + ", " +
						"LastLogin = '" + (new Date().toISOString().replace('T',' ').replace('Z','')) + "' " +
						"WHERE ID = " + UserAccountsResult[0].ID;
		
		mySqlConnection.query(update);
		
		// Delete the password from the return object
		delete UserAccountsResult[0].Password;
		returnObj.loginResult.status = 'success';
		returnObj.loginResult.userInformation = UserAccountsResult[0];

		if(UserAccountsResult[0].ID && UserAccountsResult[0].Name) console.log('User Found.');
		
		// Pull data from AvailableGroups joined SecurityGroups to get the S3 Directory
		var securityGroupQuery = "SELECT * FROM SecurityGroups LEFT JOIN AvailableGroups ON AvailableGroups.Group = SecurityGroups.ID WHERE AvailableGroups.UserID = '" 
									+ UserAccountsResult[0].ID + "'";
		//console.log(securityGroupQuery);

		const securityGroupResult = mySqlConnection.query(securityGroupQuery);
		
		var vizAccessQuery = "SELECT * from Visualizations V LEFT JOIN VisualizationGroups VG ON V.ID = VG.VizID WHERE VG.Group = " + securityGroupResult[0].ID;
		//console.log(vizAccessQuery);

		const vizAccessResult = mySqlConnection.query(vizAccessQuery);
		
		for (var i = 0; i < vizAccessResult.length; i++) {
			vizAccessPathList.push(vizAccessResult[i].Path);
		}
		
		// Add the information to the session object
		returnObj.sessionObj = {
			isUserLoggedIn: true,
			userInformation: returnObj.loginResult.userInformation,
			institutionDir: "./Resources/Institutions/" + securityGroupResult[0].S3Directory + "/",
			vizAccessPathList: vizAccessPathList,
			funnelInfo: {}
		}
		// console.log(returnObj)
		
		// Computing Funnel values visible to the user.
		var xmlObj = parseXML(returnObj.sessionObj.institutionDir + "syncedvisualizations.xml");
		var funnelObj = {};

		for (var outerIndex = 0; outerIndex < xmlObj.root.children.length; outerIndex++) {
			var temp = xmlObj.root.children[outerIndex];
			var childrenList = [];
			
			for (var innerIndex = 0; innerIndex < temp.children.length; innerIndex++) {
				childrenList.push([	temp.children[innerIndex].attributes.name, temp.children[innerIndex].content ]);
			}
			
			funnelObj[temp.attributes.name] = childrenList.length > 0 ? childrenList : false;
		}

		returnObj.loginResult.funnelInfo = funnelObj;
		returnObj.sessionObj.funnelInfo = funnelObj;
		returnObj.loginResult.userInformation.institutionDir = returnObj.sessionObj.institutionDir;
	}
	
	return returnObj;
}


const generateTableSchema = function(data) {
	var query = "CREATE TABLE IF NOT EXISTS ? ( ";
	var keys = Object.keys(data);
	var type = "TEXT";
	
	for (var index = 0; index < keys.length; index++) {
		type = isNaN(data[keys[index]]) ? "TEXT" : "INT";
		query = query + " `" + keys[index] + "` " + type;
		
		if (index != keys.length-1) {
			query = query + ",";
		}
		else {
			query = query + ");";
		}
	}

	return query;
}

const generateInsertQuery = function (arrData) {
	var returnQuery = "INSERT INTO ? VALUES ";
	var values, temp;
	
	for (var index=0; index < arrData.length; index++) {
		//values = '("' + Object.keys(arrData[index]).toString() + '")';

		values = JSON.stringify(Object.values(arrData[index]));
		values = values.replace('[','(').replace(/]$/,")");
		
		if (index != arrData.length-1){
			returnQuery = returnQuery + values + ",";
		}
		else {
			returnQuery = returnQuery + values;
		}
	}
	
	return returnQuery;
}

/**
 * Returns "./users/hashvalue/sessionid/"
 */
const createUserDirectory = function(hashValue, sessionID) {
	
	var usersFolderPath = path.resolve('./' + USERS_DIR_NAME + '/');
	var userDir = path.resolve('./' + USERS_DIR_NAME + '/' + hashValue);
	var userSessionDir = path.resolve(userDir + "/" + sessionID);
	var userVizDir = path.resolve(userSessionDir + "/Viz");
	var userDataDir = path.resolve(userSessionDir + "/Data");
	var userRecentViewsFile = path.resolve(userDir + "/" + RECENT_FILE_NAME);
	var userSettingsFile = path.resolve(userDir + "/" + SETTINGS_FILE_NAME);

	if (!fs.existsSync(usersFolderPath)) {
		fs.mkdirSync(usersFolderPath);
	}

	if (!fs.existsSync(userDir)) {
		fs.mkdirSync(userDir);
	}
	
	if (!fs.existsSync(userRecentViewsFile)) {
		fs.closeSync(fs.openSync(userRecentViewsFile, 'w'));
		//fs.writeFileSync(userRecentViewsFile,'[["test",1513474692019, 1513474692019]]');
	}
	
	if (!fs.existsSync(userSettingsFile)) {
		fs.closeSync(fs.openSync(userSettingsFile, 'w'));
		//fs.writeFileSync(userRecentViewsFile,'[["test",1513474692019, 1513474692019]]');
	}
	
	if (!fs.existsSync(userSessionDir)) {
		fs.mkdirSync(userSessionDir);
	}
	
	if (!fs.existsSync(userVizDir)) {
		fs.mkdirSync(userVizDir);
	}
	
	if (!fs.existsSync(userDataDir)) {
		fs.mkdirSync(userDataDir);
	}
	
	return userSessionDir;
}

const deteleUserSessionDirectory = function(dir) {
	if (fs.existsSync(path.resolve(dir))) {
		if (shell.exec("rm -rf " + path.resolve(dir)).code !== 0) {
			shell.echo('Error: Failed to delete dir');
			return false;
		}
		else {
			console.log(dir + " was deleted successfully");
		}
	}
	else {
		console.log(dir + " does not exist");
	}
}

const parseXML = function(filename) {
	var abspath = path.resolve(filename);
	console.log('trying to open file: ' + abspath);
	try {
		var xml = fs.readFileSync(abspath, 'utf8');
		var obj = parse(xml);
	}
	catch(err) {
		console.log('error is : ' + err);
	}
	return obj;
}

const insertIntoMySqlDB = function(insertQuery) {
	try {
		mySqlConnection.query(insertQuery);
	}
	catch(err) {
		console.log(err);
	}
}

// const getLegendInfo = function(filename) {
// 	var fileData = fs.readFileSync(filename, "utf8");
	
// 	var returnObj = { }
	
// 	try {
// 		var doc = new dom().parseFromString(fileData);
// 		// console.log('refering file ' + filename);
// 		returnObj = xpath.select("//Legends/Legend", doc)[0].firstChild.data;
// 	}
// 	catch(err) {
// 		console.log(err);
// 		returnObj = null;
// 	}
	
// 	return returnObj;
// }

const getFrontEndFilterInfo = function(filename) {
	var fileData = fs.readFileSync(filename, "utf8");
	// console.log('*************************', filename, fileData, "********************");

	var returnObj = {
		tableName: '',
		fieldList: [],
		selectAll: [],
		filterAllowedColumnList: [],
		allTablesList: [],
		datasourceId: '',
		initialX: '',
		initialY: '',
		initialZ: '',
		legendPath: ''
	}
	
	try {
		var doc = new dom().parseFromString(fileData);
		console.log('refering file ' + filename);

		// console.log(fileData)
		returnObj.tableName = xpath.select1("//FrontEnd/Filter/FilterField/@table", doc).value;
		var fieldNames = xpath.select("//FrontEnd/Filter/FilterField/@field", doc);
		var datasourceId = xpath.select("//Datasources/Datasource/@id", doc);
		var selectAllValues = xpath.select("//FrontEnd/Filter/@selectall", doc);
		var fieldList = xpath.select("//ElasticList/ElasticField/@field", doc);
		var initialX = xpath.select("//Glyphs/Glyph/Position/X/Binding/@fieldId", doc);
		var initialY = xpath.select("//Glyphs/Glyph/Position/Y/Binding/@fieldId", doc);
		var initialZ = xpath.select("//Glyphs/Glyph/Position/Z/Binding/@fieldId", doc);
		returnObj.legendPath = xpath.select("//Legends/Legend", doc)[0].data;

		var allTables = xpath.select("//FrontEnd/Filter/FilterField/@table", doc);
		var allTablesList = [];

		for (var i = 0; i < allTables.length; i++) {
			if (allTablesList.indexOf(allTables[i].value) == -1) {
				allTablesList.push(allTables[i].value);
			}
		}

		returnObj.allTablesList = allTablesList;


		var inputFieldIDs = xpath.select("//InputFields/InputField/@name", doc);
		var inputFieldNames = xpath.select("//InputFields/InputField/@field", doc);
		console.log(initialX[0].value, initialY[0].value, initialZ[0].value, "XYZ VALS DATA");
		var inputIDs = [];
		var inputNames = [];

		for (var i = 0; i < inputFieldIDs.length; i++) {
			inputIDs.push(inputFieldIDs[i].value);
		}

		for (var i = 0; i < inputFieldNames.length; i++) {
			inputNames.push(inputFieldNames[i].value);
		}

		// console.log(inputIDs, inputNames, "INPUT IDS & NAMES")
		returnObj.datasourceId = datasourceId[0].value;

		var xIndex = inputIDs.indexOf(initialX[0].value);
		var yIndex = inputIDs.indexOf(initialY[0].value);
		var zIndex = inputIDs.indexOf(initialZ[0].value);

		console.log(xIndex, yIndex, zIndex, "XYZ INDEXES");

		returnObj.initialX = (xIndex != -1 ? inputNames[xIndex] : null);
		returnObj.initialY = (yIndex != -1 ? inputNames[yIndex] : null);
		returnObj.initialZ = (zIndex != -1 ? inputNames[zIndex] : null);
		
		for (var j = 0; j < fieldList.length; j++) {
			returnObj.filterAllowedColumnList.push(fieldList[j].value);
		}


		for (var i = 0; i < fieldNames.length; i++) {
			returnObj.fieldList.push(fieldNames[i].value); 
			returnObj.selectAll.push(selectAllValues[i] ? selectAllValues[i].value : true);
		}
	}
	catch(err) {
		console.log(err);
		returnObj = null;
	}
	
	return returnObj;
}

const fetchS3Obj = function(key, instit){
	
	const params = {
		//CHANGE BACK TO NOTREDAME
			Bucket: `viz-group-${instit}`,
			Key: key
		};

	let s3 = new AWS.S3();

	s3.config.update({
			accessKeyId: "AKIAUWRBLENVICAQBC4Z", 
			secretAccessKey: "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",
			region: "us-east-2"
	});

	return new Promise((resolve, reject) => {
			s3.getObject(
			params ,
				function(err, data) {
					if(err !== null){
						reject(err);
					}
					else if(data){
						// console.log(typeof data.Body, '==============', data.Body)
						resolve(data.Body.toString('utf8'));
					}
				})
	})
}

// const fetchSDTFileFromS3 = function(s3Directory, arrListOfFiles) {
// 	/*
// 	if (shell.exec(global.getSDTFileScript).code !== 0) {
// 		shell.echo('Error: Downloading file from S3 failed');
// 		shell.exit(1);
// 		return false;
// 	}
// 	*/

// 	var commandToCopy = '';
// 	var commandToUnzip = '';
// 	var zipFilePath = '';
// 	var abspath = path.resolve('./Resources/Institutions/'+s3Directory);

// 	for (var i = 0 ; i < arrListOfFiles.length; i++) {
// 		commandToCopy = 'aws s3 cp --quiet "s3://visualizationdata/' + arrListOfFiles[i] + '" ' + abspath;
// 		zipFilePath = abspath + "/" + arrListOfFiles[i].split('/')[1];
// 		commandToUnzip = 'unzip -o "' + zipFilePath + '" -d ' + abspath;

// 		console.log(commandToCopy);
// 		console.log(commandToUnzip);
		
// 		if (shell.exec(commandToCopy).code == 0) {
// 			shell.exec(commandToUnzip);
// 			shell.exec('rm -rf "' + zipFilePath + '"');
// 		}
			
// 	}

// 	return true;
// }

// const createVizWithJavaCode = function(params, name) {
// 	var server = "localhost";
// 	var port = "6066";
// 	var command = "java -jar ./DataEngine/src/utility.jar " + server + " " + port + " ";
// 	console.log(params, 'PARAMS');
// 	//params = "{output:\"/home/ec2-user/dbserver/DataEngine/test\",sdt:\"/home/ec2-user/dbserver/DataEngine/test/Global Admissions/Global Admissions.sdt\",query:[\"719598f5-ff35-47b4-bf7a-9bdfbb30140f\",\"GlobalAdmissions\",\"SELECT * FROM GlobalAdmissions WHERE StaffAssigned IN ('Staff 1') AND Year IN ('2017')\"]}"
// 	var args = JSON.stringify(params).replace(/\\/g, '');
// 	args = '"' + args.replace(/"/g, '\\"') + '"';
// 	console.log(args, 'local backend args')
// 	var newName = "";

// 	console.log("Running command :::::::::::::::: " + command + args);
	
// 	try {
// 		if (shell.exec(command + args).code !== 0) {
// 			shell.echo('Error: Failed to create output');
// 			return false;
// 		}

// 		else {
// 			shell.echo('Success: Created the file at ' + params.output);
// 			var newName = params.output + "/" + name + ".json";
// 			var renameCommand = "mv " + params.output + "/data.json " + newName;
// 			if (shell.exec(renameCommand).code !== 0) {
// 				shell.echo('Error: Failed to rename');
// 				return false;
// 			}
// 		}
// 	}
// 	catch(err) {
// 		console.log('java utility call failed!');
// 	}
	
// 	return true;
// }

const saveViewInRecentViews = function(viewObj, userId) {
	var userRecentViewsFile = path.resolve('./' + USERS_DIR_NAME + '/' + userId + "/" + RECENT_FILE_NAME);
	var fileContent = [];
	
	if (userRecentViewsFile) {
		fileContent = fs.readFileSync(userRecentViewsFile, 'utf8');
		if (typeof fileContent == 'string' && fileContent != null && fileContent != "") {
			fileContent = JSON.parse(fileContent);
		}
		else {
			fileContent = [];
		}
	}
	
	if (fileContent && util.isArray(fileContent)) {
		if (fileContent.length > MAX_RECENT_VIEWS_PER_USER) {
			//remove the 1st element. We follow a first in first out procedure.
			fileContent.shift(); 
		}
		
		if (!viewObj.savedView) {
			fileContent.push([
				viewObj.originalVizName,
				viewObj.time,
				viewObj.date,
				viewObj.frontEndFilters,
				viewObj.datasourceId,
				viewObj.filterAllowedColumnList,
				viewObj.sdtPath,
				viewObj.tableName
			]);
		}
		else {
			fileContent.push([
				viewObj.originalVizName+": "+ viewObj.savedVizName,
				viewObj.time,
				viewObj.date,
				viewObj.savedViewId
			]);
		}
		fs.writeFileSync(userRecentViewsFile,JSON.stringify(fileContent));
	}
	else {
		console.log("ERROR WHILE READING RECENT FILE.");
	}
}

Promise.resolve()
  // First, try connect to the login database
  .then(() => mySqlConnection = new mysql({
				host     : 'sgxinstance.cqu1pnqua5at.us-west-2.rds.amazonaws.com',
				user     : 'sgxadmin',
				password : '#sgxpw13#',
				database : 'sgxdb',
				port	 : '3306'
			})
	)
  .then(() => console.log(`SQL Connection up on helper FXNs.`))
  .catch(err => console.error(err.stack));

module.exports = {
	authenticateUser,
	authenticateUserNew,
	generateInsertQuery,
	generateTableSchema,
	createUserDirectory,
	deteleUserSessionDirectory,
	// parseXML,
	insertIntoMySqlDB,
	// getLegendInfo,
	getFrontEndFilterInfo,
	fetchS3Obj,
	// fetchSDTFileFromS3,
	// createVizWithJavaCode,
	saveViewInRecentViews
};
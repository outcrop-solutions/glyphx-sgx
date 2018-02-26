//ec2-35-162-196-131.us-west-2.compute.amazonaws.com:5001/
/**
 * This is the script that runs on the server and accepts user URL hits and responds accordingly.
 * We are assuming that the SynchronizeService.sh keeps the database file in sync.
 * This will connect to the database and provides an interface for accessing the database.
 * This will host the server on port 5001.
 */
 
 //res.setHeader('Cache-Control', 'public, max-age=' + (31536000));
 //res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
 
 /** FORMATS
	format of login object returned:
	
	{
	  "status": "success",
	  "userInformation": {
		"ID": 7,
		"Name": "Test Development",
		"Email": "devtest",
		"Institution": 4,
		"LastLogin": "2017-10-24T17:36:54.000Z",
		"LoginCount": 1225,
		"Admin": 0,
		"VersionNum": "1.2.00",
		"institutionDir": "./Resources/Institutions/devtest/",
		"loggedInTime": "2017-10-24T17:37:47.411Z",
		"idleTime": 0
	  },
	  "funnelInfo": {
		"Marketing and Recruiting": [
		  [
			"Prospects",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Prospect and Segmentation\\Prospects Only.sdt"
		  ],
		  [
			"High School Profiles",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Prospect and Segmentation\\High School Profiles.sdt"
		  ],
		  [
			"Prospects and High Schools (Geospatial)",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Prospect and Segmentation\\Prospects and High Schools (Geospatial).sdt"
		  ]
		],
		"Admissions": [
		  [
			"Global Admissions",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Global Admissions\\Global Admissions.sdt"
		  ],
		  [
			"Applicants",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Admissions Officer\\Applicants.sdt"
		  ]
		],
		"Financial Aid": [
		  [
			"FA Package Summaries (Grid)",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Financial Aid\\FA Package Summaries Grid.sdt"
		  ],
		  [
			"FA Package Summaries",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Financial Aid\\FA Package Summaries.sdt"
		  ]
		],
		"Custom": [
		  [
			"Adversity",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Custom Views\\CB Contextual Dashboard (Adversity) v05.sdt"
		  ],
		  [
			"Applicants",
			"C:\\ProgramData\\SynGlyphX\\GlyphEd\\Dev\\Custom Views\\Applicants.sdt"
		  ]
		]
	  },
	  "savedViews": [],
	  "recentViews": []
	}
	
	format of session object: 
	
	req.session.user = {
			isUserLoggedIn: true,
			userInformation: returnObj.loginResult.userInformation,
			institutionDir: "./Resources/Institutions/" + securityGroupResult[0].S3Directory + "/",
			vizAccessPathList: vizAccessPathList,
			funnelInfo: {},
			userDbPath: ./.users/hashvalue/Data,
			userDir: ./.users/hashvalue,
			id: hash value of user
		}
	
  */

const port = 5001;
const RECENT_FILE_NAME = "RecentList.json";
const SETTINGS_FILE_NAME = "UserSettings.json";
const USERS_DIR_NAME = ".users";
const MAX_RECENT_VIEWS_PER_USER = 20;

var now = require('performance-now');
var path = require("path");
var fs = require('fs');
var util = require('util');

//Sql and promise settings
var Promise = require('bluebird');
var sqlite = require('sqlite');
var mysql = require('sync-mysql');
var sha256 = require('js-sha256');
var mySqlConnection;

//sdt file reading configuration
var shell = require('shelljs');
var parse = require('xml-parser');
var xpath = require('xpath');
var dom = require('xmldom').DOMParser;


/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// SERVER CONFIGURATION /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

var express = require('express');
var compression = require('compression');
var session = require('express-session');
var bodyParser = require('body-parser');

const app = express();
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'html');
app.use(bodyParser.urlencoded({ extended: false })); // parse application/x-www-form-urlencoded
app.use(bodyParser.json()); // parse application/json
app.use(compression()); // gzip compression.

/** ALLOW CORS **/
app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", req.get('origin'));
  res.header('Access-Control-Allow-Credentials', true);
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});

// Session Management //
app.use(session(
	{
		secret: 'SynGlyphX',
		resave: false,
		rolling: true,
		cookie: {
			httpOnly: true,
			secure: false,
			maxAge: 1800000 // 30(minutes) * 60 * 1000
		},
		saveUninitialized: false
	}
));

/**
 * For serving the application via this server.
 */
app.use(express.static(path.join(__dirname, '.')));

/**
 * Get Institution IMG for the home page
 */
app.get('/customerImg/:instName', function (req, res) {
	var tempPath = "./Resources/Institutions/devtest/customer.png";
	console.log(req.params.instName);
	
	res.setHeader('Cache-Control', 'public, max-age=' + (250000));
	
	//check if file exists.
	if (fs.existsSync(path.resolve("./"+ req.params.instName + "/customer.png"))) {
		res.sendFile(path.resolve("./"+ req.params.instName + "/customer.png"));
	}
	else {
		res.sendFile(path.resolve(tempPath));
	}
});

app.get('/vizdata/:key', function (req, res) {
	var tempPath = req.params.key;
	console.log(req.params.key);
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	//check if file exists.
	if (fs.existsSync(path.resolve(tempPath))) {
		res.sendFile(path.resolve(tempPath));
	}
	else {
		console.log('file does not exist');
	}
});
	
/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// HELPER FUNCTIONS /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
	
function authenticateUser(data) {
	
	var returnObj = {
		sessionObj : {},	
		loginResult: {
			status: 'failure',
			userInformation: {}
		}
	};
			
	// Check the userAccounts table for such user.
	var loginQuery = "SELECT * FROM UserAccounts WHERE UserAccounts.Email= '" + data.username + "'";
	const UserAccountsResult = mySqlConnection.query(loginQuery);
	var hash = sha256(data.password);
	
	// Check whether the passwords match
	if (UserAccountsResult && UserAccountsResult != "" && UserAccountsResult != null && (hash == UserAccountsResult[0].Password || data.password == UserAccountsResult[0].Password)) {
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

function generateTableSchema(data) {
	var query = "CREATE TABLE IF NOT EXISTS ? ( ";
	var keys = Object.keys(data);
	var type = "TEXT";
	
	for (var index = 0; index < keys.length; index++) {
		type = isNaN(data[keys[index]]) ? "TEXT" : "INT";
		query = query + " " + keys[index] + " " + type;
		
		if (index != keys.length-1) {
			query = query + ",";
		}
		else {
			query = query + ");";
		}
	}

	return query;
}

function generateInsertQuery(arrData) {
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
function createUserDirectory(hashValue, sessionID) {
	
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

function deteleUserSessionDirectory(dir) {
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

function parseXML(filename) {
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

function insertIntoMySqlDB(insertQuery) {
	try {
		mySqlConnection.query(insertQuery);
	}
	catch(err) {
		console.log(err);
	}
}

function getFrontEndFilterInfo(filename) {
	var fileData = fs.readFileSync(filename, "utf8");
	
	var returnObj = {
		tableName: '',
		fieldList: [],
		selectAll: [],
		filterAllowedColumnList: [],
		datasourceId: '',
		initialX: '',
		initialY: '',
		initialZ: ''
	}
	
	try {
		var doc = new dom().parseFromString(fileData);
		console.log('refering file ' + filename);

		//returnObj.tableName = xpath.select("//FrontEnd/Filter/FilterField/@table", doc).value;
		returnObj.tableName = xpath.select1("//FrontEnd/Filter/FilterField/@table", doc).value;
		var fieldNames = xpath.select("//FrontEnd/Filter/FilterField/@field", doc);
		var datasourceId = xpath.select("//Datasources/Datasource/@id", doc);
		var selectAllValues = xpath.select("//FrontEnd/Filter/@selectall", doc);
		var fieldList = xpath.select("//ElasticList/ElasticField/@field", doc);
		var initialX = xpath.select("//Glyphs/Glyph/Position/X/Binding/@fieldId", doc);
		var initialY = xpath.select("//Glyphs/Glyph/Position/Y/Binding/@fieldId", doc);
		var initialZ = xpath.select("//Glyphs/Glyph/Position/Z/Binding/@fieldId", doc);

		var inputFieldIDs = xpath.select("//InputFields/InputField/@name", doc);
		var inputFieldNames = xpath.select("//InputFields/InputField/@field", doc);

		var inputIDs = [];
		var inputNames = [];

		for (var i = 0; i < inputFieldIDs.length; i++) {
			inputIDs.push(inputFieldIDs[i].value);
		}

		for (var i = 0; i < inputFieldNames.length; i++) {
			inputNames.push(inputFieldNames[i].value);
		}

		returnObj.datasourceId = datasourceId[0].value;

		var xIndex = inputIDs.indexOf(initialX[0].value);
		var yIndex = inputIDs.indexOf(initialY[0].value);
		var zIndex = inputIDs.indexOf(initialZ[0].value);


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

function fetchSDTFileFromS3(s3Directory, arrListOfFiles) {
	/*
	if (shell.exec(global.getSDTFileScript).code !== 0) {
		shell.echo('Error: Downloading file from S3 failed');
		shell.exit(1);
		return false;
	}
	*/

	var commandToCopy = '';
	var commandToUnzip = '';
	var zipFilePath = '';
	var abspath = path.resolve('./Resources/Institutions/'+s3Directory);

	for (var i = 0 ; i < arrListOfFiles.length; i++) {
		commandToCopy = 'aws s3 cp --quiet "s3://visualizationdata/' + arrListOfFiles[i] + '" ' + abspath;
		zipFilePath = abspath + "/" + arrListOfFiles[i].split('/')[1];
		commandToUnzip = 'unzip -o "' + zipFilePath + '" -d ' + abspath;

		console.log(commandToCopy);
		console.log(commandToUnzip);
		
		if (shell.exec(commandToCopy).code == 0) {
			shell.exec(commandToUnzip);
			shell.exec('rm -rf "' + zipFilePath + '"');
		}
			
	}

	return true;
}

function createVizWithJavaCode(params, name) {
	var server = "localhost";
	var port = "6066";
	var command = "java -jar ./DataEngine/src/utility.jar " + server + " " + port + " ";
	//params.query = "{output:\"/home/ec2-user/dbserver/DataEngine/test\",sdt:\"/home/ec2-user/dbserver/DataEngine/test/Global Admissions/Global Admissions.sdt\",query:[\"719598f5-ff35-47b4-bf7a-9bdfbb30140f\",\"GlobalAdmissions\",\"SELECT * FROM GlobalAdmissions WHERE StaffAssigned IN ('Staff 1') AND Year IN ('2017')\"]}"
	var args = JSON.stringify(params).replace(/\\/g, '');
	args = '"' + args.replace(/"/g, '\\"') + '"';
	var newName = "";

	console.log("Running command :::::::::::::::: " + command + args);
	
	try {
		if (shell.exec(command + args).code !== 0) {
			shell.echo('Error: Failed to create output');
			return false;
		}

		else {
			shell.echo('Success: Created the file at ' + params.output);
			var newName = params.output + "/" + name + ".json";
			var renameCommand = "mv " + params.output + "/data.json " + newName;
			if (shell.exec(renameCommand).code !== 0) {
				shell.echo('Error: Failed to rename');
				return false;
			}
		}
	}
	catch(err) {
		console.log('java utility call failed!');
	}
	
	return true;
}

function saveViewInRecentViews(viewObj, userId) {
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

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// ROUTES ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

app.all('*', checkUser);

function checkUser(req, res, next) {
  if (req.path == '/login' || req.path.includes('/customerImg') || req.path.includes('/viz/') || req.path.includes('test')) {
  	return next();
  }

  // Authenticate user
  if (req.session.user &&  req.session.user.isUserLoggedIn) {
	next();
  }
  else {
	console.log("Request for :" + req.path + ' , But user is not logged in');
	res.send({ isUserLoggedIn: false });
  }
}

// Login route accepts a post request with date in form of {username:"", password:""}
app.get('/login', async (req, res, next) => {
	if (!req.query.username || !req.query.password) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	var objToPass = {
		host : req.get('host'),
		username : req.query.username,
		password : req.query.password
	}
	
	console.log("---------------------------------" + Date());
	console.log("Attempt Login from: " + req.query.username);

	var userObj = authenticateUser(objToPass);
	var savedViews = [];
	
	// On Success
	if (userObj.loginResult.status == 'success') {
		
		// Fetch the recent views and saved views list.
		userObj.loginResult.savedViews = [];
		userObj.loginResult.recentViews = [];
		
		// Create user directory on the server.
		// Store paths into session variable.
		userObj.sessionObj.id = sha256(req.query.username);
		userObj.sessionObj.userDir = createUserDirectory(userObj.sessionObj.id,req.sessionID);
		userObj.sessionObj.userDbPath = userObj.sessionObj.userDir + "/Data/subsetDatabase.sqlite";
		
		req.session.user = userObj.sessionObj;
	}
	
	res.send(userObj.loginResult);
});

app.get('/copySdtFiles', async (req, res, next) => {
	// Future To-do copy the sdt files on request.
});

app.post('/manageAnnouncements', async (req, res, next) => {
	if (!req.body && !req.body.type) {
		return res.sendStatus(400);
	} 

	var operation = req.body.operation;
	var userId = req.session.user.userInformation.ID;
	var config = req.body.config;
	var resultObj = {};
	var insertOptionsQuery = 'Insert into PollOptions (QuestionID,Description) values ';
	
	console.log(req.body);
	
	switch(operation) {
		// Create the announcement.
		case 'create':
			var createAnnouncementQuery = 'Insert into Announcements (Type,Description) values';
			
			createAnnouncementQuery += "('" + config.type + "','" + JSON.stringify(config.content) + "')";
			mySqlConnection.query(createAnnouncementQuery);
			
			var lastInsertedRow = mySqlConnection.query("Select * from Announcements Where ID=(SELECT LAST_INSERT_ID() as id)");
			var id = lastInsertedRow[0] ? lastInsertedRow[0].ID : null;
			resultObj.createdAnnouncementId = id;
			
			// If its a poll create the announcements.
			if (config.type.toLowerCase().indexOf('poll') > -1 && id != null) {
				for (var index = 0; index < config.options.length; index++) {
					insertOptionsQuery += "(" + id + ",'" + config.options[index] + "')";
					
					if (index != config.options.length-1) {
						insertOptionsQuery += ',';
					}
					else {
						insertOptionsQuery += ';';
					}
				}
				
				mySqlConnection.query(insertOptionsQuery);
			}

			resultObj.status = "success";
			break;
		
		case 'delete':
			var deleteAnnouncementQuery = 'Delete from Announcements where ID=' + config.AnnouncementId;
			mySqlConnection.query(deleteAnnouncementQuery);
			resultObj.status = "success";
			break;
		
		case 'fetch':
			//resultObj = mySqlConnection.query("select * from Announcements a left outer join PollOptions p on a.ID=p.QuestionID;");
			resultObj = mySqlConnection.query("select a.type,a.id as announcementId,a.CreationDate as announcementDate,a.description as announcementDescription,p.QuestionID as QuestionId,p.Description as optionDescription,p.CreationDate as optionsDate from Announcements a left outer join PollOptions p on a.ID=p.QuestionID;");
			break;
		
		case 'pollResult':
			if (config.AnnouncementId != null) {
				
				var pollQuery = "Insert into UserPollOptionSelection (UserID,QuestionID,AnswerID) values";
				
				// First we insert the selected ones into the UserPollOptionSelection table.
				for (var index = 0; index < config.selectedOptionIds.length; index++) {
					pollQuery += "(" + userId + "," + config.AnnouncementId + "," + config.selectedOptionIds[index] + ")";
					
					if (index != config.selectedOptionIds.length - 1) {
						pollQuery += ',';
					}
					else {
						pollQuery += ';';
					}
				}
				
				mySqlConnection.query(pollQuery);
				
				// We fetch the results for this poll.
				resultObj = mySqlConnection.query("Select * from UserPollOptionSelection where QuestionID=" + config.AnnouncementId);
			}
		break;
	}
	
	res.send(resultObj);
});

app.post('/saveUserSettings', async (req, res, next) => {
	if (!req.body && !req.body.userSettings) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var userDir = './' + USERS_DIR_NAME + '/' + sess.user.id;
	var userSettingsFile = path.resolve(userDir + "/" + SETTINGS_FILE_NAME);
	
	var fileContent = req.body.userSettings;
	fs.writeFileSync(userSettingsFile, JSON.stringify(fileContent));
});

app.get('/fetchUserSettings',async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var userDir = './' + USERS_DIR_NAME + '/' + sess.user.id;
	var userSettingsFile = path.resolve(userDir + "/" + SETTINGS_FILE_NAME);
	res.sendFile(userSettingsFile);
});

app.post('/checkFrontEndFilterQuery', async (req, res, next) => {
	if (!req.body && !req.body.frontEndFilters) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	var parentTableDataRows,parentTableColumnSchema;
	var data = req.body.frontEndFilters.slice();
	var innerIndex,outerIndex;
	var returnObj = {
		match: false,
		query: "SELECT * FROM " + req.body.tableName + " WHERE "
	}
	
	// 2D Array
	//[[],[],[]]
	for (outerIndex = 0; outerIndex < data.length; outerIndex++) {
		var dataItem = data[outerIndex].slice();
		var columnName = dataItem[0];

		// Removes the 1st element that is the name.
		dataItem.shift(); 

		var values = '("' + dataItem.toString() + '")';
		returnObj.query = returnObj.query + columnName + " IN " + values.replace(/,/g , '","');
		
		if (outerIndex != data.length-1) {
			returnObj.query = returnObj.query + " AND ";
		}

		else {
			returnObj.query = returnObj.query + ";";
		}
	}
	
	console.log(returnObj.query);
	
	const [mainDb] = await Promise.all([
		sqlite.open(req.session.user.institutionDir + "data.db", { Promise }),
	]);
	
	// Fetch the rows from the main database.
	await mainDb.get(returnObj.query)
	.then((result) => parentTableDataRows = result)
	.catch(err => console.log(err));
		
	if (parentTableDataRows != {} && parentTableDataRows != null) {
		returnObj.match = true;
		
		// Saving the view in the recent views. To distinguish between click of recent view and normal click.
		if (req.body.time) {
			saveViewInRecentViews(
				{
					tableName: req.body.tableName, 
					originalVizName: req.body.originalVizName, 
					datasourceId: req.body.datasourceId, 
					filterAllowedColumnList:  req.body.filterAllowedColumnList, 
					sdtPath: req.body.sdtPath,
					frontEndFilters: req.body.frontEndFilters,
					savedView: false,
					time: req.body.time,
					date: req.body.date
				},
				req.session.user.id
			);
		}
	}
	
	res.send(returnObj);
})

app.post('/saveView', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 
	
	var sess = req.session;
	var vizId = req.body.vizId;
	var originalVizName = req.body.originalVizName;
	var frontEndFilterString = req.body.frontEndFilter;
	var savedVizName = req.body.savedVizName;
	var filterQuery = req.body.filterQuery;
	var recentViewObj = {
		originalVizName: originalVizName, 
		savedVizName: savedVizName, 
		time: req.body.time,
		date: req.body.date,
		savedViewId: vizId,
		savedView: true
	}
	
	var insertQuery = "INSERT INTO UserSavedVisualizations " + "(Name,OriginalVizName,UserID,QueryString,InstitutionId,frontEndFilterString) VALUES (?)";

	var insertQueryValues = "'" + savedVizName + "','" + originalVizName + "','" + sess.user.userInformation.ID + "','" + filterQuery + "','" + sess.user.userInformation.Institution
							+ "','" + frontEndFilterString + "'";

	insertQuery = insertQuery.replace('?',insertQueryValues);
		
	// Check whether the same named viz already exists. If it does then just update the query string created.
	try {
		if (vizId != null) {
			var selectQuery = "Select * from UserSavedVisualizations Where ID=" + vizId;
			const selectQueryResult = mySqlConnection.query(selectQuery);
			
			if (selectQueryResult.length > 0) {
				var updateQuery = "Update UserSavedVisualizations SET QueryString='" + filterQuery + "', frontEndFilterString='" + frontEndFilterString + "' Where ID=" + vizId;
				console.log(updateQuery);
				mySqlConnection.query(updateQuery);
			}
			else {
				insertIntoMySqlDB(insertQuery);
			}
		}
		else {
			insertIntoMySqlDB(insertQuery);
		}
		
		var lastInsertedRow = mySqlConnection.query("Select * from UserSavedVisualizations Where ID=(SELECT LAST_INSERT_ID() as id)");
		
		if (vizId == null && lastInsertedRow && util.isArray(lastInsertedRow) && lastInsertedRow[0] != '') {
			recentViewObj.savedViewId = lastInsertedRow[0];
		}
		
		//Save in recentViews
		//saveViewInRecentViews(recentViewObj,req.session.user.id);
		
		//res.sendStatus(200);
		res.send(lastInsertedRow[0]);
	}
	catch(err) {
		res.send(err);
	}
});

app.get('/fetchSavedViews', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 
	
	var sess = req.session;
	
	try {
		var savedViews =  mySqlConnection.query("Select * from UserSavedVisualizations Where UserID=" + sess.user.userInformation.ID);
		res.send({ savedViews: savedViews });
	}
	catch(err) {
		console.log(err);
		res.send("ERROR");
	}
});

app.get('/deleteSavedViews', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	
	try {
		mySqlConnection.query("delete from UserSavedVisualizations Where ID=" + req.query.ID);
		res.send("Success");
	}
	catch(err) {
		console.log(err);
		res.send("ERROR");
	}
});

app.get('/fetchRecentViews', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var userDir = './' + USERS_DIR_NAME + '/' + sess.user.id;
	var userRecentViewsFile = path.resolve(userDir + "/" + RECENT_FILE_NAME);

	res.sendFile(userRecentViewsFile);
});

app.get('/fetchSelectedVizData', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var rowIds = req.query.rowIds;
	var temp;
	var query = "SELECT * FROM "  + req.query.tableName + " WHERE rowid";
	var returnObj = {
		data: []
	};

	//console.log("rowIds " + rowIds);

	if (rowIds) {
		
		if (typeof rowIds == 'array') {
			temp = JSON.stringify(rowIds).replace(/]$/, ')').replace("[", "(");
		}
		else if (typeof rowIds == 'string') {
			temp = rowIds.replace(/]$/, ')').replace("[", "(");
		}
		
		query = query + " IN " + temp;
		
		console.log(query);
		await sqlite.open(sess.user.userDbPath, { Promise });
		
		try {
			await sqlite.all(query)
			.then(function(data){
				returnObj.data = data;
			})
			.then(() => sqlite.close())
			.catch(err => console.log(err));
			
			res.send(returnObj);
		}
		catch(err) {
			console.log(err);
			res.send("ERROR");
		}
	}
	else {
		res.send([]);
	}
});

app.get('/fetchSelectedRowData', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var filterQuery = req.query.filterQuery;
	var returnObj = {
		data: []
	};
	
	console.log("filterQuery: " + filterQuery);

	if (filterQuery) {
		await sqlite.open(sess.user.userDbPath, { Promise });
		
		try {
			await sqlite.all(filterQuery)
			.then(function(data) {
				returnObj.data = data;
			})
			.then(() => sqlite.close())
			.catch(err => console.log(err));
			
			res.send(returnObj);
		}
		catch(err) {
			console.log(err);
			res.send("ERROR");
		}
	}
	else {
		res.send([]);
	}
});

app.get('/getXYZData', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var searchQuery = req.query.searchQuery;
	var returnObj = {
		data: []
	};
	
	console.log("searchQuery: " + searchQuery);

	if (searchQuery) {
		await sqlite.open(sess.user.userDbPath, { Promise });
		
		try {
			await sqlite.all(searchQuery)
			.then(function(data) {
				returnObj.data = data;
			})
			.then(() => sqlite.close())
			.catch(err => console.log(err));
			
			res.send(returnObj);
		}
		catch(err) {
			console.log(err);
			res.send("ERROR");
		}
	}
	else {
		res.send([]);
	}
});

app.post('/getInitialXYZ', async (req, res, next) => {
	if (!req.body || (req.body && !req.body.query)) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	var start = now();
	var end;
	
	var sdtPath = req.session.user.institutionDir + req.body.sdtPath.replace(/\\/g, '/') + '';

	var frontEndFilterObj = getFrontEndFilterInfo(sess.user.institutionDir + sdtPath.replace(/\\/g, '/') + '');

	res.send(frontEndFilterObj);
	
});

app.post('/applyFilters', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 
	
	//req.session.touch();

	var sess = req.session;
	var filterObj = req.body.filterObj;
	var query = "SELECT rowid FROM "  + req.body.tableName + " WHERE ";
	var returnObj = {
		data: []
	};
	
	// { colName:{selectedValues:[]},colName:{selectedValues:[]},colName:{selectedValues:[]} }

	// Form the query.
	var colNames = Object.keys(filterObj);
	var temp, flag = false;

	for (var index = 0; index < colNames.length; index++) {
		if (filterObj[colNames[index]].selectedValues.length > 0) {			
			if (flag) {
				query = query + " AND ";
			}
			temp = JSON.stringify(filterObj[colNames[index]].selectedValues);
			temp = temp.replace('[', '(').replace(/]$/, ")");
			query = query + colNames[index] + " IN " + temp;
			flag = true;
		}
	}
	
	console.log(query);
	await sqlite.open(sess.user.userDbPath, { Promise });
	
	await sqlite.all(query)
	.then(function(data) {
		returnObj.data = data;
	})
	.then(() => sqlite.close())
	.catch(err => console.log(err));
	
	res.send(returnObj);
});

app.post('/loadVisualization', async (req, res, next) => {
	if (!req.body || (req.body && !req.body.query)) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	var start = now();
	var end;
	
	var sdtPath = req.session.user.institutionDir + req.body.sdtPath.replace(/\\/g, '/') + '';
	var tableName = req.body.tableName;
	var query = req.body.query.replace(/"/g, "'");
	var filterAllowedColumnsQuery = "";
	var filterAllowedColumnList = req.body.filterAllowedColumnList ? req.body.filterAllowedColumnList : "*";
	
	var parentTableDataRows, parentTableColumnSchema;
	var innerIndex, outerIndex;
	
	// Update the query to only bring in filter allowed column list.
	try {
		filterAllowedColumnsQuery = query.replace("*", "rowid," + filterAllowedColumnList.toString());
		filterAllowedColumnsQuery = filterAllowedColumnsQuery ? filterAllowedColumnsQuery.split("WHERE")[0] : query;
		console.log(filterAllowedColumnsQuery);
	}
	catch(err) {
		console.log(err);
	}
	
	// Call the java utility to create the viz
	var dataFileCreated = createVizWithJavaCode({
		output: path.resolve(req.session.user.userDir + '/Viz'),
		sdt: path.resolve(sdtPath),
		query: [req.body.datasourceId, tableName, query]
	}, tableName);
	
	if (dataFileCreated) {
		const [mainDb, usersDb] = await Promise.all([
			sqlite.open(req.session.user.institutionDir + "data.db", { Promise }),
			sqlite.open(req.session.user.userDbPath, { Promise })
		]);
	
		// Fetch the rows from the main database.
		await mainDb.all(query)
		.then((result) => parentTableDataRows = result)
		.catch(err => console.log(err));
		
		if (Array.isArray(parentTableDataRows) && parentTableDataRows.length > 0) {
			// Create the Create query for ther user db.
			var createQuery = generateTableSchema(parentTableDataRows[0]);
			createQuery = createQuery.replace('?', tableName);
			
			// Create the table inside the user db.
			try {
				await usersDb.run(createQuery)
			}
			catch(err) {
				console.log(err);
			}
			
			// Drop the existing table data to put in new data.
			var deleteQuery = "DELETE FROM " + tableName;
			await usersDb.run(deleteQuery);
			
			// Insert the subset data that was pulled from the main db.
			var insertQuery = generateInsertQuery(parentTableDataRows);
			insertQuery = insertQuery.replace('?', tableName);
			await usersDb.run(insertQuery);
			
			var resultColumns = "";
			await usersDb.all(filterAllowedColumnsQuery)
			.then((res) => resultColumns = res)
			.catch(err => console.log(err));
			
			end = now();
			console.log((end-start).toFixed(3) + " miliseconds");
			
			res.send({ data: resultColumns, glyphViewerKey: './' + USERS_DIR_NAME + '/' + req.session.user.id + '/' + req.sessionID + '/Viz/' + tableName});
		}
		else {
			end = now();
			console.log((end-start).toFixed(3) + " miliseconds");
			res.send([]);
		}
	}
	else {
		res.send([]);
	}
	
});

app.get('/frontEndFilterData/:sdtFile', async (req, res, next) => {
	if (!req.params.sdtFile) {
		return res.sendStatus(400);
	} 
	
	// Set to 2 hours.
	res.setHeader('Cache-Control', 'public, max-age=' + (7200)); 
	
	var sess = req.session;
	var sdtPath = req.params.sdtFile;
	var savedVizData = '';

	if (req.params.sdtFile.indexOf("&&&") > -1) {
		sdtPath = req.params.sdtFile.split("&&&")[0];
		savedVizData = req.params.sdtFile.split("&&&")[1];
	}

	var query, rows;

	var returnObj = {
		frontEndFilterData: {},
		tableName: "",
		filterAllowedColumnList: [],
		initialX: '',
		initialY: '',
		initialZ: '',
		inputIDs: [],
		inputNames: []
	};
	
	var frontEndFilterObj = getFrontEndFilterInfo(sess.user.institutionDir + sdtPath.replace(/\\/g, '/') + '');

	console.log(frontEndFilterObj);
	
	// Check the functionatily.
	if (frontEndFilterObj != null) {
		await sqlite.open(sess.user.institutionDir + "data.db", { Promise });

		for (var i = 0; i < frontEndFilterObj.fieldList.length; i++) {
			query = "SELECT DISTINCT " + frontEndFilterObj.fieldList[i] + " FROM " + frontEndFilterObj.tableName;
			console.log(query);
			try {
				
				await sqlite.all(query)
				.then((result) => rows = result)
				.catch(err => console.log(err));
				
				//console.log(rows);
				returnObj.frontEndFilterData[frontEndFilterObj.fieldList[i]] = {
					values: rows,
					selectAll: frontEndFilterObj.selectAll[i]							
				}
			} 
			catch (err) {
				console.log(err);
			}
		}

		await sqlite.close();

		returnObj.tableName = frontEndFilterObj.tableName;
		returnObj.datasourceId = frontEndFilterObj.datasourceId;
		returnObj.filterAllowedColumnList = frontEndFilterObj.filterAllowedColumnList;

		returnObj.initialX = frontEndFilterObj.initialX;
		returnObj.initialY = frontEndFilterObj.initialY;
		returnObj.initialZ = frontEndFilterObj.initialZ;		
	}
	
	if (savedVizData != '') {
		savedVizData = savedVizData.split(","); // time, originalVizName, vizid, savedvizname
		saveViewInRecentViews(
			{
				originalVizName: savedVizData[1], 
				savedVizName: savedVizData[3], 
				time: savedVizData[0],
				date: savedVizData[0],
				savedViewId: savedVizData[2],
				savedView: true
			},
			req.session.user.id
		);
	}
	
	res.send(returnObj);
});

app.get('/isUserLoggedIn', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	var sess = req.session;
	console.log(sess);
	var returnObj = {
		isUserLoggedIn: true,
		userInformation: sess.user.userInformation,
		funnelInfo: sess.user.funnelInfo
	}

	res.send(returnObj);
});

app.get('/logout', function(req, res) {
	
	// Delete the session files.
	console.log(req.session.user);
	deteleUserSessionDirectory(req.session.user.userDir);
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	req.session.destroy(function(err) {
		if (err) {
			console.log(err);
			res.send({"loggedOut": false});
		} 
		else {
			res.send({"loggedOut": true});
		}
	});
});


/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// INITIAL SERVER CONFIGURATION //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

Promise.resolve()
  // First, try connect to the login database
  .then(() => mySqlConnection = new mysql({
				host     : 'sgxinstance.cqu1pnqua5at.us-west-2.rds.amazonaws.com',
				user     : 'sgxadmin',
				password : '#sgxpw13#',
				database : 'sgxdb',
				port: '3306'
			})
	)
  .then(() => app.listen(port))
  .then(() => console.log('Server is up and running!'))
  .catch(err => console.error(err.stack));
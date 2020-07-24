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

let AWS = require('aws-sdk');

let XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;
let http = require('http');

// Twilio configuration
// let twilio = require("twilio");


//deprecated Twilio sid and token used
// const accountSid = "ACf8bdf11b425c1a2b4dc32fd8e269d030";
// const authToken = "c943d8e261b1a814c75ca2cd7319798a";
// const serviceSid = "ISf8431dd8f893479cac16a5bcca564c1a";
// const apiSid = "SKac248c8e57613ec7645dd6ad40cb97a0";


//const accountSid = "AC4ac45e4fe73943f85369e6d6b90074cd";
//const authToken = "0628a5b87c40ba6b576d049bc66c9a11";

// const AccessToken = twilio.jwt.AccessToken;
// const ChatGrant = AccessToken.ChatGrant;
// const client = require('twilio')(accountSid, authToken);
// const service = client.chat.services(serviceSid);

// AWS API-Gateway/Lambda Configuration

const apiSecret = "W33ki0JguPHEnl8IWrnrziblhFsO3zAb";
const apiGatewayKey = "DljEtaIfiyTF0xENjqXeabNDZLtPxRd738rflzCd";
const apiGatewayBaseURL = 'https://bfjqbtjmo4.execute-api.us-east-2.amazonaws.com/deployment/';
const apiGatewayURL = `${apiGatewayBaseURL}items`;
const apiGatewayURLFrontEndFilters = `${apiGatewayBaseURL}frontEndFiltersEC2`;
const apiGatewayURLSelectedRowData = `${apiGatewayBaseURL}fetchSelectedRowDataEC2`;
const apiGatewayURLGenViz = `${apiGatewayBaseURL}loadVisualizationEC2`;
// const apiGatewayURLGenDb = `${apiGatewayBaseURL}loadVisualizationSqlEC2`;
const apiGatewayURLAppFilts = `${apiGatewayBaseURL}applyFiltersEC2`;
const apiGatewayURLGetXYZData = `${apiGatewayBaseURL}getXYZDataEC2`; 
const apiGatewayURLGetLegend = `${apiGatewayBaseURL}getLegendEC2`;
// const apiGatewayURLCreateShareLink = `${apiGatewayBaseURL}createShareLink`;
// const apiGatewayURLAuthShareLink = `${apiGatewayBaseURL}authShareLink`;
const apiGatewayURLFetchRecentViews = `${apiGatewayBaseURL}fetchRecentViewsEC2`;
/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// SERVER CONFIGURATION /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

let dotenv = require('dotenv');
let passport = require('passport');
let Auth0Strategy = require('passport-auth0');

var express = require('express');
var compression = require('compression');
var session = require('express-session');
var bodyParser = require('body-parser');

let {
	authenticateUser,
	authenticateUserNew,
	generateInsertQuery,
	generateTableSchema,
	createUserDirectory,
	deteleUserSessionDirectory,
	insertIntoMySqlDB,
	getFrontEndFilterInfo,
	fetchS3Obj,
	saveViewInRecentViews
} = require('./ServerFunctions.js');

const router = express.Router();

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
  .then(() => console.log(`MY SQL up A-M.`))
  .catch(err => console.error(err.stack));

router.post('/frontEndFiltersEC2', function (req, res) {
	let rawS3Key = req.body.key;
	let newKey = rawS3Key.replace("\\", "/");
	console.log('Front End Filters EC2 Key:' + newKey);
	let instit = req.session.user.institutionDir.slice(25, req.session.user.institutionDir.length-1);
	let instit_new = instit.replace("_", "-") + '-source';
	console.log("instit_new "+instit_new);
	//if(instit === 'glyphed_demo') instit_new = 'glyphed-demo-source';
	//if(instit === 'notredame') instit_new = 'notredame-source';
	let frontEndFiltsArr = [],
	fieldListArr = [],
	inputIDs = [],
	inputNames = [],
	selectAllObj = {};

	fetchS3Obj(newKey, instit_new)
	.then(res => {
		// console.log(res, "RESPONSE");
		if(res){
			let docu = new dom().parseFromString(res);
			let frontEndFilts;
			let errFront, errElastic, selectAllValues, fieldList;

			

			let tableName = xpath.select1("//Datasources/Datasource/Tables/Table/@name", docu).value;
			let datasourceId = xpath.select("//Datasources/Datasource/@id", docu);
			let initialX = xpath.select("//Glyphs/Glyph/Position/X/Binding/@fieldId", docu);
			let initialY = xpath.select("//Glyphs/Glyph/Position/Y/Binding/@fieldId", docu);
			let initialZ = xpath.select("//Glyphs/Glyph/Position/Z/Binding/@fieldId", docu);

			/* FINDING XYZ PARAMETERS */
			let inputFieldIDs = xpath.select("//InputFields/InputField/@name", docu);
			let inputFieldNames = xpath.select("//InputFields/InputField/@field", docu);
			// console.log(initialX[0].value, initialY[0].value, initialZ[0].value, "XYZ VALS DATA");
			
			for (var i = 0; i < inputFieldIDs.length; i++) {
				inputIDs.push(inputFieldIDs[i].value);
			}

			for (var i = 0; i < inputFieldNames.length; i++) {
				inputNames.push(inputFieldNames[i].value);
			}

			// console.log(inputIDs, inputNames);

			let xIndex = inputIDs.indexOf(initialX[0].value);
			let yIndex = inputIDs.indexOf(initialY[0].value);
			let zIndex = inputIDs.indexOf(initialZ[0].value);

			// console.log(xIndex, yIndex, zIndex);

			let x = (xIndex != -1 ? inputNames[xIndex] : null);
			let y = (yIndex != -1 ? inputNames[yIndex] : null);
			let z = (zIndex != -1 ? inputNames[zIndex] : null);

			try {
				frontEndFilts = xpath.select("//FrontEnd/Filter/FilterField/@field", docu);
				selectAllValues = xpath.select("//FrontEnd/Filter/@selectall", docu);

				/* FINDING FRONT END FILTERS AND SELECT ALL */
				for(let i = 0; i < frontEndFilts.length; i++){
					frontEndFiltsArr.push(frontEndFilts[i].value);
					selectAllObj[frontEndFilts[i].value] = selectAllValues[i] ? selectAllValues[i].value : true;
				}
			}
			catch(err){
				errFront = "NO FRONT END FILTERS";
			}

			try {
				fieldList = xpath.select("//ElasticList/ElasticField/@field", docu);

				/* FINDING FILTERED ALLOWED COLUMN LIST */
				for (var j = 0; j < fieldList.length; j++) {
					fieldListArr.push(fieldList[j].value);
				}	
			}
			catch(err){
				errElastic = "NO ELASTIC FIELDS";
				// return {
				// 	tableName, 
				// 	frontEndFiltsArr : (errFront === "NO FRONT END FILTERS" ? [] : frontEndFiltsArr), 
				// 	datasourceId: datasourceId[0].value, 
				// 	selectAllValues: (errFront === "NO FRONT END FILTERS" ? {} : selectAllObj),
				// 	filterAllowedColumnList: [],
				// 	x,
				// 	y,
				// 	z};
			}

				/* END OF FINDING XYZ PARAMETERS */
			// console.log(frontEndFilts, 'FIELDLIST')

			// console.log(tableName, frontEndFilts)
			return {
				tableName, 
				frontEndFiltsArr, 
				datasourceId: datasourceId[0].value, 
				selectAllValues: selectAllObj,
				filterAllowedColumnList: fieldListArr,
				x,
				y,
				z};
		}
	}).then(result => {
		console.log('RESULT OF FRONT END FILTS EC2', result.tableName, result.filterAllowedColumnList.length)
		if(result){
			
			let payload = {
				data: {
					filterList: result.frontEndFiltsArr, 
					tableName: result.tableName,
					institution: instit}
			};
			// console.log(result, "MY FRONT END FILTS");
			var xmlHttp = new XMLHttpRequest();
				// xmlHttp.withCredentials = true;

				xmlHttp.onreadystatechange = function() { 
					
						if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
							// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
							let addRes = JSON.parse(xmlHttp.responseText);
							// console.log(addRes, 'addRes of front end filters ec2');
							if(addRes.errorType){
								throw new Error('ERROR WITH LAMBDA: ', addRes);
							} else {
								addRes.body.tableName = result.tableName;
								addRes.body.datasourceId = result.datasourceId;
								addRes.body.selectAll = result.selectAllValues;
								addRes.body.filterAllowedColumnList = result.filterAllowedColumnList;
								addRes.body.initialX = result.x;
								addRes.body.initialY = result.y;
								addRes.body.initialZ = result.z;
								addRes = JSON.stringify(addRes);
							   // console.log(addRes)
							   res.send(addRes);
							}	
						}
						else if (xmlHttp.status === 500) {
							res.send(xmlHttp.statusText);
						}
				}
				// True for asynchronous 
			xmlHttp.open("POST", apiGatewayURLFrontEndFilters, true);
			xmlHttp.setRequestHeader("Content-Type", "application/json");
			xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
			xmlHttp.send(JSON.stringify(payload));
		}
		// else if(result.err === 'NO FRONT END FILTERS'){
		// 	res.send(JSON.stringify({err: "NO FRONT END FILTERS"}));
		// }
	}).catch(err => {
		console.log(err)
	});

});

router.get('/getLegendURL/:sdtFile', async (req, res, next) => {
	// let returnObj;

	if (!req.params.sdtFile) {
		return res.sendStatus(400);
	} 

	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate');
	
	var sess = req.session;
	// console.log('SESSION DATA FROM GET LEGEND URL',sess.user.institutionDir);
	let instit = sess.user.institutionDir.slice(25, sess.user.institutionDir.length-1);
	var sdtPath = req.params.sdtFile;
	// console.log(sess.user.institutionDir, sdtPath, 'getlegendUrl');
	if (req.params.sdtFile.indexOf("&&&") > -1) {
		sdtPath = req.params.sdtFile.split("&&&")[0];
		savedVizData = req.params.sdtFile.split("&&&")[1];
	}
	
	if(sess.user) {
		// let newPath = getLegendInfo(sess.user.institutionDir + sdtPath.replace(/\\/g, '/') + '');
		let xmlHttp = new XMLHttpRequest();

		xmlHttp.onreadystatechange = function() { 
		
			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
				// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
				// let addRes = JSON.parse(xmlHttp.responseText);
				//  addRes = JSON.stringify(addRes);
				// console.log(addRes)
				res.send(xmlHttp.responseText);
			}
			else if (xmlHttp.status === 500) {
				res.send(xmlHttp.statusText);
			}
		}

		// True for asynchronous 
		xmlHttp.open("POST", apiGatewayURLGetLegend, true);
		xmlHttp.setRequestHeader("Content-Type", "application/json");
		xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
		xmlHttp.send(JSON.stringify(
			{
				data:{
					sdtPath,
					institution: instit
				}
			}
		));
	}
	
});

router.post('/checkFrontEndFilterQuery', async (req, res, next) => {
	if (!req.body && !req.body.frontEndFilters) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	// console.log(req.session, 'session info of check front end filt query');

	var parentTableDataRows,parentTableColumnSchema;
	let data; 

	var innerIndex,outerIndex;
	var returnObj = {
		match: false,
		query: (req.body.frontEndFilters.length > 0 ? 
			("SELECT * FROM " + req.body.tableName.replace("'", "''") + " WHERE ") : 
				("SELECT * FROM " + req.body.tableName.replace("'", "''") + ";"))
	}

	if(req.body.frontEndFilters){
		data = req.body.frontEndFilters.slice();
		// 2D Array
		//[[],[],[]]
		for (outerIndex = 0; outerIndex < data.length; outerIndex++) {
			var dataItem = data[outerIndex].slice();
			var columnName = dataItem[0];

			// Removes the 1st element that is the name.
			dataItem.shift(); 

			var values = '("' + dataItem.toString() + '")';
			returnObj.query = returnObj.query + columnName.replace("'", "''") + " IN " + values.replace(/,/g , '","').replace("'", "''").replace(/"/g, "'");
			
			if (outerIndex != data.length-1) {
				returnObj.query = returnObj.query + " AND ";
			}

			else {
				returnObj.query = returnObj.query + ";";
			}
		}
	}
	
	const [mainDb] = await Promise.all([
		sqlite.open(req.session.user.institutionDir + "data.db", { Promise }),
	]);
	
	console.log("QUERY BEING PASSED IN: ",returnObj.query)
	// Fetch the rows from the main database.
	await mainDb.get(returnObj.query)
	.then((result) => parentTableDataRows = result)
	.catch(err => console.log(err, "HERE ERROR"));
		
	if (parentTableDataRows != {} && parentTableDataRows != null) {
		returnObj.match = true;
		
		// Saving the view in the recent views. To distinguish between click of recent view and normal click.
		if (req.body.time && req.session.user.userInformation.Email !== 'Guest1') {
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
});

router.get('/fetchSavedViews', async (req, res, next) => {
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

router.get('/deleteSavedViews', async (req, res, next) => {
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

router.get('/fetchRecentViews', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var userDir = './' + USERS_DIR_NAME + '/' + sess.user.id;
	var userRecentViewsFile = path.resolve(userDir + "/" + RECENT_FILE_NAME);

	res.sendFile(userRecentViewsFile);
});

router.post('/fetchRecentViewsEC2', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 
	
	let sess = req.session;

	let xmlHttp = new XMLHttpRequest();

	xmlHttp.onreadystatechange = function() { 
		if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
			// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
			console.log(typeof xmlHttp.responseText, xmlHttp.responseText, 'res text for fetch Recent Views EC2 request');
			res.send(xmlHttp.responseText);
		}
		else if (xmlHttp.status === 500) {
			console.log(xmlHttp.statusText, 'status text error for fetch Recent Views EC2 request');
			res.send(xmlHttp.statusText);
		}
	}

	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLFetchRecentViews, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify({
		data: {
			body: req.body,
			userId: sess.user.id,
		}
	}));
});

router.get('/fetchSelectedVizData', async (req, res, next) => {
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

router.get('/fetchSelectedRowData', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	var sess = req.session;
	var filterQuery = req.query.filterQuery;
	var returnObj = {
		data: []
	};
	
	console.log("filterQuery1: " + filterQuery);

	if (filterQuery) {
		await sqlite.open(sess.user.userDbPath, { Promise });
		
		try {
			await sqlite.all(filterQuery)
			.then(function(data) {
				// console.log(data, "HMMM")
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

router.post('/fetchSelectedRowDataEC2', (req, res) => {
	if (!req.body.query) {
		return res.sendStatus(400);
	} 
	var filterQuery = req.body.query;
	
	console.log("filterQuery2: " + filterQuery);

	let xmlHttp = new XMLHttpRequest();
	// xmlHttp.withCredentials = true;

	xmlHttp.onreadystatechange = function() { 
		
			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
				// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
				// let addRes = JSON.parse(xmlHttp.responseText);
				//  addRes = JSON.stringify(addRes);
				// console.log(addRes)
				res.send(xmlHttp.responseText);
			}
			else if (xmlHttp.status === 500) {
				res.send(xmlHttp.statusText);
			}
	}
	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLSelectedRowData, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify(
		{
			data: {
				query: filterQuery,
				user: req.session.user
			}
		}
		));

});

router.get('/getXYZData', async (req, res, next) => {
	// if (!req.body) {
	// 	return res.sendStatus(400);
	// } 

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

router.post('/getXYZDataEC2', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 

	let xmlHttp = new XMLHttpRequest();

	xmlHttp.onreadystatechange = function() { 
		if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
			// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
			console.log(typeof xmlHttp.responseText, xmlHttp.responseText, 'load viz res text for XYZ request');
			res.send(xmlHttp.responseText);
		}
		else if (xmlHttp.status === 500) {
			console.log(xmlHttp.statusText, 'status text error for XYZ request');
			res.send(xmlHttp.statusText);
		}
	}

	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLGetXYZData, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify({
		data: {
			body: req.body,
			user: req.session.user
		}
	}));
});

router.post('/applyFilters', async (req, res, next) => {
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
			query = query + '`' + colNames[index] + "`" + " IN " + temp;
			flag = true;
		}
	}
	
	console.log(query, 'QUERY');
	await sqlite.open(sess.user.userDbPath, { Promise });
	
	await sqlite.all(query)
	.then(function(data) {
		// console.log(data, "DATA FROM APPLY FILTERS");
		returnObj.data = data;
	})
	.then(() => sqlite.close())
	.catch(err => console.log(err));
	
	res.send(returnObj);
});

router.post('/applyFiltersEC2', async (req, res, next) => {
	// console.log(req.body, "REQ BODY FROM APPLY FILTS");
	if (!req.body) {
		return res.sendStatus(400);
	} 

	let xmlHttp = new XMLHttpRequest();

	xmlHttp.onreadystatechange = function() { 
		if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
			// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
			console.log(typeof xmlHttp.responseText, xmlHttp.responseText, 'load viz res text for Apply Filts request');
			res.send(xmlHttp.responseText);
		}
		else if (xmlHttp.status === 500) {
			console.log(xmlHttp.statusText, 'status text error for Apply Filts request');
			res.send(xmlHttp.statusText);
		}
	}

	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLAppFilts, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify({
		data: {
			body: req.body,
			user: req.session.user
		}
	}));
});

router.post('/loadVisualization', async (req, res, next) => {
	if (!req.body || (req.body && !req.body.query)) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	// var start = now();
	// var end;
	var sdtPath = req.session.user.institutionDir + req.body.sdtPath.replace(/\\/g, '/') + '';
	var tableName = req.body.tableName;
	var query = req.body.query.replace(/"/g, "'");
	// console.log('******************************************************************', req.body.filterAllowedColumnList, "********************************************************");
	var filterAllowedColumnsQuery = "";
	var filterAllowedColumnList = req.body.filterAllowedColumnList ? req.body.filterAllowedColumnList : "*";
	
	var parentTableDataRows, parentTableColumnSchema;
	var innerIndex, outerIndex;
	
	// Update the query to only bring in filter allowed column list.
	try {
		if (filterAllowedColumnList.length > 0) {
			var facl = '`' + filterAllowedColumnList.toString().replace(/,/g, '`,`') + '`';
			filterAllowedColumnsQuery = query.replace("*", "rowid," + facl);
		}
		else {
			filterAllowedColumnsQuery = query.replace("*", "rowid, *");
		}
		filterAllowedColumnsQuery = filterAllowedColumnsQuery ? filterAllowedColumnsQuery.split("WHERE")[0] : query;
		console.log('QUERY FOR LOAD VIZ: ' + filterAllowedColumnsQuery);
	}
	catch(err) {
		console.log(err);
	}
	
	// Call the java utility to create the viz
	// var dataFileCreated = createVizWithJavaCode({
	// 	output: path.resolve(req.session.user.userDir + '/Viz'),
	// 	sdt: path.resolve(sdtPath),
	// 	query: [req.body.datasourceId, tableName, query]
	// }, tableName);
	
	// if (dataFileCreated) {
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
			// console.log(parentTableDataRows[0], 'PARENT TABLE DATA ROWS');
			var createQuery = generateTableSchema(parentTableDataRows[0]);
			createQuery = createQuery.replace('?', tableName);

			console.log("\n CREATE QUERY");
			console.log(createQuery, '\n');
			
			// Create the table inside the user db.
			try {
				await usersDb.run(createQuery)
			}
			catch(err) {
				console.log("THIS AWAIT DOESNT WORK");
				console.log(err);
			}
			
			// Drop the existing table data to put in new data.
			var deleteQuery = "DELETE FROM " + tableName;
			console.log("\n DELETE QUERY");
			console.log(deleteQuery, '\n');
			await usersDb.run(deleteQuery);
			
			// Insert the subset data that was pulled from the main db.
			var insertQuery = generateInsertQuery(parentTableDataRows);
			insertQuery = insertQuery.replace('?', tableName);
			await usersDb.run(insertQuery);
			
			var resultColumns = "";
			await usersDb.all(filterAllowedColumnsQuery)
			.then((res) => resultColumns = res)
			.catch(err => console.log(err+"\n"+filterAllowedColumnsQuery));
			
			// end = now();
			// console.log((end-start).toFixed(3) + " miliseconds");
			
			res.send({ data: resultColumns, glyphViewerKey: './' + USERS_DIR_NAME + '/' + req.session.user.id + '/' + req.sessionID + '/Viz/' + tableName});
		}
		else {
			// end = now();
			// console.log((end-start).toFixed(3) + " miliseconds");
			res.send([]);
		}
	// }
	// else {
	// 	res.send([]);
	// }
	
});

router.get('/frontEndFilterData/:sdtFile', async (req, res, next) => {
	if (!req.params.sdtFile) {
		return res.sendStatus(400);
	} 

	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate');
	
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
		inputNames: [],
		legendPath: ''
	};
	
	var frontEndFilterObj = getFrontEndFilterInfo(sess.user.institutionDir + sdtPath.replace(/\\/g, '/') + '');

	// console.log(frontEndFilterObj);
	
	// Check the functionatily.
	if (frontEndFilterObj != null) {
		await sqlite.open(sess.user.institutionDir + "data.db", { Promise });
		for (var i = 0; i < frontEndFilterObj.fieldList.length; i++) {
			query = "SELECT DISTINCT " + frontEndFilterObj.fieldList[i] + " FROM " + frontEndFilterObj.tableName;
			console.log(query);
			try {
				await sqlite.all(query)
				.then((result) => rows = result
				)
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


router.get('/isUserLoggedIn', async (req, res, next) => {
	if (!req.body) {
		return res.sendStatus(400);
	} 
	
	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	var sess = req.session;
	// console.log(sess.user.funnelInfo, "0000000000000000000000000000");
	var returnObj = {
		isUserLoggedIn: true,
		userInformation: sess.user.userInformation,
		funnelInfo: sess.user.funnelInfo
	}

	res.send(returnObj);
});

router.get('/logout', function(req, res) {
	
	// Delete the session files.
	// console.log(req.session.user);
	console.log('Logging out...');
	deteleUserSessionDirectory(req.session.user.userDir);
	
	// res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
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

module.exports = router;
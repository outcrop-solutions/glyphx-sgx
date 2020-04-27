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
  .then(() => console.log(`MY SQL up N-Z.`))
  .catch(err => console.error(err.stack));

router.get('/vizdata/:key', function (req, res) {
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

router.post('/saveView', async (req, res, next) => {
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
				// console.log(updateQuery);
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

router.post('/testing', (req, res) => {
	if(!req.body){
		return res.sendStatus(400);
	}
	console.log('###########################',req.body,'################################');
	res.send('TESTING');
});

module.exports = router;
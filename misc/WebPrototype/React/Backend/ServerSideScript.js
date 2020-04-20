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

let AWS = require('aws-sdk');

// Twilio configuration
let twilio = require("twilio");
let XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;
let http = require('http');

const accountSid = "ACf8bdf11b425c1a2b4dc32fd8e269d030";
const authToken = "c943d8e261b1a814c75ca2cd7319798a";
//const accountSid = "AC4ac45e4fe73943f85369e6d6b90074cd";
//const authToken = "0628a5b87c40ba6b576d049bc66c9a11";
const serviceSid = "ISf8431dd8f893479cac16a5bcca564c1a";
const apiSid = "SKac248c8e57613ec7645dd6ad40cb97a0";

const AccessToken = twilio.jwt.AccessToken;
const ChatGrant = AccessToken.ChatGrant;
const client = require('twilio')(accountSid, authToken);
const service = client.chat.services(serviceSid);

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
const apiGatewayURLCreateShareLink = `${apiGatewayBaseURL}createShareLink`;
const apiGatewayURLAuthShareLink = `${apiGatewayBaseURL}authShareLink`;
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

var WebSocketServer = require('ws').Server;

const app = express();
let server = http.createServer(app);

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

// var strategy = new Auth0Strategy(
// 	{
// 	  domain: process.env.AUTH0_DOMAIN,
// 	  clientID: process.env.AUTH0_CLIENT_ID,
// 	  clientSecret: process.env.AUTH0_CLIENT_SECRET,
// 	  callbackURL:
// 		process.env.AUTH0_CALLBACK_URL || 'http://localhost:3000/callback'
// 	},
// 	function (accessToken, refreshToken, extraParams, profile, done) {
// 	  // accessToken is the token to call Auth0 API (not needed in the most cases)
// 	  // extraParams.id_token has the JSON Web Token
// 	  // profile has all the information from the user
// 	  return done(null, profile);
// 	}
//   );

/**
 * For serving the application via this server.
 */
// console.log("Server address: ",server.address(), server.address.address)
app.use(express.static(path.join(__dirname, '.')));

// async(request, response) => {
// 	console.log("REQ:", request, "RES: ", response)
// 	await handler(request, response, {
// 		"rewrites": [
// 			{ "source": "/", "destination": "/index.html" },
// 			{ "source": "/home", "destination": "/index.html" },
// 			{ "source": "/login", "destination": "/index.html" }
// 		]
// 	});
// }

let cookieArray = [];

app.get('/aws', function (req, res) {
	// console.log('***********', apiGatewayKey, apiGatewayURL, '*********')
	var xmlHttp = new XMLHttpRequest();
    // xmlHttp.withCredentials = true;

    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
			console.log(xmlHttp.responseText,'responseeeeeeeeeeeeeeeeeeeee');
            res.send(xmlHttp.responseText);
        }
        else if (xmlHttp.status === 500) {
            res.send(xmlHttp.statusText);
        }
    }

    // True for asynchronous 
	xmlHttp.open("GET", apiGatewayURL, true);
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(null);

});


app.post('/aws', function (req, res) {
	console.log('***********', apiGatewayKey, apiGatewayURL, req.body, '*********')
	var xmlHttp = new XMLHttpRequest();
    // xmlHttp.withCredentials = true;

    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
            res.send(xmlHttp.responseText);
        }
        else if (xmlHttp.status === 500) {
            res.send(xmlHttp.statusText);
        }
    }

    // True for asynchronous 
	xmlHttp.open("POST", apiGatewayURL, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(req.body);

});

app.post('/frontEndFiltersEC2', function (req, res) {
	let rawS3Key = req.body.key;
	let newKey = rawS3Key.replace("\\", "/");
	console.log('Front End Filters EC2 Key:' + newKey);
	let instit = req.session.user.institutionDir.slice(25, req.session.user.institutionDir.length-1);
	let instit_new;
	if(instit === 'glyphed_demo') instit_new = 'glyphed-demo-source';
	if(instit === 'notredame') instit_new = 'notredame-source';
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
/**
 * TWILIO SERVER ACTIONS
 */
app.get("/token", function(req, res) {
	let userInfo = req.query.userInfo;
	//console.log("username is: ", username);
	console.log(userInfo)
	let token = new AccessToken(
		accountSid,
		apiSid,
		apiSecret,
		{
			identity: userInfo,
			ttl: 40000
		}
	);

	let grant = new ChatGrant({ serviceSid: serviceSid });

	token.addGrant(grant);

	const tokenJwt = token.toJwt();
	console.log("token: " + tokenJwt);
	console.log(grant)

	res.send(tokenJwt);
});

/**
 * Get Institution IMG for the home page
 */
app.get('/customerImg/:instName', function (req, res) {
	var tempPath = "./Resources/Institutions/devtest/customer.png";
	
	res.setHeader('Cache-Control', 'public, max-age=' + (250000));
	
	//check if file exists.
	if (fs.existsSync(path.resolve("./"+ req.params.instName + "/customer.png"))) {
		res.sendFile(path.resolve("./"+ req.params.instName + "/customer.png"));
	}
	else {
		res.sendFile(path.resolve(tempPath));
	}
});

// app.get('/getLegendImg/:legendURL', function (req, res) {
// 	var tempPath = "./Resources/Institutions/devtest/customer.png";
// 	console.log(req.params.legendURL);
	
// 	res.setHeader('Cache-Control', 'public, max-age=' + (250000));
	
// 	//check if file exists.
// 	if (fs.existsSync(path.resolve(req.params.legendURL))) {
// 		console.log(path.resolve(req.params.legendURL), 'legend URL path');
// 		res.sendFile(path.resolve(req.params.legendURL));
// 	}
// 	else {
// 		res.sendFile(path.resolve(tempPath));
// 	}
// });


app.get('/getLegendURL/:sdtFile', async (req, res, next) => {
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

function authenticateUserNew(data) {
	
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


function generateTableSchema(data) {
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

function getLegendInfo(filename) {
	var fileData = fs.readFileSync(filename, "utf8");
	
	var returnObj = { }
	
	try {
		var doc = new dom().parseFromString(fileData);
		// console.log('refering file ' + filename);
		returnObj = xpath.select("//Legends/Legend", doc)[0].firstChild.data;
	}
	catch(err) {
		console.log(err);
		returnObj = null;
	}
	
	return returnObj;
}

function getFrontEndFilterInfo(filename) {
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

function fetchS3Obj(key, instit){
	
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
	console.log(params, 'PARAMS');
	//params = "{output:\"/home/ec2-user/dbserver/DataEngine/test\",sdt:\"/home/ec2-user/dbserver/DataEngine/test/Global Admissions/Global Admissions.sdt\",query:[\"719598f5-ff35-47b4-bf7a-9bdfbb30140f\",\"GlobalAdmissions\",\"SELECT * FROM GlobalAdmissions WHERE StaffAssigned IN ('Staff 1') AND Year IN ('2017')\"]}"
	var args = JSON.stringify(params).replace(/\\/g, '');
	args = '"' + args.replace(/"/g, '\\"') + '"';
	console.log(args, 'local backend args')
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
  if (req.path == '/login' || req.path.includes('/customerImg') || req.path.includes('/viz/') 
	  || req.path.includes('test') || req.path.includes('/authShareLink') /* || req.path.includes('/loginTwo') */ 
	  || req.path.includes('/loginThree') || req.path.includes('/enableConnection')) {
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
		console.log(userObj.sessionObj)
	}
	
	res.send(userObj.loginResult);
});

// app.get('/loginTwo', async (req, res, next) => {
// 	if (!req.query.username) {
// 		return res.sendStatus(400);
// 	} 
	
// 	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
// 	var objToPass = {
// 		host : req.get('host'),
// 		username : req.query.username
// 	}
	
// 	console.log("---------------------------------" + Date());
// 	console.log("Attempt Login from: " + req.query.username);

// 	var userObj = authenticateUserNew(objToPass);
// 	var savedViews = [];
	
// 	// On Success
// 	if (userObj.loginResult.status == 'success') {
		
// 		// Fetch the recent views and saved views list.
// 		userObj.loginResult.savedViews = [];
// 		userObj.loginResult.recentViews = [];
		
// 		// Create user directory on the server.
// 		// Store paths into session variable.
// 		userObj.sessionObj.id = sha256(req.query.username);
// 		userObj.sessionObj.userDir = createUserDirectory(userObj.sessionObj.id,req.sessionID);
// 		userObj.sessionObj.userDbPath = userObj.sessionObj.userDir + "/Data/subsetDatabase.sqlite";
		
// 		req.session.user = userObj.sessionObj;
// 		console.log(userObj.sessionObj)
// 	}
	
// 	res.send(userObj.loginResult);
// });

app.post('/loginThree', async (req, res, next) => {
	// console.log('REQ BODY OF LOGIN THREE',req.body)
	if (!req.body.user_email || !req.body.token) {
		return res.sendStatus(400);
	} 

	let xmlHttp = new XMLHttpRequest();
	let auth0Obj;

	res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 

	xmlHttp.onreadystatechange = function() { 
		if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
			console.log(xmlHttp.responseText, 'response for login');
			if(typeof xmlHttp.responseText === 'string') auth0Obj = JSON.parse(xmlHttp.responseText);
			else auth0Obj = xmlHttp.responseText;
		
			if(req.body.user_email === auth0Obj.email) {

				var objToPass = {
					host : req.get('host'),
					username : req.body.user_email 
				}
				
				console.log("Login Request on - " + Date() + " from " + req.body.user_email);
				// console.log("Attempt Login from: " + req.body.user_email);

				var userObj = authenticateUserNew(objToPass);
				var savedViews = [];
				
				// On Success
				if (userObj.loginResult.status == 'success') {
					
					// Fetch the recent views and saved views list.
					userObj.loginResult.savedViews = [];
					userObj.loginResult.recentViews = [];
					
					// Create user directory on the server.
					// Store paths into session variable.
					userObj.sessionObj.id = sha256(req.body.user_email);
					userObj.sessionObj.userDir = createUserDirectory(userObj.sessionObj.id,req.sessionID);
					userObj.sessionObj.userDbPath = userObj.sessionObj.userDir + "/Data/subsetDatabase.sqlite";
					
					req.session.user = userObj.sessionObj;
					// console.log(userObj.sessionObj)
				}
				res.send(userObj.loginResult);
			}
			else return res.sendStatus(400);
		}
		else if (xmlHttp.status === 500 || xmlHttp.status === 400) {
			console.log(xmlHttp.statusText, xmlHttp.status, 'status text error for login new');
			// res.send(xmlHttp.statusText);
			return res.sendStatus(400);
		}
	}

	// True for asynchronous 
	xmlHttp.open("GET", 'https://synglyphxinside.auth0.com/userinfo', true);
	xmlHttp.setRequestHeader("Authorization", `Bearer ${req.body.token}`);
	// xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(null);
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

app.post('/fetchRecentViewsEC2', async (req, res, next) => {
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

app.post('/fetchSelectedRowDataEC2', (req, res) => {
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

app.get('/getXYZData', async (req, res, next) => {
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

app.post('/getXYZDataEC2', async (req, res, next) => {
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

app.post('/applyFiltersEC2', async (req, res, next) => {
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

app.post('/loadVisualization', async (req, res, next) => {
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

app.post('/loadVisualizationEC2', async (req, res, next) => {
	if (!req.body || (req.body && !req.body.query)) {
		return res.sendStatus(400);
	} 
	// console.log(req.body, req.session.user, req.sessionID, 'load viz ec2');
	let xmlHttp = new XMLHttpRequest();
	// xmlHttp.withCredentials = true;

	//first request to create the json file with data engine. takes ~30 seconds

	xmlHttp.onreadystatechange = function() { 
			// console.log(xmlHttp.status, 'status of load viz ec2 request');
			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
				// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
				console.log(typeof xmlHttp.responseText, 'load viz res text for Load Viz request');
				res.send(xmlHttp.responseText);
			}
			else if (xmlHttp.status === 500) {
				console.log(xmlHttp.statusText, 'status text error for Load Viz request');
				res.send(xmlHttp.statusText);
			}
	}
	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLGenViz, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify({
		data: {
			body: req.body,
			sessionID: req.sessionID,
			user: req.session.user
		}
	}));
	
	// res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
	// var start = now();
	// var end;
	
	// var sdtPath = req.session.user.institutionDir + req.body.sdtPath.replace(/\\/g, '/') + '';
	// var tableName = req.body.tableName;
	// var query = req.body.query.replace(/"/g, "'");
	// // console.log('******************************************************************', req.body.filterAllowedColumnList, "********************************************************");
	// var filterAllowedColumnsQuery = "";
	// var filterAllowedColumnList = req.body.filterAllowedColumnList ? req.body.filterAllowedColumnList : "*";
	
	// var parentTableDataRows, parentTableColumnSchema;
	// var innerIndex, outerIndex;
	
	// // Update the query to only bring in filter allowed column list.
	// try {
	// 	if (filterAllowedColumnList.length > 0) {
	// 		var facl = '`' + filterAllowedColumnList.toString().replace(/,/g, '`,`') + '`';
	// 		filterAllowedColumnsQuery = query.replace("*", "rowid," + facl);
	// 	}
	// 	else {
	// 		filterAllowedColumnsQuery = query.replace("*", "rowid, *");
	// 	}
	// 	filterAllowedColumnsQuery = filterAllowedColumnsQuery ? filterAllowedColumnsQuery.split("WHERE")[0] : query;
	// 	console.log(filterAllowedColumnsQuery);
	// }
	// catch(err) {
	// 	console.log(err);
	// }
	
	// // Call the java utility to create the viz
	// var dataFileCreated = createVizWithJavaCode({
	// 	output: path.resolve(req.session.user.userDir + '/Viz'),
	// 	sdt: path.resolve(sdtPath),
	// 	query: [req.body.datasourceId, tableName, query]
	// }, tableName);
	
	// if (dataFileCreated) {
	// 	const [mainDb, usersDb] = await Promise.all([
	// 		sqlite.open(req.session.user.institutionDir + "data.db", { Promise }),
	// 		sqlite.open(req.session.user.userDbPath, { Promise })
	// 	]);
	
	// 	// Fetch the rows from the main database.
	// 	await mainDb.all(query)
	// 	.then((result) => parentTableDataRows = result)
	// 	.catch(err => console.log(err));
		
	// 	if (Array.isArray(parentTableDataRows) && parentTableDataRows.length > 0) {
	// 		// Create the Create query for ther user db.
	// 		var createQuery = generateTableSchema(parentTableDataRows[0]);
	// 		createQuery = createQuery.replace('?', tableName);

	// 		console.log("\n\n\n\nCREATE QUERY");
	// 		console.log(createQuery);
			
	// 		// Create the table inside the user db.
	// 		try {
	// 			await usersDb.run(createQuery)
	// 		}
	// 		catch(err) {
	// 			console.log("THIS AWAIT DOESNT WORK");
	// 			console.log(err);
	// 		}
			
	// 		// Drop the existing table data to put in new data.
	// 		var deleteQuery = "DELETE FROM " + tableName;
	// 		console.log("\n\n\n\n DELETE QUERY");
	// 		console.log(deleteQuery);
	// 		await usersDb.run(deleteQuery);
			
	// 		// Insert the subset data that was pulled from the main db.
	// 		var insertQuery = generateInsertQuery(parentTableDataRows);
	// 		insertQuery = insertQuery.replace('?', tableName);
	// 		await usersDb.run(insertQuery);
			
	// 		var resultColumns = "";
	// 		await usersDb.all(filterAllowedColumnsQuery)
	// 		.then((res) => resultColumns = res)
	// 		.catch(err => console.log(err+"\n"+filterAllowedColumnsQuery));
			
	// 		end = now();
	// 		console.log((end-start).toFixed(3) + " miliseconds");
			
	// 		res.send({ data: resultColumns, glyphViewerKey: './' + USERS_DIR_NAME + '/' + req.session.user.id + '/' + req.sessionID + '/Viz/' + tableName});
	// 	}
	// 	else {
	// 		end = now();
	// 		console.log((end-start).toFixed(3) + " miliseconds");
	// 		res.send([]);
	// 	}
	// }
	// else {
	// 	res.send([]);
	// }
});

app.post('/createShareLink', (req, res, next) => {
	if(!req.body){
		return res.sendStatus(400);
	}
	// console.log(req.body, 'body from create Share Link');
	// console.log(req.session.user.userDir, 'req.session from create share link');
	let xmlHttp = new XMLHttpRequest();
	// xmlHttp.withCredentials = true;

	//first request to create the json file with data engine. takes ~30 seconds

	xmlHttp.onreadystatechange = function() { 
			// console.log(xmlHttp.status, 'status of create share link request');
			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
				// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
				console.log(typeof xmlHttp.responseText, xmlHttp.responseText, 'load viz res text for create share link request');
				res.send(xmlHttp.responseText);
			}
			else if (xmlHttp.status === 500) {
				console.log(xmlHttp.statusText, 'status text error for create share link request');
				res.send(xmlHttp.statusText);
			}
	}
	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLCreateShareLink, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify({
		data: {
			body: {...req.body,
				userDir: req.session.user.userDir
			},
		}
	}));
});

app.post('/authShareLink', (req, res, next) => {
	console.log('I HIT THE AUTH SHARE ENDPOINT')
	if(!req.body){
		return res.sendStatus(400);
	}
	console.log(req.body, 'body from auth Share Link');
	
	let xmlHttp = new XMLHttpRequest();
	// xmlHttp.withCredentials = true;

	//first request to create the json file with data engine. takes ~30 seconds

	xmlHttp.onreadystatechange = function() { 
			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
				res.send(xmlHttp.responseText);
			}
			else if (xmlHttp.status === 500) {
				console.log(xmlHttp.statusText, 'status text error for create share link request');
				res.send(xmlHttp.statusText);
			}
	}
	// True for asynchronous 
	xmlHttp.open("POST", apiGatewayURLAuthShareLink, true);
	xmlHttp.setRequestHeader("Content-Type", "application/json");
	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
	xmlHttp.send(JSON.stringify({
		data: {
			body: req.body,
		}
	}));
});


app.get('/frontEndFilterData/:sdtFile', async (req, res, next) => {
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


app.get('/isUserLoggedIn', async (req, res, next) => {
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

app.get('/logout', function(req, res) {
	
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

app.post('/testing', (req, res) => {
	if(!req.body){
		return res.sendStatus(400);
	}
	console.log('###########################',req.body,'################################');
	res.send('TESTING');
});

app.post('/enableConnection', async (req, res, next) => {
	if(!req.body){
		return res.sendStatus(400);
	}
	
	if(req.body.uid && req.body.uid.length > 0 && req.body.cookie && req.body.establish){
		cookieArray.push({
			cookie: req.body.cookie,
			uid: req.body.uid
		})
		console.log("ARR: ", cookieArray);
		res.sendStatus(200);
	}
	else if(req.body.cookie && req.body.reestablish){
		for(let i = 0 ; i < cookieArray.length; i++){
			if(cookieArray[i].cookie === req.body.cookie){
				res.send({uid: cookieArray[i].uid});
			}
		}
	}
	else if(req.body.cookie && req.body.disestablish){
		for(let i = 0 ; i < cookieArray.length; i++){
			if(cookieArray[i].cookie === req.body.cookie){
				cookieArray.splice(i, 1);
				console.log(cookieArray, 'spliced');
				res.sendStatus(200);
			}
		}
	}
	else {
		res.sendStatus(400);
	}
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
				port	 : '3306'
			})
	)
  .then(() => server.listen(port))
  .then(() => console.log(`Server is up and running on ${port} under ${server.address().address}`))
  .catch(err => console.error(err.stack));


//Desktop side

var wss = new WebSocketServer({server: server, clientTracking: true});
wss.on('connection', function(ws, req) {
	// console.log("Url: ", req.url);
	// console.log(req.headers.origin);

    ws.on('message', function(data) {
		// console.log('****************CLIENTS', wss.clients, 'CLIENTS******************');

		console.log(data, 'data from message');
		if(data.indexOf('{') > -1){
		let parsed = JSON.parse(data);
			if(!parsed || !parsed.uid) ws.send(JSON.stringify({message: "Received."}));
			if(parsed && parsed.url_uid && parsed.launch === true && parsed.sdt && parsed.query && parsed.legendURLArr){

				wss.clients.forEach(obj => {

					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Launching. \n', parsed.legendURLArr);
						// console.log(parsed.sdt, parsed.query);
						obj.send(JSON.stringify({type: "LAUNCH", sdt: parsed.sdt, query: parsed.query, 
							legendURLArr: parsed.legendURLArr, institution: (parsed.institution ? parsed.institution : "")}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.open_url){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for opening a URL. \n');
						obj.send(JSON.stringify({type: "OPEN_URL", open_url: parsed.open_url}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.filters){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Filtering. \n');
						obj.send(JSON.stringify({type: "FILTER", array: parsed.filters}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.save_viz === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Saving Viz. \n');
						obj.send(JSON.stringify({type: "SAVE_VIZ"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.save_as_viz === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Save As Viz. \n');
						obj.send(JSON.stringify({type: "SAVE_AS_VIZ"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.view_stats === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Viewing Stats. \n');
						obj.send(JSON.stringify({type: "VIEW_STATS"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.view_selected === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Viewing Selected. \n');
						obj.send(JSON.stringify({type: "VIEW_SELECTED"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.view_filtered === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Viewing Filtered. \n');
						obj.send(JSON.stringify({type: "VIEW_FILTERED"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.settings_modal === true){
				
				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Settings Modal. \n');
						obj.send(JSON.stringify({type: "SETTINGS_MODAL"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.collapse_sidenav === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for collapsing sidenav. \n');
						obj.send(JSON.stringify({type: "COLLAPSE_SIDENAV"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.expand_sidenav === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for expanding sidenav. \n');
						obj.send(JSON.stringify({type: "EXPAND_SIDENAV"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.home === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for going Home. \n');
						obj.send(JSON.stringify({type: "HOME"}));
					}
				});
			}

			if(parsed && parsed.url_uid && parsed.logout === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for Logging out. \n');
						obj.send(JSON.stringify({type: "LOGOUT"}));
					}
				});
			}
			// if(parsed && parsed.url_uid && parsed.load_done === true){
			
			// 	wss.clients.forEach(obj => {
				
			// 		if(obj.id === parsed.url_uid){
			// 			console.log('Found the WebSocket for loading vfilters. \n');
			// 			obj.send(JSON.stringify({type: "VFILTERS LOAD DONE"}));
			// 		}
			// 	});
			// }
			if(parsed && parsed.url_uid && parsed.clear_all === true){

				wss.clients.forEach(obj => {
				
					if(obj.id === parsed.url_uid){
						console.log('Found the WebSocket for clear all vfilters. \n');
						obj.send(JSON.stringify({type: "VFILTERS CLEAR ALL"}));
					}
				});
			}
		} else if(data.indexOf('uid') > -1){
			let uid = data.slice(data.indexOf('uid')+4);
			ws.id = uid;
			console.log(`UID: ${uid}`);
		}
	});
    ws.on('close', function() {
	  if(ws.id){
		  console.log(`Connection closed: ${ws.id}`);
	  } else {
		  console.log('Connection closed');
	  }
    });
    ws.on('error', function(e) {
		console.log(e, 'error message');
		console.log('error');
		ws.send('ERROR 1');
	});
	ws.onerror = function(e) {
		console.log(e, 'error message');
		console.log('error');
		ws.send('ERROR 2');
    };
});
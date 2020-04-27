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

let routesA_M = require('./ServerRoutesA-M');
let routesN_Z = require('./ServerRoutesN-Z');

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

let cookieArray = [];

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// ROUTES ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

app.all('*', checkUser);

//alowing specific paths to bypass no user authentication
function checkUser(req, res, next) {
  if (req.path == '/login' || req.path.includes('/customerImg') || req.path.includes('/viz/') 
	  || req.path.includes('test') /* || req.path.includes('/authShareLink') */ /* || req.path.includes('/loginTwo') */ 
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

/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// ROUTES NOT REQUIRING LOGIN ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

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

//new login with auth0 implementation
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

//adding a cookie & uid object to cookie array to re-add -the uid - on client side for later visualization
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

/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// ROUTES REQUIRING LOGIN ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

app.use('/',  routesA_M);
app.use('/',  routesN_Z);

/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// NOT YET IMPLEMENTED ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

app.get('/copySdtFiles', async (req, res, next) => {
	// Future To-do copy the sdt files on request.
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
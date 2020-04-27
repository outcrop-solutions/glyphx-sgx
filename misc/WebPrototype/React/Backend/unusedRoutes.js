/**
 * TWILIO SERVER ACTIONS
 */
// app.get("/token", function(req, res) {
// 	let userInfo = req.query.userInfo;
// 	//console.log("username is: ", username);
// 	console.log(userInfo)
// 	let token = new AccessToken(
// 		accountSid,
// 		apiSid,
// 		apiSecret,
// 		{
// 			identity: userInfo,
// 			ttl: 40000
// 		}
// 	);

// 	let grant = new ChatGrant({ serviceSid: serviceSid });

// 	token.addGrant(grant);

// 	const tokenJwt = token.toJwt();
// 	console.log("token: " + tokenJwt);
// 	console.log(grant)

// 	res.send(tokenJwt);
// });

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

// app.post('/manageAnnouncements', async (req, res, next) => {
// 	if (!req.body && !req.body.type) {
// 		return res.sendStatus(400);
// 	} 

// 	var operation = req.body.operation;
// 	var userId = req.session.user.userInformation.ID;
// 	var config = req.body.config;
// 	var resultObj = {};
// 	var insertOptionsQuery = 'Insert into PollOptions (QuestionID,Description) values ';
	
// 	console.log(req.body);
	
// 	switch(operation) {
// 		// Create the announcement.
// 		case 'create':
// 			var createAnnouncementQuery = 'Insert into Announcements (Type,Description) values';
			
// 			createAnnouncementQuery += "('" + config.type + "','" + JSON.stringify(config.content) + "')";
// 			mySqlConnection.query(createAnnouncementQuery);
			
// 			var lastInsertedRow = mySqlConnection.query("Select * from Announcements Where ID=(SELECT LAST_INSERT_ID() as id)");
// 			var id = lastInsertedRow[0] ? lastInsertedRow[0].ID : null;
// 			resultObj.createdAnnouncementId = id;
			
// 			// If its a poll create the announcements.
// 			if (config.type.toLowerCase().indexOf('poll') > -1 && id != null) {
// 				for (var index = 0; index < config.options.length; index++) {
// 					insertOptionsQuery += "(" + id + ",'" + config.options[index] + "')";
					
// 					if (index != config.options.length-1) {
// 						insertOptionsQuery += ',';
// 					}
// 					else {
// 						insertOptionsQuery += ';';
// 					}
// 				}
				
// 				mySqlConnection.query(insertOptionsQuery);
// 			}

// 			resultObj.status = "success";
// 			break;
		
// 		case 'delete':
// 			var deleteAnnouncementQuery = 'Delete from Announcements where ID=' + config.AnnouncementId;
// 			mySqlConnection.query(deleteAnnouncementQuery);
// 			resultObj.status = "success";
// 			break;
		
// 		case 'fetch':
// 			//resultObj = mySqlConnection.query("select * from Announcements a left outer join PollOptions p on a.ID=p.QuestionID;");
// 			resultObj = mySqlConnection.query("select a.type,a.id as announcementId,a.CreationDate as announcementDate,a.description as announcementDescription,p.QuestionID as QuestionId,p.Description as optionDescription,p.CreationDate as optionsDate from Announcements a left outer join PollOptions p on a.ID=p.QuestionID;");
// 			break;
		
// 		case 'pollResult':
// 			if (config.AnnouncementId != null) {
				
// 				var pollQuery = "Insert into UserPollOptionSelection (UserID,QuestionID,AnswerID) values";
				
// 				// First we insert the selected ones into the UserPollOptionSelection table.
// 				for (var index = 0; index < config.selectedOptionIds.length; index++) {
// 					pollQuery += "(" + userId + "," + config.AnnouncementId + "," + config.selectedOptionIds[index] + ")";
					
// 					if (index != config.selectedOptionIds.length - 1) {
// 						pollQuery += ',';
// 					}
// 					else {
// 						pollQuery += ';';
// 					}
// 				}
				
// 				mySqlConnection.query(pollQuery);
				
// 				// We fetch the results for this poll.
// 				resultObj = mySqlConnection.query("Select * from UserPollOptionSelection where QuestionID=" + config.AnnouncementId);
// 			}
// 		break;
// 	}
	
// 	res.send(resultObj);
// });

// app.post('/createShareLink', (req, res, next) => {
// 	if(!req.body){
// 		return res.sendStatus(400);
// 	}
// 	// console.log(req.body, 'body from create Share Link');
// 	// console.log(req.session.user.userDir, 'req.session from create share link');
// 	let xmlHttp = new XMLHttpRequest();
// 	// xmlHttp.withCredentials = true;

// 	//first request to create the json file with data engine. takes ~30 seconds

// 	xmlHttp.onreadystatechange = function() { 
// 			// console.log(xmlHttp.status, 'status of create share link request');
// 			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
// 				// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
// 				console.log(typeof xmlHttp.responseText, xmlHttp.responseText, 'load viz res text for create share link request');
// 				res.send(xmlHttp.responseText);
// 			}
// 			else if (xmlHttp.status === 500) {
// 				console.log(xmlHttp.statusText, 'status text error for create share link request');
// 				res.send(xmlHttp.statusText);
// 			}
// 	}
// 	// True for asynchronous 
// 	xmlHttp.open("POST", apiGatewayURLCreateShareLink, true);
// 	xmlHttp.setRequestHeader("Content-Type", "application/json");
// 	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
// 	xmlHttp.send(JSON.stringify({
// 		data: {
// 			body: {...req.body,
// 				userDir: req.session.user.userDir
// 			},
// 		}
// 	}));
// });

// app.post('/authShareLink', (req, res, next) => {
// 	console.log('I HIT THE AUTH SHARE ENDPOINT')
// 	if(!req.body){
// 		return res.sendStatus(400);
// 	}
// 	console.log(req.body, 'body from auth Share Link');
	
// 	let xmlHttp = new XMLHttpRequest();
// 	// xmlHttp.withCredentials = true;

// 	//first request to create the json file with data engine. takes ~30 seconds

// 	xmlHttp.onreadystatechange = function() { 
// 			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
// 				res.send(xmlHttp.responseText);
// 			}
// 			else if (xmlHttp.status === 500) {
// 				console.log(xmlHttp.statusText, 'status text error for create share link request');
// 				res.send(xmlHttp.statusText);
// 			}
// 	}
// 	// True for asynchronous 
// 	xmlHttp.open("POST", apiGatewayURLAuthShareLink, true);
// 	xmlHttp.setRequestHeader("Content-Type", "application/json");
// 	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
// 	xmlHttp.send(JSON.stringify({
// 		data: {
// 			body: req.body,
// 		}
// 	}));
// });

// app.get('/aws', function (req, res) {
// 	// console.log('***********', apiGatewayKey, apiGatewayURL, '*********')
// 	var xmlHttp = new XMLHttpRequest();
//     // xmlHttp.withCredentials = true;

//     xmlHttp.onreadystatechange = function() { 
//         if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
// 			console.log(xmlHttp.responseText,'responseeeeeeeeeeeeeeeeeeeee');
//             res.send(xmlHttp.responseText);
//         }
//         else if (xmlHttp.status === 500) {
//             res.send(xmlHttp.statusText);
//         }
//     }

//     // True for asynchronous 
// 	xmlHttp.open("GET", apiGatewayURL, true);
// 	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
// 	xmlHttp.send(null);

// });

// app.post('/saveUserSettings', async (req, res, next) => {
// 	if (!req.body && !req.body.userSettings) {
// 		return res.sendStatus(400);
// 	} 

// 	var sess = req.session;
// 	var userDir = './' + USERS_DIR_NAME + '/' + sess.user.id;
// 	var userSettingsFile = path.resolve(userDir + "/" + SETTINGS_FILE_NAME);
	
// 	var fileContent = req.body.userSettings;
// 	fs.writeFileSync(userSettingsFile, JSON.stringify(fileContent));
// });

// app.get('/fetchUserSettings',async (req, res, next) => {
// 	if (!req.body) {
// 		return res.sendStatus(400);
// 	} 

// 	var sess = req.session;
// 	var userDir = './' + USERS_DIR_NAME + '/' + sess.user.id;
// 	var userSettingsFile = path.resolve(userDir + "/" + SETTINGS_FILE_NAME);
// 	res.sendFile(userSettingsFile);
// });


// app.post('/aws', function (req, res) {
// 	console.log('***********', apiGatewayKey, apiGatewayURL, req.body, '*********')
// 	var xmlHttp = new XMLHttpRequest();
//     // xmlHttp.withCredentials = true;

//     xmlHttp.onreadystatechange = function() { 
//         if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
//             res.send(xmlHttp.responseText);
//         }
//         else if (xmlHttp.status === 500) {
//             res.send(xmlHttp.statusText);
//         }
//     }

//     // True for asynchronous 
// 	xmlHttp.open("POST", apiGatewayURL, true);
// 	xmlHttp.setRequestHeader("Content-Type", "application/json");
// 	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
// 	xmlHttp.send(req.body);

// });

// app.post('/loadVisualizationEC2', async (req, res, next) => {
// 	if (!req.body || (req.body && !req.body.query)) {
// 		return res.sendStatus(400);
// 	} 
// 	// console.log(req.body, req.session.user, req.sessionID, 'load viz ec2');
// 	let xmlHttp = new XMLHttpRequest();
// 	// xmlHttp.withCredentials = true;

// 	//first request to create the json file with data engine. takes ~30 seconds

// 	xmlHttp.onreadystatechange = function() { 
// 			// console.log(xmlHttp.status, 'status of load viz ec2 request');
// 			if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
// 				// console.log(xmlHttp.responseText, typeof xmlHttp.responseText);
// 				console.log(typeof xmlHttp.responseText, 'load viz res text for Load Viz request');
// 				res.send(xmlHttp.responseText);
// 			}
// 			else if (xmlHttp.status === 500) {
// 				console.log(xmlHttp.statusText, 'status text error for Load Viz request');
// 				res.send(xmlHttp.statusText);
// 			}
// 	}
// 	// True for asynchronous 
// 	xmlHttp.open("POST", apiGatewayURLGenViz, true);
// 	xmlHttp.setRequestHeader("Content-Type", "application/json");
// 	xmlHttp.setRequestHeader("x-api-key", apiGatewayKey);
// 	xmlHttp.send(JSON.stringify({
// 		data: {
// 			body: req.body,
// 			sessionID: req.sessionID,
// 			user: req.session.user
// 		}
// 	}));
	
// 	// res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate'); 
	
// 	// var start = now();
// 	// var end;
	
// 	// var sdtPath = req.session.user.institutionDir + req.body.sdtPath.replace(/\\/g, '/') + '';
// 	// var tableName = req.body.tableName;
// 	// var query = req.body.query.replace(/"/g, "'");
// 	// // console.log('******************************************************************', req.body.filterAllowedColumnList, "********************************************************");
// 	// var filterAllowedColumnsQuery = "";
// 	// var filterAllowedColumnList = req.body.filterAllowedColumnList ? req.body.filterAllowedColumnList : "*";
	
// 	// var parentTableDataRows, parentTableColumnSchema;
// 	// var innerIndex, outerIndex;
	
// 	// // Update the query to only bring in filter allowed column list.
// 	// try {
// 	// 	if (filterAllowedColumnList.length > 0) {
// 	// 		var facl = '`' + filterAllowedColumnList.toString().replace(/,/g, '`,`') + '`';
// 	// 		filterAllowedColumnsQuery = query.replace("*", "rowid," + facl);
// 	// 	}
// 	// 	else {
// 	// 		filterAllowedColumnsQuery = query.replace("*", "rowid, *");
// 	// 	}
// 	// 	filterAllowedColumnsQuery = filterAllowedColumnsQuery ? filterAllowedColumnsQuery.split("WHERE")[0] : query;
// 	// 	console.log(filterAllowedColumnsQuery);
// 	// }
// 	// catch(err) {
// 	// 	console.log(err);
// 	// }
	
// 	// // Call the java utility to create the viz
// 	// var dataFileCreated = createVizWithJavaCode({
// 	// 	output: path.resolve(req.session.user.userDir + '/Viz'),
// 	// 	sdt: path.resolve(sdtPath),
// 	// 	query: [req.body.datasourceId, tableName, query]
// 	// }, tableName);
	
// 	// if (dataFileCreated) {
// 	// 	const [mainDb, usersDb] = await Promise.all([
// 	// 		sqlite.open(req.session.user.institutionDir + "data.db", { Promise }),
// 	// 		sqlite.open(req.session.user.userDbPath, { Promise })
// 	// 	]);
	
// 	// 	// Fetch the rows from the main database.
// 	// 	await mainDb.all(query)
// 	// 	.then((result) => parentTableDataRows = result)
// 	// 	.catch(err => console.log(err));
		
// 	// 	if (Array.isArray(parentTableDataRows) && parentTableDataRows.length > 0) {
// 	// 		// Create the Create query for ther user db.
// 	// 		var createQuery = generateTableSchema(parentTableDataRows[0]);
// 	// 		createQuery = createQuery.replace('?', tableName);

// 	// 		console.log("\n\n\n\nCREATE QUERY");
// 	// 		console.log(createQuery);
			
// 	// 		// Create the table inside the user db.
// 	// 		try {
// 	// 			await usersDb.run(createQuery)
// 	// 		}
// 	// 		catch(err) {
// 	// 			console.log("THIS AWAIT DOESNT WORK");
// 	// 			console.log(err);
// 	// 		}
			
// 	// 		// Drop the existing table data to put in new data.
// 	// 		var deleteQuery = "DELETE FROM " + tableName;
// 	// 		console.log("\n\n\n\n DELETE QUERY");
// 	// 		console.log(deleteQuery);
// 	// 		await usersDb.run(deleteQuery);
			
// 	// 		// Insert the subset data that was pulled from the main db.
// 	// 		var insertQuery = generateInsertQuery(parentTableDataRows);
// 	// 		insertQuery = insertQuery.replace('?', tableName);
// 	// 		await usersDb.run(insertQuery);
			
// 	// 		var resultColumns = "";
// 	// 		await usersDb.all(filterAllowedColumnsQuery)
// 	// 		.then((res) => resultColumns = res)
// 	// 		.catch(err => console.log(err+"\n"+filterAllowedColumnsQuery));
			
// 	// 		end = now();
// 	// 		console.log((end-start).toFixed(3) + " miliseconds");
			
// 	// 		res.send({ data: resultColumns, glyphViewerKey: './' + USERS_DIR_NAME + '/' + req.session.user.id + '/' + req.sessionID + '/Viz/' + tableName});
// 	// 	}
// 	// 	else {
// 	// 		end = now();
// 	// 		console.log((end-start).toFixed(3) + " miliseconds");
// 	// 		res.send([]);
// 	// 	}
// 	// }
// 	// else {
// 	// 	res.send([]);
// 	// }
// });
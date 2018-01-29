const express = require('express');
const bodyParser = require('body-parser');
const ejs = require('ejs');
const paypal = require('paypal-rest-sdk');
const nodemailer = require('nodemailer');
const session = require('express-session');
const mysql = require('sync-mysql');
const util = require('util');
const crypto = require('crypto');

mySqlConnection = new mysql({
    host     : 'sgxinstance.cqu1pnqua5at.us-west-2.rds.amazonaws.com',
    user     : 'sgxadmin',
    password : '#sgxpw13#',
    database : 'sgxdb',
    port: '3306'
})
            

const transporter = nodemailer.createTransport({
    /*
    //service: 'Godaddy',
    //service: "Godaddy",
    host: 'smtp.office365.com',
    port: '587',
    secureConnection: false,
    tls: { ciphers: 'SSLv3' },
    //host: "smtp.office365.com",
    //host: "smtpout.secureserver.net",
    //port: 25,         
    //secure: false,
    auth: {
        user: 'marwane@synglyphx.com',
        pass: 'SGX2013!'
    }
    */
    host: 'smtp.office365.com',
    port: '587',
    secureConnection: false,
    auth: {
        user: 'marwane@synglyphx.com',
        pass: 'SGX2013!'
    }
});



paypal.configure({
  'mode': 'live', //sandbox or live
  'client_id': 'ASiaxohdEJ1ez_1H6UuA-sTbb6f4X6I5cLy1IA1EXYWNjTxgn5-Nn083_zmoo0PmQUB2DxeyQtPPdjGk',
  'client_secret': 'EKMewmGxnndcppsJdvdLtISI95OCbNHIN1XSIKsOAm4o03ze7oeQtZU4dVSW5B-uKCaG1Q9GHnK3bYyJ'
});


const app = express();


// Session Management //
app.use(session({
    secret: 'SynGlyphXSHOP',
    resave: false,
    cookie: {
        httpOnly: true,
        secure: false,
        maxAge: 1800000 // 30(minutes) * 60 * 1000
    },
   saveUninitialized: false
}));
    

app.use(express.static(__dirname + '/public'));
app.use(bodyParser.urlencoded({ extended: true }));
//var middleware = [express.static(__dirname + '/public'), express.bodyParser()];
//app.use('/', middleware);

app.set('view engine', 'ejs');

app.get('/', (req, res) => res.render('index'));


app.post('/pay', (req, res) => {

    var promoCode = "";
    var parseTotal;
    var emailRegEx;

    if (req.body.user.PROMO) {
        promoCode = req.body.user.PROMO;
    }

    try {
        //get price of promo code for total
        var promoCodeResults = mySqlConnection.query("Select * from Promo Where Code='" + promoCode + "'");
        if(util.isArray(promoCodeResults) && promoCodeResults[0]){
            if (promoCodeResults.length > 0) {
                emailRegEx = promoCodeResults[0].InstEmail;

                var re = new RegExp(emailRegEx, "g");
                var reMatch = req.body.user.email.match(re);
                if (reMatch == null) {
                    parseTotal = "3525.00";
                }
                else {
                    parseTotal = promoCodeResults[0].TotalAmount;
                }
            }
            else {
                parseTotal = "3525.00";
            }
        }
        else {
            parseTotal = "3525.00";
        }
    }
    catch(err) {
        console.log(err);
    }

    
    try {
        req.session.user = { 
            name: req.body.user.name,
            email: req.body.user.email,
            password: req.body.user.password,
            PROMO: req.body.user.PROMO,
            total: parseTotal
        }
    }
    catch(err) {
        console.log(err);
    }
    

    const create_payment_json = {
        "intent": "sale",
        "payer": {
            "payment_method": "paypal"
        },
        "redirect_urls": {
            "return_url": "http://ec2-52-11-58-74.us-west-2.compute.amazonaws.com:5000/success",
            "cancel_url": "http://ec2-52-11-58-74.us-west-2.compute.amazonaws.com:5000/"
        },
        "transactions": [{
            "item_list": {
                "items": [{
                    "name": "GlyphIT Suite",
                    "sku": "GlyphIT001",
                    "price": req.session.user.total,
                    "currency": "USD",
                    "quantity": 1
                }]
            },
            "amount": {
                "currency": "USD",
                "total": req.session.user.total
            },
            "description": "thing"
        }]
    };

    try {
        paypal.payment.create(create_payment_json, function (error, payment) {
            if (error) {
                console.log(error);
            } else {
                console.log(payment);
                
                
                for (let i = 0; i < payment.links.length; i++) {
                    if (payment.links[i].rel === 'approval_url') {
                        res.redirect(payment.links[i].href);
                    }
                }
                
            }
        });
    }
    catch(err) {
        console.log(err);
    }
});


app.get('/success', (req, res) => {
        const payerId = req.query.PayerID;
        const paymentId = req.query.paymentId;

        const execute_payment_json = {
            "payer_id": payerId,
            "transactions": [{
                "amount": {
                    "currency": "USD",
                    "total": req.session.user.total
                }
            }]
        };

        try {
            paypal.payment.execute(paymentId, execute_payment_json, function (error, payment) {
                if (error) {
                    console.log(error.response);
                } else {
                    console.log(JSON.stringify(payment));

                    try {
		
                        var selectResult = mySqlConnection.query("Select ID from Institutions Where Name='Web Registered'");
                        var Institution = selectResult[0].ID;
                        //console.log("Institution:" + Institution );

                        //Insert the new user in user accounts.
                        var insertQuery = "Insert into UserAccounts (Name,Email,Password,Institution) Values (?)"; 
                        var insertvalues = "'" + req.session.user.name + "','" + req.session.user.email + "','" + req.session.user.password + "'," + Institution; //you will have to put the quotes as well.
                        insertQuery = insertQuery.replace('?',insertvalues);
                        mySqlConnection.query(insertQuery);
                        //console.log("insertQuery:" + insertQuery );

                        var lastInsertedRow = mySqlConnection.query("Select * from UserAccounts Where ID=(SELECT LAST_INSERT_ID() as id)");
                        var userID = lastInsertedRow[0].ID;
                        //console.log("userID:" + userID );
						
						var ts = Math.trunc((((new Date().getTime()/1000)/86400)+365)*86400);

                        if (req.session.user.PROMO == "ND40120VERMA") {
                            ts = Math.trunc((((new Date().getTime()/1000)/86400)+182)*86400);
                        }

						var text = userID.toString().valueOf() + "1".valueOf() + ts.toString().valueOf();
						var hashCode = crypto.createHash('md5').update(text).digest("hex");

						timeCode = ts.toString(16);

						var key = "";
						key += hashCode.substring(0, 8) + "-";
						key += hashCode.substring(8, 16) + "-";
						key += hashCode.substring(16, 24) + "-";
						key += hashCode.substring(24, 32) + "-";
						key += timeCode;

                        //Insert the new user in UsageLicenses.
                        var insertLicenseValues = userID + ",1,365,'"+ key + "'";

                        if (req.session.user.PROMO == "ND40120VERMA") {
                            insertLicenseValues = userID + ",1,182,'"+ key + "'";
                        }

                        mySqlConnection.query("Insert into UsageLicenses Values ("+insertLicenseValues+")");

                        mySqlConnection.query("INSERT into User_Promo values ("+lastInsertedRow[0].ID+",'"+ req.session.user.PROMO +"','"+req.session.user.total+"')");


                        var mailOptions = {
                            from: 'marwane@synglyphx.com',
                            to: req.body.user.email,
                            subject: 'Weclome to SynGlyphX',
                            text:   ("Hello,\n\nThank you for purchasing the GlyphIT® suite of software. We are very excited to have you as a part of the SynGlyphX® community."
                                    + "\n\nAttached you'll find the installation instructions to get you started using our software. \n\nIf you run into any issues you can reach us at: support@synglyphx.com"
                                    + "\nTo learn more about SynGlyphX and what we do, please visit us at our website: http://www.synglyphx.com/ \n\nBest, \nThe SynGlyphX Team"),
                            attachments: [{
                                filename: 'GlyphIT Installation Instructions.pdf',
                                path: 'public/res/GlyphIT Installation Instructions.pdf',
                                contentType: 'application/pdf'
                            }], function (err, info) {
                                if(err){
                                    console.log("EMAIL WOOP ERROR");
                                    console.error(err);
                                    //res.send(err);
                                }
                                else{
                                    console.log(info);
                                    //res.send(info);
                                }
                            }
                        };

                        transporter.sendMail(mailOptions, function(error, info){
                            if (error) {
                                console.log("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n MAIL ERROR ----------------------------------");
                                console.log(error);
                            } else {
                                console.log("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n MAIL SENT ----------------------------------");
                                console.log('Email sent: ' + info.response);
                            }
                        });
                    }
                    catch(err) {
                        console.log(err);
                    }
                    

                    res.render('success');
                }
            });
        }
        catch(err) {
            console.log(err);
        }

});

app.get('/queryPromoCode', (req, res) => {
    //console.log(req.query.promoCode);
    try { 
        var promoCodeResults = mySqlConnection.query("Select * from Promo Where Code='" + req.query.promoCode + "'");
        if(util.isArray(promoCodeResults) && promoCodeResults[0]){
            //console.log(promoCodeResults[0].TotalAmount);
            res.send({total: promoCodeResults[0].TotalAmount, discount: 3525 - promoCodeResults[0].TotalAmount, original: 3525, emailRegEx: promoCodeResults[0].InstEmail});
        }
        else {
            res.send("");
        }
    }
    catch(err) {
        console.log(err);
    }
});

app.get('/freePromoCode', (req, res) => {
    //console.log("/FREEPROMO -------------------------------------- ");

    var uInfo = JSON.parse(req.query.userInfo);

    var uTotal;

    try {
        //get price of promo code for total
        var promoCodeResults = mySqlConnection.query("Select * from Promo Where Code='" + uInfo.PROMO + "'");
        if(util.isArray(promoCodeResults) && promoCodeResults[0]){
            if (promoCodeResults.length > 0) {
                var emailRegEx = promoCodeResults[0].InstEmail;

                var re = new RegExp(emailRegEx, "g");
                var reMatch = uInfo.email.match(re);
                if (reMatch == null) {
                    uTotal = "-1";
                }
                else {
                    uTotal = promoCodeResults[0].TotalAmount;
                }
            }
            else {
                uTotal = "-1";
            }
        }
        else {
            uTotal = "-1";
        }

    }
    catch(err) {
        console.log(err);
    }

    try {				
        var selectResult = mySqlConnection.query("Select ID from Institutions Where Name='Web Registered'");
        var Institution = selectResult[0].ID;
        //console.log("Institution:" + Institution );

        //Insert the new user in user accounts.
        var insertQuery = "Insert into UserAccounts (Name,Email,Password,Institution) Values (?)"; 
        var insertvalues = "'" + uInfo.name + "','" + uInfo.email + "','" + uInfo.password + "'," + Institution; //you will have to put the quotes as well.
        insertQuery = insertQuery.replace('?',insertvalues);
        mySqlConnection.query(insertQuery);
        //console.log("insertQuery:" + insertQuery );

        var lastInsertedRow = mySqlConnection.query("Select * from UserAccounts Where ID=(SELECT LAST_INSERT_ID() as id)");
        var userID = lastInsertedRow[0].ID;
        //console.log("userID:" + userID );
		
		var ts = Math.trunc((((new Date().getTime()/1000)/86400)+365)*86400);

        if (uInfo.PROMO == "ND40120VERMA") {
            ts = Math.trunc((((new Date().getTime()/1000)/86400)+182)*86400);
        }

		var text = userID.toString().valueOf() + "1".valueOf() + ts.toString().valueOf();
		var hashCode = crypto.createHash('md5').update(text).digest("hex");

		timeCode = ts.toString(16);

		var key = "";
		key += hashCode.substring(0, 8) + "-";
		key += hashCode.substring(8, 16) + "-";
		key += hashCode.substring(16, 24) + "-";
		key += hashCode.substring(24, 32) + "-";
		key += timeCode;

        //Insert the new user in UsageLicenses.
        var insertLicenseValues = userID + ",1,365,'"+ key + "'";

        if (uInfo.PROMO == "ND40120VERMA") {
            insertLicenseValues = userID + ",1,182,'"+ key + "'";
        }

        mySqlConnection.query("Insert into UsageLicenses Values ("+insertLicenseValues+")");

        mySqlConnection.query("INSERT into User_Promo values ("+lastInsertedRow[0].ID+",'"+ uInfo.PROMO +"','"+uTotal+"')");

        var mailOptions = {
            from: 'marwane@synglyphx.com',
            to: uInfo.email,
            subject: 'Weclome to SynGlyphX',
            text:   ("Hello,\n\nThank you for purchasing the GlyphIT® suite of software.  We are very excited to have you as a part of the SynGlyphX® community."
                    + "\n\nAttached you'll find the installation instructions to get you started using our software. \n\nIf you run into any issues you can reach us at: support@synglyphx.com"
                    + "\nTo learn more about SynGlyphX and what we do, please visit us at our website: http://www.synglyphx.com/ \n\nBest, \nThe SynGlyphX Team"),
            attachments: [{
                filename: 'GlyphIT Installation Instructions.pdf',
                path: 'public/res/GlyphIT Installation Instructions.pdf',
                contentType: 'application/pdf'
            }], function (err, info) {
                if(err){
                    console.log("EMAIL WOOP ERROR");
                    console.error(err);
                    //res.send(err);
                }
                else{
                    console.log(info);
                    //res.send(info);
                }
            }
        };

        transporter.sendMail(mailOptions, function(error, info){
            if (error) {
                console.log("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n MAIL ERROR ----------------------------------");
                console.log(error);
            } else {
                console.log("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n MAIL SENT ----------------------------------");
                console.log('Email sent: ' + info.response);
            }
        });

        res.send('download');
    }
    catch(err) {
        console.log(err);
    }
    
});

app.get('/download', function(req, res) {
    res.render('download');
});


app.get('/cancel', (req, res) => res.render('cancel'));


app.listen(5000, () => console.log('Server Started'));
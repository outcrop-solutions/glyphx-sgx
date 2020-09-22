const AWS = require('aws-sdk');

const glue = new AWS.Glue();

// get reference to S3 client
exports.handler = async (event) => {

    const identity = JSON.parse(event).identity;
    //const identity = "us-east-1:d7424e02-fae8-449f-af16-39c4bbb27456";
    
    var params = {
      DatabaseInput: { /* required */
        Name: identity, /* required */
      }
    };
    
    let data = await glue.createDatabase(params, function(err, data) {
      if (err) console.log(err, err.stack); // an error occurred
      else     console.log(data);           // successful response
    }).promise();
    
    // TODO implement
    const response = {
        statusCode: 200,
        headers: {
            "Access-Control-Allow-Headers" : "Content-Type",
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "OPTIONS,POST"
        },
        body: JSON.stringify(data),
    };
    return response;
};
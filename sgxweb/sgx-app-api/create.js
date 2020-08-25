const AWS = require('aws-sdk');

const s3 = new AWS.S3();

// get reference to S3 client
exports.handler = async (event) => {

    const identity = JSON.parse(event).identity;
    
    var params = {
      Bucket: "sgx-app-user-storage", 
      Key: "private/"+identity+"/"
    };
    
    await s3.putObject(params, function(err, data) {
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
        body: JSON.stringify('Success! '+identity),
    };
    return response;
};


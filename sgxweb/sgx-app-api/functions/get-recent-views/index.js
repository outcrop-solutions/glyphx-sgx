const AWS = require('aws-sdk');

const s3 = new AWS.S3();

// get reference to S3 client
exports.handler = async (event) => {

    const identity = JSON.parse(event).identity;
    //const identity = "us-east-1:bca95e83-235b-4c4d-9fee-7c5b100c26a4";
    
    var params = {
      Bucket: "sgx-app-user-storage", 
      Prefix: "private/"+identity+"/Saved/"
    };
    
    let data = await s3.listObjects(params, function(err, data) {
       if (err) console.log(err, err.stack); // an error occurred
       else     console.log(data);           // successful response
    }).promise();
    
    var keys = [];
    data.Contents.forEach((elem) => {
        keys = keys.concat("{\"Key\": \""+elem.Key+"\",\"LastModified\": \""+elem.LastModified+"\"}");
    });
    
    // TODO implement
    const response = {
        statusCode: 200,
        headers: {
            "Access-Control-Allow-Headers" : "Content-Type",
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "OPTIONS,POST"
        },
        body: JSON.stringify(keys),
    };
    return response;
};

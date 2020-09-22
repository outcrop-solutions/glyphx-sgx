const AWS = require('aws-sdk');
const LambdaFunction = require('./lambdaFunction');
var s3 = new AWS.S3();

exports.handler = async (event, context) => {

    const identity = JSON.parse(event).identity;
    const directory = JSON.parse(event).directory;
    //const identity = "us-east-1:bca95e83-235b-4c4d-9fee-7c5b100c26a4";
    //const directory = "Practice Dummy Data";
    const tablename = directory.toLowerCase().split(" ").join("_");
    
    var params = {
        FunctionName: 'sgx-app-api-get-table-metadata', // the lambda function we are going to invoke
        Payload: '{ "identity" : "'+identity+'", "tablename" : "'+tablename+'" }'
    };
    let data = await LambdaFunction.callLambdaFunction(params);
    let results = JSON.parse(data.Payload).TableMetadata.Columns; //Name, Type
    
    let x = results[0]["Name"];
    let query = 'SELECT MIN(\\\"'+x+'\\\"), MAX(\\\"'+x+'\\\"), COUNT(\\\"'+x+'\\\"), COUNT(DISTINCT \\\"'+x+'\\\")';
    for(let i = 1; i < results.length; i++){
        
        let x = results[i]["Name"];
        //fieldname, type, min, max, count, distinct
        query += ', MIN(\\\"'+x+'\\\"), MAX(\\\"'+x+'\\\"), COUNT(\\\"'+x+'\\\"), COUNT(DISTINCT \\\"'+x+'\\\")';
        
    }
    query += " FROM "+tablename;
    
    var query_params = {
        FunctionName: 'sgx-app-api-run-athena-query', // the lambda function we are going to invoke
        Payload: '{ "identity" : "'+identity+'", "query" : "'+query+'" }'
    };
    //context.succeed(query);
    let query_results = await LambdaFunction.callLambdaFunction(query_params);
    let out = JSON.parse(query_results.Payload)[1]["Data"];
    let fields = [];
    for(let i in results) {
        let b0 = i*4, b1 = i*4+1, b2 = i*4+2, b3 = i*4+3;
        let format = {"fieldname": results[i]["Name"], 
                    "type": results[i]["Type"], 
                    "min": out[b0]["VarCharValue"], 
                    "max": out[b1]["VarCharValue"],
                    "count": out[b2]["VarCharValue"],
                    "distinct": out[b3]["VarCharValue"]};
        fields.push(format);
    }
    
    var s3_params = {
      Body: JSON.stringify(fields), 
      Bucket: "sgx-app-user-storage", 
      Key: "private/"+identity+"/"+directory+"/"+directory+".json"
     };
     let id = await s3.putObject(s3_params, function(err, data) {
       if (err) console.log(err, err.stack); // an error occurred
       else     console.log(data);           // successful response
     }).promise();
    
    //context.succeed(id);
    
    // TODO implement
    const response = {
        statusCode: 200,
        headers: {
            "Access-Control-Allow-Headers" : "Content-Type",
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "OPTIONS,POST"
        },
        body: JSON.stringify(id),
    };
    return response;
};

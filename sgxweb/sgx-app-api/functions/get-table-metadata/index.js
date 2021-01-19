const AWS = require('aws-sdk');

const athena = new AWS.Athena();

// get reference to S3 client
exports.handler = async (event, context) => {

    const identity = event.identity;
    const tablename = event.tablename;
    //const identity = "us-east-1:5194b9e1-18dc-43c5-83e5-4160861a7a68";
    //const tablename = 'dal_900_data_file';
    
    var params = {
        CatalogName: 'awsdatacatalog', /* required */
        DatabaseName: identity, /* required */
        TableName: tablename /* required */
    };
    
    let data = await athena.getTableMetadata(params, function(err, data) {
      if (err) console.log(err, err.stack); // an error occurred
      else     console.log(data);           // successful response
    }).promise();
    
    // TODO implement
    /*const response = {
        statusCode: 200,
        headers: {
            "Access-Control-Allow-Headers" : "Content-Type",
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "OPTIONS,POST"
        },
        body: JSON.stringify(data),
    };*/
    context.succeed(data);
    //return response;
};

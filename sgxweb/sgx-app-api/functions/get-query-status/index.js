const AWS = require('aws-sdk');
const athena = new AWS.Athena();

exports.handler = async (event, context) => {
    
    const queryId = JSON.parse(event).queryId;
    //const queryId = "f24a598b-c51b-46ab-83a2-0bff6a63a02d";
    
    const params = {
        QueryExecutionId: queryId /* required */
    };
    let data = await athena.getQueryExecution(params, function(err, data) {
        if (err) console.log(err, err.stack); // an error occurred
        else     console.log(data);           // successful response
    }).promise();
    
    //context.succeed(data.QueryExecution.Status.State);
    // TODO implement
    const response = {
        statusCode: 200,
        body: JSON.stringify(data.QueryExecution.Status.State),
    };
    return response;
};


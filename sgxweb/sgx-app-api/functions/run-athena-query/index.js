const AWS = require('aws-sdk');
const AthenaQuery = require('./athenaQuery');

exports.handler = async (event, context) => {

    const identity = event.identity;
    const query = event.query;
    const output = 's3://sgx-athena-query-output-log/'+identity+'/';
    //const identity = "us-east-1:d7424e02-fae8-449f-af16-39c4bbb27456";
    //const q = 'SELECT MIN(year), MIN(onetest), MIN(race), MIN(gender), MIN(degree), MIN(major) FROM students';
    
    let result = await AthenaQuery.createQueryExecutionId(query, identity, output);
    let queryId = result.QueryExecutionId;
    let check = await AthenaQuery.checkQueryCreateStatus(queryId);
    let status = check.QueryExecution.Status.State;
    while(status !== "SUCCEEDED") {
        check = await AthenaQuery.checkQueryCreateStatus(queryId);
        status = check.QueryExecution.Status.State;
    }
    let results = await AthenaQuery.getQueryResultByExecutionId(queryId);
    await AthenaQuery.stopQueryExecutionId(queryId);
    
    let body = results.ResultSet.Rows;
    context.succeed(body);
    
    // TODO implement
    /*const response = {
        statusCode: 200,
        headers: {
            "Access-Control-Allow-Headers" : "Content-Type",
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "OPTIONS,POST"
        },
        body: JSON.stringify(body),
    };
    return response;*/
};

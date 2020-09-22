const AWS = require('aws-sdk');
const athena = new AWS.Athena();

/**
 * @description createQueryExecutionId, execute for generating queryExecutionId
 * @param {Function} callback
 */
function createQueryExecutionId(query, db, output){
    /**doing resultConfiguration, but we will not save query result there. */
    const params = {
        QueryString: query, /* required */
        QueryExecutionContext: {
            Database: db
        },
        ResultConfiguration: { /* required */
            OutputLocation: output, /* required */
            EncryptionConfiguration: {
                EncryptionOption: 'SSE_S3', /* required */
            }
        }
    };
    return athena.startQueryExecution(params, function(err, data) {
        if(!err) return data;
    }).promise();
}
/**
 * @description checkQueryCreateStatus, check query status till it is not active.
 */
function checkQueryCreateStatus(queryId){
    const params = {
        QueryExecutionId: queryId /* required */
    };
    return athena.getQueryExecution(params, function(err, data) {
        if (err) return false; // an error occurred
        else{
            if(data && data.QueryExecution && data.QueryExecution.Status && data.QueryExecution.Status.State && data.QueryExecution.Status.State === 'RUNNING'){
                console.log("Athena Query status is running");
                return data;
            }
            else{
                console.log("Atehna Query status is Active");
                return data;
            }
        }
    }).promise();
}
/**
 * @description getQueryResultByExecutionId, execute for generating result based on queryExecutionId
 * @param {String} queryExecutionId
 * @param {Function} callback
 */
function getQueryResultByExecutionId(queryExecutionId){
    const params = {
        QueryExecutionId: queryExecutionId
    };
    return athena.getQueryResults(params, function(err, data) {
        if(!err)
            return data;
    }).promise();
}

/**
 * @description stopQueryExecutionId, execute for stop queryExecutionId
 * @param {String} queryExecutionId
 * @param {Function} callback
 */
function stopQueryExecutionId(queryExecutionId){
    const params = {
        QueryExecutionId: queryExecutionId
    };
    return athena.stopQueryExecution(params, function(err, data) {
        if(!err)
            return true;
        else
            return false;
    }).promise();
}

module.exports = {
    createQueryExecutionId: createQueryExecutionId,
    checkQueryCreateStatus: checkQueryCreateStatus,
    getQueryResultByExecutionId: getQueryResultByExecutionId,
    stopQueryExecutionId: stopQueryExecutionId
};
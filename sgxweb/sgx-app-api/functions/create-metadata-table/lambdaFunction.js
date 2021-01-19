const AWS = require('aws-sdk');
const lambda = new AWS.Lambda();

/**
 * @description callLambdaFunction
 * @param {Object} params
 * @param {Function} callback
 */
function callLambdaFunction(params){
    
    return lambda.invoke(params, function(err, data) {
        if (err) {
          console.log(err);
        } else {
          console.log(data.Payload);
        }
    }).promise();
}

module.exports = {
    callLambdaFunction: callLambdaFunction
};
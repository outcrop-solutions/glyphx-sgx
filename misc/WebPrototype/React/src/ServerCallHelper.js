
//const serverAddress = "ec2-35-162-196-131.us-west-2.compute.amazonaws.com:5001";
const serverPort = 5001;
const serverAddress = "http://" + window.location.hostname +":"+ serverPort + "/";

/**
 * This function makes a server call and returns the data returned from the server
 * @param {string} url 
 * @param {object} options 
 */
export function makeServerCall(url,callback,options){
    var returnObj=null;

    if(url == null || typeof callback != 'function')
    {
        console.error('Please provide complete parameters!');
        return false;
    }
        

    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
        {
            callback(xmlHttp.responseText,options);
        }
            
    }
    xmlHttp.open("GET", serverAddress+url, true); // true for asynchronous 
    xmlHttp.send(null);

    return true;
}
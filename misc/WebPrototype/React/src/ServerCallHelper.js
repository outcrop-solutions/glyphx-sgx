
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
};

export function setCookie(cname, cvalue, exdays) {
    var d = new Date();
    d.setTime(d.getTime() + (exdays*24*60*60*1000));
    var expires = "expires="+ d.toUTCString();
    document.cookie = cname + "=" + cvalue + ";" + expires;
};

export function getCookie(cname) {
    var name = cname + "=";
    var decodedCookie = decodeURIComponent(document.cookie);
    var ca = decodedCookie.split(';');
    for(var i = 0; i <ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
};

export function getLoginCookieName() {
    return 'loggedInUser';
};

export function deleteCookie(cname) {
    document.cookie = cname + '=;expires=Thu, 01 Jan 1970 00:00:01 GMT;';
};
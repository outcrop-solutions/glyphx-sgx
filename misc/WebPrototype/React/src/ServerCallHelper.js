
const serverAddress = window.APP_MODE == "DEVELOPMENT" ? "" : "http://ec2-35-162-196-131.us-west-2.compute.amazonaws.com:5000" ;

/**
 * This function makes a server call and returns the data returned from the server
 * @param {string} url 
 * @param {object} options 
 */
export function makeServerCall(url,callback,options){
    var returnObj=null;
    var saddress = serverAddress;

    if(url == null || (callback && typeof callback != 'function'))
    {
        console.error('Please provide complete parameters!');
        return false;
    }
        
    if(options && options.post)
    {
        saddress = saddress+url;
        httpPostRequest(saddress,callback,options);
    }
    else{
        saddress = saddress+url;
        httpGetRequest(saddress,callback,options);
    }
    

    return true;
};

export function httpGetRequest(saddress,callback,options){
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
        {
            if(typeof callback == 'function')
                callback(xmlHttp.responseText,options);
        }
        else if(xmlHttp.status == 500 && options && options.onServerCallError && typeof options.onServerCallError == 'function'){
            options.onServerCallError();
        }
    }
    xmlHttp.open("GET", saddress, true); // true for asynchronous 
    xmlHttp.send(null);
}

function httpPostRequest(saddress,callback,options){
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
        {
            if(typeof callback == 'function')
                callback(xmlHttp.responseText,options);
        }
        else if(xmlHttp.status == 500 && options && options.onServerCallError && typeof options.onServerCallError == 'function'){
            options.onServerCallError();
        }
    }
    xmlHttp.open("POST", saddress, "/json-handler", true);
    xmlHttp.setRequestHeader("Content-Type", "application/json");
    xmlHttp.send(JSON.stringify(options.data));

}

export function checkUserLoggedIn(onServerError){
    var xmlHttp = new XMLHttpRequest();
    var url = serverAddress + "isUserLoggedIn";
    xmlHttp.open("GET", url, false); // true for asynchronous 
    xmlHttp.send(null);
    if (xmlHttp.status === 200) {
        console.log(xmlHttp.responseText);
        return xmlHttp.responseText;
    }

    if(typeof onServerError == 'function')
        onServerError();
}
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
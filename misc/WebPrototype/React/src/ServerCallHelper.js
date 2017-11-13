
const serverAddress = window.APP_MODE == "DEVELOPMENT" ? "" : window.SERVER_URL;


/**
 * This function makes a server call and returns the data returned from the server
 * @param url: -ADCMT
 * @param callback: -ADCMT
 * @param options: -ADCMT
 */
export function makeServerCall(url, callback, options) {
    var saddress = serverAddress;

    if (url == null || (callback && typeof callback != 'function')) {
        console.error('Please provide complete parameters!');
        return false;
    }
        
    if (options && options.post) {
        saddress = saddress+url;
        httpPostRequest(saddress, callback, options);
    }
    else {
        saddress = saddress+url;
        httpGetRequest(saddress, callback, options);
    }

    return true;
};


/**
 * -ADCMT
 * @param saddress: -ADCMT
 * @param callback: -ADCMT
 * @param options: -ADCMT
 */
export function httpGetRequest (saddress, callback, options) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.withCredentials = true;

    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
            if (typeof callback == 'function') {
                callback(xmlHttp.responseText, options);
            }
        }

        else if (xmlHttp.status == 500 && options && options.onServerCallError && typeof options.onServerCallError == 'function') {
            options.onServerCallError();
        }
    }

    // True for asynchronous 
    xmlHttp.open("GET", saddress, true);
    xmlHttp.send(null);
}


/**
 * -ADCMT
 * @param saddress: -ADCMT
 * @param callback: -ADCMT
 * @param options: -ADCMT
 */
function httpPostRequest(saddress, callback, options) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.withCredentials = true;

    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
            if (typeof callback == 'function') {
                callback(xmlHttp.responseText, options);
            }
        }

        else if (xmlHttp.status == 500 && options && options.onServerCallError && typeof options.onServerCallError == 'function') {
            options.onServerCallError();
        }
    }

    xmlHttp.open("POST", saddress, "/json-handler", true);
    xmlHttp.setRequestHeader("Content-Type", "application/json");
    xmlHttp.send(JSON.stringify(options.data));
}


/**
 * -ADCMT
 * @param onServerError: -ADCMT
 */
export function checkUserLoggedIn(onServerError) {
    var xmlHttp = new XMLHttpRequest();
    var url = serverAddress + "isUserLoggedIn";

    // True for asynchronous 
    xmlHttp.open("GET", url, false); 
    xmlHttp.withCredentials = true;
    xmlHttp.send(null);

    if (xmlHttp.status === 200) {
        console.log(xmlHttp.responseText);
        return xmlHttp.responseText;
    }

    if (typeof onServerError == 'function') {
        onServerError();
    }
}


/**
 * -ADCMT
 * @param cname: -ADCMT
 * @param cvalue: -ADCMT
 * @param exdays: -ADCMT
 */
export function setCookie(cname, cvalue, exdays) {
    var d = new Date();
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000));
    var expires = "expires="+ d.toUTCString();
    document.cookie = cname + "=" + cvalue + ";" + expires;
};


/**
 * -ADCMT
 * @param cname: -ADCMT
 */
export function getCookie(cname) {
    var name = cname + "=";
    var decodedCookie = decodeURIComponent(document.cookie);
    var ca = decodedCookie.split(';');
    for (var i = 0; i <ca.length; i++) {
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


/**
 * -ADCMT
 */
export function getLoginCookieName() {
    return 'loggedInUser';
};


/**
 * -ADCMT
 * @param cname: -ADCMT
 */
export function deleteCookie(cname) {
    document.cookie = cname + '=;expires=Thu, 01 Jan 1970 00:00:01 GMT;';
};
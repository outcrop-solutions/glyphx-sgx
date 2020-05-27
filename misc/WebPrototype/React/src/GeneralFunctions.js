//General unique id generator used for various aspects such as annoucements.
export function guidGenerator() {
    var S4 = function() {
       return (((1+Math.random())*0x10000)|0).toString(16).substring(1);
    };
    return ("_" + S4() + S4() + "-" + S4() + "-" + S4() + "-" + S4() + "-" + S4() + S4() + S4());
}

//Specific unique id generator for cookies to establish each unique session for desktop-side.
export function cookGenerator() {
    let S4 = function(){
        return (((1+Math.random())*0x10000)|0).toString(16).substring(1);
    }
    let S4_UP = function(){
        return S4().toUpperCase();
    }

    let toggle = function(){
        return Math.round(Math.random()) === 0 ? S4() : S4_UP();
    }

    toggle()
    return (toggle() + "-" + toggle() + toggle() + toggle() + toggle() + toggle() + toggle() + toggle() + toggle());
}

//listing all cookies in a webpage
export function listCookies(name){
    let pattern = RegExp(name + "=.[^;]*");
    let matched = document.cookie.match(pattern);
    if(matched){
        let cookie = matched[0].split('=');
        return cookie[1];
    }
    return false;
}

//websocket connection options
export function webSocketSend(webSocket, uid, type){
    if(uid){
        if(type === "tutorial"){
            webSocket.send(JSON.stringify({
                url_uid: uid,
                open_url: "https://glyphed.s3.amazonaws.com/tutorials/home.html"}));
        }
        // else if(type === "save-viz"){
        //     webSocket.send(JSON.stringify({
        //         url_uid: uid,
        //         save_viz: true 
        //     }));
        // }
        // else if(type === "save-as-viz"){
        //     webSocket.send(JSON.stringify({
        //         url_uid: uid,
        //         save_as_viz: true 
        //     }));
        // }
        else if(type === "view-stats"){
            webSocket.send(JSON.stringify({
                url_uid: uid,
                view_stats: true 
            }));
        }
        else if(type === "view-selected"){
            webSocket.send(JSON.stringify({
                url_uid: uid,
                view_selected: true 
            }));
        }
        else if(type === "view-filtered"){
            webSocket.send(JSON.stringify({
                url_uid: uid,
                view_filtered: true 
            }));
        }
        else if(type === "logout"){
            webSocket.send(JSON.stringify({
                url_uid: uid,
                logout: true
            }));
        }
        else if(type === "home"){
            webSocket.send(JSON.stringify({
                url_uid: uid, 
                home: true}))
        }
        else if(type === "collapse_sidenav"){
            webSocket.send(JSON.stringify({
                url_uid: uid, 
                collapse_sidenav: true}))
        }
        else if(type === "settings_modal"){
            webSocket.send(JSON.stringify({
                url_uid: uid, 
                settings_modal: true}))
        }
        else if(type === "help"){
            webSocket.send(JSON.stringify({
                url_uid: uid, 
                open_url: "https://desk.zoho.com/portal/synglyphx/home"}))
        }
        else if(type === "expand_sidenav"){
            webSocket.send(JSON.stringify({
                url_uid: uid, 
                expand_sidenav: true}))
        }
    }
}
import React from 'react';
import { deleteCookie, getLoginCookieName, makeServerCall } from './ServerCallHelper.js';
import { hideSplashScreen } from './LoadMaskHelper.js';
import Dialog from 'material-ui/Dialog';


/**
 * Performs the logout and displays the logout page
 **/
class Logout extends React.Component {


    /**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {

    }


    deleteLoginCookie(){
        deleteCookie(getLoginCookieName());
    }


    render() {
        this.deleteLoginCookie();
        hideSplashScreen();
        makeServerCall("logout",
            function (responseText) { 
                window.location.assign("/");
            }
        );
        //makeServerCall("logout");

        return(
            <div style = {{ height: "100vh", width: "100vw", backgroundColor: "#fff" }} />
        );
    }
}

export default Logout;
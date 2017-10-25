import React from 'react';
import { deleteCookie, getLoginCookieName, makeServerCall } from './ServerCallHelper.js';
import { hideSplashScreen } from './LoadMaskHelper.js';
import Dialog from 'material-ui/Dialog';


/**
 * Performs the logout and displays the logout page
 **/
class Logout extends React.Component {

    deleteLoginCookie(){
        deleteCookie(getLoginCookieName());
    }

    render() {
        this.deleteLoginCookie();
        hideSplashScreen();
        makeServerCall("logout");
        
        return(
            <Dialog
                actions = {
                    [    ]
                }
                overlayStyle = {{ backgroundColor: 'white' }}
                contentStyle = {{ width:'30%', maxWidth: "none" }}
                modal = { true }
                open = { true }
            >   
                <center>
                    <label style = {{ height: '20px', fontSize: '18px' }} > 
                        You have been logged out.
                    </label>

                    <br/>

                    <label style = {{  height: '20px', fontSize: '18px' }} > 
                        <a href = "./login"> Click here to login </a>.
                    </label>
                </center>  
            </Dialog>
        );
    }
}

export default Logout;
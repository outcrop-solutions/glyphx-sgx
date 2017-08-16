import React from 'react';
import Dialog from 'material-ui/Dialog';
import {deleteCookie,getLoginCookieName} from './ServerCallHelper.js';
import {hideSplashScreen} from './LoadMaskHelper.js';

/**
 * No Props
 **/
class Logout extends React.Component {

    deleteLoginCookie(){
        //delete the cookie.
        deleteCookie(getLoginCookieName());
    }

    render() {
        this.deleteLoginCookie();
        hideSplashScreen();

        return(
            <Dialog
                actions = {
                    [    ]
                }
                overlayStyle = {{ backgroundColor: 'white' }}
                contentStyle = {{ width:'20%', maxWidth: "none" }}
                modal = { true }
                open = { true }
            >   
                <center>
                    <label  
                        style = {{ 
                            height: '20px',
                            fontSize: '18px'
                        }}
                    > You have been logged out.
                    </label>
                    <br/>
                    <label  
                        style = {{ 
                            height: '20px',
                            fontSize: '18px'
                        }}
                    > Please Click here to <a href="./login">login </a>.
                    </label>
                </center>  
            </Dialog>
        );
    }
}

export default Logout;
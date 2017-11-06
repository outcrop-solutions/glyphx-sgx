import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { hideSplashScreen, showLoadMask, hideLoadMask } from './LoadMaskHelper.js';
import { makeServerCall } from './ServerCallHelper.js';
import Dialog from 'material-ui/Dialog';
import Flexbox from 'flexbox-react';
import RaisedButton from 'material-ui/RaisedButton';
import IconButton from 'material-ui/IconButton';
import Snackbar from 'material-ui/Snackbar';
import Paper from 'material-ui/FontIcon';
import './General.css';


function hideHelpChat() {
    if (document.getElementById("livechat-compact-container")) {
        document.getElementById("livechat-compact-container").style.display = "none";
    }
}

window.onload = hideHelpChat;

class Login extends React.Component {

    state = {
        loginVisible: true,
        loginButtonBottomTranslateCalc: 0,
        openForgotPasswordDialog: false,
        snackbar: false
    }


    /**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {

        // Initial stuff for positioning the login button
        hideSplashScreen();
        this.calcLoginButtonPosition();

        var context = this;

        // Open the modal so it transitions up as soon as you land
        setTimeout(function () {
            context.toggleLoginForm(context.state.loginButtonBottomTranslateCalc);
        }, 1000);

        // Add event listeners for using the enter key to login
        document.getElementById("UserText").addEventListener("keyup", this.enterKeyToLogin.bind(context));
        document.getElementById("PassText").addEventListener("keyup", this.enterKeyToLogin.bind(context));

        if (document.getElementById("livechat-compact-container")) {
            document.getElementById("livechat-compact-container").style.display = "none";
        }
    }


    /**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {

        // Remove event listeners for using the enter key to login
        document.getElementById("UserText").removeEventListener("keyup", this.enterKeyToLogin);
        document.getElementById("PassText").removeEventListener("keyup", this.enterKeyToLogin);
	}



    /**
	 * Calls the code to login when the enter key is pressed
     * @param e: event instance which contains information about what caused the event
	 */
    enterKeyToLogin(e) {
        e.preventDefault();
        if (e.keyCode === 13) {
            this.buttonClick();
        }
    }


    /**
     * Caculates the position of the login button to be initially.
     **/
    calcLoginButtonPosition() {
        var ycalc = 0;
        var docHeight = document.body.offsetHeight;

        var loginButton = document.getElementById('loginButton');
        var loginButtonBottom = loginButton.getBoundingClientRect().bottom;
        
        //var translate = 'translate(0px,' + ycalc + 'px)';
        ycalc = docHeight - loginButtonBottom - 65;

        this.setState({ loginButtonBottomTranslateCalc: ycalc });

        this.toggleLoginForm(ycalc);
    }


    /**
     * Redirects to a target path
     * @param str: target path
     **/
    navigate = (str) => {
        this.props.history.push(str);
    }


    /**
     * -ADCMT
     * @param evt: -ADCMT
     * @param context: -ADCMT
     **/
    authenticate = (evt, context) => {
        var username = document.getElementById('UserText').value;
        var password = document.getElementById('PassText').value;
        var url = 'login?username=' + username + "&password=" + password;
        showLoadMask();
        
        var lblErrPass = document.getElementById('errPass');
        lblErrPass.innerText = "";
        lblErrPass.hidden = true;
        
        // Server call to check user/pass and update state.
        makeServerCall(url, context.onServerResponse, {onServerCallError: context.showMaintanencePage} );
    }


    /**
     * Reroutes to maintanance page (not redundant, used for "makeServerCall" in authenticate)
     **/
    showMaintanencePage = () => {
        this.navigate('/maintenance');
    };


    /**
     * -ADCMT
     * @param response: -ADCMT
     * @param options: -ADCMT
     **/
    onServerResponse = (response, options) => {
        var result; 
        
        try {
            result = JSON.parse(response);
        }
        catch(e) {
            result = null;
        }
        
        var lblErrPass = document.getElementById('errPass');

        if (result && result.status == 'success') { 
            // Save the details to store
            if (result.userInformation) {
                result.userInformation.loggedInTime = new Date();
                result.userInformation.idleTime = 0;
            }

            console.log(result);
            this.saveUserInfo(result.userInformation, result.funnelInfo);
        }

        else if (result && result.status == "failure") {
            console.log('Error');
            lblErrPass.hidden = false;
            lblErrPass.innerText = "Incorrect Username/Password";
        }

        else {
            this.navigate("/maintenance");
        }

        hideLoadMask();
    }


    /**
     * -ADCMT
     * @param userInfo: -ADCMT
     * @param funnelInfo: -ADCMT
     **/
    saveUserInfo = (userInfo, funnelInfo) => {
        console.log('Success');
        this.setState({ openPassword: false });
        this.props.dispatch(saveUserInfo(userInfo, funnelInfo));

        // Call function post login if provided.
        if (typeof this.props.doAfterLogin == 'function') {
            this.props.doAfterLogin(userInfo);
        }
        
        // Login cookie
        //setCookie(getLoginCookieName(), 1,0.5);

        // Redirect to home page.
        this.navigate("/home");
    }


    /**
     * -ADCMT
     * @param ycalc: -ADCMT
     **/
    toggleLoginForm = (ycalc) => {
        var loginForm = document.getElementById('loginForm');
        var loginOverlay = document.getElementById('loginOverlay');
        var elements = document.getElementsByClassName('loginFormOtherElements');
        var userText = document.getElementById('UserText');
        var visibility = '';

        if (this.state.loginVisible) {
            loginForm.style.transform = "translate(0px, " + ycalc + "px)";
           
            // Hide other elements
            visibility = 'hidden';
            loginOverlay.classList.remove('loginOverlayCSS');
            loginForm.style.backgroundColor = "transparent";
        }

        else {
            loginForm.style.transform = "translate(0px, 0px)";
            loginOverlay.classList.add('loginOverlayCSS');
            
            // Show other elements
            visibility = '';
            loginForm.style.backgroundColor = 'rgba(97, 97, 97, 0.5)';
        }

        for (var index = 0; index < elements.length; index++) {
            elements[index].style.visibility = visibility;
        }

        userText.focus();
        this.setState({ loginVisible: !this.state.loginVisible });
    }


    /**
     * -ADCMT
     * @param evt: -ADCMT
     **/
    buttonClick = (evt) => {
        if (this.state.loginVisible) {
            this.authenticate(evt, this);
        }
        
        else {
            this.toggleLoginForm(this.state.loginButtonBottomTranslateCalc);
        }
    }


    /**
     * Closes the forgot password modal through the local state
     **/
    handleCloseForgotPassDailog = () => {
        this.setState({ openForgotPasswordDialog: false, snackbar: false });
    };


    /**
     * -ADCMT
     **/
    handleForgotPassSubmit = () => {
        var email = document.getElementById('ForgotUserText') ? document.getElementById('ForgotUserText').value : null;
        console.log(email);
        this.handleCloseForgotPassDailog();

        // Send server call to email password recovery link.
        this.setState({ snackbar: true });
    }


    render() {
        return (
            <Flexbox style = {{ width: '100%', height: '100%' }} >
                <video playsInline autoPlay loop muted 
                    poster = "./Res/Img/synglyphx_bio.png" 
                    id = "bgvid"
                    onClick = { (evt) => this.toggleLoginForm(this.state.loginButtonBottomTranslateCalc) }
                >
                    {/*<source src="./Res/Vid/GlyphEd.webm" type="video/webm" />*/}
                    <source src = "./Res/Vid/GlyphEd.mp4" type = "video/mp4" />
                    Your browser does not support the video tag.
                </video>
                
                <div id = "loginOverlay" style = {{ width: '100%', height: '100%', display: 'table', overflow: 'hidden' }} >
                    <center style = {{ display: 'table-cell', verticalAlign: 'middle' }} >

                        <Paper  id = "loginForm" style = {{ backgroundColor: 'rgba(75, 38, 38, 0.50)', width: '310px', height: '327px', borderRadius: "3px"}} >

                            <div className = "loginFormOtherElements" style = {{ textAlign: 'center', paddingBottom: '0px'}} >
                                <img src = "./Res/Img/synglyphx-wht-3.png" alt = "SynGlyphX" />
                            </div>

                            <br/>
                            
                            <Flexbox 
                                flexDirection = "row" 
                                className = "loginFormOtherElements"
                                style = {{
                                    width: "80%",
                                    borderColor: "#d9d9d9 #ccc #b3b3b3",
                                    borderRadius: '5px',
                                    border: "1px solid #ccc",
                                    height: "40px",
                                    overflow: 'hidden',
                                    backgroundColor: "white"
                                }}
                            >
                                <div style = {{ width: "36px" }} >
                                    <i 
                                        className = "fa fa-user" 
                                        style = {{
                                            fontSize: '2.25rem',
                                            backgroundColor: 'white',
                                        }}
                                    />
                                </div>

                                <input 
                                    type = "text"
                                    id = "UserText"
                                    style = {{
                                        border: "none",
                                        height: 'inherit',
                                        width: '90%',
                                        borderLeft: '1px #b3b3b3 solid',
                                        float: 'right',
                                        outlineWidth: '0',
                                        fontSize: '23px',
                                        paddingLeft: "10px"
                                    }}
                                />
                            </Flexbox>

                            <br/>

                            <Flexbox 
                                flexDirection = "row" 
                                className = "loginFormOtherElements"
                                style = {{
                                    width: "80%",
                                    borderColor: "#d9d9d9 #ccc #b3b3b3",
                                    borderRadius: '5px',
                                    border: "1px solid #ccc",
                                    height: "40px",
                                    overflow: 'hidden',
                                    backgroundColor: "white"
                                }}
                            >

                                <div style = {{ width: "36px" }} >
                                    <i 
                                        className = "fa fa-lock" 
                                        style = {{
                                            fontSize: '2.25rem',
                                            backgroundColor: 'white',
                                        }}
                                    />
                                </div>

                                <input 
                                    type = "password"
                                    id = "PassText"
                                    style = {{
                                        border: "none",
                                        height: 'inherit',
                                        borderLeft: '1px #b3b3b3 solid',
                                        width: '90%',
                                        float: 'right',
                                        outlineWidth: '0',
                                        fontSize: '23px',
                                        paddingLeft: "10px"
                                    }}
                                />
                            </Flexbox>
                            
                            <label className = "loginFormOtherElements" id = "errPass" style = {{ color: 'red' }}  />

                            <br/>

                            <RaisedButton
                                id = "loginButton"
                                label = "Login"
                                secondary = { true }
                                onClick = { (evt) => this.buttonClick(evt, this) }
                                buttonStyle = {{ backgroundColor: this.props.settings.colors.buttons.general }}
                                labelStyle = {{ fontSize: '20px' }}
                                style = {{ 
                                    position: 'relative', 
                                    width: "65%",
                                    display: 'block',
                                    borderRadius: '3px',
                                    color: this.props.settings.colors.settingsModalColor.saveButton 
                                }}
                            />

                            <br />

                            <label className = "loginFormOtherElements" id = "forgotPass" style = {{ fontSize: '16px' }}>
                                <a onClick = { () => this.setState({ openForgotPasswordDialog: true }) } style = {{ color: '#b3b3b3', cursor: 'pointer' }} ><u> forgot password? </u></a>
                            </label>

                            <br />

                            <IconButton 
                                iconClassName = "fa fa-arrow-circle-down" 
                                iconStyle = {{ color: 'white' }}
                                onClick = { (evt) => this.toggleLoginForm(this.state.loginButtonBottomTranslateCalc) }
                            />
                        </ Paper>
                    </ center>
                    <br/>
                    
                    <Dialog
                        title = "Forgot Password?"
                        modal = { true }
                        open = { this.state.openForgotPasswordDialog }
                        onRequestClose = { this.handleCloseForgotPassDailog }
                        actions = {[
                            <RaisedButton
                                label = "Submit"
                                primary = { true }
                                onClick = { this.handleForgotPassSubmit }
                            />,
                            <span> &nbsp; </span>,
                            <RaisedButton
                                label = "Cancel"
                                primary = { true }
                                onClick = { this.handleCloseForgotPassDailog }
                            />
                        ]}
                    >
                        Please Enter Your Email address associated with the account. <br/>
                        <input 
                            type = "text"
                            id = "ForgotUserText"
                            style = {{ height: 'inherit', border: '1px #b3b3b3 solid', outlineWidth: '0', fontSize: '20px' }}
                        />
                    </Dialog>

                    <Snackbar
                        open = { this.state.snackbar }
                        message = "A link has been sent to your email address to reset the password."
                        autoHideDuration = { 5000 }
                        onRequestClose = { this.handleCloseForgotPassDailog }
                    />
                </div>
            </Flexbox>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const saveUserInfo = (userInfo, funnelInfo) => ({
    type: 'SAVE_USER_INFO',
    userInfo,
    funnelInfo
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(Login));
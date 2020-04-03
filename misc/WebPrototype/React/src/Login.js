/*eslint-env jquery*/
import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { hideSplashScreen, /* showLoadMask, */ hideLoadMask } from './LoadMaskHelper.js';
import { makeServerCall } from './ServerCallHelper.js';
// import Dialog from 'material-ui/Dialog';
import Flexbox from 'flexbox-react';
// import RaisedButton from 'material-ui/RaisedButton';
// import IconButton from 'material-ui/IconButton';
// import Snackbar from 'material-ui/Snackbar';
// import Paper from 'material-ui/FontIcon';
import Auth0Lock from 'auth0-lock';
import './css/General.css';


class Login extends React.Component {

    state = {
        loginVisible: true,
        loginButtonBottomTranslateCalc: 0,
        openForgotPasswordDialog: false,
        openTermsAndConditionsDialog: false,
        snackbar: false
    }


    /**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {
        // function listCookies() {
        //     var theCookies = document.cookie.split(';');
        //     var aString = '';
        //     for (var i = 1 ; i <= theCookies.length; i++) {
        //         aString += i + ' ' + theCookies[i-1] + "\n";
        //     }
        //     return aString;
        // }
        // console.log('COOKIES: ',listCookies())
        // console.log(encodeURI("e755b8411d20f7fef458f43afe5eef07f7a44d32071a17df867028236d9e7a48"));
        
        if (window.location.href.indexOf("http://") !== -1 && window.location.href.indexOf("localhost") === -1 
        && window.location.href.indexOf("ec2-34-215-50-82") === -1 && window.location.href.indexOf("ec2-34-221-39-241") === -1
         && window.location.href.indexOf("ec2-52-41-239-60") === -1) {
            window.location.href = window.location.href.replace("http://", "https://");
        }

        // Initial stuff for positioning the login button
        hideSplashScreen();

        //makes sure to display zoho dialog bubble
        window.setTimeout(() => {if(document.getElementById('titlediv')){
            $('#titlediv').css('display', 'block');
        }}, 1000);
        // this.calcLoginButtonPosition();

        // var context = this;

        // Open the modal so it transitions up as soon as you land
        // setTimeout(function () {
        //     context.toggleLoginForm(context.state.loginButtonBottomTranslateCalc);
        // }, 500);

        // Add event listeners for using the enter key to login
        // document.getElementById("UserText").addEventListener("keyup", this.enterKeyToLogin.bind(context));
        // document.getElementById("PassText").addEventListener("keyup", this.enterKeyToLogin.bind(context));

        // document.getElementById("UserText").addEventListener('keyup', this.capsLockCheck);
        // document.getElementById("PassText").addEventListener('keyup', this.capsLockCheck);
        // document.getElementById("UserText").addEventListener('mousedown', this.capsLockCheck);
        // document.getElementById("PassText").addEventListener('mousedown', this.capsLockCheck);
        let global_state = this;
        this.lock = new Auth0Lock(
            'tEK0nmyA6Dj84oTEyYJw15FBl3X4BWd2',
            'synglyphxinside.auth0.com',
            {
                // allowedConnections: ["Username-Password-Authentication"/* ,"google-oauth2" */],
                autofocus: true,
                allowSignUp: false,
                rememberLastLogin: false,
                socialButtonStyle: "big",
                languageDictionary: { success: {logIn: 'Successful! Logging in to GlyphEd...'}, "title":""},
                allowShowPassword: true,
                closable: false,
                auth: {redirect : false},
                avatar: null,
                theme: {
                    primaryColor: "#3A99D8",
                    logo: './Res/Img/sgx_inside.png',
                }
            }
        );

        this.lock.on("authenticated", function(authResult) {
            console.log(authResult, global_state)
            // if(authResult.accessToken){
            // }
            // Use the token in authResult to getUserInfo() and save it if necessary
            this.getUserInfo(authResult.accessToken, function(error, user) {
              if (error) {
                // Handle error
                console.log('ERROR WITH GET USER INFO', error)
                return;
              }
              else {
                  console.log(user);
                  window.setTimeout(() => {
                    makeServerCall('loginThree', global_state.onServerResponse, 
                        {
                            post: true,
                            onServerCallError: global_state.showMaintanencePage,
                            data: {
                                user_email: user.email,
                                token: authResult.accessToken
                            }
                        }
                    );

                }, 1800);
              }
            });
      
              //we recommend not storing Access Tokens unless absolutely necessary
            //   wm.set(privateStore, {
            //     accessToken: authResult.accessToken
            //   });
      
            //   wm.set(privateStore, {
            //     profile: profile
            //   });
      
            // });
        });
        
        this.lock.show();

        document.title = "GlyphEd - Login";
    }


    /**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {

        // Remove event listeners for using the enter key to login
        // document.getElementById("UserText").removeEventListener("keyup", this.enterKeyToLogin);
        // document.getElementById("PassText").removeEventListener("keyup", this.enterKeyToLogin);

        // document.getElementById("UserText").removeEventListener('keyup', this.capsLockCheck);
        // document.getElementById("PassText").removeEventListener('keyup', this.capsLockCheck);
        // document.getElementById("UserText").removeEventListener('mousedown', this.capsLockCheck);
        // document.getElementById("PassText").removeEventListener('mousedown', this.capsLockCheck);
        this.lock.hide();
	}


    /**
     * Checks for Caps Lock when logging in
     * @param e event instance 
     */

    // capsLockCheck(e) {
    //     let caps_lock_on = e.getModifierState('CapsLock');
    
    //     if(caps_lock_on) $('#caps_lock_ux').css('display', 'block');
    //     else if(!caps_lock_on) $('#caps_lock_ux').css('display', 'none');
    // }

    /**
	 * Calls the code to login when the enter key is pressed
     * @param e: event instance which contains information about what caused the event
	 */
    // enterKeyToLogin(e) {
    //     e.preventDefault();
    //     if (e.keyCode === 13) {
    //         this.buttonClick();
    //     }
    // }


    /**
     * Caculates the position of the login button to be initially.
     **/
    // calcLoginButtonPosition() {
    //     var ycalc = 0;
    //     var docHeight = document.body.offsetHeight;

    //     var loginButton = document.getElementById('loginButton');
    //     var loginButtonBottom = loginButton.getBoundingClientRect().bottom;
        
    //     //var translate = 'translate(0px,' + ycalc + 'px)';
    //     ycalc = docHeight - loginButtonBottom - 65;

    //     this.setState({ loginButtonBottomTranslateCalc: ycalc });

    //     this.toggleLoginForm(ycalc);
    // }

    /**
     * -ADCMT
     * @param evt: -ADCMT
     * @param context: -ADCMT
     **/
    // authenticate = (evt, context) => {
    //     var username = document.getElementById('UserText').value;
    //     var password = document.getElementById('PassText').value;
    //     var url = 'login?username=' + username + "&password=" + password;
    //     showLoadMask();
        
    //     var lblErrPass = document.getElementById('errPass');
    //     lblErrPass.innerText = "";
    //     lblErrPass.hidden = true;
        
    //     // Server call to check user/pass and update state.
    //     makeServerCall(url, context.onServerResponse, {onServerCallError: context.showMaintanencePage} );
    // }


    // /**
    //  * Reroutes to maintanance page (not redundant, used for "makeServerCall" in authenticate)
    //  **/
    showMaintanencePage = () => {
        this.navigate('/maintenance');
    };


    /**
     * -ADCMT
     * @param response: -ADCMT
     * @param options: -ADCMT
     **/
    onServerResponse = (response, options) => {
        console.log(JSON.parse(response))
        var result; 
        
        try {
            result = JSON.parse(response);
        }
        catch(e) {
            result = null;
        }
        
        var lblErrPass = document.getElementById('errPass');

        if (result && result.status === 'success') { 
            // Save the details to store
            if (result.userInformation) {
                result.userInformation.loggedInTime = new Date();
                result.userInformation.idleTime = 0;
            }

            //console.log(result);
            this.saveUserInfo(result.userInformation, result.funnelInfo, result.savedViews);
        }

        else if (result && result.status === "failure") {
            //console.log('Error');
            lblErrPass.hidden = false;
            lblErrPass.innerText = "Incorrect Username/Password";
        }

        else if (result && result.status === "expired") {
            //console.log('Error');
            lblErrPass.hidden = false;
            lblErrPass.innerText = "User License has expired";
        }

        else {
            this.props.history.push("/maintenance");
        }

        hideLoadMask();
    }


    /**
     * -ADCMT
     * @param userInfo: -ADCMT
     * @param funnelInfo: -ADCMT
     **/
    saveUserInfo = (userInfo, funnelInfo, savedViews) => {
        //console.log('Success');
        this.setState({ openPassword: false });
        this.props.dispatch(saveUserInfo(userInfo, funnelInfo, savedViews));

        // Call function post login if provided.
        if (typeof this.props.doAfterLogin === 'function') {
            this.props.doAfterLogin(userInfo);
        }
        
        // Login cookie
        //setCookie(getLoginCookieName(), 1,0.5);

        // Redirect to home page.
        this.props.history.push("/home");
    }


    /**
     * -ADCMT
     * @param ycalc: -ADCMT
     **/
    // toggleLoginForm = (ycalc) => {
    //     var loginForm = document.getElementById('loginForm');
    //     var loginOverlay = document.getElementById('loginOverlay');
    //     var elements = document.getElementsByClassName('loginFormOtherElements');
    //     var userText = document.getElementById('UserText');
    //     var visibility = '';

    //     if (this.state.loginVisible) {
    //         loginForm.style.transform = "translate(0px, " + ycalc + "px)";
           
    //         // Hide other elements
    //         visibility = 'hidden';
    //         loginOverlay.classList.remove('loginOverlayCSS');
    //         loginForm.style.backgroundColor = "transparent";
    //     }

    //     else {
    //         loginForm.style.transform = "translate(0px, 0px)";
    //         loginOverlay.classList.add('loginOverlayCSS');
            
    //         // Show other elements
    //         visibility = '';
    //         loginForm.style.backgroundColor = 'rgba(97, 97, 97, 0.5)';
    //     }

    //     for (var index = 0; index < elements.length; index++) {
    //         elements[index].style.visibility = visibility;
    //     }

    //     userText.focus();
    //     this.setState({ loginVisible: !this.state.loginVisible });
    // }


    /**
     * -ADCMT
     * @param evt: -ADCMT
     **/
    // buttonClick = (evt) => {
    //     if (this.state.loginVisible) {
    //         this.authenticate(evt, this);
    //     }
        
    //     else {
    //         this.toggleLoginForm(this.state.loginButtonBottomTranslateCalc);
    //     }
    // }


    /**
     * Closes the forgot password modal through the local state
     **/
    // handleCloseForgotPassDialog = () => {
    //     this.setState({ openForgotPasswordDialog: false, snackbar: false});
    // };


    /**
     * -ADCMT
     **/
    // handleForgotPassSubmit(){
        // var email = document.getElementById('ForgotUserText') ? document.getElementById('ForgotUserText').value : null;
        // this.handleCloseForgotPassDialog();

        // Send server call to email password recovery link.
        // this.setState({ snackbar: true });

        // let text = document.getElementsByClassName("retrieve-Pass-Text");
        // console.log('text', text);
    // }


    render() {
        return (
            <Flexbox style = {{ width: '100%', height: '100%' }} >
                {/* <video playsInline autoPlay loop muted 
                    poster = "./Res/Img/synglyphx_bio.png" 
                    id = "bgvid"
                    // onClick = { (evt) => this.toggleLoginForm(this.state.loginButtonBottomTranslateCalc) }
                >
                    <source src = "./Res/Vid/GlyphEd.mp4" type = "video/mp4" />
                    Your browser does not support the video tag.
                </video> */}
                
                {/* <div id = "loginOverlay" style = {{ width: '100%', height: '100%', display: 'table', overflow: 'hidden' }} >
                    <center style = {{ display: 'table-cell', verticalAlign: 'middle' }} >

                        <Paper  id = "loginForm" style = {{ 
                            backgroundColor: 'rgba(75, 38, 38, 0.50)', 
                            fontSize: "2.50vh",
                            width: '17vw', 
                            height: 'fit-content', 
                            borderRadius: "3px"}} >

                            <div className = "loginFormOtherElements" style = {{ textAlign: 'center', paddingBottom: '0px'}} >
                                <img src = "./Res/Img/sgx_inside.png" alt = "SynGlyphX" style = {{ width: "16.15vw", marginTop: "2.61vh" }} />
                            </div>

                            <br/>
                            
                            <Flexbox 
                                flexDirection = "row" 
                                className = "loginFormOtherElements"
                                style = {{
                                    width: "85%",
                                    borderColor: "#d9d9d9 #ccc #b3b3b3",
                                    border: "1px solid #ccc",
                                    height: "4.15vh",
                                    overflow: 'hidden',
                                    backgroundColor: "white"
                                }}
                            >
                                <div style = {{ width: "1.9vw" }} >
                                    <i 
                                        className = "fa fa-user" 
                                        style = {{
                                            fontSize: '3.75vh',
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
                                        fontSize: '1.668vh',
                                        paddingLeft: "1.043vh"
                                    }}
                                />
                            </Flexbox>

                            <br/>

                            <Flexbox 
                                flexDirection = "row" 
                                className = "loginFormOtherElements"
                                style = {{
                                    width: "85%",
                                    borderColor: "#d9d9d9 #ccc #b3b3b3",
                                    border: "1px solid #ccc",
                                    height: "4.15vh",
                                    overflow: 'hidden',
                                    backgroundColor: "white",
                                    marginBottom: "1.031vh"
                                }}
                            >

                                <div style = {{ width: "1.9vw" }} >
                                    <i 
                                        className = "fa fa-lock" 
                                        style = {{
                                            fontSize: '3.75vh',
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
                                        fontSize: '2.398vh',
                                        paddingLeft: "1.043vh"
                                    }}
                                />
                            </Flexbox>

                            <div id="caps_lock_ux" style={{
                                    display: "none", 
                                    color: "red", 
                                    backgroundColor: "white",
                                    letterSpacing: "0.5px",
                                    fontSize: "2.19vh", 
                                    width: "14.5vw",
                                    fontFamily: "ITCFranklinGothicStd-DmCd"}}>
                                Caps Lock On
                            </div>

                            <div
                                className = "loginFormOtherElements" 
                                id = "errPass" 
                                style = {{ 
                                    color: 'red',
                                    width: "14.5vw",
                                    backgroundColor: "white",
                                    fontFamily: "ITCFranklinGothicStd-DmCd",
                                    marginTop: "3px",
                                    letterSpacing: "0.5px",
                                    fontSize: "2.19vh" }}  
                            >
                            </div>

                            <div>
                                <label className = "loginFormOtherElements" id = "forgotPass" style = {{ color: "#fff" }} >
                                    <a 
                                        this.setState({openForgotPasswordDialog: true})} 
                                    style = {{ 
                                        cursor: "pointer", 
                                        fontFamily: "ITCFranklinGothicStd-DmCd", 
                                        letterSpacing: "1px", 
                                        fontSize: "1.67vh" }} 
                                    >
                                        Forgot password?
                                    </a>
                                </label>
                            </div>

                            <br/>

                            <RaisedButton
                                id = "loginButton"
                                label = "Login"
                                secondary = { true }
                                onClick = { (evt) => this.buttonClick(evt, this) }
                                buttonStyle = {{ backgroundColor: this.props.settings.colors.buttons.general, height: "3.75vh" }}
                                labelStyle = {{ fontSize: '2.08vh' }}
                                style = {{ 
                                    position: 'relative', 
                                    width: "65%",
                                    display: 'block',
                                    borderRadius: '3px',
                                    minWidth: "9.176vh",
                                    color: this.props.settings.colors.settingsModalColor.saveButton 
                                }}
                            />

                            <label className = "loginFormOtherElements" id = "forgotPass" style = {{ fontSize: '16px' }}>
                                <a onClick = { () => this.setState({ openForgotPasswordDialog: true }) } style = {{ color: '#b3b3b3', cursor: 'pointer' }} ><u> forgot password? </u></a>
                            </label>

                            <label className = "loginFormOtherElements" id = "forgotPass" style = {{ 
                                fontSize: '12px', 
                                color: "#fff", 
                                fontFamily: "ITCFranklinGothicStd-Med" }} >
                                By logging in, you agree to the &nbsp;
                                <a 
                                    onClick = { () => this.setState({ openTermsAndConditionsDialog: true }) } 
                                    style = {{cursor: 'pointer' }} ><u>Terms and Conditions.</u>
                                </a>
                            </label>

                            <br />

                            <IconButton 
                                iconClassName = "fa fa-arrow-circle-down" 
                                iconStyle = {{ color: 'white', fontSize: "2.502vh" }}
                                onClick = { (evt) => this.toggleLoginForm(this.state.loginButtonBottomTranslateCalc) }
                            />
                        </ Paper>
                    </ center>
                    <br/>
                    
                    <Dialog
                        actionsContainerStyle = {{ marginLeft: "15px", textAlign: "left" }}
                        titleStyle = {{ fontFamily: "ITCFranklinGothicStd-Med", fontSize: "26px", lineHeight: "normal" }}
                        bodyStyle = {{ fontFamily: "ITCFranklinGothicStd-DmCd", fontSize: "20px" }}
                        title = "Forgot Your Password?"
                        modal = { true }
                        open = { this.state.openForgotPasswordDialog }
                        onRequestClose = { this.handleCloseForgotPassDialog }
                        actions = {[
                            <RaisedButton
                                label = "Submit"
                                primary = { true }
                                onClick = {() => {this.handleForgotPassSubmit() } }
                            />,
                            <span> &nbsp; </span>,
                            <RaisedButton
                                label = "Cancel"
                                primary = { true }
                                onClick = {() => this.handleCloseForgotPassDialog() }
                            />
                        ]}
                    >
                        Please Enter Your Email address associated with the account. <br/><br/>
                        <input 
                            type = "text"
                            id = "ForgotUserText"
                            className = "retrieve-Pass-Text"
                            placeholder = "example@email.com"
                            style = {{ height: 'inherit', border: '1px #b3b3b3 solid', outlineWidth: '0', fontSize: '20px' }}
                        />
                    </Dialog>

                    <Snackbar
                        open = { this.state.snackbar }
                        message = "A link has been sent to your email address to reset the password."
                        autoHideDuration = { 3000 }
                        onRequestClose = { this.handleCloseForgotPassDialog }
                    />

                    <Dialog
                        title = {<div style = {{ textAlign: "center", fontWeight: "bold" }} >END-USER LICENSE AGREEMENT (EULA) </div>}
                        modal = { true }
                        open = { this.state.openTermsAndConditionsDialog }
                        onRequestClose = { this.handleCloseForgotPassDialog }
                        bodyStyle = {{ overflowY: "scroll" }}
                        actions = {[
                            <RaisedButton
                                label = "Ok"
                                primary = { true }
                                onClick = { () => this.setState({ openTermsAndConditionsDialog: false }) }
                                buttonStyle = {{ backgroundColor: this.props.settings.colors.buttons.general }} 
                            />
                        ]}
                    >
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />PLEASE READ CAREFULLY BEFORE USING THIS PRODUCT: </span> This EULA is a legal agreement between (a) Licensee (either an individual or a single entity) and (b) Admissions GlyphX, LLC. (“GlyphEd”) that governs your use of any Software Product, installed or made available by GlyphEd. Other software provided by third parties and used with a GlyphEd Product may be subject to a separate EULA. The term “Software Product” means computer software and may include associated media, printed materials, and/or electronic documentation. 
                                <span  style = {{ fontWeight: "bold" }} > <br /><br />BY CLICKING ACCEPT TO THIS DOCUMENT OR OTHERWISE TAKING ANY STEP TO INSTALL OR USE A GLYPHED PRODUCT, LICENSEE AGREES TO BE BOUND BY THE TERMS OF THIS EULA. LICENSEE MAY NOT USE, COPY, OR OTHERWISE DISTRIBUTE THIS SOFTWARE FOR ANY PURPOSES WITHOUT THE EXPRESS WRITTEN CONSENT OF GLYPHED. </span>
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            1.      GRANT OF LICENSE 
                            </span>
                            <br /> GlyphEd grants Licensee a non-exclusive, non-transferable license for one year from date of issuance to use GlyphEd’s Software Product provided Licensee agrees to and comply with all terms and conditions of this EULA.  
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            2.      OWNERSHIP 
                            </span>
                            <br />This Agreement does not convey any rights of ownership in the Software.  It is understood and agreed by the parties that the GlyphEd Software Product is, and will remain, the sole and exclusive property of GlyphEd and any third party software owners whose software is included in the Licensed Software.  Except as otherwise specifically provided in this Agreement, no license, right, title, interest in and/or to such GlyphEd Software Product is granted to Licensee by virtue of this Agreement and/or GlyphEd performance hereunder. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            3.      TRANSFER AND DISTRIBUTION 
                            </span>
                            <br />Licensee may use the Software Product in executable format for his or her own use on one single computer. Licensee may not sublicense, assign, or transfer the license of Software Product except as expressly provided in this EULA. Licensee may not copy the Software Product, except that Licensee may make a single (1) copy of the Software Product solely for backup or archival purposes. Reinstallation of archival copies may be subject to re-authentication. 
                                Licensee may not transfer, sublicense, or distribute the Software Product to any business affiliates, subcontractors or to a third party, in whole or in part, in any form, whether modified or unmodified, without GlyphEd’s express written consent. If consent is given, any distribution must include all component parts, media, printed materials, and this EULA. All business affiliates, subcontractors, or third parties must sign and return a completed EULA prior to transfer. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            4.      DATA PROTECTION 
                            </span>
                            <br />In accordance with this agreement Licensee recognizes that GlyphEd may transfer, copy, backup and store Licensee Content in the Cloud using Amazon Web Services. As such, Licensee agrees to the terms of the Amazon Web Services customer agreement found at https://aws.amazon.com/agreement/ and will ensure compliance with the terms of said agreement. GlyphEd will not monitor Licensee Content or otherwise process Licensee Content other than (i) as needed to provide, support, (ii) where instructed or permitted by Licensee, (iii) to investigate potential or suspected fraud or misuse of the data, or (iv) as otherwise required by law or to exercise or protect our legal rights. Licensee is solely responsible for the development, content, operation, maintenance and use of Licensee Content.  
                            Licensee will indemnify, hold harmless, and defend GlyphEd from and against any claims or lawsuits, including attorneys' fees, that arise from or result from any incident arising from the storage of Licensee Content at AWS.  
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            5.      PROPRIETARY RIGHTS 
                            </span>
                            <br />All intellectual property rights in this Software Product are owned by GlyphEd and are protected by law, including but not limited to United States copyright, trade secret, trademark, and other applicable laws and international treaty provisions. The Software Product structure, organization, and code are valuable trade secrets and confidential information of GlyphEd. Licensee shall not remove any product identification, copyright notices, or proprietary restrictions from the Software Product.  
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            6.      LIMITATION ON REVERSE ENGINEERING 
                            </span>
                            <br />Licensee shall neither personally nor allow any third party to reverse engineer, disassemble, decompile, modify, adapt, translate, or attempt to reconstruct or discover any source code or underlying ideas to create derivative works based on the Software Product. Nothing contained in this EULA shall be construed, expressly or implicitly, as transferring any right, license, or title to Licensee other than those explicitly granted under this EULA. Unauthorized copying of the Software Produce or failure to comply with the above restrictions will result in automatic termination of this Agreement and will constitute immediate, irreparable harm to GlyphEd for which monetary damages would be an inadequate remedy, in which case injunctive relief will be an appropriate remedy for such breach.  The obligation under this Section shall survive any termination of the Agreement. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            7.      DISCLAIMER OF WARRANTIES 
                            </span>
                            <br />Licensee acknowledges that this Software Product is offered on an “AS IS” basis and GlyphEd makes no express or implied warranties that the functions contained in the Software Product will meet your requirements or that the operation of the Software Product will be uninterrupted or error free. Computer software is inherently subject to bugs and potential incompatibility with other computer software and hardware. GlyphEd does not offer any duty of reasonable care or lack of negligence with regard to operation of the Software Product. GlyphEd is not obligated to provide technical support for the Software Product unless expressly written. Licensee should not use the Software Product for any applications in which failure could cause any significant damage or injury to persons or intangible property. <span  style = {{ fontWeight: "bold" }} >TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, GLYPHED PROVIDES THE PRODUCT “AS IS” AND WITH ALL FAULTS, AND HEREBY DISCLAIMS ALL OTHER WARRANTIES AND CONDITIONS, EITHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF TITLE AND NON-INFRINGEMENT, ANY IMPLIED WARRANTIES, DUTIES OR CONDITIONS OF MERCHANTABILITY, OF FITNESS FOR A PARTICULAR PURPOSE, AND OF LACK OF VIRUSES ALL WITH REGARD TO THE SOFTWARE PRODUCT. NO ORAL OR WRITTEN INFORMATION OR ADVICE GIVEN BY GLYPHED SHALL CREATE A WARRANTY.  GLYPHED RETAINS ALL THE RIGHTS, TITLE, AND OWNERSHIP TO THE SOFTWARE. </span>
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            8.      LIMITATION OF LIABILITY 
                            </span>
                            <br />To the maximum extent permitted by applicable law, in no event shall GlyphEd by liable for any special, incidental, indirect, or consequential damages whatsoever (including, but not limited to, damages for loss of profits, for loss or data or other information, for business interruption, for personal injury, for loss of privacy arising out of or in any way related to the use of or inability to use the Software Product, or otherwise in connection with any provision of this EULA), even if GlyphEd has been advised of the possibility of such damages and even if the remedy fails of its essential purpose. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            9.      USER FEEDBACK 
                            </span>
                            <br />Any feedback about the Software Product provided by Licensee to GlyphEd is voluntarily given, and GlyphEd shall be free to use the feedback as it sees fit without obligation or restriction of any kind, even if the feedback is designated by Licensee as confidential. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            10.  CAPACITY AND AUTHORITY TO CONTRACT 
                            </span>
                            <br />Licensee represents that Licensee is of the legal age of majority in his or her state, province, or country and, if applicable, Licensee is duly authorized by his or her employer to enter into this contract. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            11.  SERVICES 
                            </span>
                            <br />The licensor, at its discretion, may provide services in terms of upgrades and maintenance, education for using the software, and troubleshooting problems. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            12.  SEVERABILITY 
                            </span>
                            <br />In the event that any provision of this Agreement conflicts with governing law, or if any provision is held to be null, void, or otherwise ineffective or invalid by a court of competent jurisdiction, such provision shall be conformed, to the extent practicable, to obtain the original objective intended by the Parties, and the remaining terms, provisions, covenants and restrictions of this Agreement shall remain in full force and effect.  
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            13.  EXPORT 
                            </span>
                            <br />All Software Products, and any other materials delivered under this Agreement are subject to U.S. export control laws and may be subject to export or import regulations in other countries.  Licensee agrees to comply strictly with these laws and regulations and acknowledges that Licensee has the responsibility to obtain any licenses to export, re-export, or import as may be required. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            14.  FORCE MAJEURE 
                            </span>
                            <br />Except for the obligation to make payments, non-performance of either Party shall be excused to the extent that performance is rendered impossible as the result of a force majeure event (including but not limited to a strike, fire, war, flood, governmental acts or orders or restrictions, failure of suppliers or Third Party Software Owners, an act of God, or any other reason where failure to perform is beyond the reasonable control of the non-performing Party). 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            15.  INDEMNIFICATION 
                            </span>
                            <br />Licensee agrees to defend GlyphEd and its respective successors and assigns, officers, directors, employees, agents, licensors, representatives, business partners, service providers, suppliers, resellers, and contractors (“GlyphEd Indemnified Parties”) against any and all claims, demands and/or actions and indemnify and hold the GlyphEd Indemnified Parties harmless from and against any and all losses, damages, costs and expenses (including reasonable attorneys’ fees), resulting from any breach, security breach or violation of this EULA, infringement, misappropriation or any violation of the rights of any party, violation or noncompliance with any law or regulation, the breach or violation of any tern or condition of Licensee’s EULA with GlyphEd, and any use, alteration, or export of the Software Product or otherwise in connection with this EULA. We reserve the right to assume, at our expense, the exclusive defense and control of any claims or actions and all negotiations for settlement or compromise, and Licensee fully agrees to cooperate with GlyphEd upon our request. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            16.  TERM 
                            </span>
                            <br />This EULA is effective unless terminated or rejected. This EULA will also terminate if Licensee fails to comply with any term or condition set forth in Section 9 or elsewhere in this EULA. GlyphEd will give Licensee notice of any violation, and Licensee shall have ten (10) days after notification to correct. Failure to correct after 10 days will result in default. Upon termination of this EULA, Licensee must immediately destroy all copies of the Software Product immediately. Licensee must promptly submit a signed declaration that attests to the destruction of all GlyphEd Software Product in conformance with this EULA, and termination is without prejudice to GlyphEd’s entitlement to damages for a violation of the EULA. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            17.  APPLICABLE LAW 
                            </span>
                            <br />This EULA shall be construed, governed, and enforced solely and exclusively by the laws of the Commonwealth of Virginia, USA without regard to the principles of conflict of laws, or any other principals or international law, that would result in the application of another body of law, including the United Nations Convention on Contracts for the International Sale of Goods, as revised, or to the Uniform Computer Information Transactions Act, as adopted in any jurisdiction. Licensee hereby agrees that the courts located in the county where GlyphEd has its principal offices, will constitute the sole and exclusive forum for the resolution of any and all disputes arising out of or in connection with this Agreement and Licensee hereby irrevocably consents to the personal jurisdiction and venue of such courts and irrevocably waive any objections thereto. In the event either Party brings an action to enforce this Agreement (including any attachments or schedules), the prevailing Party shall be entitled to recover from the other Party all reasonable attorneys’ fees and costs associated with the enforcement proceeding, including fees and costs incurred in any appellate proceeding. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            18.  THIRD PARTY SOFTWARE 
                            </span>
                            <br />The software contains third party files, software code, and documentation writings that are reproduced and distributed under open source or BSD style licenses (collectively, the “open Source Software”).  By accepting this Agreement, Licensee is also accepting the terms and conditions that govern the Open Source Software. Upon request, GlyphEd will provide Licensee with all pages and/or links to the relevant third party software Notices and/or additional terms and conditions. 
                        </div>
                        <div> <span  style = {{ fontWeight: "bold" }} > <br />
                            19.  ENTIRE AGREEMENT 
                            </span>
                            <br />This EULA is the entire agreement between Licensee and GlyphEd relating to the Software Product and it supersedes all prior or contemporaneous oral or written communications, proposals, and representations with respect to the Software Product or any other subject matter covered by this EULA. If any provision of this EULA is held by a court of competent jurisdiction to be contrary to law, such provision shall be changed and interpreted so as to best accomplish the original provision’s objectives to the fullest extent allowed by law, and the remaining provisions of the EULA will remain in full force and effect.
                        </div>
                    </Dialog>
                </div> */}
            </Flexbox>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const saveUserInfo = (userInfo, funnelInfo, savedViews) => ({
    type: 'SAVE_USER_INFO',
    userInfo,
    funnelInfo,
    savedViews
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
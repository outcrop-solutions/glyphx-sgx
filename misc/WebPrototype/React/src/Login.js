import React, { Component } from 'react';
import Dialog from 'material-ui/Dialog';
import { connect } from 'react-redux';
import {makeServerCall} from './ServerCallHelper.js';
import RaisedButton from 'material-ui/RaisedButton';
import 'font-awesome/css/font-awesome.min.css';

class Login extends Component {

    constructor(props) {
        super(props);

        this.state = {
            openPassword: true,
        }
    }
	
	/**
	 * This function shows the in app loadmask(cicular waiting).
	 */
    showLoadMask = () => {
        document.getElementById("LoadMask").style.visibility = "visible";
    };

	
	/**
	 * This function hides the initial loadmask/splash screen.
	 */
    hideLoadMask = () => {
        var lm = document.getElementById("ipl-progress-indicator");
        if (lm) {
            setTimeout(() => {
                document.getElementById("ipl-progress-indicator").classList.add('available');
                setTimeout(() => {
                document.getElementById("ipl-progress-indicator").outerHTML = '';
                }, 2000)
            }, 1000)
        }
    };
	

	/**
	 * This function is called right after the react component is mounted.
	 * It decides whether to show the login form and calls the init().
	 */
    componentDidMount() {
        this.hideLoadMask();
    }


    authenticate = (evt,context) => {
        var username = document.getElementById('UserText').value;
        var password = document.getElementById('PassText').value;
        var url = 'login?username=' + username + "&password=" + password;
        
        var lblErrPass = document.getElementById('errPass');
        lblErrPass.innerText="";
        lblErrPass.hidden = true;
        
        var lblErrUser = document.getElementById('errUser');
        lblErrUser.innerText="";
        lblErrUser.hidden = true;
        
        //server call to check user/pass and update state.
        makeServerCall(url,context.onServerResponse,null);
    }

    onServerResponse = (Response,options) => {
        var result; 
        
        try{
            result = JSON.parse(Response);
            result = result.result;
        }
        catch(e){
            result = null;
        }
        
        var lblErrPass = document.getElementById('errPass');
        var lblErrUser = document.getElementById('errUser');

        debugger;        
        
        if(result && result.status == 'success'){ 
            //save the details to store
            if(result.userInfo)
                {
                    /*info={
                        userName: result.userInfo.UserName,
                        firstName: result.userInfo.Fname,
                        lastName: result.userInfo.Lname,
                        type: result.userInfo.Type,
                        product: result.userInfo.Product,
                        loggedInTime: new Date(),
                        idleTime: 0 // for future auto logout.
                    };*/
                    result.userInfo.loggedInTime = new Date();
                    result.userInfo.idleTime = 0;
                }
            this.saveUserInfo(result.userInfo);
        }
        else{
            console.log('Error');
            lblErrPass.hidden = false;
            lblErrPass.innerText="Incorrect Password";
        }
    }

    saveUserInfo = (info) => {
        console.log('Success');
        this.setState({openPassword:false});
        this.props.dispatch(saveUserInfo(info));

        //call function post login if provided.
        if(typeof this.props.doAfterLogin == 'function')
            {
                this.props.doAfterLogin(info);
            }
    }

    render() {
        return (
            <Dialog
                id = "pass"
                title = "Authentication"
                ref = "PasswordDailog"
                actions = {
                    [
                        <RaisedButton
                            label = "Login"
                            primary = { true }
                            onClick = { (evt) => this.authenticate(evt,this) }
                            style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                        />
                    ]
                }
                overlayStyle = {{ backgroundColor: 'white' }}
                contentStyle = {{ width:'25%', maxWidth: "none" }}
                modal = { true }
                open = { this.state.openPassword }
            >   
                {/*Username textfield*/}
                <label  
                    style = {{ 
                        height: '20px',
                        fontSize: '18px'
                    }}
                > Username:
                </label> 
                <input 
                    id = "UserText"
                    type = "text"
                    style = {{
                        height: '20px',
                        fontSize: '18px'
                    }}
                />
                <br/>
                <label id = "errUser" 
                    hidden 
                    style = {{ 
                        color:'red',
                        height: '20px',
                        fontSize: '18px'
                    }}
                />

                {/*Password textfield*/}
                <label  
                    style = {{ 
                        height: '20px',
                        fontSize: '18px'
                    }}
                > Password:
                </label> 
                <input 
                    id = "PassText"
                    type = "password"
                    //placeholder="Enter Password"
                    style = {{
                        height: '20px',
                        fontSize: '18px'
                    }}
                />
                <br/>
                <label id = "errPass" 
                    hidden 
                    style = {{ 
                        color:'red',
                        height: '20px',
                        fontSize: '18px'
                    }}
                />
            </Dialog>
        );
    }
}

/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const saveUserInfo = (info) => ({
    type: 'SAVE_USER_INFO',
    info
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
 * Connects the Login component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(Login);
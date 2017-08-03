import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import Dialog from 'material-ui/Dialog';
import { connect } from 'react-redux';
import RaisedButton from 'material-ui/RaisedButton';
import 'font-awesome/css/font-awesome.css';

class Login extends Component {

    constructor(props) {
        super(props);

        this.state ={
            openPassword: false,
            userName: null,
            firstName: null,
            lastName: null
        }
    }

    showLoginForm = () => {
         this.setState({
            openPassword: true
        });
    }

    authenticate = (evt,context) => {
        var pass = "SynGlyphX2017!";
        var userVal = document.getElementById('PassText').value;
        var lblErr = document.getElementById('errPass');
        lblErr.innerText="";
        lblErr.hidden = true;

        if(pass == userVal){
            console.log('Success');
            this.setState({openPassword:false});
            this.saveUserInfo();
        }
        else{
            console.log('Error');
            lblErr.hidden = false;
            lblErr.innerText="Incorrect Password";
        }
    }

    saveUserInfo = () => {
        var info={
            userName: this.state.userName,
            firstName: this.state.firstName,
            lastName: this.state.lastName,
            loggedInTime: new Date(),
            idleTime: 0 // for future auto logout.
        };
        this.props.dispatch(saveUserInfo(info));
    }

    render() {
        return (
            <Dialog
                id="pass"
                title = "Authentication"
                ref = "PasswordDailog"
                actions = {
                    [
                        <RaisedButton
                            label = "Login"
                            primary = { true }
                            onClick = { (evt) => this.authenticate(evt,this) }
                            style = {{ color: this.props.settings.settingsModalColor.saveButton }}
                        />
                    ]
                }
                overlayStyle={{backgroundColor: 'white'}}
                contentStyle={{width:'25%', maxWidth: "none"}}
                modal = { true }
                open = { this.state.openPassword }
            >
                <label  
                    style = {{ 
                        height: '20px',
                        fontSize: '18px'
                    }}
                > Password:
                </label> 
                <input 
                    type="text" 
                    id = "PassText"
                    type="password"
                    //placeholder="Enter Password"
                    style={{
                        height: '20px',
                        fontSize: '18px'
                    }}
                />
                <br/>
                <label id="errPass" 
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
    //userInfo: state.UserInfo,
    settings: state.filterState.Settings
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(Login);
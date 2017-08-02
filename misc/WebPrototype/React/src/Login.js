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
            openPassword: false
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
        }
        else{
            console.log('Error');
            lblErr.hidden = false;
            lblErr.innerText="Incorrect Password";
        }
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

const mapStateToProps = function(state){
  return {
    userInfo: state.userInfo,
    settings: state.filterState.Settings
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(Login);
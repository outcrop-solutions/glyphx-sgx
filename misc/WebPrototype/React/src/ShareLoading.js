import React from 'react';
import { connect } from 'react-redux';
import { hideSplashScreen } from './LoadMaskHelper.js';
import { makeServerCall } from './ServerCallHelper.js';
import { withRouter } from 'react-router-dom';
import 'rc-tooltip/assets/bootstrap.css';
import './css/General.css';
import './css/Views.css';


/**
* -ADCMT
*/
class ShareLoading extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            loadMask: false,
        }
    }
    


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
        hideSplashScreen();
        if(this.props.vars){
            console.log(this.props.vars);
            this.checkSharedHash(this.props.vars);
        }
    }

    checkSharedHash(vars) {
        let context = this;
        // console.log(vars, vars.params.id);
        return new Promise((resolve, reject) => {
            return makeServerCall('authShareLink',
                function(res,b,c) {
                    if (typeof res === 'string') res = JSON.parse(res);
                    
                    console.log(res);
                    console.log(res.body.authShare);
    
                    if(res.body) {
                        if(res.body.authShare === true){
                            context.props.dispatch(sharedLink(true));
                        }
                        else if(res.body.authShare === false){
                            context.props.dispatch(sharedLink(false));
                        }
                        
                        resolve();
                    }
                    else {
                        reject();
                    }
                
                },
                {
                    post: true, 
                    data:  { 
                        id: vars.params.id
                    }
                }
            );
        });
      }    

    render() {
        return (
            <div style = {{
                height: "100%", 
                width: "100%", 
                fontSize: "35px", 
                backgroundColor: "white"}}
            >
                <h1 style={{
                    textAlign: "center", 
                    color:"black", 
                    margin: "0px", 
                    paddingTop: "255px"}}
                >LOADING...</h1>

            </div>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const sharedLink = (falsy) => ({
    type: 'SHARE_LINK_REDIRECT',
    falsy,
});

// export const setTimer = (timeoutTimer) => ({
//     type: 'SET_TIMEOUT_TIMER',
//     timeoutTimer,
// });


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null)(ShareLoading));
import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { deleteCookie, getLoginCookieName, makeServerCall } from './ServerCallHelper.js';
import { hideSplashScreen } from './LoadMaskHelper.js';
/* import Dialog from 'material-ui/Dialog'; */


/**
 * Performs the logout and displays the logout page
 **/
class Logout extends React.Component {


    /**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {
       
        // if(window.location.href.indexOf('uid') > -1){
        //     this.props.dispatch(setUid(window.location.href.slice(
        //         window.location.href.indexOf('uid')+4
        //     )));
            
        // }
    }


    deleteLoginCookie(){
        deleteCookie(getLoginCookieName());
    }


    render() {
        let context = this;
        this.deleteLoginCookie();
        hideSplashScreen();
        makeServerCall("logout",
        function (responseText) { 
            context.props.dispatch(logoutClear());
            context.props.history.push('/');
        }
    );
        return(
            <div style = {{ height: "100vh", width: "100vw", backgroundColor: "#fff" }} />
        );
    }
}

/**
 * Constants defined to make dispatching for the redux store consistent
 **/
// export const setUid = (uid) => ({
//     type: 'SET_UID',
//     uid,
// });

export const logoutClear = () => ({
    type: 'LOGOUT_CLEAR',
});

/**
* Maps portions of the store to props of your choosing
* @param state: passed down through react-redux's 'connect'
**/
// const mapStateToProps = function (state) {
//     return {
//         uid: state.filterState.uid
//     }
// }

export default withRouter(connect()(Logout));
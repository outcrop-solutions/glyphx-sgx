import React from 'react'
import { connect } from 'react-redux';
import { /* Router, */ Route, Switch, Redirect, HashRouter } from 'react-router-dom';
import { checkUserLoggedIn } from './ServerCallHelper.js';
/* import createHistory from 'history/createBrowserHistory'; */
import Login from './Login.js';
import HomePage from './HomePage.js';
import NotFoundPage from './NotFoundPage.js'; 
import Logout from './Logout.js';
import VisualizationView from './VisualizationView.js';
import Maintenance from './Maintenance.js'
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';


/* const browserHistory = createHistory();
var interval = {}; */
var isUserLoggedIn = false;
var maintenance = false;


class ApplicationRouter extends React.Component{

  constructor (props) {
    super(props);
    var context = this;
    try {
        var res = checkUserLoggedIn(this.onServerError);
        var jsonRes = JSON.parse(res);
        var loggedIn = jsonRes ? jsonRes.isUserLoggedIn : false;
        
        if (loggedIn && jsonRes) {
            //setCookie(getLoginCookieName(),1,0.5);
            context.props.dispatch(saveUserInfo(jsonRes.userInformation,jsonRes.funnelInfo));
            isUserLoggedIn=true;
        }
    }
    catch (err) {
        // Handle error
    }
  }

  onServerError() {
      maintenance = true;
  }

  RedirectToLogin = () => (
      <Redirect to = "/login" />
  );

  LoginForm = () => (
      (maintenance ? <Redirect to = "/maintenance" /> : ( this.getLoggedInStatus() ? <Redirect to = "/home" /> : <Login/> ) )
  );

  HomeView = () => (
      (maintenance ? <Redirect to = "/maintenance" /> : ( this.getLoggedInStatus() ? <HomePage /> : <Redirect to = "/login" /> ) )
  );

  VisualizationWindow = () => (
      (maintenance ? <Redirect to = "/maintenance" /> : ( this.getLoggedInStatus() ? <VisualizationView /> : <Redirect to = "/login" /> ) )
  );

  LogoutView = () => (
      <Logout />
  );

  Maintenance = () => (
      (maintenance ? <Maintenance /> : <Redirect to = "/login" /> )
  );


  /**
   * We are checking both as the dispatch doesn't updates the store asynchronously and thus we have to set a flag in this class to true and check that.
   */
  getLoggedInStatus() {
      return (this.props.isUserLoggedIn || isUserLoggedIn);
  }

    render() {
        return (
            <MuiThemeProvider>
                <HashRouter >
                    <Switch>
                        <Route exact path = "/login" component = { this.LoginForm } />

                        <Route exact path = "/" component = { this.RedirectToLogin } />

                        <Route exact path = "/home" component = { this.HomeView } />

                        <Route exact path = "/glyph-viewer" component = { this.VisualizationWindow } />


                        <Route exact path = "/logout" component = { this.LogoutView } />

                        <Route exact path = "/maintenance" component = { this.Maintenance } />

                        <Route path = "*" component = { NotFoundPage } />
                  </Switch>
              </HashRouter>
          </MuiThemeProvider>
      );
  };  
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
const mapStateToProps = function (state) {
    return {
        settings: state.filterState.Settings,
        userInfo: state.filterState.UserInfo,
        isUserLoggedIn: state.filterState.isUserLoggedIn
    }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(ApplicationRouter);
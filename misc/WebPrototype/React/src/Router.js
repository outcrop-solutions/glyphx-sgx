import React from 'react'
import { Router, Route, Switch, Redirect } from 'react-router-dom';
import Login from './Login.js';
import HomePage from './HomePage.js';
import { connect } from 'react-redux';
import {hideSplashScreen} from './LoadMaskHelper.js';
import NotFoundPage from './NotFoundPage.js'; 
import {makeServerCall,getCookie,getLoginCookieName,setCookie,checkUserLoggedIn} from './ServerCallHelper.js';
import createHistory from 'history/createBrowserHistory';
import Logout from './Logout.js';
import VisualizationView from './VisualizationView.js';
import Maintenance from './Maintenance.js'
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import { CSSTransitionGroup } from 'react-transition-group';

const browserHistory = createHistory();
var interval={};
var isUserLoggedIn = false;
var maintenance=false;

class ApplicationRouter extends React.Component{

  constructor(props){
    super(props);
    var context = this;
    try{
      var res = checkUserLoggedIn(this.onServerError);
      var loggedIn = JSON.parse(res) ? JSON.parse(res).isLoggedIn : false;
      if(loggedIn)
        {
          setCookie(getLoginCookieName(),1,0.5);
          context.props.dispatch(saveUserInfo(JSON.parse(res)));
          isUserLoggedIn=true;
        }
    }
    catch(err){

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
    <Maintenance />
  );

  /**
   * We are checking both as the dispatch doesn't updates the store asynchronously and thus we have to set a flag in this class to true and check that.
   */
  getLoggedInStatus() {
    return this.props.isUserLoggedIn || isUserLoggedIn;
  }

  render() {
    return (
      <MuiThemeProvider>
        <Router history={browserHistory}>
          <Switch>
              <Route exact path = "/login" component = { this.LoginForm } />
              <Route exact path = "/" component = { this.RedirectToLogin } />
              <Route exact path = "/home" component = { this.HomeView } />
              <Route exact path = "/glyph-viewer" component = { this.VisualizationWindow } />
              <Route exact path = "/logout" component = { this.LogoutView } />
              <Route exact path = "/maintenance" component = { this.Maintenance } />
              <Route path = "*" component = { NotFoundPage } />
          </Switch>
        </Router>
      </MuiThemeProvider>
  );
  };

  
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
  settings: state.filterState.Settings,
  userInfo: state.filterState.UserInfo,
  isUserLoggedIn: state.filterState.isUserLoggedIn
}
}


/**
* Connects the TopNav component to the redux store
**/
export default connect(mapStateToProps)(ApplicationRouter);
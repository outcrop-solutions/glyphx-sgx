import React from 'react'
import { Router, Route, Switch, Redirect } from 'react-router-dom';
import Login from './Login.js';
import HomePage from './HomePage.js';
import NotFoundPage from './NotFoundPage.js'; 
import {makeServerCall,getCookie,getLoginCookieName} from './ServerCallHelper.js';
import createHistory from 'history/createBrowserHistory';
import Logout from './Logout.js';
import VisualizationView from './VisualizationView.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';

const browserHistory = createHistory();

const ApplicationRouter = () => (
    <MuiThemeProvider>
      <Router history={browserHistory}>
        <Switch>
            <Route exact path = "/login" component = { LoginForm } />
            <Route exact path = "/" component = { RedirectToLogin } />
            <Route exact path = "/home" component = { HomeView } />
            <Route exact path = "/glyph-viewer" component = { VisualizationWindow } />
            <Route exact path = "/logout" component = { logoutView } />
            <Route path = "*" component = { NotFoundPage } />
        </Switch>
      </Router>
    </MuiThemeProvider>
);

const RedirectToLogin = () => (
  <Redirect to = "/login" />
);

const LoginForm = () => (
  ( getLoggedInStatus() ? <Redirect to = "/home" /> : <Login/> )
);

const HomeView = () => (
  ( getLoggedInStatus() ? <HomePage /> : <Redirect to = "/login" /> )
);

const VisualizationWindow = () => (
  ( getLoggedInStatus() ? <VisualizationView /> : <Redirect to = "/login" /> )
);

const logoutView = () => (
  //( getLoggedInStatus() ? <Logout /> : <Redirect to = "/login" /> )
  <Logout />
);

function getLoggedInStatus() {
  
  if(getCookie(getLoginCookieName()) != "")
  {
      return true;
  }

  return false;
}

export default ApplicationRouter;
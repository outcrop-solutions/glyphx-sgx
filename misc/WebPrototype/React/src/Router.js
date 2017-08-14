import React from 'react'
import { BrowserRouter as Router, Route, Switch } from 'react-router-dom';
import Login from './Login.js';
import HomePage from './HomePage.js';
import VisualizationView from './VisualizationView.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';

const ApplicationRouter = () => (
  <Router>
    <MuiThemeProvider>
        <Switch>
            <Route exact path = "/" component = { LoginForm } />          
            <Route exact path = "/login" component = { LoginForm } />
            <Route exact path = "/home" component = { HomeView } />
            <Route exact path = "/glyph-viewer" component = { VisualizationView } />
            <Route path = "*" component = { NotFound } />
        </Switch>
    </MuiThemeProvider>
  </Router>
);

const LoginForm = () => (
  <div>
    <Login
        //doAfterLogin = { (prop) => this.doAfterLogin(prop) }
    />
  </div>
);

const HomeView = () => (
  <div>
    <HomePage />
  </div>
);

const VisualizationWindow = () => (
  <div>
    <VisualizationView />
  </div>
);

const NotFound = () => (
  <div>
    4040404040404040404040404040404040404040404040404
    <HomePage />
    4040404040404040404040404040404040404040404040404
  </div>
);

export default ApplicationRouter;
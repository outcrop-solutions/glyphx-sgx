import React from 'react'
import {
  BrowserRouter as Router,
  Route,
  Link
} from 'react-router-dom';
import Login from './Login.js';
import TopNav from './topNav.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';

const ApplicationRouter = () => (
  <Router>
    <MuiThemeProvider>
        <div>
            <Route exact path="/" component={LoginForm}/>
            <Route exact path="/home" component={TopNavApp}/>
        </div>
    </MuiThemeProvider>
  </Router>
);

const LoginForm = () => (
  <div>
    <Login
        doAfterLogin={(prop) => this.doAfterLogin(prop)}
    />
  </div>
);

const TopNavApp = () => (
  <div>
    <TopNav />
  </div>
)

export default ApplicationRouter;
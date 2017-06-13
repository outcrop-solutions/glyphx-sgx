import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import FilterNav from './filterNav';
import TextField from 'material-ui/TextField';

import AppBar from 'material-ui/AppBar';
import IconButton from 'material-ui/IconButton';
import FlatButton from 'material-ui/FlatButton';
import FontIcon from 'material-ui/FontIcon';

import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import './topNav.css';

    

class TopNav extends Component {
    render() {
        return (
            <MuiThemeProvider> 
                <div className="TopNav">

                    <AppBar
                      title={<span style={navLogo.title}>LOGO</span>}
                      iconElementRight={<IconButton><FontIcon className="fa fa-filter fa-2x" /></IconButton>}
                    />

                    <div id="filterNav" className="sidenav">
                        <FilterNav></FilterNav>
                    </div>
                    
                </div>
          </MuiThemeProvider>
        );
    }
}

// Hides the filter side nav by translating it off the screen so it doesnt resize and 
// wont have to be reloaded after it is "closed"
function toggleNav() {
    if (document.getElementById("filterNav").style.transform == "translate(460px, 0px)") {
        document.getElementById("filterNav").style.transform = "translate(0px, 0px)";
    }
    else {
        document.getElementById("filterNav").style.transform = "translate(460px, 0px)";
    }
}

const navLogo = {
  title: {
    cursor: 'pointer',
  },
};

export default TopNav;
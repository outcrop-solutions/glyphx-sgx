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

injectTapEventPlugin();

class TopNav extends Component {
    render() {
        return (
            <MuiThemeProvider> 
                <div className="TopNav" style={{width:'100%',height:'100%'}}>

                    <AppBar className="navbar-color"
                        id="AppBar"
                        title={<span style={styles.navLogo}>LOGO</span>}
                        iconElementRight={<IconButton onClick={toggleNav}><FontIcon className="fa fa-filter fa-2x" /></IconButton>}
                    />

                    <div id="filterNav" className="sidenav sidenavbar">
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

const styles = {
    navLogo: {
        cursor: 'pointer',
    },
};

export default TopNav;
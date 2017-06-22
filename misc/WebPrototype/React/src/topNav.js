import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import FilterNav from './filterNav';
import {Flex} from 'react-flex-material';
import TextField from 'material-ui/TextField';
import AppBar from 'material-ui/AppBar';
import IconButton from 'material-ui/IconButton';
import FlatButton from 'material-ui/FlatButton';
import FontIcon from 'material-ui/FontIcon';
import Dialog from 'material-ui/Dialog';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import './topNav.css';

injectTapEventPlugin();

class TopNav extends Component {
    state = {
        LoadMask : false
    };

    onSaveDailog = (context) => {
        this.setState(
            {
                LoadMask : true
            }
        )
    };

    render() {
        return (
            <MuiThemeProvider> 
                <Flex layout="column" style={{width:'100%',height:'100%'}}>

                    <Flex flex="20">
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
                    </Flex>

                    <Flex flex="80">
                        {/* The 3D rendering engine */}
                        <FlatButton
                            label="Save"
                            primary={true}
                            onClick={() => this.onSaveDailog(this)}/>,

                        <Dialog
                            modal={true}
                            open={this.state.LoadMask}>
                            <CircularProgress size={60} style={{left: '50%', top: '50%'}} thickness={7} />
                        </Dialog>
                        
                    </Flex>

                </Flex>
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
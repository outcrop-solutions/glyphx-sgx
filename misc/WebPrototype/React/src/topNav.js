import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import FilterNav from './filterNav';
import {Flex} from 'react-flex-material';
import AppBar from 'material-ui/AppBar';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import './topNav.css';

injectTapEventPlugin();


class TopNav extends Component {
    state = {
        LoadMask : false
    };

    showLoadMask = () => {
        document.getElementById("LoadMask").style.visibility = "visible";
    };

    hideLoadMask = () => {

        var lm = document.getElementById("ipl-progress-indicator");
        if(lm) {
            setTimeout(() => {
                document.getElementById("ipl-progress-indicator").classList.add('available');
                setTimeout(() => {
                document.getElementById("ipl-progress-indicator").outerHTML = '';
                }, 2000)
            }, 1000)
        }

    };

    componentDidMount() {
        this.hideLoadMask();
    }

    render() {
        return (
            <MuiThemeProvider> 
                <div style={{width:'100%',height:'100%'}}>
                    
                    <div 
                        id="LoadMask1"  
                        style={{ 
                            position: 'absolute',    
                            width: '100%',
                            height: '100%',
                            zIndex: 1500,
                            backgroundColor: '#000000',
                            opacity: 0.5,
                            textAlign: 'center',
                            visibility: 'hidden',
                            display: 'table'
                        }} >

                        <CircularProgress id="innerLoadMask"
                            size={60} 
                            style={
                                {
                                        display: 'table-cell',
                                        verticalAlign: 'middle'
                                }
                            }
                            thickness={7} />

                    </div>
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
                            
                        </Flex>

                    </Flex>

                </div>
          </MuiThemeProvider>
        );
    }
}

// Hides the filter side nav by translating it off the screen so it doesnt resize and 
// wont have to be reloaded after it is "closed"
function toggleNav() {
    if (document.getElementById("filterNav").style.transform === "translate(460px, 0px)") {
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
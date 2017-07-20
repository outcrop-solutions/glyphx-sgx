import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import FilterNav from './filterNav';
import {Flex} from 'react-flex-material';
import AppBar from 'material-ui/AppBar';
import {Toolbar, ToolbarGroup, ToolbarSeparator, ToolbarTitle} from 'material-ui/Toolbar';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import FloatingActionButton from 'material-ui/FloatingActionButton';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import { connect } from 'react-redux';
import DropDownMenu from 'material-ui/DropDownMenu';
import {RadioButton, RadioButtonGroup} from 'material-ui/RadioButton';
import MenuItem from 'material-ui/MenuItem';
import './topNav.css';

injectTapEventPlugin();


class TopNav extends Component {
    state = {
        LoadMask : false,
        openSettings: false,
        themeSelection: 0,
        themeTempSelection: 0,
        glyphViewLoaded: false,
        menuOpen: false
    };

    showLoadMask = () => {
        document.getElementById("LoadMask").style.visibility = "visible";
    };

    hideLoadMask = () => {

        var lm = document.getElementById("ipl-progress-indicator");
        if (lm) {
            setTimeout(() => {
                document.getElementById("ipl-progress-indicator").classList.add('available');
                setTimeout(() => {
                document.getElementById("ipl-progress-indicator").outerHTML = '';
                }, 2000)
            }, 1000)
        }

    };

    componentDidMount() {
        var context = this;
        context.timeout = window.setInterval(function(){
            if(context.state.glyphViewLoaded){
                context.hideLoadMask();
                clearInterval(context['timeout']);
            }
        },250);
    }

    onLoadGlyphView(){
        this.setState({
            glyphViewLoaded: true
        });
    }

    promptSelectChange = (event, index, value) => this.handleSelectChange(event, index, value);

    handleSelectChange(event, index, value) {
        this.setState({ themeTempSelection: value });
        
    }

    onSettingsSave() {
        this.setState({ themeSelection: this.state.themeTempSelection, openSettings: false });
        this.props.dispatch(editThemeSettings(this.state.themeTempSelection));
    }

    openCloseMenu() {
        var menuItems = document.getElementsByClassName('toggleOptionsMenuItems');
        var len = menuItems.length;
        var translate = 70;

        if(this.state.menuOpen)
        {
            for(var i=0;i<len;i++){
                menuItems[i].style.transform = '';
            }
            this.setState({menuOpen:false});
        }
        else{
            for(var i=0;i<len;i++){
                menuItems[i].style.transform = 'translate(0px,-'+translate+'px)';
                translate = translate + 50;
            }
            this.setState({menuOpen:true});
        }
    }

    render() {
        return (
            <MuiThemeProvider> 
                <div style = {{ width:'100%', height:'100%' }}>
                    
                    <div 
                        id = "LoadMask1"  
                        style = {{ 
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
                            size = { 60 } 
                            style = {{
                                display: 'table-cell',
                                verticalAlign: 'middle'
                            }}
                            thickness = { 7 } 
                        />
                    </div>

                    <Flex layout = "column" style = {{ position:'absolute', width:'100%', height:'100%' }}>

                        <Flex >
                            <div className = "TopNav" style = {{ width:'100%', height:'100%' }}>
								
								<Toolbar className="navbar-color" style = {{ padding: '0px', backgroundColor: this.props.settings.topNavbar.barBackground}}>
									<ToolbarGroup>
										<span style = { styles.navLogo }>
											<a href = "http://www.synglyphx.com/" target = "_blank" rel = "noopener noreferrer">
												<img src = "./Res/Img/synglyphx-wht-3.png" style = {{ width: '300px' }} alt = "SynGlyphX"/>
											</a>
										</span>
									</ToolbarGroup>
									
									{/*<ToolbarGroup>
										<IconButton style={{color:'white'}}>
											<FontIcon className="fa fa-arrows fa-2x" style={{color:'white'}}/>
										</IconButton>
										<IconButton style={{color:'white'}}>
											<FontIcon className="fa fa-video-camera fa-2x" style={{color:'white'}}/>
										</IconButton>
										<IconButton  style={{color:'white'}}>
											<FontIcon className="fa fa-user-o fa-2x" style={{color:'white'}}/>
										</IconButton>
										<IconButton  style={{color:'white'}}>
											<FontIcon className="fa fa-filter fa-2x" style={{color:'white'}}/>
										</IconButton>
										<IconButton  style={{color:'white'}}>
											<FontIcon className="fa fa-filter fa-2x" style={{color:'white'}}/>
										</IconButton>
										<IconButton style={{color:'white'}}>
											<FontIcon className="fa fa-filter fa-2x" style={{color:'white'}}/>
										</IconButton>
									</ToolbarGroup>
									*/}
									
									<ToolbarGroup>
										<ToolbarSeparator />
										<IconButton onClick={toggleNav}>
											<FontIcon className="fa fa-filter fa-2x" color='white'/>
										</IconButton>

                                        <IconButton onClick = { () => this.setState({ openSettings: true }) }>
											<FontIcon className="fa fa-cogs fa-2x" color='white'/>
										</IconButton>
                                        
                                        <IconButton onClick={toggleNav}>
											<FontIcon className="fa fa-info fa-2x" color='white'/>
										</IconButton>
                                        
                                        <IconButton onClick={openUserProfileMenu}>
											<FontIcon className="fa fa-user fa-2x" color='white'/>
										</IconButton>
                                        
                                        
									</ToolbarGroup>
								</Toolbar>

                                <div 
                                    id="filterNav" 
                                    className="sidenav sidenavbar"
                                >
                                    <FilterNav></FilterNav>
                                </div>

                                <Dialog
                                    title = "Settings"
                                    actions = {
                                        [
                                            <FlatButton
                                                label = "Save"
                                                primary = { true }
                                                onClick = { () => this.onSettingsSave() }
                                                style = {{ color: this.props.settings.settingsModalColor.saveButton }}
                                            />,
                                            <FlatButton
                                                label = "Cancel"
                                                primary = { true }
                                                onClick = { () => this.setState({ openSettings: false, themeTempSelection: this.state.themeSelection }) }
                                                style = {{ color: this.props.settings.settingsModalColor.cancelButton }}
                                            />
                                        ]
                                    }
                                    modal = { true }
                                    open = { this.state.openSettings }
                                >
                                    <DropDownMenu
                                        value = {this.state.themeTempSelection}
                                        onChange = { this.promptSelectChange }
                                        style = {{ width: "300px" }}
                                        autoWidth = { false }
                                    >
                                        <MenuItem value = { 0 } primaryText = "SynGlyphX" />
                                        <MenuItem value = { 1 } primaryText = "SynGlyphX Reversed" />
                                        <MenuItem value = { 2 } primaryText = "Gannon" />
                                        <MenuItem value = { 3 } primaryText = "Gannon Reversed" />
                                    </DropDownMenu>

                                    

                                </Dialog>
                                
                            </div>
                        </Flex>

                        <Flex flex = "100" style = {{ overflow: 'hidden' }}>
                            {/* The 3D rendering engine */}

                            <iframe 
                                id="GlyphViewer" 
                                onLoad={this.onLoadGlyphView.bind(this)} 
                                title = "3D rendering engine" 
                                style = {{ transition:'1s' ,width:'100%', height:'100%' }} 
                                src = "https://s3.amazonaws.com/synglyphx/demo.html" 
                            /> 
                            <FloatingActionButton 
                                style={{
                                    bottom: '20px',
                                    left: '20px',
                                    position: 'absolute',
                                    zIndex: '10',
                                    transition: '0.5s'
                                }} 
                                onClick={this.openCloseMenu.bind(this)}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                style={{
                                    bottom: '25px',
                                    left: "30px",
                                    position: 'absolute',
                                    zIndex: '5',
                                    transition: '0.5s'
                                }} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                style={{
                                    bottom: '25px',
                                    left: "30px",
                                    position: 'absolute',
                                    zIndex: '5',
                                   transition: '0.5s'
                                }} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                style={{
                                    bottom: '25px',
                                    left: "30px",
                                    position: 'absolute',
                                    zIndex: '5',
                                    transition: '0.5s'
                                }} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                style={{
                                    bottom: '25px',
                                    left: "30px",
                                    position: 'absolute',
                                    zIndex: '5',
                                    transition: '0.5s'
                                }} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                style={{
                                    bottom: '25px',
                                    left: "30px",
                                    position: 'absolute',
                                    zIndex: '5',
                                    transition: '0.5s'
                                }} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                        </Flex>
                    </Flex>
                </div>
          </MuiThemeProvider>
        );
    }
}

function openUserProfileMenu(){
	//Open a userProfileMenu.
}

// Hides the filter side nav by translating it off the screen so it doesnt resize and 
// wont have to be reloaded after it is "closed"
function toggleNav() {
    var gv = document.getElementById('GlyphViewer');
    var filterNav = document.getElementById("filterNav");
    var width = gv.clientWidth - filterNav.clientWidth;

    if (filterNav.style.transform === "translate(460px, 0px)") {
        
        gv.style.width = width + "px";
        filterNav.style.transform = "translate(0px, 0px)";
    }
    else {
        
        gv.style.width = "100%";
        filterNav.style.transform = "translate(460px, 0px)";
    }
}

const styles = {
    navLogo: {
        cursor: 'pointer',
    },
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editThemeSettings = (theme) => ({
    type: 'EDIT_THEME',
    theme
});

/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(TopNav);
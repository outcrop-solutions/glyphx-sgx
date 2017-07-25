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
        menuOpen: false,
        overlapFilterNav: true,
        iframeWidthNonOverlap: 0,
		fullScreenMode: false,
		topNavHeight: 0
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

        var gv = document.getElementById('GlyphViewer');
        var filterNav = document.getElementById("filterNav");
        var topNav = document.getElementById("TopNav");
        var width = gv.clientWidth - filterNav.clientWidth;

        this.setState({iframeWidthNonOverlap: width, topNavHeight: topNav.clientHeight});
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

        //Handle change in OverLapping of filterNav
        var overlap = this.refs["radioControl"].getSelectedValue();
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;
        this.setState({overlapFilterNav: overlap});

        if(filterNavOpen)
        {
            this.updateGlyphViewer(overlap);
        }

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

    updateGlyphViewer(fullWidth,fullHeight){
        var gv = document.getElementById('GlyphViewer');
		var topNav = document.getElementById('TopNav');

        if(fullWidth){
            gv.style.width = "100%";
        }
        else {
            gv.style.width = this.state.iframeWidthNonOverlap + "px";
        }
		
		if(fullHeight){
			//gv.style.height = document.body.clientHeight + "px";
			topNav.style.height = '0px';
			topNav.style.overflow = 'hidden';
		}
		else if(fullHeight != null){
			topNav.style.height = this.state.topNavHeight + "px";
			topNav.style.overflow = '';
			//gv.style.height = (document.body.clientHeight - topNav.clientHeight) + "px";
		}
		
    }

    // Hides the filter side nav by translating it off the screen so it doesnt resize and 
    // wont have to be reloaded after it is "closed"
    toggleNav() {
        var gv = document.getElementById('GlyphViewer');
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;
        var overlap = this.state.overlapFilterNav;
        
        if (!filterNavOpen) {
            //open the filterNav sidebar
            filterNav.style.transform = "translate(0px, 0px)";
        }
        else {
            filterNav.style.transform = "translate(460px, 0px)";
        }

        if(!overlap)
            this.updateGlyphViewer(filterNavOpen);
    }
	
	toggleFullScreenMode(evt){
		var topNavBar = document.getElementById('TopNav');
		var topNavBarRef = this.refs['topNavToolbar'];
		var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;
		var iconDiv = evt && evt.currentTarget ? evt.currentTarget.querySelector('.fa') : null;
		
		if(!this.state.fullScreenMode){
			
			//Hide the filterSideNav if visible
			if(filterNavOpen)
				this.toggleNav();
			
			//Update the glyphviewer
			this.updateGlyphViewer(true,true);
			if(iconDiv){
				debugger;
				iconDiv.classList.remove('fa-arrows-alt');
				iconDiv.classList.add('fa-compress');
				
				//evt.currentTarget.classList.remove fa-compress
			}
			this.setState({fullScreenMode: true});
		}
		else{
			//Update the glyphviewer
			this.updateGlyphViewer(true,false);
			
			iconDiv.classList.add('fa-arrows-alt');
			iconDiv.classList.remove('fa-compress');
			
			this.setState({fullScreenMode: false});
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
                            <div className = "TopNav" id="TopNav" style = {{ width:'100%', height:'100%',transition: '1s' }}>
								
								<Toolbar 
									className = "navbar-color" 
									style = {{ padding: '0px', backgroundColor: this.props.settings.topNavbarColor.barBackground }}
									ref= "topNavToolbar"
								>
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
										<IconButton onClick={this.toggleNav.bind(this)}>
											<FontIcon className="fa fa-filter fa-2x" color='white'/>
										</IconButton>

                                        <IconButton onClick = { () => this.setState({ openSettings: true }) }>
											<FontIcon className="fa fa-cogs fa-2x" color='white'/>
										</IconButton>
                                        
                                        <IconButton onClick={this.toggleNav.bind(this)}>
											<FontIcon className="fa fa-info fa-2x" color='white'/>
										</IconButton>
                                        
                                        <IconButton onClick={openUserProfileMenu}>
											<FontIcon className="fa fa-user fa-2x" color='white'/>
										</IconButton>
                                        
                                        
									</ToolbarGroup>
								</Toolbar>

                                <div 
                                    id  ="filterNav" 
                                    className = "sidenav sidenavbar"
                                >
                                    <FilterNav></FilterNav>
                                </div>

                                <Dialog
                                    title = "Settings"
                                    ref = "Settings"
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
                                <label><h4> Theme Settings </h4></label>
                                    <DropDownMenu
                                        value = { this.state.themeTempSelection }
                                        onChange = { this.promptSelectChange }
                                        iconStyle = {{ fill: this.props.settings.settingsModalColor.text}}
                                        underlineStyle = {{ borderColor: this.props.settings.settingsModalColor.text }}
                                        selectedMenuItemStyle = {{ backgroundColor: this.props.settings.settingsModalColor.selectedBackground, color: this.props.settings.settingsModalColor.selectedText}}
                                    >
                                        <MenuItem value = { 0 } primaryText = "SynGlyphX" />
                                        <MenuItem value = { 1 } primaryText = "Gannon" />
                                    </DropDownMenu>

                                    <br />
                                    <label><h4> Overlap Settings </h4></label>

                                    <RadioButtonGroup 
                                        name="Overlap Control" 
                                        defaultSelected={this.state.overlapFilterNav}
                                        ref = "radioControl"
                                    >
                                        <RadioButton
                                            value={true}
                                            label="Overlap Filter Window(Does Not Resize the GlyphViewer)"
                                        />
                                        <RadioButton
                                            value={false}
                                            label="Not Overlap Filter Window(Resizes the GlyphViewer)"
                                        />
                                    </RadioButtonGroup>

                                </Dialog>
                                
                            </div>
                        </Flex>

                        <Flex id="iframeDiv" flex = "100" style = {{ overflow: 'hidden' }}>
                            {/* The 3D rendering engine */}

                            <iframe 
                                id="GlyphViewer" 
                                onLoad={this.onLoadGlyphView.bind(this)} 
                                title = "3D rendering engine" 
                                style = {{ transition:'1s' ,width:'100%', height:'100%' }} 
                                src = "https://s3.amazonaws.com/synglyphx/demo.html" 
                            /> 
                            <FloatingActionButton 
                                backgroundColor= {this.props.settings.overviewButtonsColor.background}
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
                                backgroundColor= {this.props.settings.overviewButtonsColor.background}
                                style={styles.floatingMiniStyles} 
                                className="toggleOptionsMenuItems"
                                mini={true}
								onClick={this.toggleFullScreenMode.bind(this)}
                            >

                                <i className = "fa fa-arrows-alt" style = {{ fontSize: '1rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                backgroundColor= {this.props.settings.overviewButtonsColor.background}
                               style={styles.floatingMiniStyles} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >
							{/*//fa-eye-slash(for the alternate toggle icon) */}
                                <i className = "fa fa-eye" style = {{ fontSize: '1rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
                                backgroundColor= {this.props.settings.overviewButtonsColor.background}
                                style={styles.floatingMiniStyles} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-video-camera" style = {{ fontSize: '1rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
								backgroundColor= {this.props.settings.overviewButtonsColor.background}
                                style={styles.floatingMiniStyles} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                            
                            <FloatingActionButton 
								backgroundColor= {this.props.settings.overviewButtonsColor.background}
                                style={styles.floatingMiniStyles} 
                                className="toggleOptionsMenuItems"
                                mini={true}
                            >

                                <i className = "fa fa-pencil" style = {{ fontSize: '1rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                            
                            </FloatingActionButton>
                        </Flex>
                    </Flex>
                </div>
          </MuiThemeProvider>
        );
    }
}

const floatingMiniStyles = {
	
};

function openUserProfileMenu(){
	//Open a userProfileMenu.
}

const styles = {
    navLogo: {
        cursor: 'pointer',
    },
	floatingMiniStyles: {
		bottom: '25px',
		left: "28px",
		position: 'absolute',
		zIndex: '5',
		transition: '0.5s'
	}
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
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(TopNav);
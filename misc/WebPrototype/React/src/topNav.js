import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import FilterNav from './filterNav';
import {Flex} from 'react-flex-material';
import {Toolbar, ToolbarGroup, ToolbarSeparator} from 'material-ui/Toolbar';
import IconButton from 'material-ui/IconButton';
import Badge from 'material-ui/Badge';
import List from 'material-ui/List/List';
import ListItem from 'material-ui/List/ListItem';
import Popover from 'material-ui/Popover';
import Avatar from 'material-ui/Avatar';
import FontIcon from 'material-ui/FontIcon';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import FloatingActionButton from 'material-ui/FloatingActionButton';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import { connect } from 'react-redux';
import Login from './Login.js';
import DropDownMenu from 'material-ui/DropDownMenu';
import {RadioButton, RadioButtonGroup} from 'material-ui/RadioButton';
import MenuItem from 'material-ui/MenuItem';
import StatisticModal from './StatisticModal.js'
import AnnouncementDialog from './AnnouncementDialog.js'
import './topNav.css';
import 'font-awesome/css/font-awesome.min.css';

injectTapEventPlugin();


class TopNav extends Component {
    
	//initial state of the component.
	state = {
        openSettings: false,
        themeSelection: 0,
        themeTempSelection: 0,
        glyphViewLoaded: false,
        menuOpen: false,
        overlapFilterNav: true,
        iframeWidthNonOverlap: 0,
		fullScreenMode: false,
        topNavHeight: 0,
        userProfileMenuOpen:false,
        userInfoAnchorEl: {},
        authenticate:true,
        imgLogoSrc: <a href = "http://www.synglyphx.com/" target = "_blank" rel = "noopener noreferrer">
						<img src = "./Res/Img/synglyphx-wht-3.png" style = {{ width: '300px' }} alt = "SynGlyphX"/>
					</a>
    };
	
	/**
	 * This function shows the in app loadmask(cicular waiting).
	 */
    showLoadMask = () => {
        document.getElementById("LoadMask").style.visibility = "visible";
    };
	
	/**
	 * This function hides the initial loadmask/splash screen.
	 */
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
	
	/**
	 * This function is called right after the react component is mounted.
	 * It decides whether to show the login form and calls the init().
	 */
    componentDidMount() {
        this.refs['LoginForm'] ? this.refs['LoginForm'].getWrappedInstance().showLoginForm() : null;
        this.init();
    }
	
	/**
	 * This function does all the initialization that is needed.
	 */
    init(){
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
	
	/**
	 * OnLoad of the glyphviewer(iframe) we set this to true so that the loadmask can hide.
	 */
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
	
	/**
	 * This function opens/closes the floating menu on the bottom left corner.
	 */
    openCloseFloatingMenu() {
        var menuItems = document.getElementsByClassName('toggleOptionsMenuItems');
        var len = menuItems.length;
        var translate = 70; // as the 1st menu button should be little more higher than the spacing between the buttons.

        if(this.state.menuOpen)
        {
            for(var i=0;i<len;i++){
                menuItems[i].style.transform = '';
            }
            this.setState({menuOpen:false});
        }
        else{
            for(var j=0;j<len;j++){
                menuItems[j].style.transform = 'translate(0px,-'+translate+'px)';
                translate = translate + 50;
            }
            this.setState({menuOpen:true});
        }
    }
	
	/**
	 * This function updates the height and width of the glyphviewer(iframe).
	 * @param {bool} fullWidth- true if wanted full screen & false/null otherwise.
	 * @param {bool} fullHeight- true if wanted full screen & false/null otherwise.
	 */
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
			topNav.style.height = '0px';
			topNav.style.overflow = 'hidden';
		}
		else if(fullHeight != null){
			topNav.style.height = this.state.topNavHeight + "px";
			topNav.style.overflow = '';
		}
		
    }

    /**
	 * Hides the filter side nav by translating it off the screen so it doesnt resize and 
     * wont have to be reloaded after it is "closed"
	 */ 
    toggleNav() {
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
	
	/**
	 * This function toggles the full screen mode of the glyphviewer(iframe).
	 * @param {object} event- event object.
	 */
	toggleFullScreenMode(evt){
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
				iconDiv.classList.remove('fa-arrows-alt');
				iconDiv.classList.add('fa-compress');
			}

			this.setState({fullScreenMode: true});
		}
		else{
			//Update the glyphviewer
			this.updateGlyphViewer(true,false);

			if(iconDiv){
				iconDiv.classList.add('fa-arrows-alt');
			    iconDiv.classList.remove('fa-compress');
				
			}
			
			this.setState({fullScreenMode: false});
		}
		
    }
    
    /**
	 * This function is a callback from the login.
	 * It calls the necessary function that need to be called after login.
	 * @param {object} props - this is the user info object returned from the login call.
	 */
    doAfterLogin = (props) => {
        var ann = this.refs['announcements'] ? this.refs['announcements'].getWrappedInstance() : null;
        
        
        //Update user information in the top right user info menu.
        this.updateUserInfoMenu(props);

        //Check if announcements and force display at login true.
        if(ann && ann.state.forceDisplayAfterLogin)
        {
            ann.setState({
				currentVisibility: true,
			});
        }
    }

	/**
	 * This updates the userinfo menu with the user data returned from login call.
	 * Also depending on the user product the logo will be displayed.(GlyphEd or SynGlyphX)
	 * @param {object} userInfo - this is the user info object returned from the login call.
	 */
    updateUserInfoMenu = (userInfo) => {
        //update user info.
        if(userInfo)
        {
            //type of product 
            if(userInfo.product == "GlyphEd"){
                this.setState({
                    imgLogoSrc : 
                    <a href = "http://www.glyphed.co/" target = "_blank" rel = "noopener noreferrer">
						<img src = "./Res/Img/GlyphED-wht-3.png" style = {{ width: '200px' }} alt = "GlyphEd"/>
					</a>
                });
            }
            else{
                 this.setState({
                    imgLogoSrc : 
                    <a href = "http://www.synglyphx.com/" target = "_blank" rel = "noopener noreferrer">
						<img src = "./Res/Img/synglyphx-wht-3.png" style = {{ width: '300px' }} alt = "SynGlyphX"/>
					</a>
                });
            }

            this.setState({
                userFullName : userInfo.firstName + " " + userInfo.lastName,
                userInitials : (userInfo.firstName ? userInfo.firstName[0] : "") + (userInfo.lastName ? userInfo.lastName[0] : "")
            });
        }
        
    }
	
	/**
	 * This function displays the announcements dialog.
	 */
    displayAnnouncements = () => {
		//user specific Announcements.
		var dialogBox;
		if(this.refs['announcements'])
		{
			dialogBox = this.refs['announcements'].getWrappedInstance();
			dialogBox.setState({
				currentVisibility: true,
				displayCheckBox: false
			});
		}
    }
	
	/**
	 * Open/Close the userinfo menu.
	 * @param {object} event- event object.
	 */
    openCloseUserInfoMenu = (event) => {
        // This prevents ghost click.
        if(event && event.preventDefault){
            event.preventDefault();
            this.setState({
            userProfileMenuOpen : !this.state.userProfileMenuOpen,
            userInfoAnchorEl: event.currentTarget
        });
        }
        else{
            this.setState({
            userProfileMenuOpen : !this.state.userProfileMenuOpen
        });
        }
            

        
        
    }

    render() {
        

        return (
            <MuiThemeProvider> 
                <div style = {{ width:'100%', height:'100%' }}>
					
					{/* Login Screen */}
                    {this.state.authenticate ? 
                        <Login ref="LoginForm"
                            doAfterLogin={(prop) => this.doAfterLogin(prop)}
                        /> : null}
						
					{/* Circular Load Mask to show when server calls made. */}
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
					
					{/* Actual Application body that you see */}
                    <Flex layout = "column" style = {{ position:'absolute', width:'100%', height:'100%' }}>

                        <Flex >
                            <div className = "TopNav" id="TopNav" style = {{ width:'100%', height:'56px',transition: '1s' }}>
								
								
								{/* Top Toolbar */}
								<Toolbar 
									className = "navbar-color" 
									style = {{ padding: '0px', backgroundColor: this.props.settings.topNavbarColor.barBackground }}
									ref= "topNavToolbar"
								>
									{/* Logo */}
									<ToolbarGroup>
										<span style = { styles.navLogo }>
												{this.state.imgLogoSrc}
										</span>
									</ToolbarGroup>
									
									{/* Top Right icons */}
									<ToolbarGroup>
										<ToolbarSeparator />
										<IconButton onClick={this.toggleNav.bind(this)}>
											<FontIcon className="fa fa-filter fa-2x" color='white'/>
										</IconButton>

                                        <IconButton onClick = { () => this.setState({ openSettings: true }) }>
											<FontIcon className="fa fa-cogs fa-2x" color='white'/>
										</IconButton>
                                        
                                        <IconButton onClick={this.displayAnnouncements.bind(this)}>
                                            <FontIcon className="fa fa-bell fa-2x" color='white'/> 
                                            <FontIcon id="notificationBadge" className="fa fa-exclamation-circle fa-1x notificationBadge" />
                                        </IconButton>
                                        
                                        <IconButton onClick = {(event) => this.openCloseUserInfoMenu(event) }>
											<FontIcon className = "fa fa-user fa-2x" color = 'white'/>
										</IconButton>
                                        
                                        <Popover
                                            open={this.state.userProfileMenuOpen}
                                            anchorEl={this.state.userInfoAnchorEl}
                                            onRequestClose={this.openCloseUserInfoMenu}
                                            anchorOrigin= {{"horizontal":"right","vertical":"bottom"}}
                                            targetOrigin= {{"horizontal":"right","vertical":"top"}}
                                        >
                                            <List>
                                                <ListItem
                                                    disabled={false}
                                                    leftAvatar={
                                                        <Avatar
                                                            //src="./Res/Img/x.png"
                                                        >
                                                            {this.state.userInitials}
                                                        </Avatar>
                                                    }
                                                    primaryText= {this.state.userFullName}
                                                    secondaryText= {this.props.userInfo ? this.props.userInfo.type : null}
                                                >
                                                    
                                                </ListItem>
                                                <MenuItem className="menuItemStyling" primaryText="Help &amp; feedback" />
                                                <MenuItem className="menuItemStyling" primaryText="Settings" />
                                                <MenuItem className="menuItemStyling" primaryText="Sign out" />
                                            </List>
                                        </Popover>
                                        
									</ToolbarGroup>
								</Toolbar>
								
								{/* Filter Side Bar */}
                                <div 
                                    id  = "filterNav" 
                                    className = "sidenav"
                                >
                                    <FilterNav></FilterNav>
                                </div>
								
								{/* Announcements Dialog */}
								<AnnouncementDialog ref='announcements'/>
								
								{/* Settings Dialog */}
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

                            <StatisticModal />

                            <iframe 
                                id="GlyphViewer" 
                                onLoad={this.onLoadGlyphView.bind(this)} 
                                title = "3D rendering engine" 
                                style = {{ transition:'1s' ,width:'100%', height:'100%', border: 'none' }} 
                                src = "https://s3.amazonaws.com/synglyphx/demo.html" 
                            /> 
							
							{/* Floating buttons bottom left */}
                            <FloatingActionButton 
                                backgroundColor= {this.props.settings.overviewButtonsColor.background}
                                style={{
                                    bottom: '20px',
                                    left: '20px',
                                    position: 'absolute',
                                    zIndex: '10',
                                    transition: '0.5s'
                                }} 
                                onClick={this.openCloseFloatingMenu.bind(this)}
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
                        </Flex>
                    </Flex>
                </div>
          </MuiThemeProvider>
        );
    }
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
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(TopNav);
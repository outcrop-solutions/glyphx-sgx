import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { Toolbar, ToolbarGroup, ToolbarSeparator } from 'material-ui/Toolbar';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
import List from 'material-ui/List/List';
import ListItem from 'material-ui/List/ListItem';
import Popover from 'material-ui/Popover';
import MenuItem from 'material-ui/MenuItem';
import Avatar from 'material-ui/Avatar';
import SettingsModal from './SettingsModal.js';
import AlertsModal from './AlertsModal.js'
import HelpModal from './HelpModal.js';
import AdminWizardModal from './AdminWizardModal.js';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './General.css';


/**
 * -ADCMT
 * @param homePage: -ADCMT
 * @param tutorialStage: -ADCMT
 **/
class TopNavBar extends React.Component {

	state = {
        userProfileMenuOpen: false,
        userInfoAnchorEl: {},
        displayAlertsCheckbox: true,
        imgLogoSrc: <img src = "./Res/Img/GlyphED-wht-3.png" style = {{ width: '200px' }} alt = "GlyphEd" className = "noselect" draggable = { false } />

    };
    

    /**
     * Performs a logout by redirecting the site to the loagout page
     **/
    logout = () => {
        this.props.history.push("/logout");
    };


    /**
     * Opens the Admin wizard and closes the user menu
     **/
    openAdminWizard() {
        this.props.dispatch(editModalDisplay(null, null, null, true));
        this.setState({ userProfileMenuOpen: false });
    }


    /**
	 * Hides the filter side nav by translating it off the screen so it doesnt resize and 
     * Wont have to be reloaded after it is "closed"
	 */ 
    toggleNav() {
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;
        var gv = document.getElementById('GlyphViewerContainer');

        var sidebarButton = document.getElementById("showSideBar");
        sidebarButton.style.display = "";

        if (!filterNavOpen) {
            //open the filterNav sidebar
            filterNav.style.transform = "translate(0px, 0px)";
            if (!this.props.settings.sideBarOverlap) {
                gv.style.width = "calc(100% - 450px)";
            }

            else {
                gv.style.width = "100%";
            }
        }

        else {
            filterNav.style.transform = "translate(460px, 0px)";
            gv.style.width = "100%";
        }
    }


    /**
	 * Toggles the visibility of the user menu.
	 * @param {object} e: event object.
	 */
    ToggleUserInfoMenu = (e) => {
        // This prevents ghost click.
        if (e && e.preventDefault) {
            e.preventDefault();
            this.setState({ userProfileMenuOpen : !this.state.userProfileMenuOpen, userInfoAnchorEl: e.currentTarget });
        }

        else {
            this.setState({ userProfileMenuOpen : !this.state.userProfileMenuOpen });
        }
    }


    /**
	 * Displays the alerts modal
	 */
    displayAlerts() {
        this.props.dispatch(editModalDisplay(null, true, null, null)); 
        this.setState({ displayAlertsCheckbox: false });
    }


    onClickLogo() {
        this.props.history.push('/home');
    }


    render() {
        return(
            <Toolbar 
                style = {{ padding: '0px', height: "36px", backgroundColor: this.props.settings.colors.topNavbarColor.barBackground }}
                ref = "topNavToolbar"
            >
                {/* Logo */}
                <ToolbarGroup style = {{ zIndex: (this.props.tutorialStage === 8 ? "300" : "5") }} >
                    <span style = {{ cursor: 'pointer' }} onClick = { () => this.onClickLogo() } >
                        {this.props.homePage ? 
                            this.state.imgLogoSrc
                            :
                            <Tooltip
                                placement = 'left'
                                mouseEnterDelay = { 0.5 }
                                mouseLeaveDelay = { 0.15 }
                                destroyTooltipOnHide = { false }
                                trigger = { Object.keys( {hover: 1} ) }
                                overlay = { 
                                    <div> 
                                        Return to home page.
                                    </div> 
                                }
                            >
                                <i 
                                    className = "fa fa-home"
                                    style = {{
                                        fontSize: '1.8em',
                                        color: this.props.settings.colors.collapsibleColor.mainIcon,
                                        margin: "0px 0px 1px 8px"
                                    }}
                                /> 
                            </Tooltip>
                        }
                    </span>
                </ToolbarGroup>
                
                {/* Top Right icons */}
                <ToolbarGroup>
                    <ToolbarSeparator />

                    <IconButton 
                        //onClick = { () => this.props.dispatch(editModalDisplay(null, null, true, null)) } 
                        onClick = { () => window.open("https://synglyphx.atlassian.net/servicedesk/customer", '_blank') }
                        style = {{ zIndex: (this.props.tutorialStage === 4 ? "300" : "5") }}
                        className = { (this.props.tutorialStage === 4 ? "pulse" : "") }
                    >
                        <FontIcon className = "fa fa-question-circle fa-2x" color = '#ffffff' />
                    </IconButton>

                    <IconButton 
                        onClick = { () => this.props.dispatch(editModalDisplay(true, null, null, null)) } 
                        style = {{ zIndex: (this.props.tutorialStage === 5 ? "300" : "5") }}
                        className = { (this.props.tutorialStage === 5 ? "pulse" : "") }
                    >
                        <FontIcon className = "fa fa-cogs fa-2x" color = '#ffffff' />
                    </IconButton>
                    

                    {/*
                    <IconButton  
                        onClick = { this.displayAlerts.bind(this) } 
                        style = {{ zIndex: (this.props.tutorialStage === 6 ? "300" : "5") }}
                        className = { (this.props.tutorialStage === 6 ? "pulse" : "") }
                    >
                        <FontIcon className = "fa fa-bell fa-2x" color = '#ffffff' /> 
                        <FontIcon id = "notificationBadge" className = "fa fa-exclamation-circle fa-1x notificationBadge" />
                    </IconButton>
                    */}
                    
                    <IconButton 
                        onClick = { (event) => this.ToggleUserInfoMenu(event) } 
                        style = {{ zIndex: (this.props.tutorialStage === 7 ? "300" : "5") }}
                        className = { (this.props.tutorialStage === 7 ? "pulse" : "") }
                    >
                        <FontIcon className = "fa fa-user fa-2x" color = '#ffffff' />
                    </IconButton>

                    {/*<IconButton onClick = { this.toggleNav.bind(this) } style = {{ margin: "0px 0px 0px -5px", display: (this.props.homePage ? "none" : "") }} >*/}
                        {/*<FontIcon className = "fa fa-caret-up fa-2x" color = '#ffffff' style = {{ transform: 'rotateZ(90deg)', margin: "0px 0px 0px 4px" }} />*/}
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Hide filter column.
                                </div> 
                            }
                        >
                            <i 
                                className = "fa fa-caret-up cursorHand" 
                                style = {{
                                    fontSize: '2.3em',
                                    color: this.props.settings.colors.collapsibleColor.mainIcon,
                                    transform: 'rotateZ(90deg)',
                                    margin: "0px 0px 0px 4px",
                                    display: (this.props.homePage ? "none" : "")
                                }}
                                onClick = { this.toggleNav.bind(this) }
                            /> 
                        </Tooltip>
                    {/*</IconButton>*/}
                </ToolbarGroup>

                {/* User Icon Popover */}
                <Popover
                    open = { this.state.userProfileMenuOpen }
                    anchorEl = { this.state.userInfoAnchorEl }
                    onRequestClose = { this.ToggleUserInfoMenu }
                    anchorOrigin = {{ "horizontal":"right", "vertical":"bottom" }}
                    targetOrigin = {{ "horizontal":"right", "vertical":"top" }}
                >
                    <List>
                        <ListItem
                            disabled = { false }
                            leftAvatar = {
                                <Avatar
                                    //src="./Res/Img/x.png"
                                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                                >
                                    { this.props.userInfo ? (this.props.userInfo.Name.includes(" ") ? this.props.userInfo.Name.charAt(0) + this.props.userInfo.Name.charAt(this.props.userInfo.Name.indexOf(" ") + 1) : this.props.userInfo.Name.charAt(0)) : "" }
                                </Avatar>
                            }
                            primaryText = { this.props.userInfo ? this.props.userInfo.Name : ""}
                        />

                        <MenuItem onClick = { () => this.openAdminWizard() } className = "menuItemStyling" primaryText = "Admin Wizard" />
                        {/* {this.props.userInfo.admin ? show : dont} */}
                        {/* <MenuItem className = "menuItemStyling" primaryText = "User Settings" /> */}
                        <MenuItem onClick = { this.logout } className = "menuItemStyling" primaryText = "Sign out" />
                        
                        
                        
                    </List>
                </Popover>

                {/* Modals */}
                <SettingsModal homePage = { this.props.homePage } />
				<AlertsModal checkBoxDisplay = { this.state.displayAlertsCheckbox } />
                <HelpModal />
                
                {/* {this.props.userInfo.admin ? render : dont} */}
                <AdminWizardModal />

            </Toolbar>          
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (settingsModal, alertsModal, helpModal, adminModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    settingsModal,
    alertsModal,
    helpModal,
    adminModal
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(TopNavBar));
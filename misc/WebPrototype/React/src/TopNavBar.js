import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { Toolbar, ToolbarGroup, ToolbarSeparator } from 'material-ui/Toolbar';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
// import Icon from 'material-ui/core/Icon';
import List from 'material-ui/List/List';
import ListItem from 'material-ui/List/ListItem';
import Popover from 'material-ui/Popover';
import MenuItem from 'material-ui/MenuItem';
import Avatar from 'material-ui/Avatar';
import { deleteCookie, getLoginCookieName, makeServerCall } from './ServerCallHelper.js';
import { hideSplashScreen } from './LoadMaskHelper.js';
import SettingsModal from './SettingsModal.js';
import AlertsModal from './AlertsModal.js'
import HelpModal from './HelpModal.js';
import ShareImg from './images/share.png';
import LinkImg from './images/link.png';
/* import AdminWizardModal from './AdminWizardModal.js'; */
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './css/General.css';


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
        let context = this;
        return new Promise((resolve, reject) => {
            makeServerCall("logout",
            function (responseText) { 
                resolve(true);
            }
        );
        }).then(res =>{
            if(res === true){
                window.location.reload();
                context.props.dispatch(logoutClear());
                deleteCookie(getLoginCookieName());
                hideSplashScreen();
            }
        }).catch(err =>{
            // console.log(err);
        });
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

    returnHome() {
        if (document.getElementById('GlyphViewer')) {
            var iframe = document.getElementById('GlyphViewer').contentWindow;
            iframe.closeSceneView();
        }
        
        this.props.history.push('/home')
    }

    createShareLink(){
        // console.log(this.props.vizParams);
        makeServerCall('createShareLink',
            function(res,b,c) {
                if (typeof res === 'string') res = JSON.parse(res);
                
                // debugger;
                console.log(res);

                // if (Array.isArray(res.body.data) && res.body.data.length > 0) {
                //     // var result = context.convertToCompatibleDataObject(res.data);
                //     // console.log(result);
                //     // context.makeFilterStructure(result);
                //     // context.setState({ tableData: result, loadingDone: true });
                //     // context.props.dispatch(setStatData(result));
                //     // context.props.updateViz(res.glyphViewerKey);
                // }
                // else {
                //     // 0 records matched.
                //     console.log('bad time');
                // }
            },
            {
                post: true, 
                data:  { 
                    sdtPath: this.props.vizParams.sdtPath,
                    tableName: this.props.vizParams.tableName,
                    query: this.props.vizParams.query,
                    datasourceId: this.props.vizParams.datasourceId,
                    filterAllowedColumnList: this.props.vizParams.filterAllowedColumnList,
                    originalVizName: this.props.vizParams.originalVizName,
                    frontEndFilters: this.props.vizParams.frontEndFilters ? this.props.vizParams.frontEndFilters : this.props.vizParams.frontEndFilterString
                }
            }
        );
    }


    render() {
        // console.log(this.props.vizParams);
        return(
            <Toolbar 
                style = {{ padding: '0px', height: "36px", backgroundColor: this.props.settings.colors.topNavbarColor.barBackground }}
                ref = "topNavToolbar"
            >
                {/* Logo */}
                <ToolbarGroup style = {{ zIndex: (this.props.tutorialStage === 8 ? "300" : "5") }} >
                    <span style = {{ cursor: 'pointer' }} onClick = { () => this.returnHome() } >
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
                                        Return to Home Page
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
                        onClick = { () => window.open("https://desk.zoho.com/portal/synglyphx/home", '_blank') }
                        style = {{ zIndex: (this.props.tutorialStage === 4 ? "300" : "5") }}
                        className = { (this.props.tutorialStage === 4 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Help
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-question-circle fa-2x" color = '#ffffff' />
                        </Tooltip>
                    </IconButton>

                    <IconButton 
                        //onClick = { () => this.props.dispatch(editModalDisplay(null, null, true, null)) } 
                        onClick = { () => this.createShareLink() }
                        style = {{ 
                            zIndex: (this.props.tutorialStage === 4 ? "300" : "5"),
                            display: (
                                this.props.userInfo.Admin === 1
                                && Object.keys(this.props.statData).length !== 0  ? "" : "none" )
                        }}
                        className = { (this.props.tutorialStage === 4 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Share
                                </div> 
                            }
                        >
                            <FontIcon >
                                <img style={{backgroundColor: "white", borderRadius: "50%", padding: "1px"}} src={LinkImg}/>
                            </FontIcon>
                        </Tooltip>
                    </IconButton>

                    <IconButton 
                        onClick = { () => this.props.dispatch(editModalDisplay(true, null, null, null)) } 
                        style = {{ zIndex: (this.props.tutorialStage === 5 ? "300" : "5") }}
                        className = { (this.props.tutorialStage === 5 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Settings
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-cogs fa-2x" color = '#ffffff' />
                        </Tooltip>
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
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Profile
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-user fa-2x" color = '#ffffff' />
                        </Tooltip>
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
                                    Hide Filter Side Bar
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

                        {/* <MenuItem onClick = { () => this.openAdminWizard() } className = "menuItemStyling" primaryText = "Admin Wizard" /> */}
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
                {/* <AdminWizardModal /> */}

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

export const logoutClear = () => ({
    type: 'LOGOUT_CLEAR',
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
    statData: state.filterState.StatisticsData,
    vizParams: state.filterState.VizParams,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(TopNavBar));
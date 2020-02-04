/*eslint-env jquery*/
import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { Toolbar, ToolbarGroup, ToolbarSeparator } from 'material-ui/Toolbar';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
// import Icon from 'material-ui/core/Icon';
// import List from 'material-ui/List/List';
// import ListItem from 'material-ui/List/ListItem';
// import Popover from 'material-ui/Popover';
// import MenuItem from 'material-ui/MenuItem';
// import Avatar from 'material-ui/Avatar';
import { deleteCookie, getLoginCookieName, makeServerCall } from './ServerCallHelper.js';
import { webSocketSend } from './GeneralFunctions.js';
import { hideSplashScreen } from './LoadMaskHelper.js';
import SettingsModal from './SettingsModal.js';
import AlertsModal from './AlertsModal.js'
import HelpModal from './HelpModal.js';
// import ShareImg from './images/share.png';
// import LinkImg from './images/link.png';
/* import AdminWizardModal from './AdminWizardModal.js'; */
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './css/General.css';
// import { white } from 'material-ui/styles/colors';


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
        imgLogoSrc: <img src = "./Res/Img/GlyphED-wht-3.png" style = {{ width: '10.07vw' }} alt = "GlyphEd" className = "noselect" draggable = { false } />,
        toggle_profile: false
    };

    /**
     * Performs a logout by redirecting the site to the loagout page
     **/
    logout(){    
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
        // console.log('this one is being called1')
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(23.6vw, 0px)" ? false : true;
        // var gv = document.getElementById('GlyphViewerContainer');

        var sidebarButton = document.getElementById("showSideBar");
        sidebarButton.style.display = "";

        if (!filterNavOpen) {
            //open the filterNav sidebar
            filterNav.style.transform = "translate(0px, 0px)";
            // if (!this.props.settings.sideBarOverlap) {
            //     gv.style.width = "calc(100% - 23.5vw)";
            // }

            // else {
            //     gv.style.width = "100%";
            // }
        }

        else {
            filterNav.style.transform = "translate(23.6vw, 0px)";
            // gv.style.width = "100%";
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
        this.props.dispatch(legendArrClear());
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

    showName () {
        if(this.props.userInfo.Name.length > 0){
            if(this.props.userInfo.Name.indexOf(' ') > 0) {
                return this.props.userInfo.Name.slice(0, this.props.userInfo.Name.indexOf(' '));
            }
            else return this.props.userInfo.Name;
        }
        else return "Guest";
    }

    render() {
        let customer_icon;
        if(this.props.userInfo.institutionDir){
            // customer_icon = 'data:image/png;base64,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'
            customer_icon =  window.SERVER_URL + "customerImg/" + window.encodeURIComponent(this.props.userInfo.institutionDir);
        }
         else {
            customer_icon = (this.props.userInfo.Name.includes(" ") ? this.props.userInfo.Name.charAt(0) + this.props.userInfo.Name.charAt(this.props.userInfo.Name.indexOf(" ") + 1) : this.props.userInfo.Name.charAt(0));
         }
        
        // console.log(this.props.vizParams);
        return(
            <Toolbar 
                style = {{ padding: '0px', height: (this.props.homePage ? "6.72vh" : "3.72vh"), backgroundColor: "#031a72" }}
                ref = "topNavToolbar"
            >       
                <ToolbarGroup style = {{ zIndex: (this.props.tutorialStage === 8 ? "300" : "5") }} >
                    {this.props.homePage ? <div></div> : 
                        <Tooltip
                        onClick = { () => {
                            this.returnHome(); 
                            webSocketSend(this.props.webSocket, this.props.uid, "home");
                        }}
                        placement = 'bottomLeft'
                        mouseEnterDelay = { 0.15 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlay = { 
                            <div style={{height: "1.877vh", width: "1.9vw", fontSize:"1.460vh"}}> 
                                Home
                            </div> 
                        }
                    >
                        <i 
                            className = "fa fa-home"
                            style = {{
                                fontSize: '2.71vh',
                                color: this.props.settings.colors.collapsibleColor.mainIcon,
                                margin: "0.52vh 0px 0.1vh 0.83vh",
                                cursor: "pointer"
                            }}
                        /> 
                    </Tooltip>}

                    <IconButton 
                        onClick = {() => this.setState({toggle_profile: true})}
                        style = {{
                            height: "5.005vh",
                            width: "2.010vw", 
                            zIndex: (this.props.tutorialStage === 7 ? "300" : "5"), 
                            padding: "1.251vh" }}
                        className = { (this.props.tutorialStage === 7 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = {(this.props.homePage ? "right" : "bottomLeft")}
                            mouseEnterDelay = { 0.15 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div style={{height: "1.877vh", width: "1.85vw", fontSize:"1.460vh"}}> 
                                    Profile
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-user fa-2x" color = '#ffffff' 
                                style={{
                                    fontSize: (this.props.homePage ? "2.920vh" : "2.50vh")
                            }} />
                        </Tooltip>
                    </IconButton>
                    <div
                        onMouseLeave = {() => this.setState({toggle_profile : false})} 
                        id="myDropdown" className="dropdown-content" style={{
                            display : (this.state.toggle_profile ? 'block' : 'none'),
                            position: 'absolute',
                            backgroundColor: 'white',
                            minWidth: '10.885vw',
                            overflow: 'auto',
                            boxShadow: '0px 0.826vh 1.651vh 0px rgba(0,0,0,0.2)',
                            zIndex: '1',
                            top: (this.props.homePage ? "5.263vh" : "3.863vh"),
                            fontSize: "2.064vh",
                            left: "0.206vh"
                    }}>
                        <div style={{height: "5.779vh", fontSize: "1.98vh"}} className="showUserMenu">
                                <img style={{
                                    margin: "auto 1.032vh auto 1.032vh",
                                    borderRadius: "50%",
                                    fontSize: "2.064vh", 
                                    height: "4.128vh",
                                    width: "4.128vh",
                                    top: "0.826vh",
                                    left: "1.651vh",
                                    wordBreak: "break-word"
                                }} alt="User Profile Icon" src={customer_icon} />
                                <span style={{margin: "auto auto auto 0"}}>
                                    { this.props.userInfo ? this.props.userInfo.Name : ""}
                                </span>
                        </div>
                        <div 
                            style={{height: "3.096vh", fontSize: "1.651vh", cursor: "pointer"}} 
                            className="showUserMenu signoutUser"  
                            onClick = {() => {this.logout(); webSocketSend(this.props.webSocket, this.props.uid, "logout");} } >
                                <span style={{margin: "auto 0 auto 1.032vh"}}>
                                    SIGN OUT
                                </span>
                        </div>
                    </div>
                    
                    {this.props.homePage ? <div style={{
                        color: "white",
                        fontFamily: "ITCFranklinGothicStd-DmCd", 
                        fontSize: "2.086vh", 
                        margin: "1.1vh 0px 0px 1.042vh",
                        letterSpacing: "0.5px"}}
                    >
                        Welcome, {this.showName()}
                    </div> : <div></div>}
                </ToolbarGroup>

                {/* Logo */}
                <ToolbarGroup style = {{ zIndex: (this.props.tutorialStage === 8 ? "300" : "5") }} >
                    <span 
                        style = {{ cursor: 'pointer'}} 
                        onClick = { () => {
                            this.returnHome(); 
                            webSocketSend(this.props.webSocket, this.props.uid, "home");
                        }} >
                        {this.props.homePage ? 
                            this.state.imgLogoSrc
                            :
                            <div></div>
                        }
                    </span>
                </ToolbarGroup>
                
                {/* Top Right icons */}
                <ToolbarGroup>
                    <ToolbarSeparator />

                    <IconButton 
                        //onClick = { () => this.props.dispatch(editModalDisplay(null, null, true, null)) } 
                        onClick = { () => /* window.open("https://desk.zoho.com/portal/synglyphx/home", '_blank') */
                            this.props.uid ? webSocketSend(this.props.webSocket, this.props.uid, "help") : null}
                        style = {{ 
                            height: "5.005vh",
                            width: "2.010vw", 
                            marginRight: "0.55vh", 
                            zIndex: (this.props.tutorialStage === 4 ? "300" : "5"), 
                            padding: "1.251vh" }}
                        className = { (this.props.tutorialStage === 4 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = 'bottomRight'
                            mouseEnterDelay = { 0.15 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div style={{height: "1.877vh", width: "1.35vw", fontSize:"1.460vh"}}> 
                                    Help
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-question-circle fa-2x" color = '#ffffff' 
                                style={{
                                    fontSize: (this.props.homePage ? "2.920vh" : "2.50vh")
                            }} />
                        </Tooltip>
                    </IconButton>

                    {/* <IconButton 
                        //onClick = { () => this.props.dispatch(editModalDisplay(null, null, true, null)) } 
                        onClick = { () => this.createShareLink() }
                        style = {{ 
                            zIndex: (this.props.tutorialStage === 4 ? "300" : "5"),
                            display: (
                                this.props.userInfo.Admin === 1
                                && Object.keys(this.props.statData).length !== 0  ? "" : "none" ),
                           
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
                    </IconButton> */}

                    <IconButton 
                        onClick = { () => (this.props.uid ? webSocketSend(this.props.webSocket, this.props.uid, "settings_modal") : 
                            this.props.dispatch(editModalDisplay(true, null, null, null)) )  } 
                        style = {{ 
                            height: "5.005vh",
                            width: "2.010vw", 
                            zIndex: (this.props.tutorialStage === 5 ? "300" : "5"), 
                            marginRight: (this.props.homePage ? "2.043vh" : "1.043vh"), 
                            padding: "1.251vh" }}
                        className = { (this.props.tutorialStage === 5 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = 'bottomRight'
                            mouseEnterDelay = { 0.15 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div style={{height: "1.877vh", width: "2.2vw", fontSize:"1.460vh"}}> 
                                    Settings
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-cogs fa-2x" color = '#ffffff' 
                                style={{
                                    fontSize: (this.props.homePage ? "2.920vh" : "2.50vh")
                            }} />
                        </Tooltip>
                    </IconButton>

                    {/* <IconButton 
                        // onClick = { () => this.props.dispatch(editModalDisplay(true, null, null, null)) } 
                        style = {{ 
                            height: "5.005vh",
                            zIndex: (this.props.tutorialStage === 5 ? "300" : "5"), 
                            marginRight: "0.97vh", 
                            width: "6.48vw", 
                            padding: "1.251vh" }}
                        className = { (this.props.tutorialStage === 5 ? "pulse" : "") }
                    >
                        <Tooltip
                            placement = 'bottomRight'
                            mouseEnterDelay = { 0.15 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div style={{height: "1.877vh", width: "3vw", fontSize:"1.460vh"}}> 
                                    Live Chat
                                </div> 
                            }
                        >
                            <FontIcon style={{fontSize: "2.503vh"}}>
                            <img alt="Live Chat Icon" src='./Res/Img/LiveChat.png'
                                style={{ width: "5vw", height: "2.6vh" }} 
                                onClick={() => window.open('https://zoho.com', '_blank')}/>
                            </FontIcon>
                        </Tooltip>
                    </IconButton>
                     */}

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

                    {/*<IconButton onClick = { this.toggleNav.bind(this) } style = {{ margin: "0px 0px 0px -5px", display: (this.props.homePage ? "none" : "") }} >*/}
                        {/*<FontIcon className = "fa fa-caret-up fa-2x" color = '#ffffff' style = {{ transform: 'rotateZ(90deg)', margin: "0px 0px 0px 4px" }} />*/}
                        <Tooltip
                            placement = 'bottomRight'
                            mouseEnterDelay = { 0.15 }
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
                                    fontSize: '3.754vh',
                                    color: this.props.settings.colors.collapsibleColor.mainIcon,
                                    transform: 'rotateZ(90deg)',
                                    margin: "0px 0px 0px 0.417vh",
                                    display: (this.props.homePage ? "none" : "")
                                }}
                                onClick = {() => {this.toggleNav(); webSocketSend(this.props.webSocket, this.props.uid, "collapse_sidenav"); } }
                            /> 
                        </Tooltip>
                    {/*</IconButton>*/}
                </ToolbarGroup>

                {/* User Icon Popover */}
                {/* <Popover
                    canAutoPosition= { false }
                    open = { this.state.userProfileMenuOpen }
                    anchorEl = { this.state.userInfoAnchorEl }
                    onRequestClose = { this.ToggleUserInfoMenu }
                    anchorOrigin = {{ "horizontal":"left", "vertical":"bottom" }}
                    targetOrigin = {{ "horizontal":"left", "vertical":"top" }}
                >
                    <List>
                        <ListItem
                            disabled = { false }
                            leftAvatar = {
                                <Avatar
                                    src={customer_icon}
                                    style= {{ 
                                        backgroundColor: "white",
                                        fontSize: "2.064vh", 
                                        height: "4.128vh",
                                        width: "4.128vh",
                                        top: "0.826vh",
                                        left: "1.651vh",
                                        backgroundColor: this.props.settings.colors.overviewButtonsColor.background
                                    }}
                                >
                                    { this.props.userInfo ? (this.props.userInfo.Name.includes(" ") ? this.props.userInfo.Name.charAt(0) + this.props.userInfo.Name.charAt(this.props.userInfo.Name.indexOf(" ") + 1) : this.props.userInfo.Name.charAt(0)) : "" }
                                </Avatar>
                            }
                            innerDivStyle = {{padding: "2.064vh 1.651vh 2.064vh 7.43vh"}}
                            style= {{
                                fontSize: "1.98vh", 
                                fontFamily: "ITCFranklinGothicStd-Med"}}
                            primaryText = { this.props.userInfo ? this.props.userInfo.Name : ""}
                        />

                        <MenuItem onClick = { () => this.openAdminWizard() } className = "menuItemStyling" primaryText = "Admin Wizard" />
                        {this.props.userInfo.admin ? show : dont}
                        <MenuItem className = "menuItemStyling" primaryText = "User Settings" />
                        <MenuItem 
                            onClick = {() => {this.logout(); this.webSocketSend("logout");} } 
                            className = "menuItemStyling" 
                            primaryText = "Sign out" 
                        />
                        
                        
                        
                    </List>
                </Popover> */}

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

export const legendArrClear = () => ({
    type: 'CLEAR_LEGEND_URL_ARR',
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
    webSocket: state.filterState.webSocket,
    uid: state.filterState.uid
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(TopNavBar));
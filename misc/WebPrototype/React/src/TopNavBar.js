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

    componentDidMount(){
        //zoho chat dialog bubble
        window.setTimeout(() => {if(document.getElementById('titlediv')){
            $('#titlediv').css('display', 'block');
        }}, 1000);

        if(!this.props.homePage){
            //turn off chat if in viz page
            this.toggleLiveChat();
        }
        else {
            window.setTimeout(() => { 
                if(document.getElementById('zsiq_float')){
                    $('#zsiq_float').css('display', 'block');
                }
            }, 1000);
        }
    }

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

    /**
     * Toggles Zoho Live Chat
     */
    toggleLiveChat(){
        // console.log(document.getElementById('zsiq_float').style.display)
        if(document.getElementById('zsiq_float') && (document.getElementById('zsiq_float').style.display === 'inline-block' || 
            document.getElementById('zsiq_float').style.display === 'block')){
            $('#zsiq_float').css('display', 'none');
        
        } else if(document.getElementById('zsiq_float') && document.getElementById('zsiq_float').style.display === 'none'){
            $('#zsiq_float').css('display', 'block');
            $('#titlediv').css('display', 'block');
        }
    }

    render() {

        let customer_icon;
        if(this.props.userInfo.institutionDir){

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
                        onClick = { () => this.toggleLiveChat()}
                        style = {{ 
                            height: "5.005vh",
                            width: "2.010vw", 
                            marginRight: "1.25vh", 
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
                                <div style={{height: "6.477vh", width: "2.4vw", fontSize:"1.460vh"}}> 
                                    Toggle Live Chat
                                </div> 
                            }
                        >
                            <FontIcon className = "fa fa-comment" color = 'white' 
                                style={{
                                    fontSize: (this.props.homePage ? "2.920vh" : "2.50vh")
                            }} />
                        </Tooltip>
                    </IconButton>

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
                                <div style={{height: "1.877vh", width: "1.45vw", fontSize:"1.460vh"}}> 
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
                                <div style={{height: "1.877vh", width: "2.4vw", fontSize:"1.460vh"}}> 
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
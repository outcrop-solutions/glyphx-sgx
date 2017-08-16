import React from 'react';
import { connect } from 'react-redux';
import {Toolbar, ToolbarGroup, ToolbarSeparator} from 'material-ui/Toolbar';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
import List from 'material-ui/List/List';
import ListItem from 'material-ui/List/ListItem';
import Popover from 'material-ui/Popover';
import MenuItem from 'material-ui/MenuItem';
import Avatar from 'material-ui/Avatar';
import SettingsModal from './SettingsModal.js';
import AnnouncementModal from './AnnouncementModal.js'


/**
 * No Props
 **/
class TopNavBar extends React.Component {


    /**
     * Initial state of the component.
     **/
	state = {
        userProfileMenuOpen:false,
        userInfoAnchorEl: {},
        displayAnnouncementsCheckbox: true,
        imgLogoSrc: <a href = "http://www.glyphed.co/" target = "_blank" rel = "noopener noreferrer">
						<img src = "./Res/Img/GlyphED-wht-3.png" style = {{ width: '200px' }} alt = "GlyphEd"/>
					</a>
    };
    
    logout = () => {
        //show logout page
        window.location.pathname = "/logout";
    };

    /**
	 * Hides the filter side nav by translating it off the screen so it doesnt resize and 
     * wont have to be reloaded after it is "closed"
	 */ 
    toggleNav() {
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;
        var gv = document.getElementById('GlyphViewerContainer');

        if (!filterNavOpen) {
            //open the filterNav sidebar
            filterNav.style.transform = "translate(0px, 0px)";
            if(!this.props.settings.sideBarOverlap) {
                gv.style.width = this.props.glyphWindowWidth + "px";
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
            this.setState({
                userProfileMenuOpen : !this.state.userProfileMenuOpen,
                userInfoAnchorEl: e.currentTarget
            });
        }
        else {
            this.setState({
                userProfileMenuOpen : !this.state.userProfileMenuOpen
            });
        }
    }


    /**
	 * Displays the announcements modal
	 */
    displayAnnouncements() {
        this.props.dispatch(editModalDisplay(null, true)); 
        this.setState({ displayAnnouncementsCheckbox: false });
    }

    render(){

        return(
            <Toolbar 
                style = {{ padding: '0px', backgroundColor: this.props.settings.colors.topNavbarColor.barBackground }}
                ref = "topNavToolbar"
            >
                {/* Logo */}
                <ToolbarGroup>
                    <span style = {{ cursor: 'pointer' }} >
                        {this.state.imgLogoSrc}
                    </span>
                </ToolbarGroup>
                
                {/* Top Right icons */}
                <ToolbarGroup>
                    <ToolbarSeparator />

                    <IconButton onClick = { this.toggleNav.bind(this) } >
                        <FontIcon className = "fa fa-filter fa-2x" color = '#ffffff' />
                    </IconButton>

                    <IconButton /*onClick = {  }*/ >
                        <FontIcon className = "fa fa-question-circle fa-2x" color = '#ffffff' />
                    </IconButton>

                    <IconButton onClick = { () => this.props.dispatch(editModalDisplay(true, null)) } >
                        <FontIcon className = "fa fa-cogs fa-2x" color = '#ffffff' />
                    </IconButton>
                    
                    <IconButton  onClick = { this.displayAnnouncements.bind(this) } >
                        <FontIcon className = "fa fa-bell fa-2x" color = '#ffffff' /> 
                        <FontIcon id = "notificationBadge" className = "fa fa-exclamation-circle fa-1x notificationBadge" />
                    </IconButton>
                    
                    <IconButton onClick = { (event) => this.ToggleUserInfoMenu(event) } >
                        <FontIcon className = "fa fa-user fa-2x" color = '#ffffff' />
                    </IconButton>
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
                                    {(this.props.userInfo.FName ? this.props.userInfo.FName[0] : "") + (this.props.userInfo.LName ? this.props.userInfo.LName[0] : "")}
                                </Avatar>
                            }
                            primaryText = { this.props.userInfo.FName + " " + this.props.userInfo.LName }
                            secondaryText = { this.props.userInfo ? this.props.userInfo.Type : null }
                        />

                        <MenuItem className = "menuItemStyling" primaryText = "Help &amp; feedback" />
                        <MenuItem className = "menuItemStyling" primaryText = "Settings" />
                        <MenuItem onClick={this.logout} className = "menuItemStyling" primaryText = "Sign out" />
                    </List>
                </Popover>

                {/* Modals */}
                <SettingsModal glyphWindowWidth = { this.props.glyphWindowWidth } />
				<AnnouncementModal checkBoxDisplay = { this.state.displayAnnouncementsCheckbox } />
            </Toolbar>          
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (settingsModal, announcementsModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    settingsModal,
    announcementsModal,
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
export default connect(mapStateToProps)(TopNavBar);
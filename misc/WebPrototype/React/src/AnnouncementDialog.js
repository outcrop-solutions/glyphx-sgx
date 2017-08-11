import React, { Component } from 'react';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Checkbox from 'material-ui/Checkbox';
import { connect } from 'react-redux';

/**
 * This class represents the announcements dialog that can be used to send the user information about updates and latest news.
 * Initially when the application is 1st loaded, this will make a server call to check for important updates and show it to the user upon login.
 * However, this can be used even after that. On click of the notification(bell) icon on the toolbar this will get displayed.
 */

class AnnouncementDialog extends Component {
	
	constructor(props){
		super(props);
		
		var serverResponse = this.checkForAnnouncements();

		this.state = {
			displayText: serverResponse.textToDisplay,
			shouldBeDisplayed: (serverResponse.textToDisplay == "" || serverResponse.textToDisplay == null) ? false : true,
			displayCheckBox: true,
			currentVisibility: false,
			forceDisplayAfterLogin: serverResponse.forceDisplayAfterLogin
		}
		
	}

	/**
	 * Make server call to check for announcements
	 * Check whether the new dialog box needs to be displayed at user login or not.
	 */
	checkForAnnouncements() {
		var returnObj ={
			textToDisplay: "Notifications will disappear after 1 week.",
			forceDisplayAfterLogin: false
		};

		//Server call.


		return returnObj;
	}
	
	/**
	 * On application startup, there is a badge on the notification icon.
	 * This function checks to see if there is any notification and hides/shows the icon.
	 */
	componentDidMount() {
		var notificationBadge = document.getElementById('notificationBadge');
		if(!this.state.shouldBeDisplayed && notificationBadge)
		{
			notificationBadge.style.display = 'none';
		}
	}
	
	/**
	 * This function is called on click of "OK" on the dialog box.
	 * It checks whether the checkbox for "do not show me this message again" is checked.
	 * If yes then sends a server call to update the information on the server.
	 */
	onOkAnnouncements = (evt) => {
		var notificationBadge = document.getElementById('notificationBadge');
		console.log('Announcements seen');
		
		if(this.refs['dontDisplayAgain'] && this.refs.dontDisplayAgain.isChecked())
		{
			//send server call to notify that this message is seen by the user.
			console.log('Announcements not to show again');
		}
		
		if(notificationBadge)
		{
			notificationBadge.style.display = 'none';
		}
		
		this.setState({currentVisibility: false });
	}
	
	
	
	render(){
		return(
			<Dialog
				title = "Announcements"
				actions = {
					[
						<FlatButton
							label = "OK"
							primary = { true }
							onClick = { (evt) => this.onOkAnnouncements(evt) }
							style = {{ color: this.props.settings.settingsModalColor.saveButton }}
						/>
					]
				}
				modal = { true }
				open = { this.state.currentVisibility }
			>
				<label><h4> {this.state.displayText == "" ? "No Announcements." : this.state.displayText} </h4></label>
					
					{this.state.displayCheckBox ? <Checkbox
						iconStyle = {{ fill: this.props.settings.elasticColor.checkBox }}
						ref="dontDisplayAgain"
						label="Don't show this message again."
					/> : null}
			</Dialog>
		);
	}
}

/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const saveUserInfo = (info) => ({
    type: 'SAVE_USER_INFO',
    info
});

/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings
  }
}


/**
 * Connects the Announcements Dialog component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(AnnouncementDialog);
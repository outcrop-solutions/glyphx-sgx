import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Checkbox from 'material-ui/Checkbox';


/**
 * This class represents the alerts dialog that can be used to send the user notifications for chat messages and user feed posts.
 * Initially when the application is first loaded, this will make a server call to check for important updates and show it to the user upon login.
 * On click of the notification(bell) icon on the toolbar this will get displayed.
 * @param checkBoxDisplay: -ADCMT
 */
class AlertsModal extends React.Component {
	
	constructor(props) {
		super(props);

		var serverResponse = this.checkForAlerts();

		this.state = {
			displayText: serverResponse.textToDisplay,
			shouldBeDisplayed: (serverResponse.textToDisplay === "" || serverResponse.textToDisplay == null) ? false : true,
			forceDisplayAfterLogin: serverResponse.forceDisplayAfterLogin
		}
	}


	/**
	 * React built-in which is called when component mounts
	 * This function checks to see if there is any notification and hides/shows the icon.
	 */
	componentDidMount() {
		console.log('asfdsdf')
		var notificationBadge = document.getElementById('notificationBadge');
		if (!this.state.shouldBeDisplayed && notificationBadge) {
			notificationBadge.style.display = 'none';
		}
	}


	/**
	 * Make server call to check for alerts
	 * Check whether the new dialog box needs to be displayed at user login or not.
	 */
	checkForAlerts() {
		var returnObj = {
			textToDisplay: "Notifications will disappear after 1 week.",
			forceDisplayAfterLogin: false
		};

		// Add Server call here.

		return returnObj;
	}

	
	/**
	 * This function is called on click of "OK" on the dialog box.
	 * It checks whether the checkbox for "do not show me this message again" is checked.
	 * If yes then sends a server call to update the information on the server.
	 */
	onOkAlerts = () => {
		var notificationBadge = document.getElementById('notificationBadge');
		//console.log('Alerts seen');
		
		if (this.refs['dontDisplayAgain'] && this.refs.dontDisplayAgain.isChecked()) {
			//send server call to notify that this message is seen by the user.
			//console.log('Alerts not to show again');
		}
		
		if (notificationBadge) {
			notificationBadge.style.display = 'none';
		}
		
		this.props.dispatch(editModalDisplay(false));
	}
	
	
	render() {
		var checkBox = (
			<Checkbox
				iconStyle = {{ fill: this.props.settings.colors.elasticColor.checkBox }}
				ref = "dontDisplayAgain"
				label = "Don't show this message again."
			/>
		);

		return(
			<Dialog
				title = "Alerts"
				modal = { true }
				open = { this.props.alertsDisplay }
				actions = {
					[
						<FlatButton
							label = "OK"
							primary = { true }
							onClick = { (evt) => this.onOkAlerts(evt) }
							style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
						/>
					]
				}
			>
				<label><h4> {this.state.displayText === "" ? "No Alerts." : this.state.displayText} </h4></label>
					
				{this.props.checkBoxDisplay ? checkBox : null}

			</Dialog>
		);
	}
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (alertsModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    alertsModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
	alertsDisplay: state.filterState.ModalDisplay.alertsModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(AlertsModal);

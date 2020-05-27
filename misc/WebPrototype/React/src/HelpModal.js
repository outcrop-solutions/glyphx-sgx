import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
/* import Flexbox from 'flexbox-react'; */
import RaisedButton from 'material-ui/RaisedButton';
/* import TextField from 'material-ui/TextField'; */
import Divider from 'material-ui/Divider';
import FontIcon from 'material-ui/FontIcon';
/* import Collapsible from 'react-collapsible';
import SearchBox from './SearchBox.js'; */
import './css/General.css';


/**
 * Bug reporting, FAQ, and user manual
 */
class HelpModal extends React.Component {

	render() {
		return(
			<Dialog
				title = { <div> <FontIcon className = "fa fa-question-circle fa-2x" color = '#ffffff' /> &nbsp;&nbsp;Help </div> }
				modal = { true }
				open = { this.props.helpDisplay }
				bodyStyle = {{ padding: "0px 24px 10px" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", lineHeight: "12px", padding: "10px 30px 14px"}}
				actions = {
					[
						<FlatButton
							label = "Close"
							primary = { true }
							onClick = { () => this.props.dispatch(editModalDisplay(false)) }
							style = {{ backgroundColor: this.props.settings.colors.settingsModalColor.saveButton, color: "#ffffff", marginTop: "-2px", height: '35px', lineHeight: '35px', width: "110px" }}

						/>
					]
				}
			>
				<div style = {{ textAlign: "center", margin: "20px 0px 3px", color: "#000000" }} > Send Feedback </div>

				<div style = {{ marginBottom: "20px" }} >
					<Divider style = {{ backgroundColor: "#acacac", height: "2px" }} />
				</div>


					<div style = {{ width: "200px", margin: "0 auto" }} >
						<RaisedButton 
							label = "View User Manual"
							style = {{ width: "100%", margin: "0px auto 18px" }}
							buttonStyle = {{
								height: '35px',
								lineHeight: '35px',
								backgroundColor: this.props.settings.colors.buttons.general
							}} 
							labelStyle = {{
								fontSize: '12px',
								textAlign: "center",
								color: this.props.settings.colors.overviewButtonsColor.text,
								margin: "0px 0px 0px -3px",
								paddingLeft: "0px",
								paddingRight: "0px"
							}}
							overlayStyle = {{ height: '35px', lineHeight: '35px' }}
							//onClick = { () => this.hideTextArea() }
							primary = { true } 
						/>
					</div>

					<script src="https://jsd-widget.atlassian.com/assets/embed.js" data-base-url="https://jsd-widget.atlassian.com" data-key="238f869b-0a67-4919-9d2c-62ad12a50518" data-jsd-embedded=""></script>


			</Dialog>
		);
	}
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (helpModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    helpModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
	helpDisplay: state.filterState.ModalDisplay.helpModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(HelpModal);
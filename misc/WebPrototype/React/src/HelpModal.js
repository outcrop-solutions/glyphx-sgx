import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Flexbox from 'flexbox-react';
import RaisedButton from 'material-ui/RaisedButton';
import TextField from 'material-ui/TextField';
import Divider from 'material-ui/Divider';
import FontIcon from 'material-ui/FontIcon';
import Collapsible from 'react-collapsible';
import './General.css';


/**
 * Bug reporting, FAQ, and user manual
 */
class HelpModal extends React.Component {

	state = {
		selection: "none",
		opacity: 0
	}

	
	/**
	 * Hides FAQ & User manual to display a text area
	 * @param selection: bug report or feature request
	 */
	displayTextArea(selection) {
		var context = this;

		this.setState({ selection: selection });

		setTimeout(function () {
			context.setState({ opacity: 1 });
		}, 50);
	}


	/**
	 * Hides test area and makes FAQ & User manual visible again
	 */
	hideTextArea() {
		var context = this;

		this.setState({ opacity: 0 });
		
		setTimeout(function () {
			context.setState({ selection: "none" });
		}, 200);
	}
	
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
							label = "Back"
							primary = { true }
							onClick = { () => this.hideTextArea() }
							style = {{ backgroundColor: "#707192", color: "#ffffff", display: (this.state.selection === "none" ? "none" : ""), margin: "-2px 3px 0px 0px", height: '35px', lineHeight: '35px', width: "110px" }}

						/>,
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

				<Flexbox flexDirection = "row" >
					<Flexbox style = {{ width: "50%" }} >
						<RaisedButton 
							label = "Report Bug"
							style = {{ width: "80%", margin: "10px auto" }}
							buttonStyle = {{
								height: '40px',
								lineHeight: '40px',
								backgroundColor: (this.state.selection === "Feature" ? "#707192" : this.props.settings.colors.collapsibleColor.mainBackground )
							}} 
							labelStyle = {{
								fontSize: '14px',
								color: this.props.settings.colors.overviewButtonsColor.text,
								margin: "0px 0px 0px -3px",
								paddingLeft: "0px",
								paddingRight: "0px"
							}}
							overlayStyle = {{ height: '40px', lineHeight: '40px' }}
							onClick = { () => this.displayTextArea("Bug") }
							primary = { true } 
						/>
					</Flexbox>

					<Flexbox style = {{ width: "50%" }} >
						<RaisedButton 
							label = "Request Feature"
							style = {{ width: "80%", margin: "10px auto" }}
							buttonStyle = {{
								height: '40px',
								lineHeight: '40px',
								backgroundColor: (this.state.selection === "Bug" ? "#707192" : this.props.settings.colors.collapsibleColor.mainBackground )
							}} 
							labelStyle = {{
								fontSize: '14px',
								color: this.props.settings.colors.overviewButtonsColor.text,
								margin: "0px 0px 0px -3px",
								paddingLeft: "0px",
								paddingRight: "0px"
							}}
							overlayStyle = {{ height: '40px', lineHeight: '40px' }}
							onClick = { () => this.displayTextArea("Feature") }
							primary = { true } 
						/>
					</Flexbox>
				</Flexbox>

				<div className = "fade" style = {{ width: "100%", opacity: this.state.opacity, display: (this.state.selection === "none" ? "none" : ""), padding: "15px 36px 0px" }} >
					<div style = {{ backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
						<TextField 
							hintText = "Please be as descriptive as possible."
							style = {{ width: "100%" }}
							multiLine = { true }
							rows = { 1 }
							rowsMax = { 4 }
							hintStyle = {{ color: "#707192", marginLeft: "6px" }}
							textareaStyle = {{ marginLeft: "6px", width: "98%" }}
							underlineStyle = {{ borderColor: "grey" }}
							underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
						/>
					</div>

					<div style = {{ width: "200px", margin: "0 auto" }} >
						<RaisedButton 
							label = { (this.state.selection === "Bug" ? "Submit Bug" : "Submit Feature") }
							style = {{ width: "100%", margin: "13px auto -7px" }}
							buttonStyle = {{ height: '35px', lineHeight: '35px', backgroundColor: this.props.settings.colors.buttons.general }} 
							labelStyle = {{
								fontSize: '12px',
								textAlign: "center",
								color: this.props.settings.colors.overviewButtonsColor.text,
								margin: "0px 0px 0px -3px",
								paddingLeft: "0px",
								paddingRight: "0px"
							}}
							overlayStyle = {{ height: '35px', lineHeight: '35px' }}
							onClick = { () => this.hideTextArea() }
							primary = { true } 
						/>
					</div>
				</div>

				<div className = "fade" style = {{ opacity: (this.state.opacity === 0 ? 1 : 0), display: (this.state.selection === "none" ? "" : "none") }} >
					<div style = {{ textAlign: "center", margin: "20px 0px 2px", color: "#000000" }} > FAQ </div>
					<div style = {{ margin: "0 auto", width: "85%", marginBottom: "20px" }} >
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
							onClick = { () => this.hideTextArea() }
							primary = { true } 
						/>
					</div>

					<div style = {{ backgroundColor: "#707192", padding: "10px", width: "81%", margin: "0 auto", borderRadius: "3px", height: "230px" }} >
						<div style = {{ backgroundColor: "#707192", height: "210px", overflowY: "auto" }} >
							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginBottom: "3px", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

							<div style = {{ backgroundColor: "#ffffff", marginRight: "5px" }} >
								<Collapsible
									transitionTime = {200}
									triggerClassName = "faqCollapse"
									triggerOpenedClassName  = "faqCollapse"
									trigger = {<div style = {{ margin: "0 auto", fontSize: "15px", width: "378px", color: "#000000" }}>How do I share my saved views with team members?</div> }
								>
									<div style = {{ padding: "0px 30px", textAlign: "center", wordBreak: "break-word" }} >
										When you enter the team room you wish to share a view with, you can link one of your saved views by pressing the cube icon located to the far right of the text editting toolbar
									</div>
								</Collapsible>
							</div>

						</div>
					</div>

					<div style = {{ textAlign: "center", marginTop: "15px" }} > Still can't find what youre looking for? (insert link to livechat here) {/* <div data-id="2eaf959d51" class="livechat_button"><a href="https://www.livechatinc.com/help-desk-software/?partner=lc_9235035&amp;utm_source=chat_button">help desk software</a></div> */} </div>
				</div>

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
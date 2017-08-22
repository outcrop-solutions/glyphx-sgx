import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';



class HelpModal extends React.Component {
	
	render(){
		return(
			<Dialog
				title = "Help"
				actions = {
					[
						<FlatButton
							label = "Close"
							primary = { true }
							onClick = { () => this.props.dispatch(editModalDisplay(false)) }
							style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
						/>
					]
				}
				modal = { true }
				open = { this.props.helpDisplay }
			>
				help modal
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
 * Connects the Announcements Dialog component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(HelpModal);
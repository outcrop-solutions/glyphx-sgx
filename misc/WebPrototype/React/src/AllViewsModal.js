import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';



class allViewsModal extends React.Component {

	handleBackClick = () => {
		this.props.dispatch(editModalDisplay(false));
	}
	
	render() {
		return(
			<Dialog
				title = "All Views"
				actions = {
					[
						<FlatButton
                            label = "Launch"
                            primary = { true }
                            //onClick = {  }
                            style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                        />,
						<FlatButton
							label = "Back"
							primary = { true }
							onClick = { this.handleBackClick }
							style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
						/>
					]
				}
				modal = { true }
				open = { this.props.allViewsDisplay }
			>
				{this.props.type}
			</Dialog>
		);
	}
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (allViewsModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    allViewsModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
	allViewsDisplay: state.filterState.ModalDisplay.allViewsModal,
  }
}


/**
 * Connects the Announcements Dialog component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(allViewsModal);
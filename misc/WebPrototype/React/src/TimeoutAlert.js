import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import Select from 'react-select';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './css/SettingsModal.css';


/**
 * -ADCMT
 **/
class TimeoutAlert extends React.Component {

    /**
     * Reverts to previous settings if any where changed and closes the modal
     **/
    onSettingsCancel() {
        this.props.dispatch(editModalDisplay(false));
    }

    render() {

        return(
            <Dialog
				title = { <div> Alert </div> }
                ref = "Settings"
                modal = { true }
				open = { this.props.timeoutDisplay }
				bodyStyle = {{ padding: "0px 24px 10px" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", lineHeight: "12px", padding: "10px 30px 14px"}}
				actions = {
					[
                        <FlatButton
                            label = "Ok"
                            primary = { true }
                            onClick = { () => this.onSettingsCancel() }
                            style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
                        />
					]
				}
			>

                <div style = {{ margin: "20px 0px 3px", color: "#000000", fontSize: "20px", fontWeight: "bold", textAlign: "center" }} > Your session will expire in 5 minutes due to inactivity. </div>
                
            </Dialog>             
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (timeoutModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    timeoutModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    timeoutDisplay: state.filterState.ModalDisplay.timeoutModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(TimeoutAlert);
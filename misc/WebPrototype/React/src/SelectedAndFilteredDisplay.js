import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { makeServerCall } from './ServerCallHelper.js';
import Dialog from 'material-ui/Dialog';
import Flexbox from 'flexbox-react';
import FlatButton from 'material-ui/FlatButton';
import Snackbar from 'material-ui/Snackbar';
import RaisedButton from 'material-ui/RaisedButton';
import SearchBox from './SearchBox.js';
import ComponentLoadMask from './ComponentLoadMask.js';
import ReactHTMLTableToExcel from 'react-html-table-to-excel';
import { guidGenerator } from './GeneralFunctions.js';
import './General.css';


/**
 * @param data: array of objects representing data to display
 */
class SelectedAndFilteredDisplay extends React.Component {

	state = {
		selectionList: [],
		dragState: 0,
		snackbarVisible: false,
		data: [],
		table: "",
		selection: "",
		mouseup: true,
		selectAll: [],
		loadMask: true
	}
	

	/**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

	}
	

	/**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {

	}


	/**
     * React built-in which acts as a listener for when props change
     * @param nextProps: The props the component would have after the change
     **/
	componentWillReceiveProps(nextProps) {

	
    }


	render() {
		var data = this.props.data;
		var context = this;
        var dataKeys;

        if (data[0]) {
            dataKeys = Object.keys(data[0]);

            var displayKeys = dataKeys.map( function(key) {
                return (
                    <th key = {key} style = {{ paddingRight: "20px" }} >{key}</th>
                )
            });

        }

        var displayData = [];

        for (var i = 0; i < data.length; i++) {
            var dataRow = [];

            for (var j = 0; j < dataKeys.length; j++) {
                dataRow.push(<td style = {{ paddingRight: "20px" }} key = { guidGenerator() } >{data[i][dataKeys[j]] ? data[i][dataKeys[j]] : "-"}</td>)
            }

            displayData.push(<tr key = { guidGenerator() } >{dataRow}</tr>);
        }


		return(
			<Dialog
				title = { null }
				contentStyle = {{ width: "95%", maxWidth: "none" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", fontSize: "36px", lineHeight: "16px" }}
				modal = { true }
				open = { this.props.selectedFilteredModal }
				actions = {
					[
						<FlatButton
							label = "Back"
							primary = { true }
							onClick = { () => this.props.dispatch(editModalDisplay(false)) }
							style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
						/>,
                        <ReactHTMLTableToExcel
                            id = "test-table-xls-button"
                            className = "download-table-xls-button"
                            table = "table-to-xls"
                            filename = "tablexls"
                            sheet = "tablexls"
                            buttonText = "Download as XLS"
                        />
					]
				}
			>
                <div>
                    <table id="table-to-xls" >
                        <tbody>
                            <tr>
                                {displayKeys}
                            </tr>

                            {displayData}
                        </tbody>
                    </table>
                </div>
			</Dialog>
		);
	}
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (selectedFilteredModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    selectedFilteredModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
	selectedFilteredModal: state.filterState.ModalDisplay.selectedFilteredModal,
  }
}


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(SelectedAndFilteredDisplay));
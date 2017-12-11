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

	onClickDownload() {
		// Generate our CSV string from out HTML Table
		var csv = this.tableToCSV( document.querySelector( "#dataTable" ) );

		// Create a CSV Blob
		var blob = new Blob( [ csv ], { type: "text/csv"} );

		// Determine which approach to take for the download
		if ( navigator.msSaveOrOpenBlob ) {
			// Works for Internet Explorer and Microsoft Edge
			navigator.msSaveOrOpenBlob( blob, this.props.VizParams.tableName + ".csv" );
		}

		else {
			// Attempt to use an alternative method
			var anchor = document.body.appendChild(
				document.createElement( "a" )
			);

			// If the [download] attribute is supported, try to use it
			if ( "download" in anchor ) {
				anchor.download = this.props.VizParams.tableName + ".csv";
				anchor.href = URL.createObjectURL( blob );
				anchor.click();
			}
		}
	}

	tableToCSV (table) {
		// We'll be co-opting `slice` to create arrays
		var slice = Array.prototype.slice;

		return slice.call( table.rows ).map(function ( row ) {
			return slice.call( row.cells ).map(function ( cell ) {
				return '"t"'.replace( "t", cell.textContent );
			}).join( "," );
		}).join( "\r\n" );
	}


	render() {
		var data = this.props.data;
		var context = this;
        var dataKeys;

        if (data[0]) {
            dataKeys = Object.keys(data[0]);

            var displayKeys = dataKeys.map( function(key) {
                return (
                    <th key = {key} >{key}</th>
                )
            });

        }

        var displayData = [];

        for (var i = 0; i < data.length; i++) {
            var dataRow = [];

            for (var j = 0; j < dataKeys.length; j++) {
                dataRow.push(<td key = { guidGenerator() } style = {{ whiteSpace: "nowrap" }} >{data[i][dataKeys[j]] ? data[i][dataKeys[j]] : "-"}</td>)
            }

            displayData.push(<tr key = { guidGenerator() } >{dataRow}</tr>);
        }


		return(
			<Dialog
				title = { null }
				contentStyle = {{ width: "93%", maxWidth: "none" }}
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
                        <RaisedButton 
							label = { "Download" }
							style = {{
								width: "112px",
								margin: "0px 10px 9px 0px"
							}}
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
							overlayStyle = {{
								height: '35px',
								lineHeight: '35px',
							}}
							onClick = { () => this.onClickDownload() }
							primary = {true } 
						/>
					]
				}
			>
                <div id = "tableContainer" style = {{ width: "100%", height: "50vh", overflow: "auto" }} >
                    <table id = "dataTable" className = "blueTable" >
						<thead>
							<tr>
                                {displayKeys}
                            </tr>
						</thead>
                        <tbody>
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
	VizParams: state.filterState.VizParams,
	selectedFilteredModal: state.filterState.ModalDisplay.selectedFilteredModal,
  }
}


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(SelectedAndFilteredDisplay));
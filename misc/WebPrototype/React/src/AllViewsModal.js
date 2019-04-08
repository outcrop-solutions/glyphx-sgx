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
import './css/General.css';


/**
 * This component handles the front-end filters
 * @param type: string name of the selection made
 * @param typeURL: URL corresponding with the type to make backend call
 */
class allViewsModal extends React.Component {

	state = {
		selectionList: [],
		dragState: 0,
		snackbarVisible: false,
		data: [],
		table: "",
		selection: "",
		mouseup: true,
		selectAll: [],
		loadMask: false,
		loadDone: false
	}
	constructor(props){
		super(props);
		this.onLaunchResultCallback = this.onLaunchResultCallback.bind(this);
	}

	/**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
		// Mouseup listener used to handle click drag selection
		window.onmouseup = this.handleMouseUp.bind(this);
	}
	

	/**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {
		// Removing listener so it doesnt linger across the site
		window.onmouseup = null;
	}


	/**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
		return (this.state.selectionList !== nextState.selectionList || this.state.data !== nextState.data 
			|| this.props.allViewsDisplay !== nextProps.allViewsDisplay || this.state.snackbarVisible !== nextState.snackbarVisible
		);

		/*
        if (this.state.selectionList != nextState.selectionList || this.state.data != nextState.data) {
            return true;
        }

		if (this.props.allViewsDisplay != nextProps.allViewsDisplay) {
			return true;
		}

        return false;
		*/
    };


	/**
     * React built-in which acts as a listener for when props change
     * @param nextProps: The props the component would have after the change
     **/
	componentWillReceiveProps(nextProps) {

		// Only care if URL changes because then new data needs to be loaded
        if (nextProps.typeURL !== this.props.typeURL) {

			// Show the window load-mask as backend call is being made
			this.props.dispatch(editModalDisplay(false, null));
			this.setState({ loadMask: true, loadDone: false });

            var context = this;
			var index = nextProps.typeURL.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");

			//debugger;

			// Get the data corresponding to the URL
			// debugger;
			makeServerCall(window.encodeURI('frontEndFilterData/' + nextProps.typeURL.substring(index + 1) ),
				function (responseText) { 
					var response = JSON.parse(responseText);
					var preData = response.frontEndFilterData; 
					var data = [];
					var selectAll = {};
					var keyArray = Object.keys(preData);
					var collator = new Intl.Collator(undefined, {numeric: true, sensitivity: 'base'});

					// debugger;

					//console.log(response);

					context.props.dispatch(updateFilterFromSnapshot({}));

					// Seperate and store the actual data and the selectAll boolean of each column
					for (var i = 0; i < keyArray.length; i++) {
						var dataCol = [];
						selectAll[keyArray[i]] = preData[keyArray[i]]["selectAll"];

						for (var j = 0; j < preData[keyArray[i]]["values"].length; j++) {
							dataCol.push(preData[keyArray[i]]["values"][j][keyArray[i]]);
						}

						if (isNaN(dataCol[0])) {
							dataCol.sort(collator.compare);
						}

						else {
							dataCol.sort(
								function (a, b) {
									return a - b;
								}
							);
						}

						dataCol.unshift(keyArray[i]);

						//debugger;
						data.push(dataCol);
					}

					// debugger;

					// Post the new data to the state and hide the window load-mask
					setTimeout(function(){
						context.setState({ 
							data: data, 
							table: response.tableName, 
							selectAll: selectAll, 
							filterAllowedColumnList: response.filterAllowedColumnList, 
							selectionList: [], 
							loadMask: false,
							loadDone: true, 
							datasourceId: response.datasourceId })
						}, 2000);
					context.props.dispatch( setTimer(new Date().getTime()) );
					context.props.dispatch(editModalDisplay(true));
				}		
			);	
		}
		
    }


	/**
	 * Resets the drag state on mouse up since dragging has stopped
	 */
	handleMouseUp() {
		if (this.props.allViewsDisplay) {
			this.setState({ dragState: 0 });
		}
	}


	/**
	 * Closes the modal through the redux store
	 */
	// handleBackClick = () => {
	// 	this.props.dispatch(editModalDisplay(false, null));
	// }


	/**
	 * Handles select all and deselect all for individual columns
	 * @param col: [colName, val1, val2, val3, ...]
	 * @param action: "select" / "deselect"
	 */
	selectDeselectCol(col, action) {

		// If the selectAll value for the column allows select all or you're trying to deselect all
		if (this.state.selectAll[col[0]] === "true" || action !== "select") {
			var sList = this.state.selectionList.slice();
			var index = this.checkSelected([col[0], col[1]], sList);

			if (action === "select") {
				if (index !== false) {
					sList[index[0]] = col;
				}
				else {
					sList.push(col);
				}
			}

			else {
				if (index !== false) {
					sList.splice(index[0], 1);
				}
			}

			this.setState({ selectionList: sList });
		}
	}
	

	/**
	 * Handles select all and deselect all for all the columns
	 * @param data: Array which contains arrays corresponding to each column
	 * @param action: "select" / "deselect"
	 */
	selectDesectAll(data, action) {
		if (action === "deselect") {
			this.setState({ selectionList: [] });
		}

		else {
			var sList = [];
			for (var i = 0; i < data.length; i++) {
				if (this.state.selectAll[data[i][0]] === "true") {
					var newList = [data[i][0]];

					for (var j = 1; j < data[i].length; j++) {
						newList.push(data[i][j]);
					}

					sList.push(newList);
				}
			}

			this.setState({ selectionList: sList });
		}
	}


	/**
	 * Handles click selection
	 * @param selection: [colName, selectionName]
	 * @param e: event instance which contains information about the click
	 */
	toggleSelection(selection, e) {
		var sList = this.state.selectionList.slice();
		var index = this.checkSelected(selection, sList);

		// Allow multiselection so long as the selectAll is allowed for that column and the ctrl key is pressed
		if ( (!e.ctrlKey && !e.shiftKey && !e.altKey) || !(this.state.selectAll[selection[0]] === "true") ) {
			if (index !== false) {
				sList.splice(index[0], 1);
				index = false;
			}
		}
		
		if (index !== false && index[1] !== false) {
			if (this.state.dragState === 0) {
				this.setState({ dragState: false, selection: selection });
				sList[index[0]].splice(index[1], 1);
			}
		}

		else {
			if (this.state.dragState === 0) {
				this.setState({ dragState: true, selection: selection });

				if (index !== false) {
					sList[index[0]].push(selection[1]);
				}
				else {
					sList.push( [selection[0], selection[1]] );
				}
			}
		}

		this.setState({ selectionList: sList });
	}


	/**
	 * Handles drag selection
	 * @param selection: [colName, selectionName]
	 */
	toggleDragSelection(selection) {
		// Only allow drag if that column allows selectAll
		if (this.state.selectAll[selection[0]] === "true") {
			var i, val, tempIndex;
			var sList = this.state.selectionList.slice();
			var index = this.checkSelected(selection, sList);

			var oldSelectionIndex = this.checkSelected(this.state.selection, this.state.data);
			var newSelectionIndex = this.checkSelected(selection, this.state.data);

			// Don't allow cross-column drag selection
			if (newSelectionIndex[0] === oldSelectionIndex[0]) {

				// Match was found
				if (index !== false && index[1] !== false) {
					if (this.state.dragState === false) {

						// Remove match
						sList[index[0]].splice(index[1], 1);

						// Remove all values between match and start of drag (incase one is missed)
						if (newSelectionIndex[1] > oldSelectionIndex[1]) {
							for (i = newSelectionIndex[1]; i > oldSelectionIndex[1]; i--) {
								val = this.state.data[newSelectionIndex[0]][i];
								tempIndex = sList[index[0]].indexOf(val);

								if (tempIndex !== -1) {
									sList[index[0]].splice(tempIndex, 1)
								}
							}
						}

						else {
							for (i = newSelectionIndex[1]; i < oldSelectionIndex[1]; i++) {
								val = this.state.data[newSelectionIndex[0]][i];
								tempIndex = sList[index[0]].indexOf(val);

								if (tempIndex !== -1) {
									sList[index[0]].splice(tempIndex, 1)
								}
							}
						}
					}
				}

				else {
					if (this.state.dragState === true) {

						// Match not found but col exists
						if (index !== false) {

							// Add match
							sList[index[0]].push(selection[1]);
							
							// Add all values between match and start of drag (incase one is missed)
							if (newSelectionIndex[1] > oldSelectionIndex[1]) {
								for (i = newSelectionIndex[1]; i > oldSelectionIndex[1]; i--) {
									val = this.state.data[newSelectionIndex[0]][i];
									if (sList[index[0]].indexOf(val) === -1) {
										sList[index[0]].push(val);
									}
								}
							}

							else {
								for (i = newSelectionIndex[1]; i < oldSelectionIndex[1]; i++) {
									val = this.state.data[newSelectionIndex[0]][i];
									if (sList[index[0]].indexOf(val) === -1) {
										sList[index[0]].push(val);
									}
								}
							}
						}

						else {
							sList.push([selection[0], selection[1]]);
						}
					}
				}

				this.setState({ selectionList: sList });
			}
		}
	}


	/**
	 * Checks if the value is selected
	 * @param array: [colName, value] value to check 
     * @param sList: List of selected values
	 * @returns: 	case: not found & nothing from that col has been selected yet: 		FALSE
	 * 				case: col found but value has not been selected: 					[colIndex, false]
	 * 				case: col found and val found:										[colIndex, valIndex]
	 */
	checkSelected(array, sList) {
		for (var i = 0; i < sList.length; i++) {
			if (sList[i][0] === array[0]) {
				var index = sList[i].indexOf(array[1]);
				if (index !== -1) {
					return [i, index];
				}
				else {
					return [i, false];
				}
			}
		}

		return false;
	}


	/**
	 * Checks if the value is selected (returns only true and false values for display)
	 * @param array: [colName, value] value to check 
     * @param sList: List of selected values
	 */
	checkSelectedDisplay(array, sList) {
		for (var i = 0; i < sList.length; i++) {
			if (sList[i][0] === array[0]) {
				if (sList[i].indexOf(array[1]) !== -1) {
					return true;
				}
				return false;
			}
		}
		return false;
	}


	/**
	 * Determines if the launch button should be disabled or not
	 */
	shouldLaunchBeDisabled() {
		var i;
		var selectAll = this.state.selectAll;
		var sList = this.state.selectionList;
		var keys = Object.keys(selectAll);
		var notSelectAll = [];
		
		for (i = 0; i < keys.length; i++) {
			if (selectAll[keys[i]] === "false") {
				notSelectAll.push(keys[i]);
			}
		}

		if (notSelectAll.length > 0) {
			for (i = 0; i < notSelectAll.length; i++) {
				var shouldBeDisabled = true;
				for (var j = 0; j < sList.length; j++) {
					if (notSelectAll[i] === sList[j][0]) {
						shouldBeDisabled = false;
						break;
					}
				}
				if (shouldBeDisabled) {
					return true;
				}
			}
		}

		if (sList.length === 0) {
			return true;
		}

		return false;
	}


	/**
     * This method searches on the data of table. Allows Multisearch using "," as a separator. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table and the textfield.
     */
	onKeyUpMultiSearch = (context, id) => {
		var i, j;

		// Search box
        var input = document.getElementById("tf-" + id);
		var filter = input.value.toUpperCase();

		// Column values
		var table = document.getElementById(id);
        var tr = table.getElementsByTagName("div");

		// Message "Search to View. Count: #"
		var fillerText = document.getElementById("st-" + id);

		// Message "Please refine the search."
		var errorText = document.getElementById("se-" + id);

		// Generate array of valid search values
		var fVals = filter.split(',');
		var filterValues = [];
		for (i = 0; i < fVals.length; i++) {
			fVals[i] = fVals[i].trim();
			if (fVals[i] !== "") {
				filterValues.push(fVals[i]);
			}
		}

		// No valid search value, show all
		if (filterValues.length === 0) {
			for (i = 0; i < tr.length; i++) {
				tr[i].style.display = "";
			}

			errorText.style.display = "none";

			if (fillerText !== null) {
				fillerText.style.display = "none";
			}
		}
		else {
			// Search all the records for the filter values
			for (i = 0; i < tr.length; i++) {
				var shouldBeVisible = false;
				
				for (j = 0; j < filterValues.length; j++) {
					
					if (tr[i].innerText !== "") {
						if (tr[i].innerText.toUpperCase().indexOf(filterValues[j]) !== -1) {
							shouldBeVisible = true;
							break;
						}
					}

					else {
						if (tr[i].parentElement.innerText.toUpperCase().indexOf(filterValues[j]) !== -1) {
							shouldBeVisible = true;
							break;
						}
					}
					
				}
				
				if (shouldBeVisible) {
					tr[i].style.display = "";
				}
				else {
					tr[i].style.display = "none";
				}
			}

			errorText.style.display = "none";

			if (fillerText !== null) {
				fillerText.style.display = "none";
			}
		}
	}


	/**
     * This method searches on the data of table. Allows Multisearch using "," as a separator. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table and the textfield.
     */
	onBlurMultiSearch = (context, id) => {
		var i, j;

		// Search box
        var input = document.getElementById("tf-" + id);
		var filter = input.value.toUpperCase();

		// Column values
		var table = document.getElementById(id);
        var tr = table.getElementsByTagName("div");

		// Message "Search to View. Count: #"
		var fillerText = document.getElementById("st-" + id);

		// Message "Please refine the search."
		var errorText = document.getElementById("se-" + id);

		// Generate array of valid search values
		var fVals = filter.split(',');
		var filterValues = [];
		for (i = 0; i < fVals.length; i++) {
			fVals[i] = fVals[i].trim();
			if (fVals[i] !== "") {
				filterValues.push(fVals[i]);
			}
		}

		// No valid search value, hide all and display filler message
		if (filterValues.length === 0) {
			if (tr.length > 500) {
				for (i = 0; i < tr.length; i++) {
					tr[i].style.display = "none";
				}
			}
			else {
				for (i = 0; i < tr.length; i++) {
					tr[i].style.display = "";
				}
				errorText.style.display = "none";
			}

			if (fillerText !== null) {
				fillerText.style.display = "";
			}
		}

		else {
			// Tracks if the search timed out
			var errorSet = false;

			// Save epoch time before starting search
			var date = new Date();
			var seconds = Math.round(date.getTime() / 1000);

			// Search all the records for the filter values
			for (i = 0; i < tr.length; i++) {

				// Every 200 records check if the seach should be timed out
				if (i % 200 === 0) {
					var date2 = new Date();
					var seconds2 = Math.round(date2.getTime() / 1000);
					if (seconds + 2 < seconds2) {

						// Hide all records that were displayed
						for (j = 0; j < tr.length; j++) {
							tr[j].style.display = "none";
						}

						// Display the error text
						errorText.style.display = "";
						errorSet = true;
						break;
					}
				}

				var shouldBeVisible = false;
				
				for (j = 0; j < filterValues.length; j++) {
					// If there is a space in the search term then search the full text for it
					if (filterValues[j].includes(" ")) {
						if (tr[i].innerHTML.toUpperCase().indexOf(filterValues[j]) !== -1) {
							shouldBeVisible = true;
							break;
						}
					}
					else {
						// Otherwise check the beginning of each word
						var words = tr[i].innerText.toUpperCase().split(" ");
						for (var k = 0; k < words.length; k++) {
							if (words[k].startsWith(filterValues[j])) {
								shouldBeVisible = true;
								break;
							}
						}
					}
				}
				
				if (shouldBeVisible) {
					tr[i].style.display = "";
				}
				else {
					tr[i].style.display = "none";
				}
			}

			if (!errorSet) {
				errorText.style.display = "none";
			}

			if (fillerText !== null) {
				fillerText.style.display = "none";
			}
		}
	}

	onLaunchResultCallback(success) {
		console.log("-----------------------Stage 4 reached");

		if (success) {
			this.props.history.push('/glyph-viewer');
		}
		else {
			this.setState({ snackbarVisible: true });
		}
	}

	render() {
		var data = this.state.data;
		var context = this;

		var displayData = this.state.data.map( function(col) {
			return (
				<Flexbox 
					flexDirection = "column" 
					key = { col } 
					style = {{ 
						width: "100%", 
						border: "1px solid", 
						borderLeft: (col === data[0] ? "1px solid" : "none"),
						borderBottomLeftRadius: (col === data[0] ? "3px" : ""),
						borderTopLeftRadius: (col === data[0] ? "3px" : ""),
						borderBottomRightRadius: (col === data[data.length - 1] ? "3px" : ""),
						borderTopRightRadius: (col === data[data.length - 1] ? "3px" : "")
					}} 
				>
					<div 
						className = "noselect" 
						style = {{ 
							backgroundColor: "#42459c", 
							color: "#ffffff", 
							height: "27px",
    						fontSize: "21px",
							textAlign: "center"
						}} 
					> 
						<div style = {{ textAlign: "left", marginTop: "2px", marginLeft: "5px", fontSize: "18px" }} > 
							<i className = "fa fa-check" style = {{ marginRight: "3px" }} onClick = { () => context.selectDeselectCol(col, "select") } /> 
							<i className = "fa fa-times" onClick = { () => context.selectDeselectCol(col, "deselect") } /> 
						</div>

						<div style = {{ marginTop: "-16px", paddingBottom: "4px", fontSize: "14px" }} > 
							{col[0].length > 16 ? col[0].substring(0,15) + "..." : col[0]} 
						</div>
					</div>

					<div style = {{ margin: "1px 1px 0px 0px" }} >
						<SearchBox 
							ref = "SearchBox"
							settings = {{
								SearchBoxClearHover: context.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
								searchBoxUnderline: context.props.settings.colors.pinFilterColor.searchBoxUnderline,
								overviewButtonsColorBg: context.props.settings.colors.overviewButtonsColor.background,
								overviewButtonsColorText: context.props.settings.colors.overviewButtonsColor.text,
								tableSelectColor: context.props.settings.colors.tableSelectColor.background
							}}
							onTextFieldValueChange = { col.length > 500 ? (evt) => context.onBlurMultiSearch(context, col[0]) : (evt) => context.onKeyUpMultiSearch(context, col[0]) }
							id = { "tf-" + col[0] }
							collapseButton = { false }
							shouldOnBlur = { col.length > 500 ? true : false }
						/>
					</div>

					<div id = { col[0] } style = {{ overflow: "auto" }} >

						{ (col.length > 500 ? <div id = { "st-" + col[0] } style = {{ margin: "10px 0px 0px", textAlign: "center" }} > Search to view. <br /> Count: {col.length - 1} </div> : null) }
						<div id = { "se-" + col[0] } style = {{ margin: "10px 0px 0px", textAlign: "center", display: "none" }} > Please refine the search. </div>

						{col.map( function(elem) {
							return (
								(elem !== col[0] ? 
									<div
										key = { col[0] + elem } 
										onMouseDown = { (e) => context.toggleSelection([col[0], elem], e) }
										onMouseEnter = { (e) => (e.buttons > 0 ? context.toggleDragSelection([col[0], elem]) : null ) }
										style = {{ 
											display: (col.length > 500 ? "none" : "")
										 }}
									>
										<AllViewsRow selected = { context.checkSelectedDisplay([col[0], elem], context.state.selectionList) } >
											{elem} 
										</AllViewsRow>
									</div> : ""
								)
							)
						})}

					</div>
				</Flexbox>
			)
		});

		return(
			<div
				style={{paddingTop: "10px"}}
				/*title = { this.props.type }
				contentStyle = {{ width: "95%", maxWidth: "none", backgroundColor: "#c5c5f7" }}
				bodyStyle = {{ backgroundColor: "#c5c5f7" }}
				actionsContainerStyle = {{ backgroundColor: "#c5c5f7" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", fontSize: "36px", lineHeight: "16px" }}
				modal = { true }
				open = { this.props.allViewsDisplay }
				actions = {
					[
						<FlatButton
							label = "Back"
							primary = { true }
							onClick = { this.handleBackClick }
							style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
						/>,
						<RaisedButton 
							label = { "Launch" }
							style = {{
								width: "112px",
								margin: "0px 10px 9px 0px"
							}}
							buttonStyle = {{
								height: '35px',
								lineHeight: '35px',
								backgroundColor: (this.shouldLaunchBeDisabled() ? "grey" : this.props.settings.colors.buttons.general)
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
							disabled = { this.shouldLaunchBeDisabled() }
							onClick = { () => this.props.onLaunch({
								tableName:this.state.table,
								frontEndFilters: this.state.selectionList,
								originalVizName: this.props.type,
								datasourceId: this.state.datasourceId,
								filterAllowedColumnList: this.state.filterAllowedColumnList,
							},this.onLaunchResultCallback) }
							primary = {true } 
						/>
					]
				} */
			>
				<div style = {{ marginTop: "5vh", height: "55vh", width: "100%", display: (this.state.loadMask ? "" : "none") }} > 
					<ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
				</div> 

				<div style = {{ height: "60vh", paddingBottom: "30px", display: ((this.state.loadMask === false && this.state.loadDone) ? "block" : "none") }} >
					<Flexbox flexDirection = "row" style = {{ backgroundColor: "#ffffff", height: "100%" }} >
						{displayData}
					</Flexbox>

					<Snackbar
						open = { this.state.snackbarVisible }
						message = "No matches for the selected values"
						autoHideDuration = { 2000 }
						onRequestClose = { () => this.setState({ snackbarVisible: false }) }
					/>
					<div>
						<div style={{float: 'left', display: ((this.state.loadMask === false && this.state.loadDone) ? "" : "none")}}>
							<RaisedButton 
								label = { <span> <i className = "fa fa-check" style = {{ fontSize: "22px", margin: "1px 0px 0px" }} /> Select All </span> }
								style = {{
									width: "121px",
									margin: "8px 10px 9px 0px"
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
								onClick = { () => this.selectDesectAll(data, "select") }
								primary = { true } 
							/>

							<RaisedButton 
								label = { <span> <i className = "fa fa-times" style = {{ fontSize: "22px", margin: "1px 0px 0px" }} /> Deselect All </span> }
								style = {{
									width: "135px"
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
								onClick = { () => this.selectDesectAll(data, "deselect") }
								primary = { true } 
							/>
						</div>
						<div style={{float: "right", padding: "8px 0px 8px 8px", display: ((this.state.loadMask === false && this.state.loadDone) ? "" : "none")}}>
							{/* <FlatButton
								label = "Back"
								primary = { true }
								onClick = { this.handleBackClick }
								style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton"black", backgroundColor: "#efefef" }}
							/> */}
							<Flexbox>
								<h3 style={{ 
								fontSize: "22px", 
								/* border: "1px solid black", */ 
								paddingTop: "10px",
								fontWeight: "300",
								margin: "0px",
            					padding: "8px 31px 0 0"}}><b style={{fontSize:"28px"}}>③</b> Select Filter(s) and Launch.</h3>

								<RaisedButton 
									label = { "Launch" }
									style = {{
										width: "112px",
										/* margin: "0px 10px 9px 0px" */
									}}
									buttonStyle = {{
										height: '35px',
										lineHeight: '35px',
										/* shouldLaunchBeDisabled() not working */
										backgroundColor: (this.shouldLaunchBeDisabled() ? "grey" : /*this.props.settings.colors.buttons.general) */"#000")
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
									disabled = { this.shouldLaunchBeDisabled() }
									onClick = { () => this.props.onLaunch({
										tableName:this.state.table,
										frontEndFilters: this.state.selectionList,
										originalVizName: this.props.type,
										datasourceId: this.state.datasourceId,
										filterAllowedColumnList: this.state.filterAllowedColumnList,
									},this.onLaunchResultCallback) }
									primary = {true } 
								/>
								</Flexbox>
						</div>
					</div>
				</div>

			</div>
		);
	}
}


/**
 * This component handles the front-end filters
 * @param type: string name of the selection made
 */
class AllViewsRow extends React.Component {


	/**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
	shouldComponentUpdate(nextProps, nextState){
		return (this.props.selected !== nextProps.selected);

		/*
        if (this.props.selected != nextProps.selected) {
			
            return true;
        }
        return false;
		*/
    };
	
	
	render() {
		return(
			<p
				className = {this.props.selected ? "noselect darkHover" : "noselect lightHover" }
				style = {{ 
					backgroundColor: ( this.props.selected ? "#7c78a0" : "white" ), 
					textAlign: "center",
					padding: "2px",
					margin: "0"
				}} 
			> 
				{this.props.children}
			</p>
		);
	}
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (allViewsModal, selectedFilteredModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    allViewsModal,
	selectedFilteredModal
});

export const updateFilterFromSnapshot = (snapshot) => ({
    type: 'UPDATE_FILTER_SNAPSHOT',
    snapshot
});

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
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
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(allViewsModal));
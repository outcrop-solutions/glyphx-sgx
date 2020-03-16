import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
/* import Dialog from 'material-ui/Dialog'; */
import Flexbox from 'flexbox-react';
/* import FlatButton from 'material-ui/FlatButton'; */
import Snackbar from 'material-ui/Snackbar';
import RaisedButton from 'material-ui/RaisedButton';
// import Divider from 'material-ui/Divider';
import { makeServerCall } from './ServerCallHelper.js';
import { webSocketSend } from './GeneralFunctions.js';
import SearchBox from './SearchBox.js';
import ComponentLoadMask from './ComponentLoadMask.js';
import './css/General.css';


/**
 * This component handles the front-end filters
 * @param type: string name of the selection made
 * @param typeURL: URL corresponding with the type to make backend call
 */

const tip_arr = [
	"./Res/Img/Tip1.png",
	"./Res/Img/Tip2.png",
	"./Res/Img/Tip3.png",
	"./Res/Img/Tip4.png",
	"./Res/Img/Tip5.png",
	"./Res/Img/Tip6.png",
	"./Res/Img/Tip7.png",
];

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
		loadDone: false,
		selectAll500: false,
		sdtUrl: "",
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
		var context = this;

		//for desktop side
		if(window.location.href.indexOf('uid') > -1){
			context.props.dispatch(
				setUid(window.location.href.slice(
					window.location.href.indexOf('uid')+4, 
					window.location.href.indexOf('#'))));
		}

		const socket = new WebSocket(`ws://${window.SERVER_URL.slice(7, window.SERVER_URL.length-1)}`);
		console.log(socket)
		this.props.dispatch(setSocket(socket));
		// listening
		socket.addEventListener('message', function (event) {
			let data;
			console.log(event);
			if(event.data.length > 0){
				data = JSON.parse(event.data);
				if(event.data.indexOf('{') > -1) {
					console.log(event.data)
					// console.log('Placeholder.')
				}
				else if(event.data.launch === true){
					console.log('yes launch');
				}
			}
		});
		// this.props.dispatch(setSocket(socket));

		
		//intial tip generate
		let previous_tip;
		document.getElementById('tip_gen').src = tip_arr[Math.floor(Math.random() * tip_arr.length)];

		this.timer = setInterval(() => {
			if(document.getElementById('tip_gen')){
				let new_tip = Math.floor(Math.random() * tip_arr.length);
				if(previous_tip === new_tip){
					new_tip = Math.floor(Math.random() * tip_arr.length);
				}
				
				document.getElementById('tip_gen').src = tip_arr[new_tip];
				previous_tip = new_tip;
			}
		}, 10000);

	}
	

	/**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {
		// Removing listener so it doesnt linger across the site
		window.onmouseup = null;
		clearInterval(this.timer);
	}


	/**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
		return (this.state.selectionList !== nextState.selectionList || this.state.data !== nextState.data 
			|| this.props.allViewsDisplay !== nextProps.allViewsDisplay || 
			this.state.snackbarVisible !== nextState.snackbarVisible
		);

		
        /* if (this.state.selectionList != nextState.selectionList || this.state.data != nextState.data) {
            return true;
        }

		if (this.props.allViewsDisplay != nextProps.allViewsDisplay) {
			return true;
		}

        return false; */
		
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
			this.setState({ loadMask: true, loadDone: false, data: [] });

            var context = this;
			var index = nextProps.typeURL.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");

			// console.log(nextProps.typeURL.substring(index + 1));
			let str = nextProps.typeURL.substring(index + 1).replace("\\", "/");
			context.setState({sdtUrl: str});
			console.log(str);
			// console.log(nextProps.typeURL.substring(index + 1), 'whats going on')
			//debugger;

			// Get the data corresponding to the URL
			// debugger;
			// let startTime = window.performance.now();
			// let endTime;
			// makeServerCall(window.encodeURI('frontEndFilterData/' + nextProps.typeURL.substring(index + 1) ),
			// 	function (responseText) { 
			// 		endTime = window.performance.now();
			// 		var response = JSON.parse(responseText);
			// 		var preData = response.frontEndFilterData; 
			// 		var data = [];
			// 		var selectAll = {};
			// 		var keyArray = Object.keys(preData);
			// 		console.log(preData, keyArray, endTime-startTime)
			// 		var collator = new Intl.Collator(undefined, {numeric: true, sensitivity: 'base'});

			// 		// debugger;

			// 		//console.log(response);

			// 		context.props.dispatch(updateFilterFromSnapshot({}));

			// 		// Seperate and store the actual data and the selectAll boolean of each column
			// 		for (var i = 0; i < keyArray.length; i++) {
			// 			var dataCol = [];
			// 			selectAll[keyArray[i]] = preData[keyArray[i]]["selectAll"];

			// 			for (var j = 0; j < preData[keyArray[i]]["values"].length; j++) {
			// 				dataCol.push(preData[keyArray[i]]["values"][j][keyArray[i]]);
			// 			}

			// 			if (isNaN(dataCol[0])) {
			// 				dataCol.sort(collator.compare);
			// 			}

			// 			else {
			// 				dataCol.sort(
			// 					function (a, b) {
			// 						return a - b;
			// 					}
			// 				);
			// 			}

			// 			dataCol.unshift(keyArray[i]);

			// 			//debugger;
			// 			data.push(dataCol);
			// 		}
			// 		console.log(selectAll, typeof selectAll);

			// 		// debugger;

			// 		// Post the new data to the state and hide the window load-mask
			// 		// setTimeout(function(){
			// 		context.setState({ 
			// 			// data: data, 
			// 			// table: response.tableName, 
			// 			selectAll: selectAll, 
			// 			// filterAllowedColumnList: response.filterAllowedColumnList, 
			// 			// selectionList: [], 
			// 			// loadMask: false,
			// 			// loadDone: true, 
			// 			// datasourceId: response.datasourceId });
			// 		});
			// 		context.props.dispatch( setTimer(new Date().getTime()) );
			// 		context.props.dispatch(editModalDisplay(true));
			// 	}		
			// );	
				makeServerCall('frontEndFiltersEC2', 
				function(responseText) {
					// let megaArr = [];
					let response = JSON.parse(responseText);
					console.log(response.body, response);
					/*
					*RESPONSE BODY STRUCTURE
					*{
					*statusCode: #
					*	body: {
					*		filters: [ [], [] ...],
					*		tableName: "",
					*		datasourceId: "",
					*       selectAll: {},
					*		filterAllowedColumnList: [],
					*		initialX: "",
					*		initialY: "",
					*		initialZ: ""
					*}	
					*}
					*/
					context.props.dispatch(updateFilterFromSnapshot({}));
					if(response && response.statusCode === 200){
						let results = response.body;
						context.setState({
							table: results.tableName,
							data: results.filters,
							selectAll: results.selectAll, 
							filterAllowedColumnList: results.filterAllowedColumnList,
							datasourceId: results.datasourceId,
							loadMask: false,
							loadDone: true,
							selectionList: [] });
						context.props.dispatch(setInitialXYZ(results.initialX, results.initialY, results.initialZ));
						context.props.dispatch( setTimer(new Date().getTime()) );
						context.props.dispatch(editModalDisplay(true));
					}
				}, {
					post: true,
					data: {key: nextProps.typeURL.substring(index + 1)}
				});
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
	shouldLaunchBeDisabled(data) {
		if(data.length === 0) return false;
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
						document.getElementById("launch_button").style.cursor = "pointer";
						break;
					}
				}
				if (shouldBeDisabled) {
					if(document.getElementById("launch_button")){
						document.getElementById("launch_button").className = "launch_button_null";
						document.getElementById("launch_button").style.cursor = "not-allowed";
					} 
					return true;
				}
			}
		}

		if (sList.length === 0) {
			if(document.getElementById("launch_button")){
				document.getElementById("launch_button").className = "launch_button_null";
				document.getElementById("launch_button").style.cursor = "not-allowed";
			} 
			return true;
		}
		document.getElementById("launch_button").style.cursor = "pointer";
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

	dataFunc(data){
		var context = this;
		
		if(data){
			if(data.length && this.state.loadDone){
				return this.state.data.map( function(col) {
					console.log(document.getElementById('expand'+col[0]))
					return (
						<Flexbox 
							flexDirection = "column" 
							key = { col } 
							style = {{ 
								width: "100%", 
								maxHeight: "25.8vh", 
								marginBottom: "1.043vh"
								// borderLeft: (col === data[0] ? "1px solid" : "none"),
								// borderBottomLeftRadius: (col === data[0] ? "3px" : ""),
								// borderTopLeftRadius: (col === data[0] ? "3px" : ""),
								// borderBottomRightRadius: (col === data[data.length - 1] ? "3px" : ""),
								// borderTopRightRadius: (col === data[data.length - 1] ? "3px" : "")
							}} 
						>
							<div 
								className = "noselect" 
								style = {{ 
									backgroundColor: "#0c1836", 
									padding: "1.356vh 0px 0.938vh 1.877vh",
									color: "#ffffff",
									fontSize: "1.877vh",
									fontFamily: "ITCFranklinGothicStd-DmCd",
									minHeight: "4.8vh",
									letterSpacing: "0.5px"
								}} 
							> 

								{/* <div style = {{ fontSize: "18px", letterSpacing: "0.5px" }} >  */}
									{/* {col[0].length > 16 ? col[0].substring(0,15) + "..." : col[0]}  */}
									{col[0]}
								{/* </div> */}
								{/* <span

									// onClick= {col[0] => context.expandCollapseTable(e.target.innerText)}
								> 
								<i className = "fa fa-caret-down" style = {{ fontSize: "22px", margin: "1px 0px 0px" }} />
								</span> */}
								{/* <img src="./Res/Img/Enlarge.png"/> */}

								<div style = {{ 
										display: "inline-block", 
										float: "right", 
										margin: "0px 1.877vh 0px 0px", 
										fontSize: "1.877vh" }} > 
									<div style = {{ 
									// position: "absolute", 
									// width: "46.36vw",
									float: "left",
									margin: "-0.413vh 1.548vh 0 0",
									backgroundColor: "white" }} >
										<SearchBox 
											ref = "SearchBox"
											settings = {{
												SearchBoxClearHover: context.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
												searchBoxUnderline: context.props.settings.colors.pinFilterColor.searchBoxUnderline,
												overviewButtonsColorBg: context.props.settings.colors.overviewButtonsColor.background,
												overviewButtonsColorText: context.props.settings.colors.overviewButtonsColor.text,
												tableSelectColor: context.props.settings.colors.tableSelectColor.background
											}}
											onTextFieldValueChange = { col.length > 500 ? 
												(evt) => { context.onBlurMultiSearch(context, col[0]); document.getElementById('expand'+col[0]).style.display = ""; } : 
													(evt) => { context.onKeyUpMultiSearch(context, col[0]); document.getElementById('expand'+col[0]).style.display = ""; } }
											id = { "tf-" + col[0] }
											collapseButton = { false }
											shouldOnBlur = { col.length > 500 ? true : false }
										/>
									</div>
									<span
									// className = "fa fa-check" 
										style = {{ 
											marginRight: "2.086vh", 
											cursor: (context.state.selectAll[col[0]] === 'true'? "pointer" : "not-allowed"),
											color: (context.state.selectAll[col[0]] === 'true' ? "white" : "darkgrey") }}
										// title= "Select All" 
										onClick = { () => {
											context.selectDeselectCol(col, "select"); 
											context.setState({selectAll500: true}) }} 
									> 
										Select All
									</span> 
									<span
										style = {{cursor: "pointer"}}
										// className = "fa fa-times" 
										onClick = { () => {
											context.selectDeselectCol(col, "deselect"); 
											context.setState({selectAll500: false}) }} 
									>
										Clear
									</span> 
								
									<i 
										id = {'collapse10481' + col[0]}
										onClick={(e) => context.expandCollapseTable(e.target.id, e.target.className)}
										style={{
											verticalAlign: "sub",
											fontSize: "3.754vh", 
											margin: "-0.834vh 0px 0px 2.503vh",
											cursor: "pointer"}} 
										className= { context.state.data[0][0] === col[0] ? "fa fa-caret-up" : "fa fa-caret-down"}
									/>
								</div>

								

							</div>

							<div
								id = {'expand' + col[0]} 
								className = "customScroll"
								style={{
									display: (context.state.data[0][0] === col[0] ? "" : "none"),
									border: "1px solid",
									borderLeft: "1px solid black",
									borderBottom: "1px solid black",
									overflowY: "scroll"	
							}}>
								{/* <div style = {{ 
										position: "absolute", 
										width: "46.36vw",
										backgroundColor: "white" }} >
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
								</div> */}

								<div /* className = "customScroll" */ id = { col[0] } style = {{ overflow: "auto", /* marginTop: "3.128vh" */ }} >

									{ (col.length > 500 ? 
										<div className = {`${context.state.selectAll500 ? "high-count-div dark-color" : "high-count-div light-color"}`} 
											id = { "st-" + col[0] } 
											> 
											500+ Results Returned. <br/> Search to Filter and View. <br /> <br /> Results: {col.length - 1} 
										</div> : null) }

									<div id = { "se-" + col[0] } style = {{ margin: "1.043vh 0px 0px", textAlign: "center", display: "none" }} > Please refine the search. </div>

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
														{elem.toString()} 
													</AllViewsRow>
												</div> : ""
											)
										)
									})}

								</div>
							</div>
						</Flexbox>
					)
				});
			}
			else if(data.length === 0 && this.state.loadDone === true){
				return(
					<div className= "no-results" style={{textAlign: "center", fontSize: "22px"}}>
						<h3 style={{paddingTop: "3.650vh", fontFamily: "ITCFranklinGothicStd-Demi"}}>No Filter Options Available.</h3>
					</div>
				);
			}
		}
	}

	wbSocketFxn(){
		let data, query;
		var outerIndex;

		if(this.state.selectionList.length > 0){
			query = "SELECT * FROM " + this.state.table + " WHERE ";
		}
		else {
			query = "SELECT * FROM " + this.state.table + ";";
		}

		if(this.state.selectionList.length > 0){
			data = this.state.selectionList.slice();
			for (outerIndex = 0; outerIndex < data.length; outerIndex++) {
				var dataItem = data[outerIndex].slice();
				var columnName = dataItem[0];

				// Removes the 1st element that is the name.
				dataItem.shift(); 

				var values = '("' + dataItem.toString() + '")';
				query = query + columnName + " IN " + values.replace(/,/g , '","');
				
				//was !=
				if (outerIndex !== data.length-1) {
					query = query + " AND ";
				}

				else {
					query = query + ";";
				}
			}
		}

		if(query.indexOf(';') && this.props.legend_url_arr){
			let instit = this.props.userInfo.institutionDir.slice(25, this.props.userInfo.institutionDir.length-1);
			let instit_new;
			if(instit === 'glyphed_demo') instit_new = 'glyphed-demo-source';
			if(instit === 'notredame') instit_new = 'notredame-source';
			this.props.webSocket.send(JSON.stringify({
				url_uid: this.props.uid,
				//CHANGING INSTITUTION
				sdt: `https://viz-group-${instit_new}.s3.us-east-2.amazonaws.com/${this.state.sdtUrl}`,
				legendURLArr: this.props.legend_url_arr,
				query,
				institution: instit,
				launch: true
			}));
		}
	}

	expandCollapseTable(target, name){
		let substring = target.slice(target.indexOf('collapse10481')+13);
		let id = 'expand' + substring;

		if(name === 'fa fa-caret-down') {
			document.getElementById(target).className = 'fa fa-caret-up';
		}
		else document.getElementById(target).className = 'fa fa-caret-down';

		if(document.getElementById(id)){
			if(document.getElementById(id).style.display === "none"){
				// console.log('what? 1')
				document.getElementById(id).style.display = "block";
			}
			else document.getElementById(id).style.display = "none";
		}
	}

	render() {
		var data = this.state.data;
		var context = this;
	
		return(
			<div style={{height: "100%", minHeight: "102vh"}}
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
						/>
					]
				} */
			>

				<h3 style={{ 
					height: "6vh",
					paddingTop: "1.043vh",
					padding: "1.877vh 0px 1.043vh 2.607vh",
					fontSize: "2.086vh", 
					fontFamily: "ITCFranklinGothicStd-DmCd",
					margin: "0px",
					fontWeight: "300",
					textTransform: "uppercase",
					letterSpacing: "1px",
					color: ((this.state.loadMask === true || this.state.loadDone === true) ? "black" : "lightgrey")
				}}>  
					<img style={{verticalAlign: "middle", marginRight: "1.043vh", height: "3.024vh"}} 
						alt="Third Step"
						src="./Res/Img/3@2x.png"/> 
					Select Additional Filter(s)
				</h3>

				<div style = {{ 
					height: "96.32vh", 
					fontSize: "3.858vh", 
					textAlign: "center",
					fontFamily: "ITCFranklinGothicStd-DmCd",
					backgroundColor: "lightgrey",
					color: "white",
					paddingTop: "1.37vh",
					display: ((this.state.loadMask === false && !this.state.loadDone) ? "block" : "none") }}>

					{/* TIP: */}

					<br/>

					{/* <div style={{
						wordBreak: "break-word", 
						padding: "1.877vh 6.257vh 0px 6.257vh",
						height: "19.812vh"}} 
					> */}

					<img style={{width: "50vw"}} id="tip_gen" alt="GlyphEd Tip"/>

					{/* </div> */}
					<div style={{marginTop: "-5.160vh"}}>
						<span 
							style={{
								// color: "darkblue", 
								color: "rgb(3, 26, 114)",
								textDecoration: "underline",
								cursor: "pointer",
								fontSize: "2.294vh",
								textTransform: "uppercase",
								fontFamily: "ITCFranklinGothicStd-Demi"
							}} 
							onClick={() => /* window.open('https://s3.amazonaws.com/synglyphx/tutorials/home.html', '_blank') */
							this.props.uid ? webSocketSend(this.props.webSocket, this.props.uid, 'tutorial') : ""}
						>
							See tutorials to learn more.
						</span>
					</div>

				</div>

				<div style = {{ height: "80vh", width: "100%", display: (this.state.loadMask ? "" : "none") }} > 
					<ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
				</div> 

				<div style = {{ 
					minHeight: "97.5vh",
					display: ((this.state.loadMask === false && this.state.loadDone) ? "block" : "none"), 
					padding: "0px 2.920vh 0px 2.711vh" }} 
				>

					<div style={{
						float: 'right', 
						marginBottom: "0.834vh",
						display: ((this.state.loadMask === false && this.state.loadDone && data.length > 0) ? "" : "none")}}>
					
						{/*<RaisedButton 
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
							onClick = { () => {this.selectDesectAll(data, "deselect")} }
							primary = { true } 
						/> */}

						<label style={{fontFamily: "ITCFranklinGothicStd-Med", fontSize: "1.877vh"}}> 
							<input 
								type="checkbox"
								className = "selectAllCheck"
								style={{marginBottom: "1.564vh", cursor: "pointer"}}
								onChange = {(e) => (
									e.target.checked === true ? this.selectDesectAll(data, "select") : 
										this.selectDesectAll(data, "deselect") )}
							/>
							Select All
						</label>
						
						<span style={{
							fontFamily: "ITCFranklinGothicStd-Med", 
							fontSize: "1.877vh", 
							verticalAlign: "text-bottom",
							padding: "0 0.619vh"}}></span>

						<RaisedButton
							buttonStyle = {{
								height: '3.650vh',
								lineHeight: '3.650vh',
								backgroundColor: "white",
								color: "black",
								fontSize: "1.877vh",
								fontFamily: "ITCFranklinGothicStd-Med",
								borderRadius: "0",
								border: "1px solid black",
							}} 
							labelStyle = {{
								textAlign: "center",
								color: "white",
								margin: "0px 0px 0px -0.313vh",
								paddingLeft: "0px",
								paddingRight: "0px",
							}}
							overlayStyle = {{
								height: '3.650vh',
								lineHeight: '3.650vh',
								width: "4.8vw"
							}}
							onClick={() => this.selectDesectAll(data, "deselect")}
							style={{}}> 
							Clear All 
						</RaisedButton>

						
					</div>

					{/* the data table*/}

					<Flexbox flexDirection = "column" style = {{ backgroundColor: "#ffffff", clear: "both" }} >
						{data ? context.dataFunc(data) : ""}
					</Flexbox>
					<br/>
					<Snackbar
						open = { this.state.snackbarVisible }
						message = "No matches for the selected values"
						autoHideDuration = { 2000 }
						onRequestClose = { () => this.setState({ snackbarVisible: false }) }
					/>
					<div>
						
						<div style={{/* float: "right",*/ 
							padding: "0px 0px 2.086vh 0px", 
							display: ((this.state.loadMask === false && this.state.loadDone) ? "" : "none")}}>
							{/* <FlatButton
								label = "Back"
								primary = { true }
								onClick = { this.handleBackClick }
								style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton"black", backgroundColor: "#efefef" }}
							/> */}
							<Flexbox>

								{/* <RaisedButton 
									label = { "Launch Viewer" }
									style = {{
										height: '60px',
										width: '350px',
										boxShadow: "0",
										margin: "auto",
									}}
									buttonStyle = {{
										backgroundColor: (this.shouldLaunchBeDisabled() ? "darkgrey" : "#fdc743")
									}} 
									labelStyle = {{
										fontSize: "20px",
										fontFamily: "ITCFranklinGothicStd-Demi",
										color: "#0c1836",
										letterSpacing: "1px",
										lineHeight: "58px",
										margin: "0px 0px 0px -3px",
										paddingLeft: "0px",
										paddingRight: "0px"
									}}
									disabled = { this.shouldLaunchBeDisabled() }
									onClick = { () => {
										this.props.onLaunch({
										tableName:this.state.table,
										frontEndFilters: this.state.selectionList,
										originalVizName: this.props.type,
										datasourceId: this.state.datasourceId,
										filterAllowedColumnList: this.state.filterAllowedColumnList,
									},this.onLaunchResultCallback), this.wbSocketFxn();
									}}
									primary = {true } 
								/> */}
								<img
									id = "launch_button"
									className="launch_button"
									alt = "GlyphIT Logo Launch Button"
									src={(this.shouldLaunchBeDisabled(data) ? "./Res/Img/GlyphIT-gray.png" : "./Res/Img/GlyphIT.png")}
									style={{
										width: "33.09%", 
										height: "100%",
										margin: "auto", 
										border: "1px solid black",
										boxShadow: "0.36496vh 0.36496vh 0.36496vh"}}
									onClick={() => {
										(this.shouldLaunchBeDisabled(data) ? 
											null :
											(this.props.onLaunch({
												tableName:this.state.table,
												frontEndFilters: this.state.selectionList,
												originalVizName: this.props.type,
												datasourceId: this.state.datasourceId,
												filterAllowedColumnList: this.state.filterAllowedColumnList,
											},this.onLaunchResultCallback), this.wbSocketFxn()
											)
										)
									}}
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
					backgroundColor: ( this.props.selected ? "#bfbfbf" : "white" ), 
					padding: "0.626vh 0.626vh 0.626vh 1.564vh",
					margin: "0px",
					fontSize: "1.668vh",
					fontFamily: "ITCFranklinGothicStd-Med",
					display: (this.props.children.length)
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

export const setInitialXYZ = (X, Y, Z) => ({
    type: 'SET_INITIAL_XYZ',
	X,
	Y,
	Z
});

export const setUid = (uid) => ({
    type: 'SET_UID',
    uid,
});

export const setSocket = (socket) => ({
    type: 'SET_SOCKET',
    socket,
});

/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
	allViewsDisplay: state.filterState.ModalDisplay.allViewsModal,
	uid: state.filterState.uid,
	webSocket: state.filterState.webSocket,
	legend_url_arr: state.filterState.legend_url_arr,
	VizParams: state.filterState.VizParams,
	userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(allViewsModal));
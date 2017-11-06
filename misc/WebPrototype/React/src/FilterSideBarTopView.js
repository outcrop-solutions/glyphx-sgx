import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { makeServerCall } from './ServerCallHelper.js';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import MenuItem from 'material-ui/MenuItem';
import Flexbox from 'flexbox-react';
import TextField from 'material-ui/TextField';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import FilterViewForm from './FilterSummaryView.js';
import Select from 'react-select';
import Snackbar from 'material-ui/Snackbar';
import './FilterSideBar.css';
import 'react-select/dist/react-select.min.css';

/**
 * This is the top section which contains the overview in the filter side-bar
 * @param initParams: - ADCMT
 * @param colList: - ADCMT
 * @param showAlert: - ADCMT
 */
class FilterSideBarTopView extends React.Component {
	
	constructor(props) {
        super(props);
	
        // Load values into the view select dropdown.
        var viewSelectItems = this.makeList(props.initParams.viewSelectItems, "viewSelectItems");

        // Store the states of all the elements
        this.state = {
            topViewVisible: true,
            hideShowButtonTextFlag: true,
            menu: { open: false },
            viewSelectValue: null,
            tableSelectValues: [],
            saveDailogOpen: false,
            viewSelectItems: viewSelectItems,
            viewNameTextFieldError: "",
            statisticsModalOpen: false,
            statisticStatSelectValues: "",
            statisticColSelectValues: "",
            undoSnackbar: false,
            redoSnackbar: false
        };
	}


    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState){
        return (this.state != nextState || this.props.initParams != nextProps.initParams || this.props.settings != nextProps.settings || this.props.statisticDisplay != nextProps.statisticDisplay);
        
        /*
        if (this.state != nextState) {
            return true;
        }
        else if (this.props.initParams != nextProps.initParams) {
            return true;
        }
        else if (this.props.settings != nextProps.settings) {
            return true;
        }
        else if (this.props.statisticDisplay != nextProps.statisticDisplay) {
            return true;
        }
        return false;
        */
    };


    /**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {
        // Initialize local storage for undo redo
        window.localStorage.setItem('history', []);
        window.localStorage.setItem('position', -1);
    }


    /**
	 * React built-in which is called when component unmounts
	 */
    componentWillUnmount() {
        // Remove local storage items no longer in use
        window.localStorage.removeItem("history");
        window.localStorage.removeItem("position");
    }
	

	/**
     * This function holds the templates of certain UI elements. Call this and pass a type
     * to generate a JSX code that you can use to render that particular element.
     * @param {array} arrValues: Single dimension array of values.
     * @param {string} type: any one of [viewSelectItems,tableSelectItems,appliedFiltersItems,columns]
     * @param {object} extra: An object to pass extra params.
     */
    makeList(arrValues, type, extra) {
        if (!Array.isArray(arrValues)) {
            return "PLEASE PROVIDE AN ARRAY";
        }

        var len = arrValues.length;
        var index;
        var arrReturn = [];
        var objReturn = null;

        switch (type) {
            case 'viewSelectItems':
                for (index = 0; index < len; index++) {
                    arrReturn.push(
                        {
                            label: arrValues[index], value: arrValues[index]
                        }
                    );
                }
                break;

            case 'tableSelectItems':
                for (index = 0; index < len; index++) {
                    arrReturn.push(
                        <MenuItem 
                            className = "menuItemStyling" 
                            value = { arrValues[index] } 
                            key = { arrValues[index] } 
                            insetChildren = { true } 
                            checked = { false } 
                            primaryText = { arrValues[index] } 
                        />
                    );
                }
                break;
                 
            default:
                return null;
        }

        return (objReturn != null ? objReturn : arrReturn);
    };

	
	/**
     * This function can be used to open close the dialog boxes.
     * @param {string} strName: This is the value for the switch statement.
     * @param {bool} open: whether the menu/dialog should be opened or closed.
     * @param {obj} evt: the event object.
     */
    handleOpenClose = (strName, open, evt) =>{
        switch (strName){
            case 'save':
                if (open) {
                    this.setState({ saveDailogOpen: true, viewNameTextFieldError: "" }); 
                }
                else {
                    this.setState({ saveDailogOpen: false });
                }
                break;
            
            case 'menu':
                 if (open) {
                    evt.preventDefault();
                    this.setState({ menu: { open: true, anchorEl: evt.currentTarget } });
                }
                else {
                    this.setState({ menu: { open: false } });
                }
                break;

            case 'statistics':

                if (open) {
                    this.setState({ statisticsModalOpen: true, menu: { open: false } });
                }
                else {
                    this.setState({ statisticsModalOpen: false });
                }
                break;
            
            default:
                return null;
        }
    };
	

	/**
	* This method is called when the save button is pressed in the save dailog.
    * @param context: This is the instance of the current class.
	*/
    onSaveDialog = (context) => {
        var viewName = document.getElementById("tf_viewName").value;
        var nameAlreadyExists = false;
        var lbl_error = document.getElementById('lbl_saveError');
        lbl_error.hidden = true;

        if (viewName === null || viewName.trim() === "" ) {
            // Error
            context.setState({ viewNameTextFieldError: "This textfield is required!" });
        }

        else {
        
            // Check if same name view already exists
            context.state.viewSelectItems.forEach(
                function(element) {
                    if (element.key == viewName ) {
                        nameAlreadyExists = true;
                    }
                }
            );

            if (nameAlreadyExists) {
                console.log("Error! A view with the same name already exists!");
                lbl_error.hidden = false;
            }

            else {
                
                // Save the view
                if (context.saveView(context,viewName)) {
					// Add it to the existing list of views!
					context.state.viewSelectItems.push(context.makeList([viewName],'viewSelectItems')[0]);
					context.setState({ viewSelectItems: context.state.viewSelectItems, viewSelectValue: viewName });

					// Close the dialog
					context.handleOpenClose('save', false);
					console.log("View saved with name: " + viewName);

					// Show the success message
					context.props.showAlert('Success The View has been saved!');
				}
				else { 
                    // Didn't save due to some network error.
					console.error("ERROR");
				}
            }
        }
    };


    /**
     * - ADCMT
     */
    onApplyStatistics() {
        this.props.dispatch(updateStatistics(this.state.statisticColSelectValues, this.state.statisticStatSelectValues, true));
        this.handleOpenClose('statistics', false);
    }


    /**
	* - ADCMT
    * @param context: - ADCMT
    * @param viewName: - ADCMT
	*/
    saveView = (context, viewName) => {
		var saveObj = {
			viewName: viewName,
			filterObj: {}
		};
		
		// Make the object to be sent to the server to save.
		saveObj.filterObj = context.props.GLOBALSTORE.Filter;
		
		// Make the call to the server to save.
		
		return true;
    };

    
    /**
     * - ADCMT
     */
    applyFilter = () => {
        console.log('Filter Applied');
        var iframe = document.getElementById('GlyphViewer').contentWindow;

        makeServerCall('applyFilters',
            function(result, b) {
                var resultJson = JSON.parse(result);
                var data = resultJson.data;
                var tempRowIds = [];
                
				if (data && Array.isArray(data)) {
					if (data.length > 1) {							
						for (var index = 0; index < data.length; index++) {
							tempRowIds.push(parseInt(Object.values(data[index]).toString(), 10));
						}
					}
					else {
						// No data was matched.
						console.log('NO MATCH');
					}
				}
				
				iframe.postMessage({action: 'filter', args: tempRowIds}, '*');
            },
            {post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.filter } 
            }
        );
    };
  

    /**
	* This method is called when an item is selected in the "select view" dropdown that is present in the top left.
    * @param value: - ADCMT
	*/
    onSelectViewChange = (value) => {
        this.setState({ viewSelectValue: value });
        console.log(value);

        // Load Filters!
    };


    /**
	* This method is called when an item is selected in the "select Table" dropdown that is present in the top left.
    * @param value: - ADCMT
	*/
    onSelectTableChange = (value) => {
        this.setState({ tableSelectValues: value });
        console.log(value);
		
    };


    /**
	* - ADCMT
    * @param value: - ADCMT
	*/
    onSelectStatisticStatChange = (value) => {
        this.setState({ statisticStatSelectValues: value });
        console.log(value);

        // Load Table Columns
    };


    /**
	* - ADCMT
    * @param value: - ADCMT
	*/
    onSelectStatisticColChange = (value) => {
        this.setState({ statisticColSelectValues: value });
        console.log(value);

        // Load Table Columns
    };


    /**
	* This method is called when the user clicks on the 'clear all' button.
	*/
    onClearAllFilters = (event) => {
        console.log("clear all");
        var filterSummaryView = this.refs.filterSummaryView.getWrappedInstance();
        var columnsFilterApplied = filterSummaryView.refs;
        //filterSummaryView.handleRowDel('StaffAssigned');
        
		var iframe = document.getElementById('GlyphViewer').contentWindow;
        iframe.postMessage({action:'clear'}, '*');
        
		
        for (var property in columnsFilterApplied) {
            columnsFilterApplied[property].onDelEvent();
        }
        
    };


    /**
	* This method is called when the user clicks on the 'Hide/Show'.
    * @param event: - ADCMT
	*/
    onHideFilteredData = (event) => {
        console.log("Hide");
        this.setState({ hideShowButtonTextFlag: !this.state.hideShowButtonTextFlag });

        //console.log(this.state.GLOBAL.getData()["Filter"]["Range"]);
        
        // If the flag true then button --> |HIDE| else button --> |SHOW|
        if (this.state.hideShowButtonTextFlag) {
            // Hide the glyphs that don't match the filter critera.
        }

        else {
            // Show all the glyphs!
        }
    };

	
    /**
	* This method is called when the user clicks on the 'New' inside the menu.
    * @param event: - ADCMT
	*/
    onMenuNewClick  = (event) => {
        console.log("new");

        // If changes were made ask the user if they would like to save their work before starting a new viz

        // Clear All Filters
        this.onClearAllFilters();

        // Clear the value of viewName
        this.setState({ viewSelectValue: null });

        // Close the menu
        this.handleOpenClose('menu', false, event);
    };


     /**
	* This method is called when the user clicks on the 'Save' inside the menu.
    * @param event: - ADCMT
	*/
    onMenuSaveClick = (event) => {
        console.log("Save");
        
        //Save As call
        if (this.state.viewSelectValue === null || this.state.viewSelectValue === "") {
            this.onMenuSaveAsClick(event);
            return;
        }

        // Save
        this.saveView(this, this.state.viewSelectValue);
        
        // Close the menu
        this.handleOpenClose('menu', false, event);
    };


    /**
	* This method is called when the user clicks on the 'Save As' inside the menu.
    * @param event: - ADCMT
	*/
    onMenuSaveAsClick = (event) => {
        console.log("Save As");
        
        this.handleOpenClose('save', true);
        this.handleOpenClose('menu', false, event);
    };


    /**
	* Functionality for Re-doing an action made by the user to the filter store
	*/
    handleRedo() {
        var position = ( parseInt(window.localStorage.getItem('position'), 10) + 1 );
        var history = this.historyToArray();

        if (position < history.length) {
            window.localStorage.setItem('position', position);
            this.props.dispatch(updateFilterFromSnapshot(history[position]));
        }
        else {
            this.setState({ redoSnackbar: true });
        }
    }


    /**
	* Functionality for Un-doing an action made by the user to the filter store
	*/
    handleUndo() {
        var position = ( parseInt(window.localStorage.getItem('position'), 10) - 1 );
        var history = this.historyToArray();
        
        if (position > -1) {
            window.localStorage.setItem('position', position);
            this.props.dispatch(updateFilterFromSnapshot(history[position])); 
        }
        else {
            this.setState({ undoSnackbar: true });
        }
    }


    /**
	* Converts the string version of the history in local storage to a usable array
	*/
    historyToArray() {
        var history = window.localStorage.getItem('history');
        var returnHistory = [];
        
        if (history.length > 0) {
            if (history.includes("!!")) {
                returnHistory = history.split("!!");

                for (var i = 0; i < returnHistory.length; i++) {
                    returnHistory[i] = JSON.parse(returnHistory[i]);
                }
            }
            else {
                returnHistory.push(JSON.parse(history));
            }
        }

        return returnHistory;
    }


    /**
	* Closes the respective snackbar
	*/
    closeSnackbar(type) {
        if (type === "undo") {
            this.setState({ undoSnackbar: false });
        }
        else {
            this.setState({ redoSnackbar: false });
        }
    }

	
	render = () => {

		// Load values into the table select dropdown.
		var tableSelectItems = this.props.initParams.tableSelectItems.map(function(value) {
			return({ label: value, value: value });
		});

        var statisticStatSelectItems = ["Count", "Min", "Max", "Average", "Median", "Sum", "Range", "St. Dev.", "Varience", "Skewness", "Kurtosis"];
        statisticStatSelectItems = statisticStatSelectItems.map(function(value) {
			return({ label: value, value: value });
		});

        var statisticColSelectItems = this.props.colList.map(function(value) {
			return({ label: value, value: value });
		});
		
		return(
            <Flexbox flexDirection = "column" id = "TopView" style = {{ height: "100%" }}>

                <Snackbar
                    open = { this.state.undoSnackbar }
                    message = "Cannot undo any further."
                    autoHideDuration = { 4000 }
                    onRequestClose = { () => this.closeSnackbar("undo") }
                    //contentStyle = {{ backgroundColor: "#ffffff" }}
                />

                <Snackbar
                    open = { this.state.redoSnackbar }
                    message = "Cannot redo any further."
                    autoHideDuration = { 4000 }
                    onRequestClose = { () => this.closeSnackbar("redo") }
                />

                {/* Row 1 */}
                <Flexbox flexDirection = "row" style = {{ margin: "-2px 0px 3px 0px" }} >
                    <Flexbox style = {{ width: "55%", margin: "9px 10px 0px 0px" }} > 
                        <Select 
                            className = "selectViewName"
                            simpleValue
                            value = { this.state.viewSelectValue } 
                            placeholder = "Select a view" 
                            options = { this.state.viewSelectItems } 
                            onChange = { this.onSelectViewChange } 
                            style = {{
                                margin: "-11px 0px 0px 0px"
                            }}
                        />
                    </Flexbox>
                    
                    <Flexbox style = {{ width: "45%", marginBottom: "-4px" }} >

                        {/*

                        <Checkbox
                            label = { <span> Apply on <br /> Change </span> }
                            labelPosition = "left"
                            style = {{ margin: " 4px 0px 0px" }}
                            inputStyle = {{ width: "70px" }}
                            labelStyle = {{ lineHeight: "12px", width: "51px", margin: "0px -20px 0px -3px" }}
                            iconStyle = {{ margin: "0px 0px 0px 10px" }}
                        />

                        */}

                        <RaisedButton
                            onClick = { () => this.handleUndo() }
                            label = { <i className = "fa fa-undo" style = {{ margin: "0px 0px 0px -3px" }} /> }
                            style = {{
                                margin: "4px 6px 11px -2px",
                                minWidth: "37px",
                                width: "37px",
                                height: "25px"
                            }}
                            buttonStyle = {{
                                height: '25px',
                                lineHeight: '25px',
                                //backgroundColor: ( this.canUndoRedo("undo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                backgroundColor: this.props.settings.colors.collapsibleColor.subBackground
                            }} 
                            labelStyle = {{
                                fontSize: '13px',
                                color: this.props.settings.colors.overviewButtonsColor.text
                            }}
                            overlayStyle = {{
                                height: '25px',
                                lineHeight: '25px'
                            }}
                            primary = { true }
                        />

                        <RaisedButton
                            onClick = { () => this.handleRedo() }
                            label = { <i className = "fa fa-repeat" style = {{ margin: "0px 0px 0px -3px" }} /> }
                            style = {{
                                margin: "4px 2px 0px 0px",
                                minWidth: "37px",
                                width: "37px",
                                height: "25px"
                            }}
                            buttonStyle = {{
                                height: '25px',
                                lineHeight: '35px',
                                //backgroundColor: ( this.canUndoRedo("redo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                backgroundColor: this.props.settings.colors.collapsibleColor.subBackground
                            }} 
                            labelStyle = {{
                                fontSize: '13px',
                                color: this.props.settings.colors.overviewButtonsColor.text
                            }}
                            overlayStyle = {{
                                height: '25px',
                                lineHeight: '25px'
                            }}
                            primary = { true }
                        />

                        <div>
                            <RaisedButton
                                onClick = { (evt) => this.handleOpenClose('menu', true, evt) }
                                label = "Menu"
                                style = {{
                                    margin: "4px 0px 11px 4px",
                                    minWidth: "112px",
                                    width: "112px"
                                }}
								buttonStyle = {{
									height: '25px',
									lineHeight: '25px',
									backgroundColor: this.props.settings.colors.overviewButtonsColor.background
								}} 
								labelStyle = {{
									fontSize: '13px',
									color: this.props.settings.colors.overviewButtonsColor.text
								}}
								overlayStyle = {{
									height: '25px',
									lineHeight: '25px'
								}}
                                primary = { true }
                            />

                            <Popover
                                open = { this.state.menu.open }
                                anchorEl = { this.state.menu.anchorEl }
                                onRequestClose = { (evt) => this.handleOpenClose('menu', false, evt) }
                                style = {{ fontSize: '13px' }}
                            >
                                <Menu>
                                    <MenuItem primaryText = "New" className = "menuItemStyling" onClick = { this.onMenuNewClick }/>
                                    <MenuItem primaryText = "Save" className = "menuItemStyling" onClick = { this.onMenuSaveClick }/>
                                    <MenuItem primaryText = "Save As" className = "menuItemStyling" onClick = { this.onMenuSaveAsClick }/>
                                    <MenuItem primaryText = "Statistics" className = "menuItemStyling" onClick = { () => this.handleOpenClose('statistics', true) }/>
                                </Menu>
                            </Popover>

                            <Dialog
                                title = "Save View"
                                modal = { true }
                                open = { this.state.saveDailogOpen }
                                actions = {
                                    [
                                        <FlatButton
                                            label = "Save"
                                            primary = { true }
                                            onClick = { () => this.onSaveDialog(this) }
                                            style = {{ color: this.props.settings.colors.saveModalColor.saveButton }}
                                        />,
                                        <FlatButton
                                            label = "Cancel"
                                            primary = { true }
                                            onClick = { () => this.handleOpenClose('save', false) }
                                            style = {{ color: this.props.settings.colors.saveModalColor.cancelButton }}
                                        />
                                    ]
                                } 
                            >
                                Please enter a name for the view you are saving. <br />
                                <TextField 
                                    id = "tf_viewName" 
                                    errorText = { this.state.viewNameTextFieldError } 
                                    floatingLabelText = "View Name" 
                                    underlineFocusStyle = {{ borderColor: this.props.settings.colors.saveModalColor.textFieldUnderline }}
                                    floatingLabelStyle = {{ color: this.props.settings.colors.saveModalColor.textFieldUnderline }}
                                /> <br />
                                <label id = "lbl_saveError" hidden style = {{ color:'red' }}> Error! A view with the same name already exists! Please provide a different name! </label>
                            </Dialog>

                        </div>
                    </Flexbox>
                </Flexbox>
                                
                {/* Row 2 */}
                <Flexbox flexDirection = "row" >
                    <FilterViewForm ref = 'filterSummaryView' onScroll = { (element, elastic) => this.props.initParams.scrollToElement(element, elastic) }/> 
                </Flexbox>
                
                {/* Row 3 */}
                <Flexbox flexDirection = "row" alignContent = "space-between" style = {{ margin: "10px 0px 8px" }} >
                    <Flexbox style = {{ width: "30%" }} > 
                        <RaisedButton 
                            label = "Clear All"
                            style = {{
                                width: "100%"
                            }}
                            buttonStyle = {{
								height: '25px',
								lineHeight: '25px',
								backgroundColor: this.props.settings.colors.overviewButtonsColor.background
							}} 
							labelStyle = {{
								fontSize: '12px',
								color: this.props.settings.colors.overviewButtonsColor.text,
                                margin: "0px 0px 0px -3px",
                                paddingLeft: "0px",
                                paddingRight: "0px"
							}}
							overlayStyle = {{
								height: '25px',
								lineHeight: '25px'
							}}
                            onClick = { this.onClearAllFilters }
                            primary = {true } 
                        />
                    </Flexbox>

                    <Flexbox style = {{ width: "5%" }} /> 

                    <Flexbox style = {{ width: "30%" }} > 
                        <RaisedButton 
                            label = { this.state.hideShowButtonTextFlag ? "Hide" : "Show" }
                            id = "buttonHideShow"
                            style = {{
                                width: "100%"
                            }}
                            buttonStyle = {{
								height: '25px',
								lineHeight: '25px',
								backgroundColor: this.props.settings.colors.overviewButtonsColor.background
							}} 
							labelStyle = {{
								fontSize: '12px',
								color: this.props.settings.colors.overviewButtonsColor.text
							}}
							overlayStyle = {{
								height: '25px',
								lineHeight: '25px'
							}}
                            onClick = { this.onHideFilteredData.bind(this) }
                            primary = { true } 
                        />

                    </Flexbox>

                    <Flexbox style = {{ width: "5%" }} /> 

                    <Flexbox style = {{ width: "30%" }} > 
                        <RaisedButton 
                            primary = { true } 
                            label = "Apply Filters" 
                            onClick = { this.applyFilter.bind(this) }
                            style = {{
                                width: "100%"
                            }}
                            buttonStyle = {{
								height: '25px',
								lineHeight: '25px',
								backgroundColor: this.props.settings.colors.overviewButtonsColor.background
							}} 
							labelStyle = {{
								fontSize: '12px',
								color: this.props.settings.colors.overviewButtonsColor.text,
                                paddingLeft: "0px",
                                paddingRight: "0px"
							}}
							overlayStyle = {{
								height: '25px',
								lineHeight: '25px'
							}}
					/>
                    </Flexbox>
                    
                </Flexbox>

                <Dialog
                    title = "Statistics"
                    modal = { true }
                    open = { this.state.statisticsModalOpen }
                    bodyStyle = {{
                        overflowY: "visible"
                    }}
                    actions = {
                        [
                            <FlatButton
                                label = "Apply"
                                primary = { true }
                                onClick = { () => this.onApplyStatistics(this) }
                                style = {{ color: this.props.settings.colors.saveModalColor.saveButton }}
                            />,
                            <FlatButton
                                label = "Cancel"
                                primary = { true }
                                onClick = { () => this.handleOpenClose('statistics', false) }
                                style = {{ color: this.props.settings.colors.saveModalColor.cancelButton }}
                            />
                        ]
                    } 
                >

                    <b>Select which columns to monitor:</b> <br />
                    <Select 
                        multi 
                        simpleValue
                        value = { this.state.statisticColSelectValues } 
                        placeholder = "Select your column(s)" 
                        options = { statisticColSelectItems } 
                        onChange = { this.onSelectStatisticColChange.bind(this) } 
                    />

                    <br />
                    <br />

                    <b>Select which statistics to apply:</b> <br />
                    <Select 
                        multi 
                        simpleValue
                        value = { this.state.statisticStatSelectValues } 
                        placeholder = "Select your statistic(s)" 
                        options = { statisticStatSelectItems } 
                        onChange = { this.onSelectStatisticStatChange.bind(this) } 
                    />
                </Dialog>
                
                {/* Row 4 */}
                <Flexbox flexDirection = "row" >
                    <Select 
                        multi 
                        className = "selectTableName"
                        simpleValue
                        value = { this.state.tableSelectValues } 
                        placeholder = "Select your table(s)" 
                        options = { tableSelectItems } 
                        onChange = { this.onSelectTableChange.bind(this) } 
                    />
                </Flexbox>
            </Flexbox>
		);
	}
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const updateFilterFromSnapshot = (snapshot) => ({
    type: 'UPDATE_FILTER_SNAPSHOT',
    snapshot
});

export const updateStatistics = (colList, statList, display) => ({
    type: 'UPDATE_STATISTICS',
    colList,
    statList,
    display
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    statisticDisplay: state.filterState.ModalDisplay.statisticsModal,
    filter: state.filterState.Filter,
	VizParams: state.filterState.VizParams
  }
};


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(FilterSideBarTopView));
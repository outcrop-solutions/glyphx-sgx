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
import SelectedAndFilteredDisplay from './SelectedAndFilteredDisplay.js';
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
        var convertedViewSelectItems = this.convertToSelectFormat(props.storedViews.savedViews);
        var tableSelectItems = [];
        if(typeof props.VizParams.tableName != 'array'){
            tableSelectItems.push({ label: props.VizParams.tableName, value: props.VizParams.tableName });
        }else{
            /*tableSelectItems = this.props.VizParams.tableSelectItems.map(function(value) {
                return({ label: value, value: value });
            });*/
        }

        // Store the states of all the elements
        this.state = {
            topViewVisible: true,
            hideShowButtonTextFlag: false,
            menu: { open: false, showOpen: false },
            viewSelectValue: null,
            tableSelectValues: [],
            tableSelectItems:tableSelectItems,
            saveDailogOpen: false,
            viewSelectItems: convertedViewSelectItems,
            viewNameTextFieldError: "",
            statisticsModalOpen: false,
            statisticStatSelectValues: "",
            statisticColSelectValues: "",
            undoSnackbar: false,
            redoSnackbar: false,
            filterIDs: null,
            deleteDailogOpen: false,
            deleteDialogLabel: 'Are you sure you want to delete ; ?',
            selectedData: []
        };
    }
    
    componentWillReceiveProps(nextProps){
        if(nextProps.storedViews.savedViews != this.props.storedViews.savedViews){
            var convertedViewSelectItems = this.convertToSelectFormat(nextProps.storedViews.savedViews);
            this.setState({viewSelectItems: convertedViewSelectItems, tableSelectValues: [nextProps.VizParams.tableName] });
        }
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

        if(this.state.tableSelectItems.length == 1){
            this.setState({
                tableSelectValues: this.state.tableSelectItems[0].value
            });
        }

        if(this.props.VizParams.savedViz){

            this.setState({
                viewSelectValue: this.props.VizParams.vizID
            });
        }

        
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
     * This function converts the server saved views structure into client select consumable.
     * @param {array} obj: The object passed as prop containing the view select values;
     */
    convertToSelectFormat(arrViewSelect) {
        var viewSelectItems=[];
        var arrReturn = [];
        //Set the Saved Views data to the store.
        var savedViews = arrViewSelect ? arrViewSelect : [];
        var arrReturn =[];
        for(var index=0;index<savedViews.length;index++){
            if(savedViews[index].OriginalVizName == this.props.VizParams.originalVizName) {
                arrReturn.push(
                    {
                        label: savedViews[index].Name, value: savedViews[index].ID
                    }
                );
            }
        }

        return arrReturn;
    }

	
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

            case 'showMenu':
                 if (open) {
                    evt.preventDefault();
                    this.setState({ menu: { showOpen: true, showAnchorEl: evt.currentTarget } });
                }
                else {
                    this.setState({ menu: { showOpen: false } });
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

            case 'delete':
                if(open){
                    this.setState({ deleteDailogOpen: true, deleteDialogLabel: evt }); 
                }
                else {
                    this.setState({ deleteDailogOpen: false, deleteDialogLabel: 'Are you sure you want to delete ; ?' });
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
                    if (element.label == viewName ) {
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
                var id = context.saveView(context,viewName,true);
                
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
	saveView = (context, viewName, isNewView) => {

        var fef = this.props.VizParams.frontEndFilters;
        var filterObj = JSON.parse(JSON.stringify(this.props.filter));
        var colName = "";
        var values;
        var finalFilterObj={};
        
        var query = "SELECT * FROM " + this.props.VizParams.tableName + " WHERE ";

        // Differentiate between Save and Save As.
        // If save i.e. isNewView is false then this means that this is an already constructed subset viz that is to be saved again.
        if(!isNewView){
            var fefString = context.props.VizParams.frontEndFilterString; //Since view already exists its information is in VizParams.
            try{
                fef = JSON.parse(fefString);
                for(var key in fef){
                    //There is a special check for selectedvalues length as not keeping this overwrites the filtering on saved views.
                    //For eg: subset is created using col1:[1,2,3,4] and If user then selects 1 and tries to save it
                    //          This code would override and still send
                    if(filterObj.hasOwnProperty(key) && filterObj[key].selectedValues.length < 1){
                        filterObj[key].selectedValues = filterObj[key].selectedValues.concat(fef[key]).unique();
                    }
                }
            }
            catch(err){
                console.log(err);
            }
            viewName = context.props.VizParams.savedVizName;
        }
        // If save as/save when no viewname is selected 
        // i.e. isNewView is true then this means that this is a totally new View that has to be saved on the server.
        else{
            //using fef and current filters make the query string.
            //Since view does not exist we have to build the query string from Front End Filter data.
            var temp = [];
            if(fef && fef.length > 0){
                for(var i=0; i<fef.length; i++){
                    colName = fef[i][0].trim();
                    
                    if(filterObj.hasOwnProperty(colName)){
                        temp = fef[i].slice();
                        temp.shift();
                        filterObj[colName].selectedValues = filterObj[colName].selectedValues.concat(temp).unique();
                    }
                }
            }
        }

        // { colName:{selectedValues:[]},colName:{selectedValues:[]},colName:{selectedValues:[]} }
        //Form the query.
        var colNames = Object.keys(filterObj);
        var temp,flag = false;
        for(var index=0;index<colNames.length;index++)
        {
            if(filterObj[colNames[index]].selectedValues.length > 0)
            {	
                if(flag)
                {
                    query = query + " AND ";
                }

                finalFilterObj[colNames[index]] = filterObj[colNames[index]].selectedValues;

                temp = JSON.stringify(filterObj[colNames[index]].selectedValues);
                temp = temp.replace('[','(').replace(/]$/,")");
                query = query + colNames[index] + " IN " + temp;
                flag = true;
            }
        }

        makeServerCall('saveView',
            function(res,b,c) {
            // Hide the loadmask.
                
                if (typeof res == 'string') {
                    res = JSON.parse(res);
                }
                
                if(res.ID){
                    context.props.VizParams.savedViz = true;
                    context.props.VizParams.vizID = res.ID;
                    context.props.VizParams.savedVizName = res.Name;
                    context.props.VizParams.frontEndFilterString = JSON.stringify(finalFilterObj);

                    context.props.dispatch(
                        setCurrentVizParams(
                                context.props.VizParams
                    ));					

					// Add it to the existing list of views!
					if(isNewView) {
                        context.state.viewSelectItems.push({label: viewName, value: res.ID});
                        context.setState({ viewSelectItems: context.state.viewSelectItems, viewSelectValue: res.ID });
                    }

					// Close the dialog
					context.handleOpenClose('save', false);
					
					// Show the success message
					context.props.showAlert('Success The View has been saved!');
				}
				else { 
                    // Didn't save due to some network error.
					console.error("ERROR");
				}

                
            },
            {
                post: true, 
                data:  {
                    filterQuery: query,
                    frontEndFilter: JSON.stringify(finalFilterObj),
                    vizId: isNewView ? null : this.props.VizParams.vizID,
                    originalVizName: this.props.VizParams.originalVizName,
                    savedVizName: viewName
                }
            }
        );
		return true;
	};

    deleteView = (context) => {
        this.handleOpenClose('delete', false);
        makeServerCall('deleteSavedViews?ID='+this.state.viewSelectValue,
            function(result, b) {
                // Show the success message
                context.props.showAlert('Success The View has been deleted!');

                //Delete all references of this view from the CurrentVizParams.

                context.setState({
                    deleteDailogOpen: false,
                    deleteDialogLabel: "Success The View has been deleted!"
                },function(){
                    //if(context.props.VizParams)
                      //  context.props.history.push('/home');
                    //After delete either go back to home or load another viz.
                })
                
            }
        );
    }

    /**
     * - ADCMT
     */
    applyFilter = () => {
        console.log('Filter Applied');
        var iframe = document.getElementById('GlyphViewer').contentWindow;

        var context = this;

        makeServerCall('applyFilters',
            function(result, b) {
                var resultJson = JSON.parse(result);
                var data = resultJson.data;
                var tempRowIds = [];
                
				if (data && Array.isArray(data)) {
					if (data.length > 0) {							
						for (var index = 0; index < data.length; index++) {
							tempRowIds.push(parseInt(Object.values(data[index]).toString(), 10));
						}
					}
					else {
						// No data was matched.
						console.log('NO MATCH');
					}
				}
				
                context.setState({ filterIDs: tempRowIds, hideShowButtonTextFlag: true });

                iframe.filterGlyphs(tempRowIds);
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
        var savedVizObj;
        this.props.storedViews.savedViews.forEach(function(v){
            if(v.ID == value)
                savedVizObj = v;
        })

        if(!savedVizObj){
            return;
        }

        var originalVizName = savedVizObj.OriginalVizName; 
        var query = savedVizObj.QueryString; 
        var funnelData;
        var keys = Object.keys(this.props.funnelData);
        var path;
        var context = this;
        var flag = true;
    
        for(var keyIndex=0;keyIndex<keys.length && flag;keyIndex++){
            funnelData = this.props.funnelData[keys[keyIndex]];
    
            for(var index=0;index<funnelData.length;index++)
            {
                if(funnelData[index][0] == originalVizName){
                    path = funnelData[index][1];
                    flag = false;
                    break;
                }
            }
        }
    
        var index = path.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");
        var sdtPath = path.substring(index + 1);
    
        makeServerCall(window.encodeURI('frontEndFilterData/' + sdtPath ),
            function (responseText) {
                var response = JSON.parse(responseText);
                
                // Post the new data to the state and hide the window load-mask
                context.props.dispatch(
                    setCurrentVizParams(
                        {
                            tableName: response.tableName,
                            datasourceId: response.datasourceId ,
                            query: query,
                            originalVizName:originalVizName,
                            filterAllowedColumnList:  response.filterAllowedColumnList,
                            sdtPath: sdtPath,
                            savedViz: true,
                            vizID:savedVizObj.ID,
                            savedVizName: savedVizObj.Name,
                            frontEndFilterString: savedVizObj.frontEndFilterString
                        }
                    )
                );

                //context.props.history.push('/glyph-viewer');
                context.props.refreshParent();
            }
        );

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
        //iframe.postMessage({action:'clear'}, '*');
        iframe.filterGlyphs([]);
        
		
        for (var property in columnsFilterApplied) {
            columnsFilterApplied[property].onDelEvent();
        }

        this.setState({ filterIDs: null, hideShowButtonTextFlag: false });
        
    };


    /**
	* This method is called when the user clicks on the 'Hide/Show'.
    * @param event: - ADCMT
	*/
    onHideFilteredData = (event) => {
        if (this.state.filterIDs !== null) {
            var buttonState = !this.state.hideShowButtonTextFlag;
            this.setState({ hideShowButtonTextFlag: buttonState });

            var iframe = document.getElementById('GlyphViewer').contentWindow;
            
            // If the flag true then hide
            if (buttonState) {
                iframe.filterGlyphs(this.state.filterIDs);
            }
            else {
                // Show all the glyphs
                iframe.filterGlyphs([]);
            }
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
        this.saveView(this, this.state.viewSelectValue,false);
        
        // Close the menu
        this.handleOpenClose('menu', false, event);
    };

     /**
	* This method is called when the user clicks on the 'Save' inside the menu.
    * @param event: - ADCMT
	*/
    onMenuDeleteClick = (event) => {
        console.log("Delete");
        var viewName="";
        var context = this;
        
        if (this.state.viewSelectValue === null || this.state.viewSelectValue === "") {
            return;
        }

        this.state.viewSelectItems.forEach(function(v){
            v.value == context.state.viewSelectValue;
            viewName = v.label;
        })
        
        var deleteDialogLabel = this.state.deleteDialogLabel.replace(";",viewName);
        
        this.handleOpenClose('delete', true, deleteDialogLabel);

        // Close the menu
        this.handleOpenClose('menu', false, event);
    };


    /**
	* This method is called when the user clicks on the 'Save' inside the menu.
    * @param event: - ADCMT
	*/
    onSelectedDataClick = (event) => {
        //var IDs = document.getElementById("GlyphViewer").contentWindow.getSelectedGlyphIDs();
        var iframe = document.getElementById('GlyphViewer').contentWindow;
        var selectedGlyphsURL = "fetchSelectedVizData?tableName=" + this.props.VizParams.tableName + "&rowIds=[" + iframe.getSelectedGlyphIDs().toString() + "]";

        var context = this;

        // Get the data corresponding to the URL
        //makeServerCall(window.encodeURI(selectedGlyphsURL),
        makeServerCall(selectedGlyphsURL,
            function (responseText) { 
                var response = responseText;
                if (typeof responseText === 'string') {
                    response = JSON.parse(response);
                }
                
                if (response.data[0]) {
                    context.setState({ selectedData: response.data });
                    context.props.dispatch(editModalDisplay(true));
                }
            }
        );

        /*
        var xhr = new XMLHttpRequest();

        xhr.onreadystatechange = function() {
            if (xhr.readyState == XMLHttpRequest.DONE) {
                var response = xhr.responseText;
                if (typeof xhr.responseText === 'string') {
                    response = JSON.parse(xhr.responseText);
                }
                
                if (response.data[0]) {
                    context.setState({ selectedData: response.data });
                    context.props.dispatch(editModalDisplay(true));
                }
            }
        }

        xhr.open('GET', selectedGlyphsURL, true);
        xhr.send(null);
        */

        this.handleOpenClose('showMenu', false);
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
                                    <MenuItem primaryText = "Delete" className = "menuItemStyling" onClick = { this.onMenuDeleteClick } disabled= {this.state.viewSelectValue === null || this.state.viewSelectValue === ""}/>
                                </Menu>
                            </Popover>

                            <Popover
                                open = { this.state.menu.showOpen }
                                anchorEl = { this.state.menu.showAnchorEl }
                                onRequestClose = { (evt) => this.handleOpenClose('showMenu', false, evt) }
                                style = {{ fontSize: '13px' }}
                            >
                                <Menu>
                                    <MenuItem primaryText = "Statistics" className = "menuItemStyling" onClick = { () => this.handleOpenClose('statistics', true) }/>
                                    <MenuItem primaryText = "Selected Data" className = "menuItemStyling" onClick = { this.onSelectedDataClick.bind(this) }/>
                                    <MenuItem primaryText = "Filtered Data" className = "menuItemStyling" onClick = { null }/>
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

                            <Dialog
                            title="Delete View"
                            actions={
                                    [
                                        <FlatButton
                                            label = "Yes"
                                            primary = { true }
                                            onClick = { () => this.deleteView(this) }
                                            style = {{ color: this.props.settings.colors.saveModalColor.saveButton }}
                                        />,
                                        <FlatButton
                                            label = "Cancel"
                                            primary = { true }
                                            onClick = { () => this.handleOpenClose('delete', false) }
                                            style = {{ color: this.props.settings.colors.saveModalColor.cancelButton }}
                                        />
                                    ]
                                }
                            modal={true}
                            open={this.state.deleteDailogOpen}
                          >
                            <label id="deleteDialogLabel"> { this.state.deleteDialogLabel }</label>
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
                    <Flexbox style = {{ width: "22%" }} > 
                        <RaisedButton 
                            label = "Clear"
                            style = {{
                                width: "100%",
                                minWidth: "0px"
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

                    <Flexbox style = {{ width: "4%" }} /> 

                    <Flexbox style = {{ width: "22%" }} > 
                        <RaisedButton 
                            label = { this.state.hideShowButtonTextFlag ? "Hide" : "Show" }
                            id = "buttonHideShow"
                            style = {{
                                width: "100%",
                                minWidth: "0px"
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
                            onClick = { this.onHideFilteredData.bind(this) }
                            primary = { true } 
                        />

                    </Flexbox>

                    <Flexbox style = {{ width: "4%" }} /> 

                    <Flexbox style = {{ width: "22%" }} > 
                        <RaisedButton
                            onClick = { (evt) => this.handleOpenClose('showMenu', true, evt) }
                            label = "View"
                            style = {{
                                width: "100%",
                                minWidth: "0px"
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

                    <Flexbox style = {{ width: "4%" }} /> 

                    <Flexbox style = {{ width: "22%" }} > 
                        <RaisedButton 
                            primary = { true } 
                            label = "Apply" 
                            onClick = { this.applyFilter.bind(this) }
                            style = {{
                                width: "100%",
                                minWidth: "0px"
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
                
                <SelectedAndFilteredDisplay data = { this.state.selectedData } />

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
                        disabled = { this.state.tableSelectItems.length == 1 } 
                        placeholder = "Select your table(s)" 
                        options = { this.state.tableSelectItems } 
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
export const editModalDisplay = (selectedFilteredModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    selectedFilteredModal,
});

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

export const setCurrentVizParams = (vizParams) => ({
    type: 'SET_VIZ_PARAMS',
    vizParams,
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
    funnelData: state.filterState.FunnelData,
    storedViews: state.filterState.StoredViews,
	VizParams: state.filterState.VizParams
  }
};

export const setCurrentSavedViews = (savedViewsList) => ({
    type: 'UPDATE_SAVED_VIEWS',
    savedViewsList,
});

/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(FilterSideBarTopView));
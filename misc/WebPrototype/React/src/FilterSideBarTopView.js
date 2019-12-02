import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import Promise from 'bluebird';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import MenuItem from 'material-ui/MenuItem';
import Flexbox from 'flexbox-react';
import TextField from 'material-ui/TextField';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Tooltip from 'rc-tooltip';
import OldSelect from 'react-select';
import Select from 'react-select'
import Snackbar from 'material-ui/Snackbar';
import { makeServerCall } from './ServerCallHelper.js';
import FilterSummaryView from './FilterSummaryView.js';
import SelectedAndFilteredDisplay from './SelectedAndFilteredDisplay.js';
import XYZRemapModal from './XYZRemapModal.js';
import 'rc-tooltip/assets/bootstrap.css';
import 'react-select/dist/react-select.min.css';
import './css/FilterSideBar.css';
import './css/General.css';


/**
 * This is the top section which contains the overview in the filter side-bar
 * @param initParams: - ADCMT
 * @param colList: - ADCMT
 * @param showAlert: - ADCMT
 */
class FilterSideBarTopView extends React.Component {
	
	constructor(props) {
        super(props);

        var tableSelectItems = [];
        if (!Array.isArray(props.VizParams.tableName)) {
            tableSelectItems.push({ label: props.VizParams.tableName, value: props.VizParams.tableName });
        }
        else {
            /*tableSelectItems = this.props.VizParams.tableSelectItems.map(function(value) {
                return({ label: value, value: value });
            });*/
        }

        // Store the states of all the elements
        this.state = {
            topViewVisible: true,
            hideShowButtonTextFlag: true,
            menu: { open: false, showOpen: false, actionOpen: false },
            viewSelectValue: null,
            tableSelectValues: [],
            tableSelectItems:tableSelectItems,
            saveDailogOpen: false,
            viewSelectItems: [],
            viewNameTextFieldError: "",
            statisticsModalOpen: false,
            statisticStatSelectValues: "",
            statisticColSelectValues: "",
            undoSnackbar: false,
            redoSnackbar: false,
            deleteDailogOpen: false,
            deleteDialogLabel: 'Are you sure you want to delete ; ?',
            selectedData: [],
            multiTable: false,
            statisticsSelectBoth: false
        };
    }
    
    componentWillReceiveProps(nextProps) {
        // debugger;
        if (nextProps.storedViews.savedViews !== this.props.storedViews.savedViews || 
            nextProps.VizParams.query !== nextProps.RecentVizDropdown.query || 
            this.props.RecentVizDropdown !== nextProps.RecentVizDropdown) {
            var convertedViewSelectItems = this.convertToSelectFormat(nextProps.storedViews.savedViews, nextProps.RecentVizDropdown);
            this.setState({ viewSelectItems: convertedViewSelectItems, tableSelectValues: [nextProps.VizParams.tableName] });
        }
    }


    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        return (this.state !== nextState || 
            this.props.initParams !== nextProps.initParams || 
            this.props.settings !== nextProps.settings || 
            this.props.filterIDs !== nextProps.filterIDs || 
            this.props.tableData !== nextProps.tableData || 
            this.props.statData !== nextProps.statData || 
            this.props.RecentVizDropdown !== nextProps.RecentVizDropdown || 
            this.props.statisticDisplay !== nextProps.statisticDisplay || 
            this.props.initialX !== nextProps.initialX || 
            this.props.initialY !== nextProps.initialY || 
            this.props.initialZ !== nextProps.initialZ 
        );
        
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

        // debugger;

        if (this.props.RecentVizDropdown === null || this.props.RecentVizDropdown === undefined) {
            this.props.dispatch( setRecentVizDropdown( this.props.VizParams ));
            this.setState({ viewSelectItems: this.convertToSelectFormat(this.props.storedViews.savedViews, this.props.VizParams) });
        }
        else {
            this.setState({ viewSelectItems: this.convertToSelectFormat(this.props.storedViews.savedViews, this.props.RecentVizDropdown) });
        }


        if (this.state.tableSelectItems.length === 1) {
            this.setState({
                tableSelectValues: this.state.tableSelectItems[0].value
            });
        }

        if (this.props.VizParams.savedViz) {
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
    convertToSelectFormat(arrViewSelect, recentVizDropdown) {
        /* var viewSelectItems = []; */
        var arrReturn = [];

        // debugger;

        if (this.props.RecentVizDropdown !== this.props.VizParams && 
            (this.props.RecentVizDropdown !== null || this.props.RecentVizDropdown !== undefined)) {
            if (recentVizDropdown !== null) {
                arrReturn.push({label: "RECENT: " + recentVizDropdown.originalVizName, value: "RECENT_VIZ_DROPDOWN" });
                // debugger;
            }
            else {
                arrReturn.push({label: "RECENT: " + this.props.RecentVizDropdown.originalVizName, value: "RECENT_VIZ_DROPDOWN" });
                // debugger;
            }
        }

        // Set the Saved Views data to the store.
        var savedViews = arrViewSelect ? arrViewSelect : [];
        for (var index = 0; index < savedViews.length; index++) {;
            if (savedViews[index].OriginalVizName === this.props.VizParams.originalVizName) {
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
        switch (strName) {
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

            case 'actionMenu':
                 if (open) {
                    evt.preventDefault();
                    this.setState({ menu: { actionOpen: true, actionAnchorEl: evt.currentTarget } });
                }
                else {
                    this.setState({ menu: { actionOpen: false } });
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

            case 'XYZ':
                this.setState({ menu: { actionOpen: false } });
                this.props.dispatch(editModalDisplay(null, true));
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
                if (open) {
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
                    if (element.label === viewName ) {
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
                context.saveView(context,viewName,true);
                
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
        /* var values; */
        var finalFilterObj={};
        
        var query = "SELECT * FROM " + this.props.VizParams.tableName + " WHERE ";

        // Differentiate between Save and Save As.
        // If save i.e. isNewView is false then this means that this is an already constructed subset viz that is to be saved again.
        if (!isNewView) {
            var fefString = context.props.VizParams.frontEndFilterString; //Since view already exists its information is in VizParams.
            try {
                fef = JSON.parse(fefString);
                for (var key in fef) {
                    //There is a special check for selectedvalues length as not keeping this overwrites the filtering on saved views.
                    //For eg: subset is created using col1:[1,2,3,4] and If user then selects 1 and tries to save it
                    //          This code would override and still send
                    if (filterObj.hasOwnProperty(key) && filterObj[key].selectedValues.length < 1) {
                        filterObj[key].selectedValues = filterObj[key].selectedValues.concat(fef[key]).unique();
                    }
                }
            }
            catch(err) {
                console.log(err);
            }
            viewName = context.props.VizParams.savedVizName;
        }
        // If save as/save when no viewname is selected 
        // i.e. isNewView is true then this means that this is a totally new View that has to be saved on the server.
        else {
            //using fef and current filters make the query string.
            //Since view does not exist we have to build the query string from Front End Filter data.
            var temp = [];
            if (fef && fef.length > 0) {
                for (var i = 0; i < fef.length; i++) {
                    colName = fef[i][0].trim();
                    
                    if (filterObj.hasOwnProperty(colName)) {
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
        var temp, flag = false;
        for (var index = 0; index < colNames.length; index++) {
            if (filterObj[colNames[index]].selectedValues.length > 0) {	
                if (flag) {
                    query = query + " AND ";
                }

                finalFilterObj[colNames[index]] = filterObj[colNames[index]].selectedValues;

                temp = JSON.stringify(filterObj[colNames[index]].selectedValues);
                temp = temp.replace('[','(').replace(/]$/,")");
                query = query + colNames[index] + " IN " + temp;
                flag = true;
            }
        }
		
        var currentDate = new Date();
        
        makeServerCall('saveView',
            function(res ,b, c) {
            // Hide the loadmask.
                
                if (typeof res === 'string') {
                    res = JSON.parse(res);
                }
                
                if (res.ID) {
                    context.props.VizParams.savedViz = true;
                    context.props.VizParams.vizID = res.ID;
                    context.props.VizParams.savedVizName = res.Name;
                    context.props.VizParams.frontEndFilterString = JSON.stringify(finalFilterObj);

                    context.props.dispatch(
                        setCurrentVizParams(
                                context.props.VizParams
                    ));					

					// Add it to the existing list of views!
					if (isNewView) {
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

                context.props.dispatch( setTimer(new Date().getTime()) );
                
            },
            {
                post: true, 
                data:  {
                    filterQuery: query,
                    frontEndFilter: JSON.stringify(finalFilterObj),
                    vizId: isNewView ? null : this.props.VizParams.vizID,
                    originalVizName: this.props.VizParams.originalVizName,
                    savedVizName: viewName,
					time: currentDate.getTime(),
					date: currentDate.getTime(),
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
                    context.props.history.push('/home');
                    //After delete either go back to home or load another viz.
                })
                
            }
        );
    }

    /**
     * - ADCMT
     */
    applyFilter = () => {
        //console.log('Filter Applied');
        if(!this.props.uid){
            var iframe = document.getElementById('GlyphViewer').contentWindow;

        var context = this;

        makeServerCall('applyFilters',
            function(result, b) {
                var resultJson = JSON.parse(result);
                // debugger;
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

                context.props.setFilterIDs(tempRowIds);
                if(!context.props.uid) iframe.filterGlyphs(tempRowIds);

                context.props.dispatch( setTimer(new Date().getTime()) );
            },
            {
                post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.filter } 
            }
        );
        
        makeServerCall('applyFiltersEC2',
            function(result, b) {
                var resultJson = JSON.parse(result);
                // debugger;
                console.log(resultJson, 'applyFilters');
                var data = resultJson.data;
                var tempRowIds = [];
                
				// if (data && Array.isArray(data)) {
				// 	if (data.length > 0) {							
				// 		for (var index = 0; index < data.length; index++) {
				// 			tempRowIds.push(parseInt(Object.values(data[index]).toString(), 10));
				// 		}
				// 	}
				// 	else {
				// 		// No data was matched.
				// 		console.log('NO MATCH');
				// 	}
				// }

                // context.props.setFilterIDs(tempRowIds);

                // iframe.filterGlyphs(tempRowIds);

                // context.props.dispatch( setTimer(new Date().getTime()) );
            },
            {
                post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.filter } 
            }
            );
        }
    };


    addToHistory() {
        var undoRedoHistory = {
            history: this.props.UndoRedoHistory.history.slice(0),
            position: this.props.UndoRedoHistory.position
        }

        // debugger;

        if (!(JSON.stringify(undoRedoHistory.history[undoRedoHistory.history.length - 1]) === JSON.stringify({filterList: this.props.filter, tableData: this.props.tableData})) ) {

            if (undoRedoHistory.position !== undoRedoHistory.history.length - 1) {
                undoRedoHistory.history = undoRedoHistory.history.slice(0, undoRedoHistory.position + 1);
            }

            undoRedoHistory.history.push({filterList: this.props.filter, tableData: this.props.tableData});
            undoRedoHistory.position = undoRedoHistory.position + 1;

            this.props.dispatch(editUndoRedoHistory(undoRedoHistory));
        }
    }
  

    /**
	* This method is called when an item is selected in the "select view" dropdown that is present in the top left.
    * @param value: - ADCMT
	*/
    onSelectViewChange = (value) => {

        // debugger;

        this.setState({ viewSelectValue: value });
        //console.log(value);

        if (value !== "RECENT_VIZ_DROPDOWN") {

            // Load Filters!
            var savedVizObj;

            this.props.storedViews.savedViews.forEach(function(v) {
                if (v.ID === value) {
                    savedVizObj = v;
                }
            })

            if (!savedVizObj) {
                return;
            }


            var originalVizName = savedVizObj.OriginalVizName; 
            var query = savedVizObj.QueryString; 
            var funnelData;
            var keys = Object.keys(this.props.funnelData);
            var path;
            var context = this;
            var flag = true;

            // debugger;
        
            for (var keyIndex = 0; keyIndex < keys.length && flag;keyIndex++) {
                funnelData = this.props.funnelData[keys[keyIndex]];
        
                for(var index = 0; index < funnelData.length; index++) {
                    if (funnelData[index][0] === originalVizName) {
                        path = funnelData[index][1];
                        flag = false;
                        break;
                    }
                }
            }
        
            var index = path.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");
            var sdtPath = path.substring(index + 1);

            // Show mask
            this.props.showHideLoadingMask(true);
        
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

                    context.props.dispatch( setTimer(new Date().getTime()) );

                    //context.props.history.push('/glyph-viewer');
                    context.props.reloadParent();

                    // Hide mask
                    context.props.showHideLoadingMask(false);
                }
            );

            makeServerCall('frontEndFiltersEC2',
                function (responseText) {
                    var response = JSON.parse(responseText);
                    console.log(response, 'response from fetch ec2 sqlite filters in sidebar top view');
                    // Post the new data to the state and hide the window load-mask
                    // context.props.dispatch(
                    //     setCurrentVizParams(
                    //         {
                    //             tableName: response.tableName,
                    //             datasourceId: response.datasourceId ,
                    //             query: query,
                    //             originalVizName:originalVizName,
                    //             filterAllowedColumnList:  response.filterAllowedColumnList,
                    //             sdtPath: sdtPath,
                    //             savedViz: true,
                    //             vizID:savedVizObj.ID,
                    //             savedVizName: savedVizObj.Name,
                    //             frontEndFilterString: savedVizObj.frontEndFilterString
                    //         }
                    //     )
                    // );

                    // context.props.dispatch( setTimer(new Date().getTime()) );

                    // //context.props.history.push('/glyph-viewer');
                    // context.props.reloadParent();

                    // // Hide mask
                    // context.props.showHideLoadingMask(false);
                },
                {
                    post: true,
                    data: {
                        key: sdtPath
                    }
                }
            );
        }

        else {
            // Post the new data to the state and hide the window load-mask
            var context = this;
            this.props.showHideLoadingMask(true);

            let pom = new Promise(function (resolve, reject) {
                context.props.dispatch(
                    setCurrentVizParams(
                        {
                            originalVizName: context.props.RecentVizDropdown.originalVizName,
                            tableName: context.props.RecentVizDropdown.tableName,
                            datasourceId: context.props.RecentVizDropdown.datasourceId ,
                            query: context.props.RecentVizDropdown.query,
                            filterAllowedColumnList:  context.props.RecentVizDropdown.filterAllowedColumnList,
                            sdtPath: context.props.RecentVizDropdown.sdtPath,
                            frontEndFilters: context.props.RecentVizDropdown.frontEndFilters
                        }
                    )
                );
                resolve('done');
            });

            pom.then(() => context.props.reloadParent()).then( () => context.props.showHideLoadingMask(false) );

            

            //this.props.dispatch( setTimer(new Date().getTime()) );

        }

    };


    /**
	* This method is called when an item is selected in the "select Table" dropdown that is present in the top left.
    * @param value: - ADCMT
	*/
    onSelectTableChange = (value) => {
        this.setState({ tableSelectValues: value });
        //console.log(value);
		
    };


    /**
	* - ADCMT
    * @param value: - ADCMT
	*/
    onSelectStatisticStatChange = (value) => {
        this.setState({ statisticStatSelectValues: value, statisticsSelectBoth: false });
        //console.log(value);

        // Load Table Columns
    };


    /**
	* - ADCMT
    * @param value: - ADCMT
	*/
    onSelectStatisticColChange = (value) => {
        this.setState({ statisticColSelectValues: value, statisticsSelectBoth: false });
        //console.log(value);

        // Load Table Columns
    };


    /**
	* This method is called when the user clicks on the 'clear all' button.
	*/
    onClearAllFilters = (event) => {
        //console.log("clear all");
        var context = this;
        var filterSummaryView = this.refs.filterSummaryView.getWrappedInstance();
        var columnsFilterApplied = filterSummaryView.refs;
        //filterSummaryView.handleRowDel('StaffAssigned');
        
        var iframe;
        if(!context.props.uid) {
            iframe = document.getElementById('GlyphViewer').contentWindow;
            iframe.filterGlyphs([]);
        }
        //iframe.postMessage({action:'clear'}, '*');

        // let pom = new Promise(function (resolve, reject) {
        //     for (var property in columnsFilterApplied) {
        //         columnsFilterApplied[property].onDelEvent();
        //     }
        //     resolve('done');
        // });

        // pom.then(() => this.props.refreshParent());

        // debugger;

        let pom = new Promise(function (resolve, reject) {
                for (var property in columnsFilterApplied) {
                    columnsFilterApplied[property].onDelEventForClearAll();
                }
                resolve('done');
        });

        pom.then(() => context.props.refreshParent()).then(() => context.applyFilter()).then(() => context.addToHistory());

    };


    /**
	* This method is called when the user clicks on the 'Hide/Show'.
    * @param event: - ADCMT
	*/
    onHideFilteredData = (event) => {
        // debugger;
        if (this.props.filterIDs !== null) {
            var buttonState = !this.state.hideShowButtonTextFlag;
            this.setState({ hideShowButtonTextFlag: buttonState });

            var iframe;
            if(!this.props.uid){
            iframe = document.getElementById('GlyphViewer').contentWindow;
            
            // If the flag true then hide
                if (buttonState) {
                    iframe.filterGlyphs(this.props.filterIDs);
                }
                else {
                    // Show all the glyphs
                    iframe.filterGlyphs([]);
                }
            }
        }
    };

	
    /**
	* This method is called when the user clicks on the 'New' inside the menu.
    * @param event: - ADCMT
	*/
    onMenuNewClick  = (event) => {
        //console.log("new");

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
        //console.log("Save");
        
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
        //console.log("Delete");
        var viewName="";
        var context = this;
        
        if (this.state.viewSelectValue === null || this.state.viewSelectValue === "") {
            return;
        }

        this.state.viewSelectItems.forEach(function(v){
            if(v.value === context.state.viewSelectValue)
                viewName = v.label;
        })
        
        var deleteDialogLabel = this.state.deleteDialogLabel.replace(";",viewName);
        
        this.handleOpenClose('delete', true, deleteDialogLabel);

        // Close the menu
        this.handleOpenClose('menu', false, event);
    };


    /**
	* 
    * @param event: - ADCMT
	*/
    onSelectedDataClick = (event) => {
        //var IDs = document.getElementById("GlyphViewer").contentWindow.getSelectedGlyphIDs();
        if(!this.props.uid){
            var iframe = document.getElementById('GlyphViewer').contentWindow;
            var selectedGlyphsURL = "fetchSelectedVizData?tableName=" + this.props.VizParams.tableName + "&rowIds=[" + iframe.getSelectedGlyphIDs().toString() + "]";
        }
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
                    context.props.dispatch(editModalDisplay(true, null));
                }

                context.props.dispatch( setTimer(new Date().getTime()) );
            }
        );

        this.handleOpenClose('showMenu', false);
    };


    /**
	* 
    * @param event: - ADCMT
	*/
    onFilteredDataClick = (event) => {
        //var IDs = document.getElementById("GlyphViewer").contentWindow.getSelectedGlyphIDs();
        // var iframe = document.getElementById('GlyphViewer').contentWindow;
        // var selectedGlyphsURL = "fetchSelectedVizData?tableName=" + this.props.VizParams.tableName + "&rowIds=[" + iframe.getSelectedGlyphIDs().toString() + "]";

        var context = this;

        makeServerCall('applyFilters',
            function(result, b) {
                var resultJson = JSON.parse(result);
                // debugger;
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

                var selectedGlyphsURL = "fetchSelectedVizData?tableName=" + context.props.VizParams.tableName + "&rowIds=[" + tempRowIds.toString() + "]";

                // debugger;

                //makeServerCall(window.encodeURI(selectedGlyphsURL),
                makeServerCall(selectedGlyphsURL,
                    function (responseText) { 
                        var response = responseText;
                        if (typeof responseText === 'string') {
                            response = JSON.parse(response);
                        }
                        
                        if (response.data[0]) {
                            context.setState({ selectedData: response.data });
                            context.props.dispatch(editModalDisplay(true, null));
                        }

                        context.props.dispatch( setTimer(new Date().getTime()) );
                    }
                );


            },
            {
                post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.filter } 
            }
        );

        this.handleOpenClose('showMenu', false);
    };


    


    /**
	* This method is called when the user clicks on the 'Save As' inside the menu.
    * @param event: - ADCMT
	*/
    onMenuSaveAsClick = (event) => {
        //console.log("Save As");
        
        this.handleOpenClose('save', true);
        this.handleOpenClose('menu', false, event);
    };


    /**
	* Functionality for Re-doing an action made by the user to the filter store
	*/
    handleRedo() {

        var newHistory = {
            history: this.props.UndoRedoHistory.history.slice(0),
            position: this.props.UndoRedoHistory.position + 1,
        };

        // debugger;
        var context = this;
        
        if (newHistory.position < newHistory.history.length) {
            this.props.setTableData(newHistory.history[newHistory.position].tableData);

            let pom = new Promise(function (resolve, reject) {
                context.props.dispatch(updateFilterFromSnapshot(newHistory.history[newHistory.position].filterList)); 
                resolve('done');
            });

            pom.then(() => context.applyFilter());

            this.props.dispatch(editUndoRedoHistory(newHistory));
        }
        else {
            this.setState({ redoSnackbar: true });
        }
    }


    /**
	* Functionality for Un-doing an action made by the user to the filter store
	*/
    handleUndo() {

        var newHistory = {
            history: this.props.UndoRedoHistory.history.slice(0),
            position: this.props.UndoRedoHistory.position - 1,
        };

        // debugger;
        var context = this;
        
        if (newHistory.position > -1) {
            this.props.setTableData(newHistory.history[newHistory.position].tableData);

            let pom = new Promise(function (resolve, reject) {
                context.props.dispatch(updateFilterFromSnapshot(newHistory.history[newHistory.position].filterList)); 
                resolve('done');
            });

            pom.then(() => context.applyFilter());

            this.props.dispatch(editUndoRedoHistory(newHistory));
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


    enterKeyToSave(e) {
        if (e.keyCode === 13) {
            e.preventDefault();
            this.onSaveDialog(this);
        }
    }

    desktopClearFilters(){
        if(this.props.uid){
            this.props.webSocket.send(JSON.stringify({
                url_uid: this.props.uid,
                clear_all: true
            }));
        }
    }

    webSocketSend(type){
        if(type === "view-stats"){
            this.props.webSocket.send(JSON.stringify({
                url_uid: this.props.uid,
                view_stats: true 
            }));
        }
        else if(type === "view-selected"){
            this.props.webSocket.send(JSON.stringify({
                url_uid: this.props.uid,
                view_selected: true 
            }));
        }
        else if(type === "view-filtered"){
            this.props.webSocket.send(JSON.stringify({
                url_uid: this.props.uid,
                view_filtered: true 
            }));
        }
    }

	render = () => {
        
        //var statisticStatSelectItems = ["Count", "Min", "Max", "Mean", "Median", "Mode", "Sum", "Range", "St. Dev.", "Variance", "Skewness", "Kurtosis"];
        var statisticStatSelectItems = ["Min", "Max", "Mean", "Median", "Mode", "Sum", "Range", "St. Dev.", "Variance"];
        statisticStatSelectItems = statisticStatSelectItems.map(function(value) {
			return({ label: value, value: value });
		});

        var statisticColSelectItems = this.props.colList.map(function(value) {
			return({ label: value, value: value });
		});

        // debugger;

        var statDataKeys = Object.keys(this.props.statData);
        var sameList = true;

        if (statDataKeys.length !== this.props.colList.length) {
            sameList = false;
        }
        else {
            for (var i = 0; i < statDataKeys.length; i++) {
                if (statDataKeys.indexOf(this.props.colList[i]) === -1) {
                    sameList = false;
                    break;
                }
            }
        }

        if (statDataKeys.length !== 0 && sameList) {
            // debugger; //WOOOOOOOOOOOOOOOOO
            for (var i = statisticColSelectItems.length -1; i > -1; i--) {
                var colKeys = Object.keys(this.props.statData[statisticColSelectItems[i].value].values)
                if (isNaN(this.props.statData[statisticColSelectItems[i].value].values[colKeys[0]].value)) {
                    statisticColSelectItems.splice(i, 1);
                }
            }
        }
        
		return(
            <Flexbox flexDirection = "column" id = "TopView" style = {{ height: "100%" }}>

                <XYZRemapModal initialX = { this.props.initialX } initialY = { this.props.initialY } initialZ = { this.props.initialZ } />

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
                <Flexbox flexDirection = "row" style = {{ margin: "-0.938vh 0px -0.209vh -0.104vh" }} >
                    <Flexbox style = {{ width: "55%", margin: "0.104vh 0.626vh 0px 0px" }} > 
                        <Select 
                            className = "selectViewName dark-theme"
                            simpleValue
                            value = { this.state.viewSelectValue } 
                            placeholder = "Select a view" 
                            options = { this.state.viewSelectItems } 
                            onChange = { this.onSelectViewChange } 
                            onOpen = { () => this.props.handleDraggableCorrection(true, true) }
                        />
                    </Flexbox>
                    
                    <Flexbox style = {{ width: "45%", marginBottom: "-0.417vh" }} >

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

                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Undo Filter
                                </div> 
                            }
                        >
                            <RaisedButton
                                onClick = { () => this.handleUndo() }
                                label = { <i className = "fa fa-undo" style = {{ margin: "0px 0px 0px -0.313vh" }} /> }
                                style = {{
                                    margin: "0.417vh 0.626vh 1.147vh -0.209vh",
                                    minWidth: "1.9vw",
                                    width: "1.9vw",
                                    height: "2.607vh"
                                }}
                                buttonStyle = {{
                                    height: "2.607vh",
                                    lineHeight: "2.607vh",
                                    //backgroundColor: ( this.canUndoRedo("undo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                    backgroundColor: this.props.settings.colors.collapsibleColor.subBackground
                                }} 
                                labelStyle = {{
                                    fontSize: "1.356vh",
                                    color: this.props.settings.colors.overviewButtonsColor.text
                                }}
                                overlayStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh'
                                }}
                                primary = { true }
                            />
                        </Tooltip>

                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Redo Filter
                                </div> 
                            }
                        >
                            <RaisedButton
                                onClick = { () => this.handleRedo() }
                                label = { <i className = "fa fa-repeat" style = {{ margin: "0px 0px 0px -0.313vh" }} /> }
                                style = {{
                                    margin: "0.417vh 0.209vh 0px 0px",
                                    minWidth: "1.9vw",
                                    width: "1.9vw",
                                    height: "2.607vh"
                                }}
                                buttonStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '3.650vh',
                                    //backgroundColor: ( this.canUndoRedo("redo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                    backgroundColor: this.props.settings.colors.collapsibleColor.subBackground
                                }} 
                                labelStyle = {{
                                    fontSize: '1.360vh',
                                    color: this.props.settings.colors.overviewButtonsColor.text
                                }}
                                overlayStyle = {{
                                    height: "2.607vh",
                                    lineHeight: "2.607vh"
                                }}
                                primary = { true }
                            />
                        </Tooltip>

                        <div>
                            <Tooltip
                                placement = 'left'
                                mouseEnterDelay = { 0.5 }
                                mouseLeaveDelay = { 0.15 }
                                destroyTooltipOnHide = { false }
                                trigger = { Object.keys( {hover: 1} ) }
                                overlay = { 
                                    <div> 
                                        Visualization Menu Options
                                    </div> 
                                }
                            >
                                <RaisedButton
                                    onClick = { (evt) => this.handleOpenClose('menu', true, evt) }
                                    label = "Menu"
                                    style = {{
                                        margin: "0.417vh 0px 1.147vh 0.417vh",
                                        minWidth: "5.8vw",
                                        width: "5.8vw"
                                    }}
                                    buttonStyle = {{
                                        height: "2.607vh",
                                        lineHeight: "2.607vh",
                                        backgroundColor: this.props.settings.colors.overviewButtonsColor.background
                                    }} 
                                    labelStyle = {{
                                        fontSize: '1.360vh',
                                        color: this.props.settings.colors.overviewButtonsColor.text
                                    }}
                                    overlayStyle = {{
                                        height: "2.607vh",
                                        lineHeight: "2.607vh"
                                    }}
                                    primary = { true }
                                />
                            </Tooltip>

                            <Popover
                                open = { this.state.menu.open }
                                anchorEl = { this.state.menu.anchorEl }
                                onRequestClose = { (evt) => this.handleOpenClose('menu', false, evt) }
                                style = {{ fontSize: '1.360vh', width: "11.691vw" }}
                            >
                                <Menu>
                                    <MenuItem primaryText = "New" className = "menuItemStyling" onClick = { this.onMenuNewClick }/>
                                    <MenuItem primaryText = "Save" className = "menuItemStyling" onClick = { this.onMenuSaveClick }/>
                                    <MenuItem primaryText = "Save As" className = "menuItemStyling" onClick = { this.onMenuSaveAsClick }/>
                                    <MenuItem primaryText = "Delete" className = "menuItemStyling" onClick = { this.onMenuDeleteClick } disabled = {this.state.viewSelectValue === null || this.state.viewSelectValue === ""}/>
                                </Menu>
                            </Popover>

                            <Popover
                                open = { this.state.menu.showOpen }
                                anchorEl = { this.state.menu.showAnchorEl }
                                onRequestClose = { (evt) => this.handleOpenClose('showMenu', false, evt) }
                                style = {{ fontSize: '1.360vh' }}
                            >
                                <Menu>
                                    <Tooltip
                                        placement = 'left'
                                        mouseEnterDelay = { 0.5 }
                                        mouseLeaveDelay = { 0.15 }
                                        destroyTooltipOnHide = { false }
                                        trigger = { Object.keys( {hover: 1} ) }
                                        overlay = { 
                                            <div> 
                                                Select and Monitor Statistics on Numeric Columns
                                            </div> 
                                        }
                                    >
                                        <MenuItem primaryText = "Statistics" className = "menuItemStyling" 
                                            onClick = { () => (this.props.uid ? this.webSocketSend("view-stats") : 
                                                this.handleOpenClose('statistics', true)) } />
                                    </Tooltip>

                                    <Tooltip
                                        placement = 'left'
                                        mouseEnterDelay = { 0.5 }
                                        mouseLeaveDelay = { 0.15 }
                                        destroyTooltipOnHide = { false }
                                        trigger = { Object.keys( {hover: 1} ) }
                                        overlay = { 
                                            <div> 
                                                View Data of Selected Glyphs in Table Format
                                            </div> 
                                        }
                                    >
                                        <MenuItem primaryText = "Selected Data" className = "menuItemStyling" 
                                            onClick = {() => (this.props.uid ? this.webSocketSend("view-selected") : 
                                                this.onSelectedDataClick.bind(this) )} />
                                    </Tooltip>
                                    <Tooltip
                                        placement = 'left'
                                        mouseEnterDelay = { 0.5 }
                                        mouseLeaveDelay = { 0.15 }
                                        destroyTooltipOnHide = { false }
                                        trigger = { Object.keys( {hover: 1} ) }
                                        overlay = { 
                                            <div> 
                                                View Data of Currently Visible Glyphs in Table Format
                                            </div> 
                                        }
                                    >
                                        <MenuItem primaryText = "Filtered Data" className = "menuItemStyling" 
                                        onClick = {() => (this.props.uid ? this.webSocketSend("view-filtered") : 
                                            this.onFilteredDataClick.bind(this)) } />
                                    </Tooltip>
                                </Menu>
                            </Popover>

                            {/*}

                            <Popover
                                open = { this.state.menu.actionOpen }
                                anchorEl = { this.state.menu.actionAnchorEl }
                                onRequestClose = { (evt) => this.handleOpenClose('actionMenu', false, evt) }
                                style = {{ fontSize: '13px' }}
                            >
                                <Menu>
                                    <MenuItem primaryText = "Change Axes" className = "menuItemStyling" onClick = { () => this.handleOpenClose('XYZ', true) }/>
                                </Menu>
                            </Popover>
                            */}

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
                                    onKeyDown = { (e) => this.enterKeyToSave(e) }
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
                    <FilterSummaryView
                        ref = 'filterSummaryView'
                        tableData = { this.props.tableData }
                        onScroll = { (element, elastic) => this.props.initParams.scrollToElement(element, elastic) }
                        refreshParent = { this.props.refreshParent }
                    /> 
                </Flexbox>
                
                {/* Row 3 */}
                <Flexbox flexDirection = "row" alignContent = "space-between" 
                    style = {{ margin: this.state.multiTable ? "0.521vh 0px" : "0.521vh 0px -0.417vh" }} >
                    <Flexbox style = {{ width: "30%" }} > 
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Clears all filters currently applied
                                </div> 
                            }
                        >
                            <RaisedButton 
                                label = "Clear"
                                style = {{
                                    width: "100%",
                                    minWidth: "0px"
                                }}
                                buttonStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh',
                                    backgroundColor: this.props.settings.colors.overviewButtonsColor.background
                                }} 
                                labelStyle = {{
                                    fontSize: '1.251vh',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    margin: "0px 0px 0px -0.313vh",
                                    paddingLeft: "0px",
                                    paddingRight: "0px"
                                }}
                                overlayStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh'
                                }}
                                onClick = { () => {this.onClearAllFilters(), this.desktopClearFilters() }}
                                primary = { true } 
                            />
                        </Tooltip>

                    </Flexbox>

                    <Flexbox style = {{ width: "5%" }} /> 

                    <Flexbox style = {{ width: "30%" }} >
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    { this.state.hideShowButtonTextFlag ? "Show All Glyphs" : "Only show Non-Filtered Glyphs." }
                                </div> 
                            }
                        >
                            <RaisedButton 
                                label = { this.state.hideShowButtonTextFlag ? "Show" : "Hide" }
                                id = "buttonHideShow"
                                style = {{
                                    width: "100%",
                                    minWidth: "0px"
                                }}
                                buttonStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh',
                                    backgroundColor: this.props.settings.colors.overviewButtonsColor.background
                                }} 
                                labelStyle = {{
                                    fontSize: '1.251vh',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    paddingLeft: "0px",
                                    paddingRight: "0px"

                                }}
                                overlayStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh'
                                }}
                                onClick = { this.onHideFilteredData.bind(this) }
                                primary = { true } 
                            />
                        </Tooltip>

                    </Flexbox>

                    <Flexbox style = {{ width: "5%" }} /> 

                    <Flexbox style = {{ width: "30%" }} >
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Click to view a list of options
                                </div> 
                            }
                        >
                            <RaisedButton
                                onClick = { (evt) => this.handleOpenClose('showMenu', true, evt) }
                                label = "View"
                                style = {{
                                    width: "100%",
                                    minWidth: "0px"
                                }}
                                buttonStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh',
                                    backgroundColor: this.props.settings.colors.overviewButtonsColor.background
                                }} 
                                labelStyle = {{
                                    fontSize: '1.251vh',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    paddingLeft: "0px",
                                    paddingRight: "0px"
                                }}
                                overlayStyle = {{
                                    height: '2.607vh',
                                    lineHeight: '2.607vh'
                                }}
                            />
                        </Tooltip>
                    </Flexbox>
                    {/*}

                    <Flexbox style = {{ width: "4%" }} /> 

                    <Flexbox style = {{ width: "22%" }} > 
                        <RaisedButton 
                            primary = { true } 
                            label = "Actions" 
                            onClick = { (evt) => this.handleOpenClose('actionMenu', true, evt) }
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
                    */}
                    
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
                                onClick = { () => {this.state.statisticColSelectValues && this.state.statisticStatSelectValues ? 
                                    this.onApplyStatistics(this) : this.setState({statisticsSelectBoth: true}) }}
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

                    <b>Select which column(s) to monitor:</b> <br />
                    <OldSelect 
                        multi 
                        simpleValue
                        value = { this.state.statisticColSelectValues } 
                        placeholder = "Select your column(s)" 
                        options = { statisticColSelectItems } 
                        onChange = { this.onSelectStatisticColChange.bind(this) } 
                    />

                    <br />
                    <br />

                    <b>Select which statistic(s) to apply:</b> <br />
                    <OldSelect 
                        multi 
                        simpleValue
                        value = { this.state.statisticStatSelectValues }  
                        placeholder = "Select your statistic(s)" 
                        options = { statisticStatSelectItems } 
                        onChange = { this.onSelectStatisticStatChange.bind(this) } 
                    />

                    <div 
                        style={{display: this.state.statisticsSelectBoth ? "" : "none"}}>
                        <b>Select at least 1 column and statistic to apply.</b>
                    </div>
                </Dialog>
                
                {/* Row 4 */}
                <Flexbox flexDirection = "row" style = {{ height: "3.233vh", display: this.state.multiTable ? "" : "none" }} >
                    <OldSelect 
                        multi 
                        className = "selectTableName dark-theme"
                        simpleValue
                        value = { this.state.tableSelectValues } 
                        disabled = { this.state.tableSelectItems.length === 1 } 
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
export const editModalDisplay = (selectedFilteredModal, XYZModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    selectedFilteredModal,
    XYZModal
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

export const editUndoRedoHistory = (undoRedoHistory) => ({
  type: 'UPDATE_HISTORY',
  undoRedoHistory
});

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});

export const setRecentVizDropdown = (recentVizDropdown) => ({
    type: 'SET_RECENT_VIZ_DROPDOWN',
    recentVizDropdown,
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
	VizParams: state.filterState.VizParams,
    UndoRedoHistory: state.filterState.UndoRedoHistory,
    statData: state.filterState.StatisticsData,
    RecentVizDropdown: state.filterState.RecentVizDropdown,
    uid: state.filterState.uid,
    webSocket: state.filterState.webSocket
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
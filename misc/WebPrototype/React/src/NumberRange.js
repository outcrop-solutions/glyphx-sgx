import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import { makeServerCall } from './ServerCallHelper.js';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import Flexbox from 'flexbox-react';
import Range from 'rc-slider/lib/Range'; 
import 'rc-slider/assets/index.css';
import './General.css';


/**
 * @param minVal: Sets the minimum value allowed for all the rangeList within the range table
 * @param maxVal: Sets the maximum value allowed for all the rangeList within the range table
 * @param colName: Name of the corresponding column for this RangeForm
 * @param data: Array of values from the eleastic table for the corresponding colName
 **/
class NumberRangeTable extends React.Component {


    /**
     * Deletes a range from the store
     * @param id: ID of the row which is to be deleted
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id, this.props.data, "Number"));
    };


    /**
     * Adds a range with the default values of: [minimum value for the column, maximum value for the column, random generated ID, false]
     **/
    handleAddEvent() {
        this.props.dispatch(addRange(this.props.colName, this.props.minVal, this.props.maxVal, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false, "Number"));
    }

    handleHistory() {
        var undoRedoHistory = {
            history: this.props.UndoRedoHistory.history.slice(0),
            position: this.props.UndoRedoHistory.position
        }

        // debugger;

        if (undoRedoHistory.position !== undoRedoHistory.history.length - 1) {
            undoRedoHistory.history = undoRedoHistory.history.slice(0, undoRedoHistory.position + 1);
        }

        undoRedoHistory.history.push({filterList: this.props.rangeList, tableData: this.props.fullTableData});
        undoRedoHistory.position = undoRedoHistory.position + 1;

        this.props.dispatch(editUndoRedoHistory(undoRedoHistory));
    }


    /**
     * - ADCMT
     */
    applyFilter = () => {
        //console.log('Filter Applied');
        var iframe = document.getElementById('GlyphViewer').contentWindow;

        var context = this;
        this.props.rangeList;
        // debugger;

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
                iframe.filterGlyphs(tempRowIds);

                context.props.dispatch( setTimer(new Date().getTime()) );
            },
            {
                post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.rangeList } 
            }
        );
    };


    /**
     * Updates a range row in the store (null values may be passed to min, max, or applied to peform no update on that field)
     * @param id: ID used to find the range in the store
     * @param min: Min value to update
     * @param max: Max value to update
     * @param applied: Applied status to update
     **/
    handleStoreUpdate(id, min, max, applied) { 
        this.props.dispatch(updateRange(this.props.colName, min, max, id, applied, this.props.data, "Number"));
    }


    /**
     * Maps ranges to DOM
     **/
    render() {
        var onRowDel = this.handleRowDel.bind(this);
        var updateStore = this.handleStoreUpdate.bind(this);
        var addToHistory = this.handleHistory.bind(this);
        var minVal = this.props.minVal;
        var maxVal = this.props.maxVal;
        var rList = this.props.rangeList[this.props.colName].rangeList;
        var settings = this.props.settings;
        var refreshTableDataOnRowSelection = this.props.refreshTableDataOnRowSelection;
        var fullTableData = this.props.fullTableData;
        var filterList = this.props.rangeList;
        var setFilterIDs = this.props.setFilterIDs;
        var UndoRedoHistory = this.props.UndoRedoHistory;
        var setTableData = this.props.setTableData;
        var applyFilter = this.applyFilter.bind(this);
        var setFilterBusy = this.props.setFilterBusy;

        var range = rList.map( function(range) {
            return (
                minVal != null ?
                <NumberRangeRow 
                    range = { range } 
                    onDelEvent = { onRowDel.bind(this) } 
                    updateStore = { updateStore }
                    addToHistory = { addToHistory }
                    key = { range[2] }
                    minVal = { minVal }
                    maxVal = { maxVal }
                    settings = { settings }
                    refreshTableDataOnRowSelection = { refreshTableDataOnRowSelection }
                    tableData = { fullTableData }
                    filterList = { filterList }
                    setFilterIDs = { setFilterIDs }
                    UndoRedoHistory = { UndoRedoHistory }
                    setTableData = { setTableData }
                    applyFilter = { applyFilter }
                    setFilterBusy = { setFilterBusy }
                    
                />
                :
                null
            )
        });

        return (
            <div style = {{ padding: "0px 15px", margin: "0px 0px 1px" }} >
                <div style = {{ borderRadius: "3px", backgroundColor: "#9f9f9f", padding: "8px 3px",  }} >

                    {/* Displays the mapped ranges */}
                    {range}

                    {/* Add range button below*/}
                    <Card containerStyle = {{ padding: "0px" }}>
                        <CardText
                            style = {{
                                padding: "0px"
                            }}
                        >
                            <Flexbox flexDirection = "row" style = {{ margin: "-4px 0px -4px 3px" }} >
                                <FontIcon
                                    onClick = { this.handleAddEvent.bind(this) }
                                    className = "fa fa-plus cursorHand"
                                    hoverColor = { this.props.settings.colors.rangeColor.addHover }
                                    style = {{
                                        fontSize: "1.7rem",
                                        margin: "12px 0px 6px 5px"
                                    }}
                                />
                            </Flexbox>
                        </CardText>
                    </Card>
                </div>
            </div>
        );
    }
}


/**
 * Defines what a row should render as on the DOM
 * Inherits props from NumberRangeTable
 **/
class NumberRangeRow extends React.Component {

    constructor(props) {
        super(props);

        this.state = {
            latestUpdate: "",
            epoch: 0,
            min: this.props.range[0],
            max: this.props.range[1],
            backgroundColor: (this.props.range[3] ? "#ffffff" : "#f1f1f1")
        }
    }

    
    /**
     * Tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        return (this.props.range != nextProps.range || this.props.settings != nextProps.settings || this.state.min !== nextState.min || this.state.max !== nextState.max);

        /*
        if (this.props.range != nextProps.range || this.props.settings != nextProps.settings) {
            return true;
        }
        if (this.state.min !== nextState.min || this.state.max !== nextState.max) {
            return true;
        }
        return false;
        */
    };
    

    /**
     * Updates the state when the component gets new props from the store
     * @param nextProps: The props the component would have after the change
     **/
    componentWillReceiveProps(nextProps) {
        this.setState({ min: nextProps.range[0], max: nextProps.range[1], backgroundColor: (nextProps.range[3] ? "#ffffff" : "#f1f1f1") });
    };


    /**
     * Updates the local state when the slider is dragged
     * @param e: the event instance of the slider: array of [min, max]
     **/
    onSlide(e) {
        this.setState({ min: e[0], max: e[1] });
    };


    /**
     * Updates the store when the slider gets released
     * @param e: the event instance of the slider: array of [min, max]
     **/
    onAfterSlide(e) {
        if (this.props.range[3]) {
            this.props.setFilterBusy(true);

            var context = this;
            let pom = new Promise(function (resolve, reject) {
                // debugger;
                context.props.setTableData(context.props.UndoRedoHistory.history[0].tableData);
                resolve('done');
            });

            pom.then(
                () => context.props.updateStore(context.props.range[2], e[0], e[1], null, context.props.range)
            ).then(
                () => context.props.applyFilter()
            ).then(
                () => context.props.refreshTableDataOnRowSelection()
            ).then(
                () => context.props.addToHistory()
            ).then (
                () => context.props.setFilterBusy(false)
            );
        }
        else {
            this.props.updateStore(this.props.range[2], e[0], e[1], null, this.props.range);
        }
    };


    /**
     * Updates local state values of min and max based on text field input
     * @param e: the event instance of the text field, html element
     **/
    onTextChange(e) {
        var value = e.target.value;

        if ( !isNaN(value) || value === "" || value === "-" ) {
            if (e.target.name === "min") {
                this.setState({ min: value });
            }

            else {
                this.setState({ max: value });
            }
        }

        /*
        // debugger;

        var minimum = parseFloat(this.props.minVal, 10);
        var maximum = parseFloat(this.props.maxVal, 10);
        var value = e.target.value;

        if ( !isNaN(value) && value !== "" ) {
            value = parseFloat(value, 10);
            if (e.target.name === "min") {
                if ( value > maximum  ) {
                    this.setState({ min: maximum });
                }

                else if (value < minimum) {
                    this.setState({ min: minimum });
                }

                else {
                    this.setState({ min: value });
                }
            }

            else {
                if ( value > maximum  ) {
                    this.setState({ max: maximum });
                }

                else if (value < minimum) {
                    this.setState({ max: minimum });
                }
                
                else {
                    this.setState({ max: value });
                }
            }
        }

        else {
            if (e.target.name === "min") {
                if (value === "") {
                    this.setState({ min: "" });
                }

                else {
                    this.setState({ min: minimum });
                }
            }

            else {
                if (value === "") {
                    this.setState({ max: "" });
                }

                else {
                    this.setState({ max: maximum });
                }
            }
        }
        */
    };


    /**
     * Updates the text fields if (min > max) and updates the store
     **/
    onTextBlur() {
        var minimum = parseFloat(this.props.minVal, 10);
        var maximum = parseFloat(this.props.maxVal, 10);
        var min = this.state.min;
        var max = this.state.max;

        debugger;

        if (min === "") {
            min = minimum;
        }

        if (max === "") {
            max = maximum;
        }

        min = parseFloat(min, 10);
        max = parseFloat(max, 10);

        if (min < minimum) {
            min = minimum;
        }

        if (max > maximum) {
            max = maximum;
        }

        if (min > max) {
            if (this.state.latestUpdate === "MIN") {
                min = max;
            }

            else if (this.state.latestUpdate === "MAX") {
                max = min;
            }
        }

        //this.props.updateStore(this.props.range[2], min, max, null, this.props.range);

        if (this.props.range[3]) {
            this.props.setFilterBusy(true);

            var context = this;
            let pom = new Promise(function (resolve, reject) {
                // debugger;
                context.props.setTableData(context.props.UndoRedoHistory.history[0].tableData);
                resolve('done');
            });

            pom.then(
                () => context.props.updateStore(context.props.range[2], min, max, null, context.props.range)
            ).then(
                () => context.props.applyFilter()
            ).then(
                () => context.props.refreshTableDataOnRowSelection()
            ).then(
                () => context.props.addToHistory()
            ).then (
                () => context.props.setFilterBusy(false)
            );
        }
        else {
            this.props.updateStore(this.props.range[2], min, max, null, this.props.range);
        }
    };


    /**
     * Updates toggle state in store but only once every 300 miliseconds (prevents a queue of actions on the store)
     * @param e: the event instance of the toggle, html element
     **/
    onToggle(e) {
        var epoch = (new Date()).getTime();

        if (epoch > this.state.epoch + 300) {
            if (e.target.checked) {
                var min = null;
                var max = null;

                if (this.state.min == "") {
                    min = this.props.minVal;
                }

                if (this.state.max == "") {
                    max = this.props.maxVal;
                }

                //this.props.updateStore(this.props.range[2], min, max, true);

                this.props.setFilterBusy(true);

                var context = this;
                let pom = new Promise(function (resolve, reject) {
                    // debugger;
                    context.props.setTableData(context.props.UndoRedoHistory.history[0].tableData);
                    resolve('done');
                });

                pom.then(
                    () => context.props.updateStore(context.props.range[2], min, max, true)
                ).then(
                    () => context.props.applyFilter()
                ).then(
                    () => context.props.refreshTableDataOnRowSelection()
                ).then(
                    () => context.props.addToHistory()
                ).then (
                    () => context.props.setFilterBusy(false)
                );

            }

            else {
                //this.props.updateStore(this.props.range[2], null, null, false);

                this.props.setFilterBusy(true);

                var context = this;
                let pom = new Promise(function (resolve, reject) {
                    context.props.updateStore(context.props.range[2], null, null, false);
                    resolve('done');
                });

                //pom.then(() => context.props.refreshTableDataOnRowSelection()).then(() => context.applyFilter()); //.then(() => context.addToHistory()).then(() => context.applyFilter())

                pom.then(
                    () => context.props.applyFilter()
                ).then(
                    () => context.props.refreshTableDataOnRowSelection()
                ).then(
                    () => context.props.addToHistory()
                ).then (
                    () => context.props.setFilterBusy(false)
                );
            }

            this.setState({ epoch: epoch });
        }        
    }


    /**
     * Deletes a row from the range table
     **/
    onDelEvent() {
        //this.props.onDelEvent(this.props.range[2]);

        if (this.props.range[3]) {
            this.props.setFilterBusy(true);

            var context = this;
            let pom = new Promise(function (resolve, reject) {
                context.props.onDelEvent(context.props.range[2]);
                resolve('done');
            });

            //pom.then(() => context.props.refreshTableDataOnRowSelection()).then(() => context.applyFilter()); //.then(() => context.addToHistory()).then(() => context.applyFilter())

            pom.then(
                () => context.props.applyFilter()
            ).then(
                () => context.props.refreshTableDataOnRowSelection()
            ).then(
                () => context.props.addToHistory()
            ).then (
                () => context.props.setFilterBusy(false)
            );
        }
        else {
            this.props.onDelEvent(this.props.range[2]);
        }
    }


    /**
     * Preprocesses min max vals, allows min > max while typing
     * @param min: the min to be processed 
     * @param max: the max to be processed 
     **/
    arrayNumConversion(min, max) {
        // Make min a valid type
        if (min === "") {
            min = this.props.minVal;
        }
        else {
            min = parseFloat(min, 10);
        }

        // Make max a valid type
        if (max === "") {
            max = this.props.maxVal;
        }
        else {
            max = parseFloat(max, 10);
        }

        // Apply a valid range
        if ( min > max) {
            if (this.state.latestUpdate === "MIN") {
                return [max, max];
            }
            else if (this.state.latestUpdate === "MAX") {
                return [min, min]; 
            }
        }
        else {
            return [min, max];
        }
    }


    /**
     * Updates the local state (used to allow real time text-slider changing without pushing overlap bounds)
     * @param val: "MIN" or "MAX", text field that called this
     **/
    updateLatest(val) {
        this.setState({latestUpdate: val});
    }


    /**
     * Loses focus from the min text field when the enter key is pressed
     * @param e: key that was pressed
     **/
    onKeyPressMin(e) {
        if(e.key === 'Enter') { 
            this.inputElementMin.blur();
        } 
    }


    /**
     * Loses focus from the max text field when the enter key is pressed
     * @param e: key that was pressed
     **/
    onKeyPressMax(e) {
        if(e.key === 'Enter') { 
            this.inputElementMax.blur();
        } 
    }

    render() {
        return (
            <Card containerStyle = {{ padding: "0px" }}>
                <CardText
                    style = {{
                        backgroundColor: this.state.backgroundColor,
                        padding: "0px"
                    }}
                >
                    <Flexbox flexDirection = "row" style = {{ margin: "-4px 0px" }} >

                        <Flexbox style = {{ width: "10%", margin: "0px 8px 0px 3px" }} > 
                            <FontIcon
                                onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash cursorHand"
                                hoverColor = { this.props.settings.colors.rangeColor.deleteHover }
                                style = {{
                                    fontSize: "1.7rem",
                                    margin: "12px 0px 6px 5px"
                                }}
                            />
                        </Flexbox>

                        <Flexbox style = {{ width: "20%", margin: "0px 0px -1px" }} > 
                            <TextField 
                                type = 'number' 
                                name = "min"
                                ref = { input => this.inputElementMin = input }
                                value = { this.state.min } 
                                hintText = { this.props.minVal.toString() }
                                style = { styleSet.textfieldStyles }
                                onChange = {
                                    (e) => this.onTextChange(e)
                                }
                                onFocus = { () => this.updateLatest("MIN") }
                                onBlur = {
                                    () => this.onTextBlur()
                                }
                                onKeyPress = {
                                    (e) => this.onKeyPressMin(e)
                                }
                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.rangeColor.textFieldUnderline }}
                                underlineStyle = {{ borderColor: "#d2d2d2", margin: "0px 0px 4px 0px" }}
                            />
                        </Flexbox>

                        <Flexbox style = {{ width: "40%", margin: "16px 17px 0px 8px" }} > 

                            <Range
                                min = { this.props.minVal }
                                max = { this.props.maxVal }
                                step = { (this.props.minVal % 1 === 0 && this.props.maxVal % 1 === 0 ? 1 : 0.1) }
                                value = {this.arrayNumConversion(this.state.min, this.state.max) }
                                defaultValue = { [this.props.minVal,this.props.maxVal] }
                                allowCross = { false }
                                onChange = {
                                    (e) => this.onSlide(e)
                                }
                                onAfterChange = {
                                    (e) => this.onAfterSlide(e)
                                }
                                railStyle = {{ backgroundColor: "#d2d2d2" }}
                                trackStyle = { [{ backgroundColor: this.props.settings.colors.rangeColor.sliderTrack }] }
                                handleStyle = { [{ backgroundColor: this.props.settings.colors.rangeColor.sliderCircle, borderColor: this.props.settings.colors.rangeColor.sliderCircle }, { backgroundColor: this.props.settings.colors.rangeColor.sliderCircle, borderColor: this.props.settings.colors.rangeColor.sliderCircle }] }
                            />
                        </Flexbox>

                        <Flexbox style = {{ width: "20%", margin: "0px -5px -1px 2px" }} > 
                            <TextField 
                                type = 'number' 
                                name = "max"
                                ref = { input => this.inputElementMax = input }
                                value = { this.state.max }
                                hintText = { this.props.maxVal.toString() }
                                style = { styleSet.textfieldStyles }
                                onChange = {
                                    (e) => this.onTextChange(e)
                                }
                                onFocus = { () => this.updateLatest("MAX") }
                                onBlur = {
                                    () => this.onTextBlur()
                                }
                                onKeyPress = {
                                    (e) => this.onKeyPressMax(e)
                                }
                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.rangeColor.textFieldUnderline }}
                                underlineStyle = {{ borderColor: "#d2d2d2",margin: "0px 0px 4px 0px" }}
                            />
                        </Flexbox>

                        <Flexbox style = {{ width: "10%", marginTop: "11px", marginRight: "11px" }} > 
                            <Toggle 
                                name = "applied" 
                                id = { this.props.range.id } 
                                toggled = { this.props.range[3] } 
                                onToggle = {  
                                    (e) => this.onToggle(e)
                                }
                                trackStyle = {{
                                    height: "12px",
                                    marginTop: "2px",
                                    width: "84%"
                                }}
                                trackSwitchedStyle = {{ backgroundColor: this.props.settings.colors.rangeColor.toggleTrack }}
                                thumbStyle = {{
                                    height: "17px",
                                    width: "17px",
                                    top: "3px"
                                }}
                                thumbSwitchedStyle = {{ backgroundColor: this.props.settings.colors.rangeColor.toggleCircle, left: "40%" }}
                            />
                        </Flexbox>

                    </Flexbox>
                </CardText>
            </Card>
        );
    }
}


/**
 * Local styling
 **/
const styleSet = {
    textfieldStyles: {
        width: "60px"
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const addRange = (colName, min, max, id, applied, rangeType) => ({
    type: 'ADD_RANGE',
    colName,
    min,
    max,
    id,
    applied,
    rangeType
});

export const removeRange = (colName, id, data, rangeType) => ({
    type: 'REMOVE_RANGE',
    colName,
    id,
    data,
    rangeType
});

export const updateRange = (colName, min, max, id, applied, data, rangeType) => ({
    type: 'UPDATE_RANGE',
    colName,
    id,
    min,
    max,
    applied, 
    data,
    rangeType,
});

export const editUndoRedoHistory = (undoRedoHistory) => ({
  type: 'UPDATE_HISTORY',
  undoRedoHistory
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
    rangeList: state.filterState.Filter,
    settings: state.filterState.Settings,
    VizParams: state.filterState.VizParams,
    UndoRedoHistory: state.filterState.UndoRedoHistory
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(NumberRangeTable);
import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import { Flex } from 'react-flex-material';
import Range from 'rc-slider/lib/Range'; 
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.css';
import './range.css';


/**
 * Main Range parent class which gets exported
 * @param minVal: Sets the minimum value allowed for all the rangeList within the range table
 * @param maxVal: Sets the maximum value allowed for all the rangeList within the range table
 * @param colName: Name of the corresponding column for this RangeForm
 * @param data: array of values from the eleastic table for the corresponding colName
 * @param rangeType: "slider", "date". Sets what stype of range to display (only slider implemented as of now)
 **/
class RangeForm extends React.Component {

    constructor(props) {
        super(props);
    }

    
    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.rangeList[this.props.colName].rangeList != nextProps.rangeList[this.props.colName].rangeList) {
            return true;
        }
        return false;
    }
    

    /**
     * Deletes a range by splicing it out of the store which causes DOM to re-map
     * @param id: ID of the row which is to be deleted
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id, this.props.data, this.props.rangeType));
    };


    /**
     * Adds a range with the default values of the current min being set to the minimum value, current max being set to the maximum value,
     * a new generated ID, and the applied switch on off.
     **/
    handleAddEvent() {
        this.props.dispatch(addRange(this.props.colName, this.props.minVal, this.props.maxVal, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false));
    }


    /**
     * Updates the store when a text field gets new values
     * @param e: the event instance of the text field, html element
     * @param id: ID used to find the range in the store
     **/
     handleTextUpdate(e, id) {
         console.log("Start text update", performance.now());
        var minimum = parseInt(this.props.minVal, 10);
        var maximum = parseInt(this.props.maxVal, 10);
        var value = e.target.value;

        if ( !isNaN(value) ) {
            if (e.target.name === "min") {
                if ( value > maximum  ) {
                    this.props.dispatch(updateRange(this.props.colName, maximum, null, id, null, null));
                }
                else if (value < minimum) {
                    this.props.dispatch(updateRange(this.props.colName, minimum, null, id, null, null));
                }
                else {
                    this.props.dispatch(updateRange(this.props.colName, value, null, id, null, null));
                }
            }

            else {
                if ( value > maximum  ) {
                    this.props.dispatch(updateRange(this.props.colName, null, maximum, id, null, null));
                }
                else if (value < minimum) {
                    this.props.dispatch(updateRange(this.props.colName, null, minimum, id, null, null));
                }
                else {
                    this.props.dispatch(updateRange(this.props.colName, null, value, id, null, null));
                }
            }
        }
        console.log("Finish text update", performance.now());
    };


    /**
     * Updates the visuals of the slider , and the text fields if needed, when a text field loses focus
     * @param id: ID used to find the range
     * @param min: current min stored in store
     * @param max: current max stored in store
     * @param latestChange: 'MIN' or 'MAX', last focused text box
     **/
    handleTextBlur(id, min, max, latestChange) {
        // Make min a valid type
        if (min === "") {
            min = this.props.minVal;
        }
        else {
            min = parseInt(min, 10);
        }

        // Make max a valid type
        if (max === "") {
            max = this.props.maxVal;
        }
        else {
            max = parseInt(max, 10);
        }

        if (min > max) {
            if (latestChange === "MIN") {
                this.props.dispatch(updateRange(this.props.colName, max, max, id, null, this.props.data, this.props.rangeType));
                console.log("DISPATCHED MAX VALUE");
            }
            else if (latestChange === "MAX") {
                this.props.dispatch(updateRange(this.props.colName, min, min, id, null, this.props.data, this.props.rangeType));
                console.log("DISPATCHED MIN VALUE");
            }
        }
    };

    
    /**
     * Updates the visuals of the slider and the text fields when the slider is dragged
     * @param e: the event instance of the slider: array of [min, max]
     * @param id: ID used to find the range in the store
     **/
    handleSliderUpdate(e, id) {
        console.log("Start slider update", performance.now());
        console.log(" MIN: " + e[0] + "MAX: " + e[1] + "   ------------------------------------------------");
        this.props.dispatch(updateRange(this.props.colName, e[0], e[1], id, null, this.props.data, this.props.rangeType));
        console.log("Finish slider update", performance.now());
    };


    /**
     * Updates the mapping data structure with true false values corresponding to toggle state of a switch
     * @param e: the event instance of the toggle, html element
     * @param id: ID used to find the range in the store
     **/
    handleSwitchToggle(e, id) {
        if (e.target.checked) {
            this.props.dispatch(updateRange(this.props.colName, null, null, id, true, this.props.data, this.props.rangeType));
        }
        else {
            this.props.dispatch(updateRange(this.props.colName, null, null, id, false, this.props.data, this.props.rangeType));
        }
    };


    /**
     * The view for Range Form will display the Rangle Table class
     * Passes data methods as props so that RangeTable has access
     **/
    render() {
        var onRowDel = this.handleRowDel.bind(this);
        var onSlide = this.handleSliderUpdate.bind(this);
        var onToggle = this.handleSwitchToggle.bind(this);
        var onTextChange = this.handleTextUpdate.bind(this);
        var onTextBlur = this.handleTextBlur.bind(this);
        var rangeList = this.props.rangeList;
        var rangeType = this.props.rangeType;
        var minVal = this.props.minVal;
        var maxVal = this.props.maxVal;

        var rList = this.props.rangeList[this.props.colName].rangeList;

        var range = rList.map( function(range) {
            return (<RangeRow 
                        range = { range } 
                        onDelEvent = { onRowDel.bind(this) } 
                        onSlide = { onSlide }
                        onToggle = { onToggle }
                        onTextChange = { onTextChange }
                        onTextBlur = { onTextBlur }
                        key = { range[2] }
                        rangeType = { rangeType }
                        minVal = { minVal }
                        maxVal = { maxVal }
                    />)
        });

        return (
            <div>

                {/* Displays the mapped ranges*/}
                {range}

                {/* Add range button, onRowAdd from the RangeForm Component */}
                <Card>
                    <CardText>

                        <Flex layout="row">
                            <Flex divider />
                            <FontIcon
                                onClick = { this.handleAddEvent.bind(this) }
                                className = "fa fa-plus fa-2x"
                                hoverColor = { blue500 }
                                style = { styleSet.iconStyles }
                            />
                        </Flex>

                    </CardText>
                </Card>
            </div>
        );
    }
}


/**
 * Defines what a row should render as on the DOM
 * Inherits props given to it from RangeTable's render method
 **/
class RangeRow extends React.Component {

    constructor(props) {
        super(props);

        this.state = {
            latestUpdate: ""
        }
    }

    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.range != nextProps.range) {
            return true;
        }
        return false;
    }

    /**
     * Deletes a row from the range table by calling parent delete method
     **/
    onDelEvent() {
        this.props.onDelEvent(this.props.range[2]);
    }

    /**
     * Preprocesses min max vals (fixes the case where min value has 10 and user is trying to type 80, 8 is the first digit and 8 < 10)
     * @param min: the min to be processed 
     * @param max: the max to be processed 
     **/
    arrayNumConversion(min, max) {

        // Make min a valid type
        if (min === "") {
            min = this.props.minVal;
        }
        else {
            min = parseInt(min, 10);
        }

        // Make max a valid type
        if (max === "") {
            max = this.props.maxVal;
        }
        else {
            max = parseInt(max, 10);
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
    onKeyPressMin (e) {
        if(e.key === 'Enter') { 
            this.inputElementMin.blur();
        } 
    }


    /**
     * Loses focus from the max text field when the enter key is pressed
     * @param e: key that was pressed
     **/
    onKeyPressMax (e) {
        if(e.key === 'Enter') { 
            this.inputElementMax.blur();
        } 
    }

    render() {
        return (
            <Card>
                <CardText>
                    <Flex layout="row">      

                        <Flex divider />  

                        <Flex flex="10">
                            <FontIcon
                                onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash fa-2x"
                                hoverColor = { red500 }
                                style = { styleSet.iconStyles }
                            />
                        </Flex>



                        <Flex flex="20">
                            <TextField 
                                type = 'number' 
                                name = "min"
                                ref ={ input => this.inputElementMin = input }
                                id = { this.props.range[2].toString() } 
                                value = { this.props.range[0] } 
                                hintText = { this.props.minVal.toString() }
                                style = { styleSet.textfieldStyles }
                                onChange = {
                                    (e) => this.props.onTextChange(e, this.props.range[2])
                                }
                                onFocus = { () => this.updateLatest("MIN") }
                                onBlur = {
                                    () => this.props.onTextBlur(this.props.range[2], this.props.range[0], this.props.range[1], this.state.latestUpdate)
                                }
                                onKeyPress = {
                                    (e) => this.onKeyPressMin(e)
                                }
                            />
                        </Flex>

                        <Flex divider />
                        <Flex divider />

                        <Flex flex="40"
                            style={{
                                margin: "16px 0px 0px -8px",
                                width: "20px"
                            }}
                        >
                            <Range
                                min = { this.props.minVal }
                                max = { this.props.maxVal }
                                value = {this.arrayNumConversion(this.props.range[0], this.props.range[1]) }
                                defaultValue = { [this.props.minVal,this.props.maxVal] }
                                allowCross = { false }
                                onChange = {
                                    (e) => this.props.onSlide(e, this.props.range[2])
                                }
                            />
                        </Flex>

                        <Flex divider />
                        <Flex divider />

                        <Flex flex="20">
                            <TextField 
                                type = 'number' 
                                name = "max"
                                ref = { input => this.inputElementMax = input }
                                id = { this.props.range[2].toString() } 
                                value = { this.props.range[1] }
                                hintText = { this.props.maxVal.toString() }
                                style = { styleSet.textfieldStyles }
                                onChange = {
                                    (e) => this.props.onTextChange(e, this.props.range[2])
                                }
                                onFocus = { () => this.updateLatest("MAX") }
                                onBlur = {
                                    () => this.props.onTextBlur(this.props.range[2], this.props.range[0], this.props.range[1], this.state.latestUpdate)
                                }
                                onKeyPress = {
                                    (e) => this.onKeyPressMax(e)
                                }
                            />
                        </Flex>

                        <Flex divider />


                        <Flex flex="10"
                            style = {{
                                margin: "11px 0px 0px -11px"
                            }} 
                        >
                            <Toggle 
                                name = "applied" 
                                id = { this.props.range.id } 
                                toggled = { this.props.range[3] } 
                                onToggle = {  
                                    (e) => this.props.onToggle(e, this.props.range[2])
                                }
                            />
                        </Flex>


                    </Flex>
                </CardText>
            </Card>
        );
    }
}


/**
 * Local styling
 **/
const styleSet = {
    iconStyles: {
        fontSize: 26,
        paddingLeft: 10,
        paddingRight: 13,
        margin: "12px 0px 0px -21px"
    },
    textfieldStyles: {
        width: "60px"
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const addRange = (colName, min, max, id, applied) => ({
    type: 'ADD_RANGE',
    colName,
    min,
    max,
    id,
    applied
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
    rangeType
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    rangeList: state.filterState.Filter.Ranges,
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(RangeForm);
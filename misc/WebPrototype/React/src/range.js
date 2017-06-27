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
 * Main Range parent class which gets exported, holds data structure used to map rangeList to the DOM
 * @param minVal: Sets the minimum value allowed for all the rangeList within the range table
 * @param maxVal: Sets the maximum value allowed for all the rangeList within the range table
 * @param colName: "tableName|colName": name of the corresponding table|column for this RangeForm
 * @param rangeType: "slider", "date". Sets what stype of range to display (only slider implemented as of now)
 **/
class RangeForm extends React.Component {
    constructor(props) {
        super(props);
    }


    /**
     * Deletes a range by splicing it out of the store which causes DOM to re-map
     * @param id: ID of the row which is to be deleted
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id));
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
        var minimum = parseInt(this.props.minVal, 10);
        var maximum = parseInt(this.props.maxVal, 10);
        var value = e.target.value;

        if ( !isNaN(value) ) {
            if (e.target.name === "min") {
                if ( value > maximum  ) {
                    this.props.dispatch(updateRange(this.props.colName, maximum, null, id, null));
                }
                else if (value < minimum) {
                    this.props.dispatch(updateRange(this.props.colName, minimum, null, id, null));
                }
                else {
                    this.props.dispatch(updateRange(this.props.colName, value, null, id, null));
                }
            }

            else {
                if ( value > maximum  ) {
                    this.props.dispatch(updateRange(this.props.colName, null, maximum, id, null));
                }
                else if (value < minimum) {
                    this.props.dispatch(updateRange(this.props.colName, null, minimum, id, null));
                }
                else {
                    this.props.dispatch(updateRange(this.props.colName, null, value, id, null));
                }
            }
        }
    };


    /**
     * Updates the visuals of the slider , and the text fields if needed, when a text field loses focus
     * @param e: the 'html' element that caused this method to be called
     * @param rangeID: ID used to find the correct range row to update
     * @param setMin(minVal): A method which updates the row's data in the min text field
     * @param setMax(maxVal): A method which updates the row's data in the max text field
     * @param setSliderMin(minVal): A method which updates the row's min value in the slider
     * @param setSliderMax(maxVal): A method which updates the row's max value in the slider
     * @param cMin: Current value in the min text field
     * @param cMax: Current value in the max text field
     * @returns: 1 if input was a valid number, 0 otherwise (nothing returns 0 yet)
     **/
    handleTextBlur(e, id) {
        var minimum = parseInt(this.props.minVal, 10);
        var maximum = parseInt(this.props.maxVal, 10);
        var value = parseInt(e.target.value, 10);

        this.props.dispatch(updateRange(this.props.colName, e[0], e[1], id, null));

        
    };

    
    /**
     * Updates the visuals of the slider and the text fields when the slider is dragged
     * @param e: the event instance of the slider: array of [min, max]
     * @param id: ID used to find the range in the store
     **/
    handleSliderUpdate(e, id) {
        this.props.dispatch(updateRange(this.props.colName, e[0], e[1], id, null));
    };


    /**
     * Updates the mapping data structure with true false values corresponding to toggle state of a switch
     * @param e: the event instance of the toggle, html element
     * @param id: ID used to find the range in the store
     **/
    handleSwitchToggle(e, id) {
        if (e.target.checked) {
            this.props.dispatch(updateRange(this.props.colName, null, null, id, true));
        }
        else {
            this.props.dispatch(updateRange(this.props.colName, null, null, id, false));
        }
    };


    /**
     * The view for Range Form will display the Rangle Table class
     * Passes data methods as props so that RangeTable has access
     **/
    render() {
        return (
            <div>
                <RangeTable
                    onRowAdd = { this.handleAddEvent.bind(this) } 
                    onRowDel = { this.handleRowDel.bind(this) }
                    onSlide = { this.handleSliderUpdate.bind(this) }
                    onToggle = { this.handleSwitchToggle.bind(this) }
                    onTextChange = { this.handleTextUpdate.bind(this) }
                    rangeList = { this.props.rangeList }
                    minVal = { this.props.minVal }
                    maxVal = { this.props.maxVal }
                    colName = { this.props.colName } 
                />
            </div>
        );
    }
}


/**
 * Displays all the rangeList in table format by mapping the data to range rows
 * Inherits props given to it from RangeForm's render method
 **/
class RangeTable extends React.Component {

    render() {
        // Lose scope of 'this' in the map function so record what you need to access beforehand
        var rowDel = this.props.onRowDel;
        var rowAdd = this.props.onRowAdd;
        var onSlide = this.props.onSlide;
        var onToggle = this.props.onToggle;
        var onTextChange = this.props.onTextChange
        var min = this.props.minVal;
        var max = this.props.maxVal;

        // Maps rows to the DOM and passes data structure methods to rows so they have access

        var rList = this.props.rangeList[this.props.colName].rangeList;

        
        var range = rList.map( function(range) {
            return (<RangeRow 
                        range = { range } 
                        onDelEvent = { rowDel.bind(this) } 
                        onSlide = { onSlide }
                        onToggle = { onToggle }
                        onTextChange = { onTextChange }
                        key = { range[2] } 
                        minVal = { min }
                        maxVal = { max }
                    />)
        });

        return (
            <div>

                {/* Displays all the mapped rangeList */}
                {range}

                {/* Add range button, onRowAdd from the RangeForm Component */}
                <Card>
                    <CardText>

                        <Flex layout="row">
                            <Flex divider />
                            <FontIcon
                                onClick = { this.props.onRowAdd }
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
    /**
     * Deletes a row from the range table by calling parent delete method
     **/
    onDelEvent() {
        this.props.onDelEvent(this.props.range[2]);
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

                        <Flex flex="80">
                            <TextSlider
                                minVal = { this.props.minVal }
                                maxVal = { this.props.maxVal }
                                onSlide = { this.props.onSlide }
                                onTextChange = { this.props.onTextChange }
                                cellData = {{
                                    id: this.props.range[2],
                                    min: this.props.range[0],
                                    max: this.props.range[1]
                                }}
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

                        <Flex divider />

                    </Flex>
                </CardText>
            </Card>
        );
    }
}


/**
 * Custom range slider component that consists of two text fields and a min-max slider
 **/
class TextSlider extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            latestUpdate: ""
        }
    }

    /**
     * Preprocesses min max vals (fixes the case where min value has 10 and user is trying to type 80, 8 is the first digit and 8 < 10)
     * @param min: the min to be processed 
     * @param max: the max to be processed 
     **/
    arrayNumConversion(min, max, minVal, maxVal) {

        // Make min valid type
        if (min == "") {
            min = this.props.minVal;
        }
        else {
            min = parseInt(min);
        }

        // Make max valid type
        if (max == "") {
            max = this.props.maxVal;
        }
        else {
            max = parseInt(max);
        }

        
        // Apply a valid range
        if ( min > max) {
            if (this.state.latestUpdate == "MIN") {
                return [max, max];
            }
            else if (this.state.latestUpdate == "MAX") {
                return [min, min]; 
            }
        }

        else {
            return [min, max];
        }
    }

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
            <Flex layout="row">
                <Flex flex="25">
                    <TextField 
                        type = 'number' 
                        name = "min"
                        ref ={ input => this.inputElementMin = input }
                        id = { this.props.cellData.id.toString() } 
                        value = { this.props.cellData.min } 
                        hintText = { this.props.minVal }
                        style = { styleSet.textfieldStyles }
                        onChange = {
                            (e) => this.props.onTextChange(e, this.props.cellData.id)
                        }
                        onFocus = { () => this.updateLatest("MIN") }
                        onBlur = {
                            (e) => this.onTextBlur(e, this.props.cellData.id, this.props.cellData.min, this.props.cellData.max)
                        }
                        onKeyPress = {
                            (e) => this.onKeyPressMin(e)
                        }
                    />
                </Flex>

                <Flex divider />
                <Flex divider />
                <Flex divider />

                <Flex flex="50"
                    style={{
                        margin: "16px 0px 0px -8px"
                    }}
                >
                    <Range
                        min = { this.props.minVal }
                        max = { this.props.maxVal }
                        value = {this.arrayNumConversion(this.props.cellData.min, this.props.cellData.max) }
                        //value = { [this.props.cellData.min, this.props.cellData.max] }
                        defaultValue = { [this.props.minVal,this.props.maxVal] }
                        allowCross = { false }
                        onChange = {
                            (e) => this.props.onSlide(e, this.props.cellData.id)
                        }
                    />
                </Flex>

                <Flex divider />
                <Flex divider />
                <Flex divider />

                <Flex flex="25">
                    <TextField 
                        type = 'number' 
                        name = "max"
                        ref = { input => this.inputElementMax = input }
                        id = { this.props.cellData.id.toString() } 
                        value = { this.props.cellData.max }
                        hintText = { this.props.maxVal }
                        style = { styleSet.textfieldStyles }
                        onChange = {
                            (e) => this.props.onTextChange(e, this.props.cellData.id)
                        }
                        onFocus = { () => this.updateLatest("MAX") }
                        onBlur = {
                            (e) => this.onTextBlur(e, this.props.cellData.id, this.props.cellData.min, this.props.cellData.max)
                        }
                        onKeyPress = {
                            (e) => this.onKeyPressMax(e)
                        }
                    />
                </Flex>
            </Flex>
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
 * constants defined to make dispatching for the redux store consistent
 **/
export const addRange = (colName, min, max, id, applied) => ({
    type: 'ADD_RANGE',
    colName,
    min,
    max,
    id,
    applied
});
export const removeRange = (colName, id) => ({
    type: 'REMOVE_RANGE',
    colName,
    id
});
export const updateRange = (colName, min, max, id, applied) => ({
    type: 'UPDATE_RANGE',
    colName,
    id,
    min,
    max,
    applied
});
export const highlightElastic = (text) => ({
    type: 'ADD_RANGE',
    text
});
export const unhighlightElastic = (text) => ({
    type: 'ADD_RANGE',
    text
});


/**
 * maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    rangeList: state.filterState.Filter.Ranges,
  }
}


/**
 * connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(RangeForm);
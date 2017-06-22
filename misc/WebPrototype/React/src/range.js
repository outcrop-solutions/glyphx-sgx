import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import {Flex} from 'react-flex-material';
import Range from 'rc-slider/lib/Range'; 
import Global from './Global.js';
import {Card, CardText} from 'material-ui/Card';
import {red500,blue500} from 'material-ui/styles/colors';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.css';
import './range.css';

/**
 * Main Range parent class which gets exported, holds data structure used to map ranges to the DOM
 * @param minVal: Sets the minimum value allowed for all the ranges within the range table
 * @param maxVal: Sets the maximum value allowed for all the ranges within the range table
 * @param colName: "tableName|colName": name of the corresponding table|column for this RangeForm
 * @param rangeType: "slider", "date". Sets what stype of range to display (only slider implemented as of now)
 **/
class RangeForm extends React.Component {
    constructor(props) {
        super(props);

        // Defining data structure used to map ranges to the DOM
        // Starts with an applied default empty range
        this.state = {
            ranges: [
                {
                    id: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
                    min: this.props.minVal.toString(),
                    max: this.props.maxVal.toString(),
                    applied: true,
                }
            ],
            GLOBAL: null
        };
    }


    /**
     * Adds a range to the global instance shared by all components that include Global
     * @param min: local min stored by row
     * @param max: local max stored by row
     **/
    handleAddGlobalRange(min, max) {
        if (min === "") {
            min = 0;
        }
        else if (isNaN(min)) {
            min = this.props.minVal;
        }

        if (max === "") {
            max = 0;
        }
        else if (isNaN(max)) {
            max = this.props.maxVal;
        }

        this.state.GLOBAL.addRange(this.props.colName, [min, max]);
    }


    /**
     * Removes a range to the global instance shared by all components that include Global
     * @param min: local min stored by row
     * @param max: local max stored by row
     **/
    handleRemoveGlobalRange(min, max) {
        if (min === "") {
            min = 0;
        }
        else if (isNaN(min)) {
            min = this.props.minVal;
        }

        if (max === "") {
            max = 0;
        }
        else if (isNaN(max)) {
            max = this.props.maxVal;
        }

        this.state.GLOBAL.removeRange(this.props.colName, [min, max]);
    }


    /**
     * Deletes a range by splicing it out of the main data structure and re-mapping
     * @param range: Range which will be deleted from the range table
     **/
    handleRowDel(range) {
        console.log(range);
        this.handleRemoveGlobalRange(range.min, range.max);
        this.state.ranges.splice(this.state.ranges.indexOf(range), 1);
        this.setState(this.state.ranges);
    };


    /**
     * Adds a range with default values by pushing it into the main data structure
     **/
    handleAddEvent() {
        var range = {
            id: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
            min: this.props.minVal.toString(),
            max: this.props.maxVal.toString(),
            applied: false, 
        }
        this.state.ranges.push(range);
        this.setState(this.state.ranges);
    }


    /**
     * Finds the corresponding range node by ID used to map ranges then updates values
     * @param rangeID: ID used to find the right range row to update
     * @param min: Value used to update min stored in data
     * @param max: Value used to update max stored in data
     **/
    handleDataUpdate(rangeID, min, max) {
        var ranges = this.state.ranges.slice();
        var newRanges = ranges.map( function(range) {
            if (range.id === rangeID) {
                if (min != null) {
                    range["min"] = min;
                }
                if (max != null) {
                    range["max"] = max;
                } 
            }
            return range;
        });
        this.setState( {ranges: newRanges} );
    }


    /**
     * Updates the state for the range row as well as the data in the main range set
     * @param rangeID: ID used to find the correct range row to update
     * @param setMin(minVal): A method which updates the row's data in the min text field
     * @param setMax(maxVal): A method which updates the row's data in the max text field
     * @param cMin: Current value in the min text field
     * @param cMax: Current value in the max text field
     * @returns: 1 if input was a valid number, 0 otherwise
     **/
     handleTextUpdate(e, rangeID, setMin, setMax, cMin, cMax) {
        var minimum = parseInt(this.props.minVal, 10);
        var maximum = parseInt(this.props.maxVal, 10);
        var value = e.target.value;

        if (isNaN(value) || value === "") {
            if (e.target.name === "min") {
                setMin(value);
                this.handleDataUpdate(rangeID, value, null);
                return 0;
            }

            else {
                setMax(value);
                this.handleDataUpdate(rangeID, null, value);
                return 0;
            }
        }

        else {
            value = parseInt(value, 10);
            if (e.target.name === "min") {
                if (minimum > value) {
                    setMin(minimum);
                    this.handleDataUpdate(rangeID, minimum, null);
                    return 1;
                }

                else if (value > maximum) {
                    setMin(maximum);
                    this.handleDataUpdate(rangeID, maximum, null);
                    return 1;
                }

                setMin(value);
                this.handleDataUpdate(rangeID, value, null);
                return 1;
            }

            else {
                if (maximum < value) {
                    setMax(maximum);
                    this.handleDataUpdate(rangeID, null, maximum);
                    return 1;
                }

                else if (value < minimum) {
                    setMax(minimum);
                    this.handleDataUpdate(rangeID, null, minimum);
                    return 1;
                }

                setMax(value);
                this.handleDataUpdate(rangeID, null, value);
                return 1;
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
    handleTextBlur(e, rangeID, setMin, setMax, setSliderMin, setSliderMax, cMin, cMax) {
        var minimum = parseInt(this.props.minVal, 10);
        var maximum = parseInt(this.props.maxVal, 10);
        var value = parseInt(e.target.value, 10);

        if (e.target.name === "min") {
            if (isNaN(cMax) || cMax === "") {
                if (value > maximum) {
                    setMin(maximum);
                    setSliderMin(maximum);
                    this.handleDataUpdate(rangeID, maximum, null);
                    return 1;
                }
            }

            else if (isNaN(cMax) === false) {
                cMax = parseInt(cMax, 10);
                if (value > cMax) {
                    setMin(cMax);
                    setSliderMin(cMax);
                    this.handleDataUpdate(rangeID, cMax, null);
                    return 1;
                }
            }
            setSliderMin(value);
            this.handleDataUpdate(rangeID, value, null);
        }

        else {
            if (isNaN(cMin) || cMin === "") {
                if (value < minimum) {
                    setMax(minimum);
                    setSliderMax(minimum);
                    this.handleDataUpdate(rangeID, null, minimum);
                    return 1;
                }
            }

            else if (isNaN(cMin) === false) {
                cMax = parseInt(cMin, 10);
                if (value < cMin) {
                    setMax(cMin);
                    setSliderMax(cMin);
                    this.handleDataUpdate(rangeID, null, cMin);
                    return 1;
                }
            }
            setSliderMax(value);
            this.handleDataUpdate(rangeID, null, value);
        }
    };

    
    /**
     * Updates the visuals of the slider and the text fields when the slider is dragged
     * @param e: the 'html' element that caused this method to be called
     * @param rangeID: ID used to find the correct range row to update
     * @param setMin(minVal): A method which updates the row's data in the min text field
     * @param setMax(maxVal): A method which updates the row's data in the max text field
     * @param setSliderMin(minVal): A method which updates the row's min value in the slider
     * @param setSliderMax(maxVal): A method which updates the row's max value in the slider
     **/
    handleSliderUpdate(e, rangeID, setMin, setMax, setSliderMin, setSliderMax) {
        setMin(e[0]);
        setMax(e[1]);
        setSliderMin(e[0]);
        setSliderMax(e[1]);
        this.handleDataUpdate(rangeID, e[0], e[1]);
    };


    /**
     * Updates the mapping data structure with true false values corresponding to toggle state of a switch
     * @param e: the 'html' element that caused this method to be called
     **/
    handleSwitchToggle(e) {
        var ranges = this.state.ranges.slice();
        var newRanges = ranges.map( function(range) {
            if (range.id === e.target.id) {
                range["applied"] = e.target.checked;
            }
            return range;
        });

        // Temporary print to view mapping data when the toggle is switched
        console.log(ranges);
        this.setState( {ranges: newRanges} );
    };


    /**
     * The view for Range Form will display the Rangle Table class
     * Passes data methods as props so that RangeTable has access
     **/
    render() {
        return (
            <div>
                <Global ref={(inst) => function(inst){
                    this.setState({GLOBAL: inst});
                    
                    }} />

                <RangeTable 
                    onTextUpdate = { this.handleTextUpdate.bind(this) } 
                    onTextBlur = { this.handleTextBlur.bind(this) }
                    onSlider = { this.handleSliderUpdate.bind(this) } 
                    onToggle = { this.handleSwitchToggle.bind(this) }
                    onRowAdd = { this.handleAddEvent.bind(this) } 
                    onRowDel = { this.handleRowDel.bind(this) } 
                    ranges = { this.state.ranges }
                    minVal = { this.props.minVal }
                    maxVal = { this.props.maxVal }
                />
            </div>
        );
    }
}


/**
 * Displays all the ranges in table format by mapping the data to range rows
 * Inherits props given to it from RangeForm's render method
 **/
class RangeTable extends React.Component {

    render() {
        // Lose scope of 'this' in the map function so record what you need to access beforehand
        var onTextUpdate = this.props.onTextUpdate;
        var onTextBlur = this.props.onTextBlur;
        var onSlider = this.props.onSlider;
        var onToggle = this.props.onToggle;
        var rowDel = this.props.onRowDel;
        var min = this.props.minVal;
        var max = this.props.maxVal;

        // Maps rows to the DOM and passes data structure methods to rows so they have access
        var range = this.props.ranges.map( function(range) {
            return (<RangeRow 
                        onTextUpdate = { onTextUpdate }
                        onTextBlur = { onTextBlur }
                        onSlider = { onSlider }
                        onToggle = { onToggle }
                        range = { range } 
                        onDelEvent = { rowDel.bind(this) } 
                        key = { range.id } 
                        minVal = { min }
                        maxVal = { max }
                    />)
        });

        return (
            <div>

                {/* Displays all the mapped ranges */}
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
        this.props.onDelEvent(this.props.range);
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
                                onTextUpdate = { this.props.onTextUpdate }
                                onTextBlur = { this.props.onTextBlur }
                                onSlider = { this.props.onSlider }
                                cellData = {{
                                    id: this.props.range.id,
                                    minValue: this.props.range.min,
                                    maxValue: this.props.range.max
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
                                toggled={ this.props.range.applied } 
                                onToggle={this.props.onToggle} 
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
            //min: this.props.minVal,
            //max: this.props.maxVal

            min: "",
            max: "",
            sliderMin: "",
            sliderMax: ""
        }
    }


    /**
     * Updates the local state value linked to the min text field
     * @param minVal: the value used to update
     **/
    updateMin(minVal) {
        this.setState({min: minVal});
    }


    /**
     * Updates the local state value linked to the max text field
     * @param maxVal: the value used to update
     **/
    updateMax(maxVal) {
        this.setState({max: maxVal});
    }


    /**
     * Updates the local state value linked to the min slider value
     * @param minVal: the value used to update
     **/
    updateSliderMin(minVal) {
        this.setState({sliderMin: minVal});
    }


    /**
     * Updates the local state value linked to the max slider value
     * @param maxVal: the value used to update
     **/
    updateSliderMax(maxVal) {
        this.setState({sliderMax: maxVal});
    }

    /**
     * Interprets data into a format that the slider can always accept without causing error
     * @param array:[val1,val2]: val1 corresponds to min position and val2 for max position
     **/
    arrayNumConversion(array) {
        if ( (isNaN(array[0]) && isNaN(array[1])) || (array[0] === "" && isNaN(array[1])) || (isNaN(array[0]) && array[1] === "") || (array[0] === "" && array[1] === "") ) {
            return [parseInt(this.props.minVal, 10), parseInt(this.props.maxVal, 10)];
        }
        else if ( isNaN(array[0]) || array[0] === "" ) {
            return [parseInt(this.props.minVal, 10), parseInt(array[1], 10)];
        }
        else if ( isNaN(array[1]) || array[1] === "" )  {
            return [parseInt(array[0], 10), parseInt(this.props.maxVal, 10)];
        }
        else {
            return [parseInt(array[0], 10), parseInt(array[1], 10)];
        }
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
                        type='number' 
                        name="min"
                        ref={input => this.inputElementMin = input}
                        id={this.props.cellData.id.toString()} 
                        value={this.state.min} 
                        onBlur={
                            (e) => this.props.onTextBlur(e, this.props.cellData.id, this.updateMin.bind(this), this.updateMax.bind(this), 
                                                            this.updateSliderMin.bind(this), this.updateSliderMax.bind(this),
                                                            this.state.min, this.state.max)
                        }
                        onChange={
                            (e) => this.props.onTextUpdate(e, this.props.cellData.id, this.updateMin.bind(this), 
                                                            this.updateMax.bind(this), this.state.min, this.state.max)
                        }
                        onKeyPress={ (e) => this.onKeyPressMin(e) }
                        hintText={this.props.minVal}
                        style = { styleSet.textfieldStyles }
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
                        min={this.props.minVal}
                        max={this.props.maxVal}
                        value={this.arrayNumConversion([this.state.sliderMin, this.state.sliderMax])}
                        defaultValue={[this.props.minVal,this.props.maxVal]}
                        allowCross={false}
                        onChange={(e) => this.props.onSlider(e, this.props.cellData.id, this.updateMin.bind(this), this.updateMax.bind(this), 
                                            this.updateSliderMin.bind(this), this.updateSliderMax.bind(this)) 
                        }
                    />
                </Flex>

                <Flex divider />
                <Flex divider />
                <Flex divider />

                <Flex flex="25">
                    <TextField 
                        type='number' 
                        name="max"
                        ref={input => this.inputElementMax = input}
                        id={this.props.cellData.id.toString()} 
                        value={this.state.max}
                        onBlur={
                            (e) => this.props.onTextBlur(e, this.props.cellData.id, this.updateMin.bind(this), this.updateMax.bind(this), 
                                                            this.updateSliderMin.bind(this), this.updateSliderMax.bind(this),
                                                            this.state.min, this.state.max)
                        }
                        onChange={
                            (e) => this.props.onTextUpdate(e, this.props.cellData.id, this.updateMin.bind(this), 
                                                            this.updateMax.bind(this), this.state.min, this.state.max)
                        }
                        onKeyPress={
                            (e) => this.onKeyPressMax(e)
                        }
                        hintText={this.props.maxVal}
                        style = { styleSet.textfieldStyles }
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


export default RangeForm;
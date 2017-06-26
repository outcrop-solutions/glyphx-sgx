import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import {Flex} from 'react-flex-material';
import Range from 'rc-slider/lib/Range'; 
import Global from './Global.js';
import {Card, CardText} from 'material-ui/Card';
import {red500,blue500} from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.css';
import './range.css';


const mapStateToProps = function(state){
  return {
    rangeList: state.filterState.Filter.Ranges,
  }
}

//this.props.dispatch(addRange(this.props.colName));
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

export const updateRange = (colName, id, min, max, applied) => ({
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
 * Main Range parent class which gets exported, holds data structure used to map rangeList to the DOM
 * @param minVal: Sets the minimum value allowed for all the rangeList within the range table
 * @param maxVal: Sets the maximum value allowed for all the rangeList within the range table
 * @param colName: "tableName|colName": name of the corresponding table|column for this RangeForm
 * @param rangeType: "slider", "date". Sets what stype of range to display (only slider implemented as of now)
 **/
class RangeForm extends React.Component {
    constructor(props) {
        super(props);

        // Defining data structure used to map rangeList to the DOM
        // Starts with an applied default empty range
        this.state = {
            /*
            rangeList: {
                'colName1': {
                    //ranges: [['min','max','id', 'applied']],
                    ranges: [[-10, 50, 1, true] ],
                },
                'colName2': {
                    //ranges: [['min','max','id', 'applied']],
                    ranges: [[-20, 60, 2, false] ],
                },
                'colName3': {
                    //ranges: [['min','max','id', 'applied']],
                    ranges: [[-30, 70, 3, true] ],
                }
            },
            */


            GLOBAL: null
        };

        /*
        store.subscribe(() => {
            this.setState({
                rangeList: store.getState().Filter.rangeList
            });
        });
        */
    }


    /**
     * Deletes a range by splicing it out of the main data structure and re-mapping
     * @param range: -----Range which will be deleted from the range table
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id));
    };


    /**
     * Adds a range with default values by pushing it into the main data structure
     **/
    handleAddEvent() {
        this.props.dispatch(addRange(this.props.colName, this.props.minVal, this.props.maxVal, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false));
    }


    /**
     * Finds the corresponding range node by ID used to map rangeList then updates values
     * @param rangeID: ID used to find the right range row to update
     * @param min: Value used to update min stored in data
     * @param max: Value used to update max stored in data
     **/
    handleDataUpdate(rangeID, min, max) {

        //HANDLENEW UPDATE

    }


    /**
     * Gets the toggle state of a row
     * @param rangeID: ID used to find the range row
     * @returns: true if row is toggled on, false otherwise
     **/
    getToggleState(rangeID) {
        //HANDLENEW TOGGLE

        for (var i = 0; i < this.state.rangeList.length; i ++) {
            if (this.state.rangeList[i].id === rangeID) {
                return this.state.rangeList[i].applied;
            }
        }
        // ID was not found
        return -1;
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

        //HANDLENEW UPDATE
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

        //HANDLENEW UPDATE
        var minimum = parseInt(this.props.minVal, 10);
        var maximum = parseInt(this.props.maxVal, 10);
        var value = parseInt(e.target.value, 10);

        if (e.target.name === "min") {
            if (isNaN(cMax) || cMax === "") {
                if (value > maximum) {
                    setMin(maximum);
                    setSliderMin(maximum);
                    this.handleDataUpdate(rangeID, maximum, null);

                    if (this.getToggleState(rangeID)) {
                        this.handleAddGlobalRange(maximum, maximum, rangeID);
                    }
                    return 1;
                }
            }

            else if (isNaN(cMax) === false) {
                cMax = parseInt(cMax, 10);
                if (value > cMax) {
                    setMin(cMax);
                    setSliderMin(cMax);
                    this.handleDataUpdate(rangeID, cMax, null);

                    if (this.getToggleState(rangeID)) {
                        this.handleAddGlobalRange(cMax, cMax, rangeID);
                    }
                    return 1;
                }
            }
            setSliderMin(value);
            this.handleDataUpdate(rangeID, value, null);

            if (this.getToggleState(rangeID)) {
                this.handleAddGlobalRange(value, cMax, rangeID);
            }
        }

        else {
            if (isNaN(cMin) || cMin === "") {
                if (value < minimum) {
                    setMax(minimum);
                    setSliderMax(minimum);
                    this.handleDataUpdate(rangeID, null, minimum);

                    if (this.getToggleState(rangeID)) {
                        this.handleAddGlobalRange(minimum, minimum, rangeID);
                    }
                    return 1;
                }
            }

            else if (isNaN(cMin) === false) {
                cMax = parseInt(cMin, 10);
                if (value < cMin) {
                    setMax(cMin);
                    setSliderMax(cMin);
                    this.handleDataUpdate(rangeID, null, cMin);

                    if (this.getToggleState(rangeID)) {
                        this.handleAddGlobalRange(cMin, cMin, rangeID);
                    }
                    return 1;
                }
            }
            setSliderMax(value);
            this.handleDataUpdate(rangeID, null, value);

            if (this.getToggleState(rangeID)) {
                this.handleAddGlobalRange(cMin, value, rangeID);
            }
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

        //HANDLENEW

        setMin(e[0]);
        setMax(e[1]);
        setSliderMin(e[0]);
        setSliderMax(e[1]);
        this.handleDataUpdate(rangeID, e[0], e[1]);
    };


    /**
     * Removes a range from the global instance when slider is initially clicked (before change)
     * Allows us to avoid tracking previous states
     * @param e: the 'html' element that caused this method to be called
     * @param rangeID: ID used to find the range to remove
     **/
    handleSliderClick(e, rangeID) {
        if (this.getToggleState(rangeID)) {
            this.handleRemoveGlobalRange(rangeID);
        }
    }


    /**
     * adds a range to the global instance when a slider is let go
     * @param e: the 'html' element that caused this method to be called
     * @param rangeID: ID added to track the range when removing at a later time
     **/
    handleSliderLetGo(e, rangeID) {
        if (this.getToggleState(rangeID)) {
            this.handleAddGlobalRange(e[0], e[1], rangeID);
        }
    }
    


    /**
     * Updates the mapping data structure with true false values corresponding to toggle state of a switch
     * @param e: the 'html' element that caused this method to be called
     **/
    handleSwitchToggle(e) {
        var rangeList = this.state.rangeList.slice();
        var min, max, id;
        var newrangeList = rangeList.map( function(range) {
            if (range.id === e.target.id) {
                range["applied"] = e.target.checked;
                min = range["min"];
                max = range["max"];
                id = range["id"];
            }
            return range;
        });

        if (e.target.checked) {
            this.handleAddGlobalRange(min, max, id);
        }
        else {
            this.handleRemoveGlobalRange(id);
        }

        // Temporary print to view mapping data when the toggle is switched
        console.log(rangeList);
        this.setState( {rangeList: newrangeList} );
    };


    /**
     * The view for Range Form will display the Rangle Table class
     * Passes data methods as props so that RangeTable has access
     **/
    render() {
        return (
            <div>
                <Global ref={inst => this.state.GLOBAL = inst} />

                <RangeTable
                    onRowAdd = { this.handleAddEvent.bind(this) } 
                    onRowDel = { this.handleRowDel.bind(this) } 
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
        var min = this.props.minVal;
        var max = this.props.maxVal;

        // Maps rows to the DOM and passes data structure methods to rows so they have access

        var rList = this.props.rangeList[this.props.colName].rangeList;

        
        var range = rList.map( function(range) {
            return (<RangeRow 
                        range = { range } 
                        onDelEvent = { rowDel.bind(this) } 
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
                                cellData = {{
                                    id: this.props.range[2],
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
            sliderMax: "",
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
                        type = 'number' 
                        name = "min"
                        ref ={ input => this.inputElementMin = input }
                        id = { this.props.cellData.id.toString() } 
                        //value = { this.state.min } 
                        hintText = { this.props.minVal }
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
                        min = { this.props.minVal }
                        max = { this.props.maxVal }
                        //value = { this.arrayNumConversion([this.state.sliderMin, this.state.sliderMax]) }
                        defaultValue = { [this.props.minVal,this.props.maxVal] }
                        allowCross = { false }
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
                        //value = { this.state.max }
                        hintText = { this.props.maxVal }
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

export default connect(mapStateToProps)(RangeForm);
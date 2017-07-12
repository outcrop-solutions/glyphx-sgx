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


/**
 * Main Range parent class which gets exported
 * @param minVal: Sets the minimum value allowed for all the rangeList within the range table
 * @param maxVal: Sets the maximum value allowed for all the rangeList within the range table
 * @param colName: Name of the corresponding column for this RangeForm
 * @param data: array of values from the eleastic table for the corresponding colName
 **/
class NumberRangeTable extends React.Component {
    /**
     * Deletes a range by splicing it out of the store
     * @param id: ID of the row which is to be deleted
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id, this.props.data, "Number"));
    };


    /**
     * Adds a range with the default values of the current min being set to the minimum value, current max being set to the maximum value,
     * a new generated ID, and the applied switch on off.
     **/
    handleAddEvent() {
        this.props.dispatch(addRange(this.props.colName, this.props.minVal, this.props.maxVal, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false));
    }


    /**
     * Updates a range row in the store (null values may be passed to min, max, or applied to peform no update on that field)
     * @param id: ID used to find the range in the store
     * @param min: Min value to update
     * @param max: Max value to update
     * @param applied: Applied status to update
     **/
    handleStoreUpdate(id, min, max, applied, previousRange) { 
        this.props.dispatch(updateRange(this.props.colName, min, max, id, applied, this.props.data, "Number", previousRange));
    }


    /**
     * Maps ranges to DOM
     **/
    render() {
        var onRowDel = this.handleRowDel.bind(this);
        var updateStore = this.handleStoreUpdate.bind(this);
        var minVal = this.props.minVal;
        var maxVal = this.props.maxVal;

        var rList = this.props.rangeList[this.props.colName].rangeList;

        var range = rList.map( function(range) {
            return (<NumberRangeRow 
                        range = { range } 
                        onDelEvent = { onRowDel.bind(this) } 
                        updateStore = { updateStore }
                        key = { range[2] }
                        minVal = { minVal }
                        maxVal = { maxVal }
                    />)
        });

        return (
            <div>

                {/* Displays the mapped ranges */}
                {range}

                {/* Add range button */}
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
 * Inherits props from RangeTable
 **/
class NumberRangeRow extends React.Component {

    constructor(props) {
        super(props);

        this.state = {
            latestUpdate: "",
            epoch: 0,
            min: this.props.range[0],
            max: this.props.range[1],
        }
    }

    
    /**
     * Tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.range != nextProps.range) {
            return true;
        }
        if (this.state.min !== nextState.min || this.state.max !== nextState.max) {
            return true;
        }
        return false;
    };
    

    /**
     * Updates the state when the component gets new props from the store
     * @param nextProps: The props the component would have after the change
     **/
    componentWillReceiveProps(nextProps) {
        this.setState({ min: nextProps.range[0], max: nextProps.range[1]});
    };


    /**
     * Updates the visuals of the slider and the text fields when the slider is dragged
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
        this.props.updateStore(this.props.range[2], e[0], e[1], true, this.props.range);
    };

    /**
     * Updates state values of min and max based on text field input
     * @param e: the event instance of the text field, html element
     **/
     onTextChange(e) {
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
    };


    /**
     * Updates the text fields if (min > max) and updates the store
     **/
    onTextBlur() {

        var min = this.state.min;
        var max = this.state.max;

        if (min === "") {
            this.setState({ min: this.props.minVal });
            min = this.props.minVal;
        }

        if (max === "") {
            this.setState({ max: this.props.maxVal });
            max = this.props.maxVal;
        }

        if (min > max) {

            if (this.state.latestUpdate === "MIN") {
                this.setState({ min: max });
                min = max;
            }

            else if (this.state.latestUpdate === "MAX") {
                this.setState({ max: min });
                max = min;
            }
        }

        this.props.updateStore(this.props.range[2], min, max, true);
    };


    /**
     * Updates toggle state in store but only once every 300 miliseconds (prevents a queue of actions on the store)
     * @param e: the event instance of the toggle, html element
     **/
    onToggle(e) {
        var epoch = (new Date()).getTime();

        if (epoch > this.state.epoch + 300) {
            if (e.target.checked) {
                this.props.updateStore(this.props.range[2], null, null, true);
            }
            else {
                this.props.updateStore(this.props.range[2], null, null, false);
            }
            this.setState({ epoch: epoch });
        }        
    }


    /**
     * Deletes a row from the range table by calling parent delete method
     **/
    onDelEvent() {
        this.props.onDelEvent(this.props.range[2]);
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
                            />
                        </Flex>

                        <Flex divider />
                        <Flex divider />

                        <Flex flex="40"
                            style = {{
                                margin: "16px 0px 0px -8px",
                                width: "20px"
                            }}
                        >
                            <Range
                                min = { this.props.minVal }
                                max = { this.props.maxVal }
                                value = {this.arrayNumConversion(this.state.min, this.state.max) }
                                defaultValue = { [this.props.minVal,this.props.maxVal] }
                                allowCross = { false }
                                onChange = {
                                    (e) => this.onSlide(e)
                                }
                                onAfterChange = {
                                    (e) => this.onAfterSlide(e)
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
                                    (e) => this.onToggle(e)
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
export const updateRange = (colName, min, max, id, applied, data, rangeType, previousRange) => ({
    type: 'UPDATE_RANGE',
    colName,
    id,
    min,
    max,
    applied, 
    data,
    rangeType,
    previousRange
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    rangeList: state.filterState.Filter,
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(NumberRangeTable);
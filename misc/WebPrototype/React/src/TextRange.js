import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import { Flex } from 'react-flex-material';
import Range from 'rc-slider/lib/Range';
import DropDownMenu from 'material-ui/DropDownMenu';
import MenuItem from 'material-ui/MenuItem';
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.css';

// Used to translate slider values to letters
const alphabet = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];

/**
 * @param colName: Name of the corresponding column for this RangeForm
 * @param data: array of values from the eleastic table for the corresponding colName
 **/
class TextRangeTable extends React.Component {


    /**
     * Deletes a range from the store
     * @param id: ID of the row which is to be deleted
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id, this.props.data, "Text"));
    };


    /**
     * Adds a range with the default values: [0, 25, generated ID, false, 1, ""]
     **/
    handleAddEvent() {
        this.props.dispatch(addTextRange(this.props.colName, 1, 0, 25, "", ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false));
    };


    /**
     * Updates a range row in the store (null values may be passed to min, max, or applied to peform no update on that field)
     * @param id: ID used to find the range in the store
     * @param min: Min value to update
     * @param max: Max value to update
     * @param applied: Applied status to update
     * @param selectType: Select Type to update
     * @param text: Text input to update
     **/
    handleStoreUpdate(min, max, id, applied, selectType, text) { 
        this.props.dispatch(updateRange(this.props.colName, selectType, min, max, text, id, applied, this.props.data, "Text"));
    };


    /**
     * Maps ranges to DOM
     **/
    render() {
        var onRowDel = this.handleRowDel.bind(this);
        var updateStore = this.handleStoreUpdate.bind(this);
        var minVal = this.props.minVal;
        var maxVal = this.props.maxVal;
        var settings = this.props.settings;

        var rList = this.props.filterList[this.props.colName].rangeList;

        var range = rList.map( function(range) {
            return (<TextRangeRow 
                        range = { range } 
                        onDelEvent = { onRowDel.bind(this) } 
                        updateStore = { updateStore }
                        key = { range[2] }
                        minVal = { minVal }
                        maxVal = { maxVal }
                        settings = { settings }
                    />)
        });


        return (
            <div>
                {/* Displays the mapped ranges */}
                {range}

                {/* Add range button below*/}
                <Card>
                    <CardText>
                        <Flex layout="row">
                            <Flex divider />
                            <FontIcon
                                onClick = { this.handleAddEvent.bind(this) }
                                className = "fa fa-plus fa-2x"
                                hoverColor = { this.props.settings.rangeColor.addHover }
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
 * Inherits props from TextRangeTable
 **/
class TextRangeRow extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            epoch: 0,
            value: this.props.range[4],
            latestUpdate: "",
            min: this.props.range[0],
            max: this.props.range[1],
            text: this.props.range[5],
        }
    }

    
    /**
     * Tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.range != nextProps.range || this.props.settings != nextProps.settings) {
            return true;
        }
        if (this.state.min != nextState.min || this.state.max != nextState.max || this.state.text != nextState.text) {
            return true;
        }
        return false;
    };
    

    /**
     * Updates the state when the component gets new props from the store
     * @param nextProps: The props the component would have after the change
     **/
    componentWillReceiveProps(nextProps) {
        this.setState({ min: nextProps.range[0], max: nextProps.range[1], value: nextProps.range[4], text: nextProps.range[5] });
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
        this.props.updateStore(e[0], e[1], this.props.range[2], null, null, null, this.props.range);
    };


    /**
     * Updates local state values of min and max based on text field input
     * @param e: the event instance of the text field, html element
     **/
     onSliderTextChange(e) {
         if (e.target.value.length <= 1) {
             if (e.target.value == "") {
                 if (e.target.name == "min") {
                    this.setState({ min: "" });
                 }
                 else {
                     this.setState({ max: "" });
                 }
             }

             var value = e.target.value.toUpperCase();
             var index = alphabet.indexOf(value);
             if (index != -1) {
                 if (e.target.name == "min") {
                    this.setState({ min: index });
                 }
                 else {
                     this.setState({ max: index });
                 }
             }
         }
    };


    /**
     * Updates the text fields if (min > max) and updates the store
     **/
    onSliderTextBlur() {
        var min = this.state.min;
        var max = this.state.max;

        if (this.state.min === "") {
            min = 0;
        }
        if (this.state.max === "") {
            max = 25;
        }

        if (min > max) {
            if (this.state.latestUpdate === "MIN") {
                min = max;
            }

            else if (this.state.latestUpdate === "MAX") {
                max = min;
            }
        }
        this.props.updateStore(min, max, this.props.range[2], null, null, null, this.props.range);
    };


    /**
     * Preprocesses min max vals, allows min > max while typing
     * @param min: the min to be processed 
     * @param max: the max to be processed 
     **/
    arrayTextConversion(min, max) {
        if (min === "" && max === "") {
            return [0, 25];
        }

        else if (min === "") {
            return [0, parseInt(max, 10)];
        }

        else if (max === "") {
            return [parseInt(min, 10), 25];
        }

        else if (min > max) {
             if (this.state.latestUpdate === "MIN") {
                return [max, max];
            }

            else if (this.state.latestUpdate === "MAX") {
                return [parseInt(min, 10), parseInt(min, 10)]; 
            }
        }

        else {
            return [parseInt(min, 10), parseInt(max, 10)];
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
        if (e.key === 'Enter') { 
            this.inputElementMin.blur();
        } 
    }


    /**
     * Loses focus from the max text field when the enter key is pressed
     * @param e: key that was pressed
     **/
    onKeyPressMax(e) {
        if (e.key === 'Enter') { 
            this.inputElementMax.blur();
        } 
    }


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
                    min = 0;
                }
                if (this.state.max == "") {
                    max = 25;
                }
                this.props.updateStore(min, max, this.props.range[2], true, null, null);
            }
            else {
                this.props.updateStore(null, null, this.props.range[2], false, null, null);
            }
            this.setState({ epoch: epoch });
        }        
    }


    /**
     * Deletes a row from the range table
     **/
    onDelEvent() {
        this.props.onDelEvent(this.props.range[2]);
    };


    /**
     * Updates local state when text changes
     * @param e: the event instance of the text field, html element
     **/
     onTextChange(e) {
        this.setState({ text: e.target.value });
    };


    /**
     * Pushes local state to store on blur
     **/
    onTextBlur() {
        // [min, max, id, applied, selectType, text]
        this.props.updateStore(null, null, this.props.range[2], null, null, this.state.text, this.props.range);
    };
    

    /**
     * Handles change of drop-down choice
     **/
    promptSelectChange = (event, index, value) => this.handleSelectChange(event, index, value);
    handleSelectChange(event, index, value) {
        this.setState({ value: value });
        this.props.updateStore("", "", this.props.range[2], false, value, "", this.props.range);
    }

    render() {
        return (
            <Card>
                <CardText>
                    <Flex layout = "row">      

                        <Flex divider />  

                        <Flex flex = "1">
                            <FontIcon
                                onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash fa-2x"
                                hoverColor = { this.props.settings.rangeColor.deleteHover }
                                style = { styleSet.iconStyles }
                            />
                        </Flex>

                        <Flex flex = "34" style = {{ width: "132px", margin: "-8px 0px 0px -19px" }}>
                            <DropDownMenu 
                                value = { this.state.value } 
                                onChange = { this.promptSelectChange } 
                                iconStyle = {{ fill: this.props.settings.rangeColor.text}}
                                //underlineStyle = {{ borderColor: this.props.settings.rangeColor.text }}
                                selectedMenuItemStyle = {{ backgroundColor: this.props.settings.rangeColor.selectedBackground, color: this.props.settings.rangeColor.selectedText}}
                            >
                                <MenuItem value = {1} label = "Contains" primaryText = "Contains" />
                                <MenuItem value = {2} label = "Not Contain" primaryText = "Does Not Contain" />
                                <MenuItem value = {3} label = "Begins With" primaryText = "Begins With" />
                                <MenuItem value = {4} label = "Ends With" primaryText = "Ends With" />
                                <MenuItem value = {5} label = "Begins [R]" primaryText = "Begins With [Range]" />
                                <MenuItem value = {6} label = "Ends [R]" primaryText = "Ends With [Range]" />        
                            </DropDownMenu>
                        </Flex>

                        <Flex divider />

                        <Flex flex = "55">
                            {this.state.value === 5 || this.state.value === 6 ? 
                                        <Flex layout = "row">
                                            <Flex flex = "25">
                                                <TextField 
                                                    type = 'text' 
                                                    name = "min"
                                                    ref = { input => this.inputElementMin = input }
                                                    value = { this.state.min === "" ? "" : alphabet[this.state.min] } 
                                                    hintText = 'A'
                                                    style = { styleSet.textfieldStyles }
                                                    underlineFocusStyle = {{ borderColor: this.props.settings.rangeColor.textFieldUnderline }}
                                                    onChange = {
                                                        (e) => this.onSliderTextChange(e)
                                                    }
                                                    onFocus = { () => this.updateLatest("MIN") }
                                                    onBlur = {
                                                        () => this.onSliderTextBlur()
                                                    }
                                                    onKeyPress = {
                                                        (e) => this.onKeyPressMin(e)
                                                    }
                                                />
                                            </Flex>

                                            <Flex divider />
                                            <Flex divider />

                                            <Flex flex = "50"
                                                style = {{
                                                    margin: "16px 0px 0px -8px",
                                                    width: "20px"
                                                }}
                                            >
                                                <Range
                                                    min = { 0 }
                                                    max = { 25 }
                                                    value = { this.arrayTextConversion(this.state.min, this.state.max) }
                                                    defaultValue = { [0, 25] }
                                                    allowCross = { false }
                                                    onChange = {
                                                        (e) => this.onSlide(e)
                                                    }
                                                    onAfterChange = {
                                                        (e) => this.onAfterSlide(e)
                                                    }
                                                    trackStyle = { [{ backgroundColor: this.props.settings.rangeColor.sliderTrack }] }
                                                    handleStyle = {[{ backgroundColor: this.props.settings.rangeColor.sliderCircle, borderColor: this.props.settings.rangeColor.sliderCircle }, { backgroundColor: this.props.settings.rangeColor.sliderCircle, borderColor: this.props.settings.rangeColor.sliderCircle }]}
                                                />
                                            </Flex>

                                            <Flex divider />
                                            <Flex divider />

                                            <Flex flex = "25">
                                                <TextField 
                                                    type = 'text' 
                                                    name = "max"
                                                    ref = { input => this.inputElementMax = input }
                                                    value = { this.state.max === "" ? "" : alphabet[this.state.max] }
                                                    hintText = 'Z'
                                                    style = { styleSet.textfieldStyles }
                                                    underlineFocusStyle = {{ borderColor: this.props.settings.rangeColor.textFieldUnderline }}
                                                    onChange = {
                                                        (e) => this.onSliderTextChange(e)
                                                    }
                                                    onFocus = { () => this.updateLatest("MAX") }
                                                    onBlur = {
                                                        () => this.onSliderTextBlur()
                                                    }
                                                    onKeyPress = {
                                                        (e) => this.onKeyPressMax(e)
                                                    }
                                                />
                                            </Flex>
                                        </Flex>
                                        
                                        : 

                                        <TextField 
                                            type = 'text' 
                                            name = "min"
                                            ref = { input => this.inputElementMin = input }
                                            value = { this.state.text } 
                                            style = {{
                                                width: "150px"
                                            }}
                                            underlineFocusStyle = {{ borderColor: this.props.settings.rangeColor.textFieldUnderline }}
                                            hintText = "Letter or Phrase"
                                            onChange = {
                                                (e) => this.onTextChange(e)
                                            }
                                            onBlur = {
                                                () => this.onTextBlur()
                                            }
                                            onKeyPress = {
                                                (e) => this.onKeyPressMin(e)
                                            }
                                        /> 
                            }
                        </Flex>

                        <Flex divider />

                        <Flex flex = "10"
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
                                trackSwitchedStyle = {{ backgroundColor: this.props.settings.rangeColor.toggleTrack }}
                                thumbSwitchedStyle = {{ backgroundColor: this.props.settings.rangeColor.toggleCircle }}
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
        width: "25px"
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const addTextRange = (colName, selectType, min, max, text, id, applied) => ({
    type: 'ADD_TEXT_RANGE',
    colName,
    selectType,
    min,
    max,
    text,
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
export const updateRange = (colName, selectType, min, max, text, id, applied, data, rangeType) => ({
    type: 'UPDATE_RANGE',
    colName,
    selectType,
    id,
    min,
    max,
    text,
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
    filterList: state.filterState.Filter,
    settings: state.filterState.Settings
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(TextRangeTable);
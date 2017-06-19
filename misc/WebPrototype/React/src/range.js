import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
import {Flex} from 'react-flex-material';
import Range from 'rc-slider/lib/Range'; 
import {Card, CardText} from 'material-ui/Card';
import {red500,blue500} from 'material-ui/styles/colors';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.css';
import './range.css';

// Main Range Parent Class
// Using this wrapper Class to allow things to be potentially included
class RangeForm extends React.Component {

    constructor(props) {
        super(props);

        this.state = {

        };

        // Defining data structure to hold information on ranges
        // Starts with a default empty 
        this.state.ranges = [
            {
                id: (+ new Date() + Math.floor(Math.random() * 999999)).toString(36),
                min: '',
                max: '',
                applied: false,
            }
        ];

    }

    // Deletes a range by splicing it out of the main data structure
    handleRowDel(range) {
        var index = this.state.ranges.indexOf(range);
        this.state.ranges.splice(index, 1);
        this.setState(this.state.ranges);
    };

    // Adds a range with default values by pushing it into the main data structure
    handleAddEvent(e) {

        // Generates random IDs
        var id = (+ new Date() + Math.floor(Math.random() * 999999)).toString(36);
        var range = {
            id: id,
            min: '',
            max: '',
            applied: false,
            
        }
        this.state.ranges.push(range);
        this.setState(this.state.ranges);

    }

    // Updates the main data structure when the min or max fields are updated
    // Happens with every new key press
    handleTextUpdate(e, min, max) {
        var update = {
            id: e.target.id,
            name: e.target.name,
            value: e.target.value
        };

        var ranges = this.state.ranges.slice();

        var newRanges = ranges.map(function(range) {
            for (var key in range) {
                if (key == update.name && range.id == update.id) {
                    range[key] = update.value;
                    /*
                    if(key == "min") {
                        min(update.value);
                    }
                    else {
                        max(update.value);
                    }
                    */
                }
            }

            return range;
        });



        this.setState({ranges:newRanges});
    };

    // Updates the main data structure with true false values when switch is toggled
    handleSwitchToggle(e) {

        var update = {
            id: e.target.id,
            name: e.target.name,
            checked: e.target.checked
        };

        var ranges = this.state.ranges.slice();

        var newRanges = ranges.map(function(range) {
            for (var key in range) {
                if (key == update.name && range.id == update.id) {
                    range[key] = update.checked;
                }
            }
            return range;
        });

        console.log(ranges);
        this.setState({ranges:newRanges});
    };

    // The view for Range Form will display the Rangle Table class
    // Passes data sctructure methods as props so that RangeTable has access
    render() {
        return (
            <RangeTable 
                onTextUpdate={this.handleTextUpdate.bind(this)} 
                onToggle={this.handleSwitchToggle.bind(this)}
                onRowAdd={this.handleAddEvent.bind(this)} 
                onRowDel={this.handleRowDel.bind(this)} 
                ranges={this.state.ranges}
                minVal={this.props.minVal}
                maxVal={this.props.maxVal}
            />
        );
    }

}

// This displays all the ranges in table format
class RangeTable extends React.Component {

    constructor(props) {
        super(props);
    }

    render() {
        var onTextUpdate = this.props.onTextUpdate;
        var onToggle = this.props.onToggle;
        var rowDel = this.props.onRowDel;
        var min = this.props.minVal;
        var max = this.props.maxVal;

        // Maps all the rows to the view and passes data structure methods to give access
        var range = this.props.ranges.map(function(range) {
            return (<RangeRow 
                        onTextUpdate={onTextUpdate} 
                        onToggle={onToggle}
                        range={range} 
                        onDelEvent={rowDel.bind(this)} 
                        key={range.id} 
                        minVal= {min}
                        maxVal= {max}
                    />)
        });

        return (
            <div
                
            >
                {range}
                <Card>
                    <CardText>
                        <Flex layout="row">
                            <FontIcon
                                onClick={this.props.onRowAdd}
                                className="fa fa-plus fa-2x"
                                hoverColor={blue500}
                                style={iconStyles}
                            />
                        </Flex>
                    </CardText>
                </Card>   
            </div>
        );
    }
}

// This defines how a single range looks and operates
class RangeRow extends React.Component {

    constructor(props) {
        super(props);

        this.state={
            minID: (+ new Date() + Math.floor(Math.random() * 999999)).toString(36),
            slideID: (+ new Date() + Math.floor(Math.random() * 999999)).toString(36),
            maxID: (+ new Date() + Math.floor(Math.random() * 999999)).toString(36)
        }
    }

    // Method to bind delete to a button 
    onDelEvent() {
        this.props.onDelEvent(this.props.range);
    }

    handleSliderUpdate(e) {
        //console.log(e[0]);
        //e[1] = 100;
        console.log(document.getElementById(e.title));
    };

    render() {
        return (
            <Card>
                <CardText 
                >
                    <Flex layout="row">        

                        <Flex flex="10">
                            <FontIcon
                                onClick={this.onDelEvent.bind(this)}
                                className="fa fa-trash fa-2x"
                                hoverColor={red500}
                                style={iconStyles}
                            />
                        </Flex>

                        <Flex flex="80">
                            <TextSlider
                                minVal={this.props.minVal}
                                maxVal={this.props.maxVal}
                                onTextUpdate={this.props.onTextUpdate}
                                onSlider={this.handleSliderUpdate.bind(this)}
                                minData={{
                                    type: "min",
                                    value: this.props.range.min,
                                    id: this.props.range.id,
                                    minID: this.state.minID,
                                    slideID: this.state.slideID
                                }}
                                slideData={{
                                    type: "slider",
                                    minID: this.state.minID,
                                    maxID: this.state.maxID,
                                    slideID: this.state.slideID
                                }}
                                maxData={{
                                    type: "max",
                                    value: this.props.range.max,
                                    id: this.props.range.id,
                                    maxID: this.state.maxID
                                }}
                            />
                        </Flex>

                        {/*
                        
                        <Flex flex="25">
                            <MinInput onTextUpdate={this.props.onTextUpdate} cellData={{
                                type: "min",
                                value: this.props.range.min,
                                id: this.props.range.id,
                                minID: this.state.minID,
                                slideID: this.state.slideID
                            }}/>
                        </Flex>

                        <Flex divider />

                        <Flex flex="30"
                            style={{
                                margin: "16px 0px 0px -8px"
                            }} 
                        >
                            <RangeSlider
                                minVal={this.props.minVal}
                                maxVal={this.props.maxVal}
                                onSlider={this.handleSliderUpdate.bind(this)}
                                cellData={{
                                    type: "slider",
                                    minID: this.state.minID,
                                    maxID: this.state.maxID,
                                    slideID: this.state.slideID
                                }}
                            />
                            
                        </Flex>

                        <Flex divider />

                        <Flex flex="25"
                            style={{
                                margin: "0px 0px 0px 15px"
                            }} 
                        >
                            <MaxInput 
                                onTextUpdate={this.props.onTextUpdate} 
                                cellData={{
                                    type: "max",
                                    value: this.props.range.max,
                                    id: this.props.range.id,
                                    maxID: this.state.maxID
                                }}
                            />
                        </Flex>

                        */}

                        <Flex flex="10"
                            style={{
                                margin: "11px 0px 0px -11px"
                            }} 
                        >
                            <AppliedSwitch onToggle={this.props.onToggle} cellData={{
                                type: "applied",
                                toggled: this.props.range.applied,
                                id: this.props.range.id
                            }}/>
                        </Flex>
                    </Flex>
                </CardText>
            </Card>
        );
    }
}

var textStyle = {
    width: "60px"

}

// Binds the Min-Input text to the data structure data
// Emulates a two-way binding
class MinInput extends React.Component {

    constructor(props) {
        super(props);
    }

    render() {
        return (
            <TextField 
                type='text' 
                ref={this.props.cellData.minID}
                name={this.props.cellData.type} 
                id={this.props.cellData.id.toString()} 
                value={this.props.cellData.value} 
                //onChange={this.props.onTextUpdate}
                onChange={(e) => this.props.onTextUpdate(e,this.props.cellData.slideID)}
                hintText="Min"
                style = {textStyle}
                title = {this.props.cellData.title}
            />
        );
    }
}

// Binds the Max-Input text to the data structure data
// Emulates a two-way binding
class MaxInput extends React.Component {

    constructor(props) {
        super(props);
    }

    render() {
        return (
            <TextField type='text' name={this.props.cellData.type} id={this.props.cellData.id.toString()} value={this.props.cellData.value} onChange={this.props.onTextUpdate}
                hintText="Max"
                style = {textStyle}
                title = {this.props.cellData.title}
            />
        );
    }
}

// Binds the Switch boolean to the data structure data
// Emulates a two-way binding
class AppliedSwitch extends React.Component {

    constructor(props) {
        super(props);
    }

    render() {
        return (
            <Toggle name={this.props.cellData.type} id={this.props.cellData.id.toString()} toggled={this.props.cellData.toggled} onToggle={this.props.onToggle} />
        );
    }
}

class RangeSlider extends React.Component {

    constructor(props) {
        super(props);

        this.state = {
            min: this.props.minVal,
            max: this.props.maxVal
        }
    }

    updateValues(lowerVal, upperVal) {
        this.state.min = lowerVal;
        this.state.max = upperVal;
        this.setState(this.state);
    }

    render() {
        return (
            <Range
                //ref={this.props.cellData.slideID}
                min={this.state.min}
                max={this.state.max}
                defaultValue={[this.props.minVal,this.props.maxVal]}
                allowCross={false}
                onChange={this.props.onSlider}
            />
        );
    }
}


class TextSlider extends React.Component {

    constructor(props) {
        super(props);

        this.state = {
            min: this.props.minVal,
            max: this.props.maxVal
        }
    }

    updateMin(min) {
        this.state.min = min;
        this.setState(this.state);
    }

    updateMax(max) {
        this.state.max = max;
        this.setState(this.state);
    }

    render() {
        return (
            <Flex>

                <TextField 
                    type='text' 
                    ref={this.props.minData.minID}
                    name={this.props.minData.type} 
                    id={this.props.minData.id.toString()} 
                    value={this.props.minData.value} 
                    //onChange={this.props.onTextUpdate}
                    onChange={(e) => this.props.onTextUpdate(e, this.updateMin, this.updateMin)}
                    hintText="Min"
                    style = {textStyle}
                />

                <Range
                    //ref={this.props.cellData.slideID}
                    min={this.state.min}
                    max={this.state.max}
                    defaultValue={[this.props.minVal,this.props.maxVal]}
                    allowCross={false}
                    onChange={this.props.onSlider}
                />


                <TextField 
                    type='text' 
                    name={this.props.maxData.type} 
                    id={this.props.maxData.id.toString()} 
                    value={this.props.maxData.value} 
                    onChange={this.props.onTextUpdate}
                    hintText="Max"
                    style = {textStyle}
                />

            </Flex>
        );
    }
}


const iconStyles = {
    fontSize: 26,
    paddingLeft: 10,
    paddingRight: 13,
    margin: "12px 0px 0px -21px"
};


export default RangeForm;
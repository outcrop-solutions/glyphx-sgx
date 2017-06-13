import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import './range.css';

// Main Range Parent Class
// Using this wrapper Class to allow things to be potentially included
class RangeForm extends React.Component {

    constructor(props) {
        super(props);

        this.state = {};

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
    handleTextUpdate(e) {
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
            />
        );
    }

}

// This displays all the ranges in table format
class RangeTable extends React.Component {

    render() {
        var onTextUpdate = this.props.onTextUpdate;
        var onToggle = this.props.onToggle;
        var rowDel = this.props.onRowDel;

        // Maps all the rows to the view and passes data structure methods to give access
        var range = this.props.ranges.map(function(range) {
            return (<RangeRow onTextUpdate={onTextUpdate} onToggle={onToggle} range={range} onDelEvent={rowDel.bind(this)} key={range.id}/>)
        });

        return (
            <div>
                <button type="button" onClick={this.props.onRowAdd} className="btn pull-right">Add</button>
                {range}
            </div>
        );
    }
}

// This defines how a single range looks and operates
class RangeRow extends React.Component {

    // Method to bind delete to a button 
    onDelEvent() {
        this.props.onDelEvent(this.props.range);
    }

    render() {
        return (
            <div>
                <input type="button" onClick={this.onDelEvent.bind(this)} value="X" className="del-btn"/>

                <MinInput onTextUpdate={this.props.onTextUpdate} cellData={{
                    type: "min",
                    value: this.props.range.min,
                    id: this.props.range.id
                }}/>
                <MaxInput onTextUpdate={this.props.onTextUpdate} cellData={{
                    type: "max",
                    value: this.props.range.max,
                    id: this.props.range.id
                }}/>
                <AppliedSwitch onToggle={this.props.onToggle} cellData={{
                    type: "applied",
                    toggled: this.props.range.applied,
                    id: this.props.range.id
                }}/>
            </div>
        );
    }
}

// Binds the Min-Input text to the data structure data
// Emulates a two-way binding
class MinInput extends React.Component {
    render() {
        return (
            <TextField type='text' name={this.props.cellData.type} id={this.props.cellData.id.toString()} value={this.props.cellData.value} onChange={this.props.onTextUpdate}
                hintText="Min"
            />
        );
    }
}

// Binds the Max-Input text to the data structure data
// Emulates a two-way binding
class MaxInput extends React.Component {
    render() {
        return (
            <TextField type='text' name={this.props.cellData.type} id={this.props.cellData.id.toString()} value={this.props.cellData.value} onChange={this.props.onTextUpdate}
                hintText="Max"
            />
        );
    }
}

// Binds the Switch boolean to the data structure data
// Emulates a two-way binding
class AppliedSwitch extends React.Component {
    render() {
        return (
            <Toggle name={this.props.cellData.type} id={this.props.cellData.id.toString()} toggled={this.props.cellData.toggled} onToggle={this.props.onToggle} />
        );
    }
}


export default RangeForm;
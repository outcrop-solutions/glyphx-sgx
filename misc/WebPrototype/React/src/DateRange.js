import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import Range from 'rc-slider/lib/Range';
import DatePicker from 'react-datepicker';
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.min.css';


/**
 * NOT READY TO BE USED, WAITING FOR DATE SAMPLES TO FINISH DEVELOPING
 **/



/**
 * Main Range parent class which gets exported
 * @param colName: Name of the corresponding column for this RangeForm
 * @param data: array of values from the eleastic table for the corresponding colName
 **/
class DateRangeTable extends React.Component {
    /**
     * Deletes a range by splicing it out of the store
     * @param id: ID of the row which is to be deleted
     **/
    handleRowDel(id) {
        this.props.dispatch(removeRange(this.props.colName, id, this.props.data, "Date"));
    };


    /**
     * Adds a range with the default values of the current min being set to the minimum value, current max being set to the maximum value,
     * a new generated ID, and the applied switch on off.
     **/
    handleAddEvent() {
        this.props.dispatch(addRange(this.props.colName, 0, 0, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false));
    }


    /**
     * Updates a range row in the store (null values may be passed to min, max, or applied to peform no update on that field)
     * @param id: ID used to find the range in the store
     * @param min: Min value to update
     * @param max: Max value to update
     * @param applied: Applied status to update
     **/
    handleStoreUpdate(id, min, max, applied) { 
        this.props.dispatch(updateRange(this.props.colName, min, max, id, applied, this.props.data, "Date"));
    }


    /**
     * Maps ranges to DOM
     **/
    render() {
        var onRowDel = this.handleRowDel.bind(this);
        var updateStore = this.handleStoreUpdate.bind(this);

        var rList = this.props.rangeList[this.props.colName].rangeList;

        var range = rList.map( function(range) {
            return (<DateRangeRow 
                        range = { range } 
                        onDelEvent = { onRowDel.bind(this) } 
                        updateStore = { updateStore }
                        key = { range[2] }
                    />)
        });

        return (
            <div>

                {/* Displays the mapped ranges */}
                {range}

                {/* Add range button */}
                <Card>
                    <CardText>
                            <FontIcon
                                onClick = { this.handleAddEvent.bind(this) }
                                className = "fa fa-plus fa-2x"
                                hoverColor = { blue500 }
                                style = { styleSet.iconStyles }
                            />
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
class DateRangeRow extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            latestUpdate: "",
            epoch: 0,
            min: -1,
            max: -1,
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
        if (this.state.min != nextState.min || this.state.max != nextState.max) {
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

    handleChangeStart(e) {
        console.log(e);
    }

    handleChangeEnd(e) {
        console.log(e);
    }

    render() {
        return (
            <Card>
                <CardText>

                            <FontIcon
                                onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash fa-2x"
                                hoverColor = { red500 }
                                style = { styleSet.iconStyles }
                            />

                            <DatePicker
                                selected = { this.state.min }
                                selectsStart
                                startDate = { this.state.min }
                                endDate = { this.state.max }
                                onChange = { (e) => this.handleChangeStart(e) }
                            />

                            <DatePicker
                                selected = { this.state.max }
                                selectsEnd
                                startDate = { this.state.min }
                                endDate = { this.state.max }
                                onChange = { (e) => this.handleChangeEnd(e) }
                            />

                            <Toggle 
                                name = "applied" 
                                id = { this.props.range.id } 
                                toggled = { this.props.range[3] } 
                                onToggle = {  
                                    (e) => this.onToggle(e)
                                }
                            />

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
    rangeList: state.filterState.Filter,
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(DateRangeTable);
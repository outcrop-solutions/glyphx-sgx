import React from 'react';
import FontIcon from 'material-ui/FontIcon';
import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import Rnd from 'react-rnd';
import { connect } from 'react-redux';
import './General.css';
import './statisticModal.css';


/**
 * -ADCMT
 **/
class StatisticModal extends React.Component {

    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState){
        return (this.props.statData != nextProps.statData || this.props.statistics != nextProps.statistics || this.props.statDisplay != nextProps.statDisplay);
    };

    generateList(keys, struct) {
        var list = [];

        for (var i = 0; i < keys.length; i++) {
            var val = parseInt(struct.values[keys[i]].value, 10);
            for (var j = 0; j < struct.values[keys[i]].count; j++) {
                list.push(val);
            }
        }

        return list;
    }

    getCount(struct) {
        var count = 0;
        var keys = Object.keys(struct.values);

        for (var i = 0; i < keys.length; i++) {
            count += struct.values[keys[i]].count;
        }

        return count;
    }

    getMin(struct) {
        var keys = Object.keys(struct.values);
        var min = parseInt(struct.values[keys[0]].value, 10);

        for (var i = 0; i < keys.length; i++) {
            var parsedVal =  parseInt(struct.values[keys[i]].value, 10);
            if (parsedVal < min) {
                min = parsedVal;
            }
        }

        return min;
    }

    getMax(struct) {
        var keys = Object.keys(struct.values);
        var max = parseInt(struct.values[keys[0]].value, 10);

        for (var i = 0; i < keys.length; i++) {
            var parsedVal =  parseInt(struct.values[keys[i]].value, 10);
            if (parsedVal > max) {
                max = parsedVal;
            }
        }

        return max;
    }

    getMean(struct) {
        return(this.getSum(struct) / this.getCount(struct));
    }

    getMedian(struct) {
        var keys = Object.keys(struct.values);
        var list = this.generateList(keys, struct);
        list.sort();

        if (list.length % 2 == 0) {

            return ((list[(list.length / 2)] + list[(list.length / 2) + 1]) / 2);
        }
        else {
            return list[(list.length / 2) - 0.5];
        }

    }

    getMode(struct) {
        var keys = Object.keys(struct.values);
        var maxCount = struct.values[keys[0]].count;
        var countArray = [struct.values[keys[0]].value]

        for (var i = 1; i < keys.length; i++) {
            if (struct.values[keys[i]].count == maxCount) {
                countArray.push(struct.values[keys[i]].value);
            }
            else if (struct.values[keys[i]].count > maxCount) {
                maxCount = struct.values[keys[i]].count;
                countArray = [struct.values[keys[i]].value];
            }
        }

        var mode = ""

        for (var i = 0; i < countArray.length; i++) {
            mode += (countArray[i] + ", ");
        }

        mode = mode.slice(0, -2)

        return mode;
    }

    getSum(struct) {
        var sum = 0;
        var keys = Object.keys(struct.values);

        for (var i = 0; i < keys.length; i++) {
            sum += (parseInt(struct.values[keys[i]].value, 10) * struct.values[keys[i]].count);
        }

        return sum;
    }

    getRange(struct) {
        return(this.getMax(struct) - this.getMin(struct));
    }

    getStdDev(struct) {
        return Math.sqrt(this.getVariance(struct));
    }

    getVariance(struct) {
        var keys = Object.keys(struct.values);
        var list = this.generateList(keys, struct);
        var mean = this.getMean(struct);

        var variance = 0;

        for (var i = 0; i < list.length; i++) {
            list[i] = list[i] - mean;
        }

        for (var i = 0; i < list.length; i++) {
            list[i] = list[i] * list[i];
        }

        for (var i = 0; i < list.length; i++) {
            variance += list[i];
        }

        return (variance / (list.length - 1));
    }

    getSkewness(struct) {

        return 0;
    }

    getKurtosis(struct) {

        return 0;
    }

    render() {
        var columns = this.props.statistics.colList.split(',');
        var stats = this.props.statistics.statList.split(',');

        debugger;

        console.log(this.props.statData);

        var header = (
            <TableHeader
                displaySelectAll = { false }
                adjustForCheckbox = { false }
            >
                <TableRow style = {{ height: "28px" }} >
                    <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableFirstCol" >Column</TableHeaderColumn>

                    {stats.indexOf("Count") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Count</TableHeaderColumn> : null}
                    {stats.indexOf("Min") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Min</TableHeaderColumn> : null}
                    {stats.indexOf("Max") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Max</TableHeaderColumn> : null}
                    {stats.indexOf("Mean") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Mean</TableHeaderColumn> : null}
                    {stats.indexOf("Median") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Median</TableHeaderColumn> : null}
                    {stats.indexOf("Mode") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Mode</TableHeaderColumn> : null}
                    {stats.indexOf("Sum") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Sum</TableHeaderColumn> : null}
                    {stats.indexOf("Range") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Range</TableHeaderColumn> : null}
                    {stats.indexOf("St. Dev.") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >St. Dev.</TableHeaderColumn> : null}
                    {stats.indexOf("Variance") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Variance</TableHeaderColumn> : null}
                    {stats.indexOf("Skewness") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Skewness</TableHeaderColumn> : null}
                    {stats.indexOf("Kurtosis") !== -1 ? <TableHeaderColumn style = {{ color: "#757575" }} className = "statTableCol" >Kurtosis</TableHeaderColumn> : null}
                </TableRow>
            </TableHeader>
        );

        var context = this;

        var rows = columns.map( function(colName) {
            return (
                <TableRow key = {colName} style = {{ height: "28px" }} >
                    <TableRowColumn className = "statTableFirstCol" >{colName}</TableRowColumn>

                    {stats.indexOf("Count") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getCount(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Min") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getMin(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Max") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getMax(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Mean") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getMean(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Median") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getMedian(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Mode") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getMode(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Sum") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getSum(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Range") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getRange(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("St. Dev.") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getStdDev(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Variance") !== -1 ? <TableRowColumn className = "statTableCol" >{context.getVariance(context.props.statData[colName])}</TableRowColumn> : null}
                    {stats.indexOf("Skewness") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Kurtosis") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                </TableRow>
            )
        });

        return (
            <Rnd
                default = {{ x: 10, y: 10, width: 80 * stats.length + 140, height: 28 * columns.length + 58 }}
                z = { 1000 }
                enableResizing = {{ top: false, right: false, bottom: false, left: false, topRight: false, bottomRight: false, bottomLeft: false, topLeft: false }}
                style = {{ display: ( this.props.statDisplay ? "block" : "none" ) }}
                onDragStart = { () => this.props.handleCorrection(true) }
                onDragStop = { () => this.props.handleCorrection(false) }
                bounds = "parent"
            >
                <div className = "statisticsBox" style = {{ height: "inherit" }} >
                    <div 
                        style = {{
                            backgroundColor: this.props.settings.colors.statisticsDragColor.topBar, 
                            height: "26px", 
                            color: "#ffffff", 
                            borderTopRightRadius: "5px", 
                            borderTopLeftRadius: "5px" 
                        }} 
                    >
                        <label style = {{ margin: "6px 0px 0px 5px", float: "left" }} > STATISTICS </label>

                        <FontIcon 
                            className = "fa fa-window-close cursorHand" 
                            style = {{ color: "#ffffff",  fontSize: "21px", margin: "2px 0px 0px 0px", float: "right", paddingRight: "2px" }} 
                            onClick = { () => this.props.dispatch(updateStatistics(null, null, false)) } 
                        />
                    </div>

                    <Table selectable = { false } >
                        {header}
                        <TableBody displayRowCheckbox = { false } > {rows} </TableBody>
                    </Table>
                </div>
            </Rnd>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const updateStatistics = (colList, statList, display) => ({
    type: 'UPDATE_STATISTICS',
    colList, 
    statList, 
    display
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    statistics: state.filterState.Statistics,
    settings: state.filterState.Settings,
    statDisplay: state.filterState.ModalDisplay.statisticsModal,
    statData: state.filterState.StatisticsData
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(StatisticModal);
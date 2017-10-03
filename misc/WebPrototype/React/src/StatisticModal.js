import React from 'react';
import FontIcon from 'material-ui/FontIcon';
import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import Rnd from 'react-rnd';
import { connect } from 'react-redux';
import 'font-awesome/css/font-awesome.min.css';
import './General.css';
import './statisticModal.css';


/**
 * No props get passed down.
 **/
class StatisticModal extends React.Component {


    /**
     * 
     **/
    render() {
        var columns = this.props.statistics.colList.split(',');
        var stats = this.props.statistics.statList.split(',');

        var header = (
            <TableHeader
                displaySelectAll = { false }
                adjustForCheckbox = { false }
            >
                <TableRow >
                    <TableHeaderColumn className = "statTableFirstCol" >Column</TableHeaderColumn>
                    {stats.indexOf("Count") !== -1 ? <TableHeaderColumn className = "statTableCol" >Count</TableHeaderColumn> : null}
                    {stats.indexOf("Min") !== -1 ? <TableHeaderColumn className = "statTableCol" >Min</TableHeaderColumn> : null}
                    {stats.indexOf("Max") !== -1 ? <TableHeaderColumn className = "statTableCol" >Max</TableHeaderColumn> : null}
                    {stats.indexOf("Average") !== -1 ? <TableHeaderColumn className = "statTableCol" >Average</TableHeaderColumn> : null}
                    {stats.indexOf("Median") !== -1 ? <TableHeaderColumn className = "statTableCol" >Median</TableHeaderColumn> : null}
                    {stats.indexOf("Sum") !== -1 ? <TableHeaderColumn className = "statTableCol" >Sum</TableHeaderColumn> : null}
                    {stats.indexOf("Range") !== -1 ? <TableHeaderColumn className = "statTableCol" >Range</TableHeaderColumn> : null}
                    {stats.indexOf("St. Dev.") !== -1 ? <TableHeaderColumn className = "statTableCol" >St. Dev.</TableHeaderColumn> : null}
                    {stats.indexOf("Varience") !== -1 ? <TableHeaderColumn className = "statTableCol" >Varience</TableHeaderColumn> : null}
                    {stats.indexOf("Skewness") !== -1 ? <TableHeaderColumn className = "statTableCol" >Skewness</TableHeaderColumn> : null}
                    {stats.indexOf("Kurtosis") !== -1 ? <TableHeaderColumn className = "statTableCol" >Kurtosis</TableHeaderColumn> : null}
                </TableRow>
            </TableHeader>
        );

        var rows = columns.map( function(colName) {
            return (
                <TableRow key = {colName}>
                    <TableRowColumn className = "statTableFirstCol" >{colName}</TableRowColumn>
                    {stats.indexOf("Count") !== -1 ? <TableRowColumn className = "statTableCol" >10101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Min") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Max") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Average") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Median") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Sum") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Range") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("St. Dev.") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Varience") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Skewness") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Kurtosis") !== -1 ? <TableRowColumn className = "statTableCol" >1.0101010101010101010101</TableRowColumn> : null}
                </TableRow>
            )
        });

        return (
            <Rnd
                default = {{ 
                    x: 10, 
                    y: 10, 
                    width: 80 * stats.length + 140, 
                    height: 51 * columns.length + 78 
                }}
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
    statDisplay : state.filterState.ModalDisplay.statisticsModal,
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(StatisticModal);
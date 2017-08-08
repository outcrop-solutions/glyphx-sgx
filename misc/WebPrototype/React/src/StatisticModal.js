import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import { Flex } from 'react-flex-material';
import Range from 'rc-slider/lib/Range';
import DropDownMenu from 'material-ui/DropDownMenu';
import IconButton from 'material-ui/IconButton';
import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import Rnd from 'react-rnd';
import { Card, CardText } from 'material-ui/Card';
import { connect } from 'react-redux';
import 'rc-slider/assets/index.css';
import 'font-awesome/css/font-awesome.min.css';
import './General.css';
import './statisticModal.css';


/**
 * @param colName: Name of the corresponding column for this RangeForm
 * @param data: Array of values from the eleastic table for the corresponding colName
 **/
class StatisticModal extends React.Component {

    handleStatisticUpdate(colList, statList, visibility) {
        this.props.dispatch(updateStatistics(colList, statList, visibility));
    };

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
                    <TableHeaderColumn style = {{ height: "inherit", width: "120px", paddingLeft: "10px", paddingRight: "10px" }} >Column</TableHeaderColumn>
                    {stats.indexOf("Count") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Count</TableHeaderColumn> : null}
                    {stats.indexOf("Min") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Min</TableHeaderColumn> : null}
                    {stats.indexOf("Max") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Max</TableHeaderColumn> : null}
                    {stats.indexOf("Average") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Average</TableHeaderColumn> : null}
                    {stats.indexOf("Median") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Median</TableHeaderColumn> : null}
                    {stats.indexOf("Sum") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Sum</TableHeaderColumn> : null}
                    {stats.indexOf("Range") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Range</TableHeaderColumn> : null}
                    {stats.indexOf("St. Dev.") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >St. Dev.</TableHeaderColumn> : null}
                    {stats.indexOf("Varience") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Varience</TableHeaderColumn> : null}
                    {stats.indexOf("Skewness") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Skewness</TableHeaderColumn> : null}
                    {stats.indexOf("Kurtosis") !== -1 ? <TableHeaderColumn style = {{ height: "inherit", paddingLeft: "0px", paddingRight: "10px" }} >Kurtosis</TableHeaderColumn> : null}
                </TableRow>
            </TableHeader>
        );


        var rows = columns.map( function(colName) {
            return (
                <TableRow key = {colName}>
                    <TableRowColumn style = {{ width: "120px", paddingLeft: "10px", paddingRight: "10px" }} >{colName}</TableRowColumn>
                    {stats.indexOf("Count") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >10101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Min") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Max") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Average") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Median") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Sum") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Range") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("St. Dev.") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Varience") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Skewness") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                    {stats.indexOf("Kurtosis") !== -1 ? <TableRowColumn style = {{ paddingLeft: "0px", paddingRight: "10px" }} >1.0101010101010101010101</TableRowColumn> : null}
                </TableRow>
            )
        });

        return (
            <Rnd
                default = {{
                    x: 10,
                    y: 10,
                    width: 80 * stats.length + 140,
                    height: 51 * columns.length + 78,
                }}
                enableResizing = {{ top: false, right: false, bottom: false, left: false, topRight: false, bottomRight: false, bottomLeft: false, topLeft: false }}
                style = {{
                    display: ( this.props.statistics.display ? "block" : "none" )
                }}
                bounds = "parent"
            >
                <div className = "statisticsBox" style = {{ height: "inherit" }} >
                    <div style = {{ backgroundColor: this.props.settings.statisticsDragColor.topBar, height: "26px", color: "#ffffff", borderTopRightRadius: "5px", borderTopLeftRadius: "5px" }} >
                        <label style = {{ margin: "6px 0px 0px 5px", float: "left" }} > STATISTICS </label>
                        <FontIcon 
                            className = "fa fa-window-close cursorHand" 
                            style = {{ color: "#ffffff",  fontSize: "21px", margin: "2px 0px 0px 0px", float: "right", paddingRight: "2px" }} 
                            onClick = { () => this.handleStatisticUpdate(null, null, false) } 
                        />
                    </div>

                    <Table
                        selectable = { false }
                    >
                        {/*
                        <TableHeader
                            displaySelectAll = { false }
                            adjustForCheckbox = { false }
                        >
                            <TableRow style = {{ height:'30px' }} >
                                <TableHeaderColumn>Column</TableHeaderColumn>
                                {headers}
                            </TableRow>
                        </TableHeader> */}

                        {header}

                        <TableBody 
                            displayRowCheckbox = { false }
                        >
                            {rows}
                        </TableBody>
                    </Table>
                </div>
            </Rnd>
        );
    }
}


/**
 * Local styling
 **/
const styleSet = {
    tableStats: {
        paddingLeft: "0px", 
        paddingRight: "10px"
    },
    tableColumnName: {
        width: "16%", 
        paddingLeft: "10px", 
        paddingRight: "10px"
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const updateStatistics = (colList, statList, visibility) => ({
    type: 'UPDATE_STATISTICS',
    colList, 
    statList, 
    visibility
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    statistics: state.filterState.Statistics,
    settings: state.filterState.Settings
  }
}


/**
 * Connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(StatisticModal);
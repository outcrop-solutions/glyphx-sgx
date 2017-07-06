import React from 'react';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import Badge from 'material-ui/Badge';
import { Flex } from 'react-flex-material';
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import 'font-awesome/css/font-awesome.css';


/**
 * Main FilterView parent class which gets exported
 **/
class FilterViewForm extends React.Component {


    /**
     * Unselects all ranges and removes all elastic selections from a column
     * @param colName: name of the corresponding column to be deleted
     **/
    handleRowDel(colName) {
        console.log("Start FilterView remove", performance.now());
        this.props.dispatch(removeFilterView(colName));
        console.log("Finish FilterView remove", performance.now());
    };


    render() {
        return (
            <Card style = {{ overflow: 'auto', width: '100%', padding: "0px"}}>
                <CardText>
                    <FilterViewTable
                        onRowDel = { this.handleRowDel.bind(this) }
                        elasticList = { this.props.elasticList }
                        rangeList = { this.props.rangeList }
                    />
                </CardText>
            </Card>
        );
    }
}


/**
 * Maps FilterView Rows based on the elastic and range selections for each column
 **/
class FilterViewTable extends React.Component {

    render() {
        // Lose scope of 'this' in the map function
        var rowDel = this.props.onRowDel;


        var eList = this.props.elasticList;
        var hList = this.props.rangeList;
        
        var viewList = [];

        console.log("Start FilterView", performance.now());

        // Find the min and max values selected bsed on the type (Text, Number, Date)
        for (var colName in eList) {
            var filterType = eList[colName].type;
            var min, max, i, curNum;
            var rCount = 0;

            debugger;
            if (filterType === "Text") {
                if (eList[colName].selectedValues.length) {
                    min = eList[colName].selectedValues[0];
                    max = eList[colName].selectedValues[0];
                }
                else if (hList[colName].highlightedValues.length) {
                    min = hList[colName].highlightedValues[0];
                    max = hList[colName].highlightedValues[0];
                }
                else {
                    continue;
                }

                if ( eList[colName].selectedValues.length) {
                    for (i = 0; i < eList[colName].selectedValues.length; i++) {
                        if (eList[colName].selectedValues[i] < min) {
                            min = eList[colName].selectedValues[i];
                        }
                        else if (eList[colName].selectedValues[i] > max) {
                            max = eList[colName].selectedValues[i];
                        }
                    }
                }

                if ( hList[colName].highlightedValues.length ) {
                    for (i = 0; i < hList[colName].highlightedValues.length; i++) {
                        if (hList[colName].highlightedValues[i] < min) {
                            min = hList[colName].highlightedValues[i];
                        }
                        else if (hList[colName].highlightedValues[i] > max) {
                            max = hList[colName].highlightedValues[i];
                        }
                    }
                }
            }

            else if (filterType === "Number") {
                if (eList[colName].selectedValues.length) {
                    min = parseInt(eList[colName].selectedValues[0], 10);
                    max = parseInt(eList[colName].selectedValues[0], 10);
                }
                else if (hList[colName].highlightedValues.length) {
                    min = parseInt(hList[colName].highlightedValues[0], 10);
                    max = parseInt(hList[colName].highlightedValues[0], 10);
                }
                else {
                    continue;
                }

                if ( eList[colName].selectedValues.length) {
                    for (i = 0; i < eList[colName].selectedValues.length; i++) {
                        curNum = parseInt(eList[colName].selectedValues[i], 10);
                        if (curNum < min) {
                            min = curNum;
                        }
                        else if (curNum > max) {
                            max = curNum;
                        }
                    }
                }

                if ( hList[colName].highlightedValues.length ) {
                    for (i = 0; i < hList[colName].highlightedValues.length; i++) {
                        curNum = parseInt(hList[colName].highlightedValues[i], 10);
                        if (curNum < min) {
                            min = curNum;
                        }
                        else if (curNum > max) {
                            max = curNum;
                        }
                    }
                }
            }

            else if (filterType === "Date") {
                console.log("Not implemented yet...");
                break;
            }

            else {
                console.log("Oh no, something went wrong when reading the filter type into the filterView:" + filterType);
                break;
            }

            // Find how many ranges are selected
            var rList = this.props.rangeList[colName].rangeList;
            for (i = 0; i < rList.length; i ++) {
                if (rList[i][3] === true) {
                    rCount++;
                }
            }
            viewList.push([eList[colName].displayName, filterType, min, max, eList[colName].selectedValues.length, rCount, colName]);
        }

        console.log("Finish FilterView", performance.now());

        var view = viewList.map( function(view) {
            return (<FilterViewRow 
                        view = { view } 
                        onDelEvent = { rowDel.bind(this) } 
                        key = { view[0] } 
                    />)
        });

        return (
            <div>
                <Flex divider />  

                <Flex layout="row"> 
                    <span style = {{ color: "#b2b2b2", width: "11px" }} ></span>
                    <span style = {{ color: "#b2b2b2", width: "123px" }} > Options </span>
                    <span style = {{ color: "#b2b2b2", width: "106px"}} > Filter </span>
                    <span style = {{ color: "#b2b2b2", width: "84px" }} > Min </span>
                    <span style = {{ color: "#b2b2b2" }} > Max </span>
                </Flex>

                <Flex divider /> 
                <Divider />
                <Flex divider /> 

                {/* Displays the mapped views*/}
                {view}
            </div>
        );
    }
}


class FilterViewRow extends React.Component {

    onDelEvent() {
        this.props.onDelEvent(this.props.view[6]);
    }

    render() {

        // Shorten display text if theyre too long
        var displayName = this.props.view[0];
        var min = this.props.view[2];
        var max = this.props.view[3];

        if(displayName.length > 25) {
            displayName = displayName.substring(0,24) + "...";
        }

        if(min.length > 12) {
            min = min.substring(0,11) + "...";
        }

        if(max.length > 12) {
            max = max.substring(0,11) + "...";
        }

        return (
            <div>
                <Flex layout="row" style = {{  height: "50px" }}>
                    <Flex divider />  

                    <Flex flex="1">
                        <FontIcon
                            onClick = { this.onDelEvent.bind(this) }
                            className = "fa fa-trash fa-2x"
                            hoverColor = { red500 }
                        />
                    </Flex>

                    <Flex divider /> 
                    <Flex divider /> 

                    <Flex flex="14">
                        <Flex layout="row"> 
                            <Flex flex="50">
                                <Badge
                                    badgeContent={this.props.view[4]}
                                    primary={true}
                                    style = {{ padding: "0px 0px 0px 0px" }}
                                    badgeStyle = {{ width: "20px", height: "20px", top: "-10px", right: "-13px" }}
                                    >
                                    <FontIcon
                                        //onClick = { this.onDelEvent.bind(this) }
                                        className = "fa fa-list-ul fa-2x"
                                        hoverColor = { blue500 }
                                    />
                                </Badge>
                            </Flex>

                            <Flex divider /> 
                            <Flex divider /> 

                            <Flex flex="50" >
                                <Badge
                                    badgeContent={this.props.view[5]}
                                    primary={true}
                                    style = {{ padding: "0px 0px 0px 0px" }}
                                    badgeStyle = {{ width: "20px", height: "20px", top: "-10px", right: "-13px" }}
                                    >
                                    <FontIcon
                                        //onClick = { this.onDelEvent.bind(this) }
                                        className = "fa fa-sliders fa-2x"
                                        hoverColor = { blue500 }
                                    />
                                </Badge>
                            </Flex>
                        </Flex> 
                    </Flex>

                    <Flex divider /> 
                    <Flex divider /> 

                    <Flex flex="85">
                        <Flex layout="row"> 
                            <Flex flex="40">
                                <Tooltip
                                    placement = 'bottom'
                                    mouseEnterDelay = { 0.5 }
                                    mouseLeaveDelay = { 0.15 }
                                    destroyTooltipOnHide = { false }
                                    trigger = { Object.keys( {hover: 1} ) }
                                    overlay = { <div> {this.props.view[0]} </div> }
                                >
                                    <span style = {{  wordWrap: "break-word", display: "block" }}>
                                        {displayName}
                                    </span> 
                                </Tooltip>
                            </Flex>

                            <Flex divider /> 

                            <Flex flex="30">
                                <Tooltip
                                    placement = 'bottom'
                                    mouseEnterDelay = { 0.5 }
                                    mouseLeaveDelay = { 0.15 }
                                    destroyTooltipOnHide = { false }
                                    trigger = { Object.keys( {hover: 1} ) }
                                    overlay = { <div> {this.props.view[2]} </div> }
                                >
                                    <span style = {{  wordWrap: "break-word" }}>
                                        {min}
                                    </span>
                                </Tooltip>
                            </Flex>

                            <Flex divider /> 

                            <Flex flex="30">
                                <Tooltip
                                    placement = 'bottom'
                                    mouseEnterDelay = { 0.5 }
                                    mouseLeaveDelay = { 0.15 }
                                    destroyTooltipOnHide = { false }
                                    trigger = { Object.keys( {hover: 1} ) }
                                    overlay = { <div> {this.props.view[3]} </div> }
                                >
                                    <span style = {{  wordWrap: "break-word" }}>
                                        {max}
                                    </span>
                                </Tooltip>
                            </Flex>
                        </Flex>
                    </Flex>
                </Flex>
            </div>
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
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const removeFilterView = (colName) => ({
    type: 'REMOVE_FILTER_VIEW',
    colName
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    elasticList: state.filterState.Filter.Elastic,
    rangeList: state.filterState.Filter.Ranges,
  }
}


/**
 * Connects the FilterViewForm component to the redux store
 **/
export default connect(mapStateToProps)(FilterViewForm);
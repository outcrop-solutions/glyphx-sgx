import React from 'react';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import Badge from 'material-ui/Badge';
import { Flex } from 'react-flex-material';
import { Card, CardText } from 'material-ui/Card';
import { red500 } from 'material-ui/styles/colors';
import { Textfit } from 'react-textfit';
import { connect } from 'react-redux';
import 'font-awesome/css/font-awesome.css';


class FilterViewForm extends React.Component {

    handleRowDel() {
        this.props.dispatch(removeAllRange(this.props.colName));
        this.props.dispatch(removeAllElastic(this.props.colName));
        this.props.dispatch(unhighlightAllElastic(this.props.colName));
    };


    render() {
        return (
            <div style = {{ overflow: 'auto', width: '100%', borderStyle: "solid", borderWidth: "1px" }} >
                <FilterViewTable
                    onRowDel = { this.handleRowDel.bind(this) }
                    elasticList = { this.props.elasticList }
                    rangeList = { this.props.rangeList }
                />
            </div>
        );
    }
}


class FilterViewTable extends React.Component {

    render() {
        // Lose scope of 'this' in the map function so record what you need to access beforehand
        var rowDel = this.props.onRowDel;

        // Maps rows to the DOM and passes data structure methods to rows so they have access
        var eList = this.props.elasticList;

        var viewList = [];
        // [colName, type, min, max, foundElastic, FoundRange]

        for (var colName in eList) {

            var filterType = eList[colName].type;
            var min, max;
            var rCount = 0;

            if (filterType == "Text") {
                if (eList[colName].selectedValues.length) {
                    min = eList[colName].selectedValues[0];
                    max = eList[colName].selectedValues[0];
                }
                else if (eList[colName].highlightedValues.length) {
                    min = eList[colName].highlightedValues[0];
                    max = eList[colName].highlightedValues[0];
                }
                else {
                    continue;
                }

                if ( eList[colName].selectedValues.length) {
                    for (var i = 0; i < eList[colName].selectedValues.length; i++) {
                        if (eList[colName].selectedValues[i] < min) {
                            min = eList[colName].selectedValues[i];
                        }
                        else if (eList[colName].selectedValues[i] > max) {
                            max = eList[colName].selectedValues[i];
                        }
                    }
                }

                if ( eList[colName].highlightedValues.length ) {
                    for (var i = 0; i < eList[colName].highlightedValues.length; i++) {
                        if (eList[colName].highlightedValues[i] < min) {
                            min = eList[colName].highlightedValues[i];
                        }
                        else if (eList[colName].highlightedValues[i] > max) {
                            max = eList[colName].highlightedValues[i];
                        }
                    }
                }
            }

            else if (filterType == "Number") {
                if (eList[colName].selectedValues.length) {
                    min = parseInt(eList[colName].selectedValues[0], 10);
                    max = parseInt(eList[colName].selectedValues[0], 10);
                }
                else if (eList[colName].highlightedValues.length) {
                    min = parseInt(eList[colName].highlightedValues[0], 10);
                    max = parseInt(eList[colName].highlightedValues[0], 10);
                }
                else {
                    continue;
                }

                if ( eList[colName].selectedValues.length) {
                    for (var i = 0; i < eList[colName].selectedValues.length; i++) {
                        var curNum = parseInt(eList[colName].selectedValues[i], 10);
                        if (curNum < min) {
                            min = curNum;
                        }
                        else if (curNum > max) {
                            max = curNum;
                        }
                    }
                }

                if ( eList[colName].highlightedValues.length ) {
                    for (var i = 0; i < eList[colName].highlightedValues.length; i++) {
                        var curNum = parseInt(eList[colName].highlightedValues[i], 10);
                        if (curNum < min) {
                            min = curNum;
                        }
                        else if (curNum > max) {
                            max = curNum;
                        }
                    }
                }
            }

            else if (filterType == "Date") {
                console.log("Not implemented yet...");
                break;
            }

            else {
                console.log("Oh no, something went wrong when reading the filter type into the filterView!");
                break;
            }

            var rList = this.props.rangeList[colName].rangeList;
            
            for (var i = 0; i < rList.length; i ++) {
                if (rList[i][3] == true) {
                    rCount++;
                }
            }

            viewList.push([colName, filterType, min, max, eList[colName].selectedValues.length, rCount]);
        }

        
        //console.log(viewList);

        var view = viewList.map( function(view) {
            return (<FilterViewRow 
                        view = { view } 
                        onDelEvent = { rowDel.bind(this) } 
                        key = { view[0] } 
                    />)
        });

        return (
            <div>
                {/* Displays the mapped ranges*/}



                        <Flex divider />  

                        <Flex layout="row"> 
                            <Flex flex="35">
                            </Flex>


                            <Flex flex="15">

                                <span style = {{ color: "#b2b2b2" }} > Filter </span>
                                

                            </Flex>

                            <Flex flex="15"  >
                                <span style = {{ color: "#b2b2b2" }} > Type </span>

                            </Flex>

                            <Flex flex="20">
                                <span style = {{ color: "#b2b2b2" }} > Min </span>
                                
                            </Flex>
                            
                            <Flex flex="5">
                                <span style = {{ color: "#b2b2b2" }} > Max </span>
                                
                            </Flex>
                        </Flex>

                        <Flex divider /> 
                        <Divider />
                        <Flex divider /> 

                                

                        {view}
                        


                
            </div>
        );
    }
}


class FilterViewRow extends React.Component {

    onDelEvent() {
        this.props.onDelEvent(this.props.range[2]);
    }

    render() {
        return (
            <div>

                

                <Flex layout="row" style = {{  height: "40px" }}>
                    <Flex divider />  

                    <Flex flex="5">
                        <FontIcon
                            //onClick = { this.onDelEvent.bind(this) }
                            className = "fa fa-trash fa-2x"
                            hoverColor = { red500 }
                            //style = { styleSet.iconStyles }
                        />
                    </Flex>
                    <Flex divider /> 

                    <Flex flex="15">
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
                                        //style = { styleSet.iconStyles }
                                    />
                                </Badge>
                            </Flex>

                            <Flex divider /> 
                            <Flex divider /> 

                            <Flex flex="50">
                                <Badge
                                    badgeContent={this.props.view[5]}
                                    primary={true}
                                    style = {{ padding: "0px 0px 0px 0px" }}
                                    badgeStyle = {{ width: "20px", height: "20px", top: "-10px", right: "-13px" }}
                                    >
                                    <FontIcon
                                        //onClick = { this.onDelEvent.bind(this) }
                                        className = "fa fa-sliders fa-2x"
                                        //style = { styleSet.iconStyles }
                                    />
                                </Badge>
                            </Flex>
                        </Flex> 
                    </Flex>

                    <Flex divider /> 
                    <Flex divider /> 


                    <Flex flex="80">
                        <Flex layout="row" style = {{ display: "table" }}> 
                            <Flex flex="30" style = {{ display: "table-cell", verticalAlign: "middle" }}>

                                 <Textfit mode="single">
                                     {this.props.view[0]}
                                </Textfit>
                                
                                

                            </Flex>

                            <Flex divider /> 

                            <Flex flex="20"  style = {{ display: "table-cell", verticalAlign: "middle" }}>
                                {this.props.view[1]}

                            </Flex>

                             <Flex divider /> 
                             <Flex divider /> 

                            <Flex flex="25" style = {{ display: "table-cell", verticalAlign: "middle" }}>
                                {this.props.view[2]}
                                
                            </Flex>
                            
                            <Flex flex="25" style = {{ display: "table-cell", verticalAlign: "middle" }}>
                                {this.props.view[3]}
                                
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
 * constants defined to make dispatching for the redux store consistent
 **/
export const removeAllRange = (colName) => ({
    type: 'REMOVE_RANGE',
    colName
});
export const removeAllElastic = (colName) => ({
    type: 'REMOVE_RANGE',
    colName
});
export const unhighlightAllElastic = (colName) => ({
    type: 'ADD_RANGE',
    colName
});


/**
 * maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    elasticList: state.filterState.Filter.Elastic,
    rangeList: state.filterState.Filter.Ranges,
  }
}


/**
 * connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(FilterViewForm);
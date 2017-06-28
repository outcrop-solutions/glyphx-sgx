import React from 'react';
import FontIcon from 'material-ui/FontIcon';
import { Flex } from 'react-flex-material';
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
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
            <div style = {{ overflow: 'auto', width: '100%' }} >
                <FilterViewTable
                    onRowDel = { this.handleRowDel.bind(this) }
                    elasticList = { this.props.elasticList }
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
            var foundElastic = false;
            var foundRange = false;

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

                foundElastic = true;

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

                foundRange = true;

                for (var i = 0; i < eList[colName].highlightedValues.length; i++) {
                    if (eList[colName].highlightedValues[i] < min) {
                        min = eList[colName].highlightedValues[i];
                    }
                    else if (eList[colName].highlightedValues[i] > max) {
                        max = eList[colName].highlightedValues[i];
                    }
                }
            }

            viewList.push([colName, filterType, min, max, foundElastic, foundRange]);
        }

        
        console.log(viewList);

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
            <Card>
                <CardText 
                    style = {{
                        padding: '0px' 
                    }} 
                >
                    <Flex layout="row">      

                        <Flex divider />  

                        <Flex flex="10">
                            <FontIcon
                                //onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash fa-2x"
                                //style = { styleSet.iconStyles }
                            />
                        </Flex>

                        <Flex flex="10">
                            <FontIcon
                                //onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-list-ul fa-2x"
                                //style = { styleSet.iconStyles }
                            />
                            <FontIcon
                                //onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-sliders fa-2x"
                                //style = { styleSet.iconStyles }
                            />
                        </Flex>

                        <Flex flex="30">
                            {this.props.view[0]}

                            {/* 
                            <TextSlider
                                minVal = { this.props.minVal }
                                maxVal = { this.props.maxVal }
                                onSlide = { this.props.onSlide }
                                onTextChange = { this.props.onTextChange }
                                onTextBlur = { this.props.onTextBlur }
                                cellData = {{
                                    id: this.props.range[2],
                                    min: this.props.range[0],
                                    max: this.props.range[1]
                                }}
                            />
                            */}

                        </Flex>

                        <Flex flex="10">
                            {this.props.view[1]}

                        </Flex>

                        <Flex flex="20">
                            {this.props.view[2]}
                            
                        </Flex>
                        
                        <Flex flex="20">
                            {this.props.view[3]}
                            
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
  }
}


/**
 * connects the RangeForm component to the redux store
 **/
export default connect(mapStateToProps)(FilterViewForm);
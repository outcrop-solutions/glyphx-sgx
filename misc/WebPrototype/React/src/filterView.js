import React from 'react';
import TextField from 'material-ui/TextField';
import Toggle from 'material-ui/Toggle';
import FontIcon from 'material-ui/FontIcon';
import { Flex } from 'react-flex-material';
import Range from 'rc-slider/lib/Range'; 
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import 'font-awesome/css/font-awesome.css';


class FilterViewForm extends React.Component {

    handleRowDel() {
        this.props.dispatch(removeRange(this.props.colName, id));
    };


    render() {
        return (
            <div>
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

        var range = rList.map( function(range) {
            return (<FilterViewRow 
                        range = { range } 
                        onDelEvent = { rowDel.bind(this) } 
                        key = { range[2] } 
                    />)
        });

        return (
            <div>
                {/* Displays the mapped ranges*/}
                {range}
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
                <CardText>
                    <Flex layout="row">      

                        <Flex divider />  

                        <Flex flex="10">
                            <FontIcon
                                //onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash fa-2x"
                                style = { styleSet.iconStyles }
                            />
                        </Flex>

                        <Flex flex="80">
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
                        </Flex>

                        <Flex divider />

                        <Flex flex="10"
                            style = {{
                                margin: "11px 0px 0px -11px"
                            }} 
                        >
                            <Toggle 
                                name = "applied" 
                                id = { this.props.range.id } 
                                toggled = { this.props.range[3] } 
                                onToggle = {  
                                    (e) => this.props.onToggle(e, this.props.range[2])
                                }
                            />
                        </Flex>

                        <Flex divider />

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
        width: "60px"
    }
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
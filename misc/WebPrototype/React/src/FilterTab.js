import React from 'react';
import {Tabs, Tab} from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import FontIcon from 'material-ui/FontIcon';
import FilterTable from './FilterTable.js';
import NumberRangeTable from './NumberRange.js';
import TextRangeTable from './TextRange.js';
import { connect } from 'react-redux';

class FilterTabs extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            slideIndex: 0, //0 is Elastic, 1 is Range
            Range: null,
            tableData: props.data,
            tableID: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
            rangeID: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36)
        };
    };

    componentDidMount() {
        /*Events.scrollEvent.register('begin', function(to, element) {
        console.log("begin", arguments);
        });
    
        Events.scrollEvent.register('end', function(to, element) {
        console.log("end", arguments);
        });
        
        scrollSpy.update();
        */

    };


    active = "RANGE";

    handleChange = (value,context) => {
        context.setState({
            slideIndex: value
        });

        if (this.active === "ELASTIC") {
            this.active = "RANGE";
        }
        else {
            this.active = "ELASTIC";
        }
        
        if (this.active === "ELASTIC") {
            if (document.getElementById(this.state.rangeID)) {
                document.getElementById(this.state.tableID).style.maxHeight = "0px";
                document.getElementById(this.state.rangeID).style.maxHeight = "440px";
            }
        }
        else {
            if (document.getElementById(this.state.tableID)) {
                document.getElementById(this.state.rangeID).style.maxHeight = "0px";
                document.getElementById(this.state.tableID).style.maxHeight = "440px";
            }
        }

    };


    render() {
        return (
            <div>
                <Tabs
                    onChange = { (value) => this.handleChange(value,this) }
                    value = { this.state.slideIndex }
                >
                    <Tab 
                        label = "Elastic" 
                        value = { 0 }
                        icon = { <FontIcon className="fa fa-list-ul" ></FontIcon> }
                        buttonStyle = { {height: "55px", backgroundColor: this.props.settings.filterTabColor.tabBackground, color: this.props.settings.filterTabColor.titleText} }
                    />
                    <Tab 
                        label = "Range" 
                        value = { 1 }
                        icon = { <FontIcon className="fa fa-sliders"></FontIcon> }
                        buttonStyle = { {height: "55px", backgroundColor: this.props.settings.filterTabColor.tabBackground, color: this.props.settings.filterTabColor.titleText} }
                    />
                </Tabs>

                

                <SwipeableViews
                    index = { this.state.slideIndex }
                    onChangeIndex = { this.handleChange }

                    style = {{
                        overflowY: "hidden",
                    }}           
                >
                    <div
                        id = { this.state.tableID }
                        style = {{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <FilterTable internalColName={this.props.internalColName} displayName={this.props.displayName} tableData={this.state.tableData} id={this.props.id} selectedRows={this.state.FilterTableSelectedRows}></FilterTable>
                    </div>
                    <div
                        id = { this.state.rangeID }
                        style = {{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        {this.props.filterList[this.props.id].type === "Number" ? 
                                <NumberRangeTable 
                                    colName = { this.props.id } 
                                    data = { this.state.tableData } 
                                    minVal = { this.props.filterList[this.props.id].bounds[0] } 
                                    maxVal = { this.props.filterList[this.props.id].bounds[1] }
                                /> : 
                            (this.props.filterList[this.props.id].type === "Text" ? 
                                    <TextRangeTable 
                                        colName = { this.props.id } 
                                        data = { this.state.tableData } 
                                    /> :  
                            "") 
                        }
                        

                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

/**
 * maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    settings: state.filterState.Settings
  }
}


/**
 * connects the FilterTabs component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(FilterTabs);

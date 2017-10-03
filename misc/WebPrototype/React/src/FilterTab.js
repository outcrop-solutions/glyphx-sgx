import React from 'react';
import {Tabs, Tab} from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import ScrollIntoView from 'scroll-into-view-if-needed';
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

    mouseIn = () => {
        var elements = document.getElementsByClassName(this.props.id + "-R");
        console.log(elements[0].scrollHeight)
        if (elements[0].scrollHeight > 345) {
            var scrollDiv = document.getElementsByClassName("sidenavbar");
            scrollDiv[0].setAttribute("class", "sidenavbar disableScroll");
        }
        
    }

    mouseOut = () => {        
        var scrollDiv = document.getElementsByClassName("sidenavbar");
        scrollDiv[0].setAttribute("class", "sidenavbar enableScroll");
    }

    handleChange = (value) => {

        this.setState({ slideIndex: value }, function() {
            console.log(value);
            if (value === 0) {
                if (document.getElementById(this.state.tableID)) {
                    document.getElementById(this.state.rangeID).style.maxHeight = "0px";
                    document.getElementById(this.state.tableID).style.maxHeight = "393px";
                    this.scroll(this.state.tableID);
                }
            }
            else {
                if (document.getElementById(this.state.rangeID)) {
                    document.getElementById(this.state.tableID).style.maxHeight = "0px";
                    document.getElementById(this.state.rangeID).style.maxHeight = "345px";
                }
            }
        });

    };


    /**
     * This function will bring the scroll to that element.
     * @param {String/Object} element: this is the name of the column
     */
    scroll = (element) => {
        if (typeof element == 'string') {
            element = document.getElementById(element);
        }

        if (element) {
            ScrollIntoView(element, false, { duration: 150 });
        }
        else
            return false;
    }


    render() {
        return (
            <div>
                <Tabs
                    onChange = { (value) => this.handleChange(value, this) }
                    value = { this.state.slideIndex }
                >
                    <Tab 
                        label = "Elastic" 
                        value = { 0 }
                        icon = { <FontIcon style = {{ color: (this.state.slideIndex === 0 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") }} className = "fa fa-list-ul" ></FontIcon> }
                        buttonStyle = { {height: "49px", backgroundColor: (this.state.slideIndex === 0 ? "#353657" : this.props.settings.colors.filterTabColor.tabBackground), color: (this.state.slideIndex === 0 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") } }
                    />
                    <Tab 
                        label = "Range" 
                        value = { 1 }
                        icon = { <FontIcon style = {{ color: (this.state.slideIndex === 1 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") }} className="fa fa-sliders"></FontIcon> }
                        buttonStyle = { {height: "49px", backgroundColor: (this.state.slideIndex === 1 ? "#353657" : this.props.settings.colors.filterTabColor.tabBackground), color: (this.state.slideIndex === 1 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") } }
                    />
                </Tabs>

                <SwipeableViews
                    index = { this.state.slideIndex }
                    onChangeIndex = { this.handleChange }

                    style = {{
                        overflowY: "hidden",
                        padding: "0px"
                    }}           
                >
                    <div
                        id = { this.state.tableID }
                        style = {{
                            maxHeight: "393px",
                            overflow: "hidden"
                        }}
                    >
                        <FilterTable 
                            internalColName = { this.props.internalColName } 
                            displayName = { this.props.displayName } 
                            tableData = { this.state.tableData } 
                            id = { this.props.id } 
                            selectedRows = { this.state.FilterTableSelectedRows } 
                            tableID = { this.state.tableID }
                        />
                    </div>
                    <div
                        id = { this.state.rangeID }
                        style = {{
                            maxHeight: "355px",
                            overflowX: "hidden",
                            marginTop: "17px"
                        }}
                        onMouseEnter = { this.mouseIn }
                        onMouseLeave = { this.mouseOut }
                        className = {this.props.id + "-R"}
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
                            "TODO: Add Date Range Here") 
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

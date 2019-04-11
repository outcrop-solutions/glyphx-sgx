import React from 'react';
import { connect } from 'react-redux';
import { Tabs, Tab } from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import ScrollIntoView from 'scroll-into-view-if-needed';
import FontIcon from 'material-ui/FontIcon';
import FilterTable from './FilterTable.js';
import NumberRangeTable from './NumberRange.js';
import TextRangeTable from './TextRange.js';



/**
 * Tabs which contain the elastic table and ranges for a given column.
 * @param internalColName: unformatted column name.
 * @param displayName: column name formatted for display.
 * @param id: ID corresponding to that column.
 * @param data: data corresponding to that column.
 * @param open: determines if the tab should render (not to slow down the page).
 */
class FilterTabs extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            slideIndex: 0, //0 is Elastic, 1 is Range
            Range: null,
            tableData: props.data,
            tableID: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
            rangeID: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
        };
    };


    componentWillReceiveProps(nextProps) {
        if(this.props.data !== nextProps.data){
            this.setState({tableData: nextProps.data});
        }
    }


    /**
     * Hide the main scrollbar when hovering inside the range table while the range table has a scrollbar itself
     * Only hides if the setting is enabled
     */
    mouseIn = () => {
        if (this.props.settings.hideScrollHover) {
            var elements = document.getElementsByClassName(this.props.id + "-R");
            if (elements[0].scrollHeight > 345) {
                var scrollDiv = document.getElementsByClassName("sidenavbar");
                scrollDiv[0].setAttribute("class", "sidenavbar disableScroll");
            }
        } 
    }


    /**
     * Show the main scrollbar when exiting from the hover
     */
    mouseOut = () => {
        if (this.props.settings.hideScrollHover) {
            var scrollDiv = document.getElementsByClassName("sidenavbar");
            scrollDiv[0].setAttribute("class", "sidenavbar enableScroll");
        }
    }


    /**
     * Enables the switching between tabs through the state
     * @param value: 
     */
    handleChange = (value) => {
        this.setState({ slideIndex: value }, 
            function() {
                if (value === 0) {
                    if (document.getElementById(this.state.tableID)) {
                        document.getElementById(this.state.rangeID).style.maxHeight = "0px";
                        document.getElementById(this.state.tableID).style.maxHeight = "1000px";
                        this.scroll(this.state.tableID);
                    }
                }

                else {
                    if (document.getElementById(this.state.rangeID)) {
                        document.getElementById(this.state.tableID).style.maxHeight = "0px";
                        document.getElementById(this.state.rangeID).style.maxHeight = "345px";
                    }
                }
            }
        );
    };


    /**
     * This function will bring the scroll to that element.
     * @param {String/Object} element: this is the name of the column
     */
    scroll = (element) => {
        if (typeof element === 'string') {
            element = document.getElementById(element);
        }

        if (element) {
            ScrollIntoView(element, false, { duration: 150 });
        }

        else {
            return false;
        }
    }


    render() {
        return (
            <div>
                <div style = {{ margin: "-5px 0px -4px" }} >
                    <Tabs onChange = { (value) => this.handleChange(value, this) } value = { this.state.slideIndex } inkBarStyle = {{ display: "none" }} >
                        <Tab 
                            label = { 
                                <div style =  {{ width: "100%", overflow: "hidden", margin: "1px 0px 0px" }} >
                                    <div style = {{ width: "88px", margin: "0 auto" }} >
                                        <FontIcon style = {{ width: "20px", float: "left", fontSize: "20px", margin: "-1px 0px 0px", color: (this.state.slideIndex === 0 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") }} className = "fa fa-list-ul" ></FontIcon> 
                                        <div style = {{ marginleft: "50px" }} >Elastic</div>
                                    </div>
                                </div>
                            }
                            value = { 0 }
                            buttonStyle = { {height: "35px", backgroundColor: (this.state.slideIndex === 0 ? "#353657" : this.props.settings.colors.filterTabColor.tabBackground), color: (this.state.slideIndex === 0 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") } }
                        />
                        <Tab 
                            label = { 
                                <div style =  {{ width: "100%", overflow: "hidden" }} >
                                    <div style = {{ width: "80px", margin: "0 auto" }} >
                                        <FontIcon style = {{ width: "20px", float: "left", fontSize: "20px", margin: "-2px 0px 0px", color: (this.state.slideIndex === 1 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") }} className = "fa fa-sliders" ></FontIcon> 
                                        <div style = {{ marginleft: "50px" }} >Range</div>
                                    </div>
                                </div>
                            }
                            value = { 1 }
                            buttonStyle = { {height: "35px", backgroundColor: (this.state.slideIndex === 1 ? "#353657" : this.props.settings.colors.filterTabColor.tabBackground), color: (this.state.slideIndex === 1 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") } }
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
                        <div id = { this.state.tableID } style = {{ maxHeight: "1000px", overflow: "hidden" }} >
                            <div 
                                style = {{ 
                                    backgroundColor: "rgba(128, 128, 128, 0.36)", 
                                    height: "100%", 
                                    position: "absolute", 
                                    width: "100%", 
                                    zIndex: "10", 
                                    display: (this.props.filterBusy ? "" : "none") 
                                }} 
                            ></div>
                            <FilterTable 
                                internalColName = { this.props.internalColName } 
                                displayName = { this.props.displayName } 
                                tableData = { this.state.tableData } 
                                id = { this.props.id } 
                                selectedRows = { this.state.FilterTableSelectedRows } 
                                refreshTableDataOnRowSelection = { (colName, selections) => this.props.refreshTableDataOnRowSelection(colName, selections) }
                                tableID = { this.state.tableID }
                                setFilterIDs = { this.props.setFilterIDs }
                                fullTableData = { this.props.fullTableData }
                                setTableData = { this.props.setTableData }
                                setFilterBusy = { this.props.setFilterBusy }
                            />
                        </div>

                        <div
                            id = { this.state.rangeID }
                            className = {this.props.id + "-R"}
                            onMouseEnter = { this.mouseIn }
                            onMouseLeave = { this.mouseOut }
                            style = {{
                                maxHeight: "0px",
                                overflowX: "hidden",
                                marginTop: "5px"
                            }}
                        >
                            <div 
                                style = {{ 
                                    backgroundColor: "rgba(128, 128, 128, 0.36)", 
                                    height: "100%", 
                                    position: "absolute", 
                                    width: "100%", 
                                    zIndex: "10", 
                                    display: (this.props.filterBusy ? "" : "none") 
                                }} 
                            ></div>
                            {this.props.filterList[this.props.id].type === "Number" ? 
                                <NumberRangeTable 
                                    colName = { this.props.id } 
                                    data = { this.state.tableData.flatValues } 
                                    minVal = { this.props.filterList[this.props.id].bounds[0] } 
                                    refreshTableDataOnRowSelection = { (colName, selections) => this.props.refreshTableDataOnRowSelection(colName, selections) }
                                    maxVal = { this.props.filterList[this.props.id].bounds[1] }
                                    fullTableData = { this.props.fullTableData }
                                    setFilterIDs = { this.props.setFilterIDs }
                                    setTableData = { this.props.setTableData }
                                    setFilterBusy = { this.props.setFilterBusy }
                                /> 
                                : 
                                (this.props.filterList[this.props.id].type === "Text" ? 
                                    <TextRangeTable 
                                        colName = { this.props.id } 
                                        refreshTableDataOnRowSelection = { (colName, selections) => this.props.refreshTableDataOnRowSelection(colName, selections) }
                                        data = { this.state.tableData.flatValues } 
                                        fullTableData = { this.props.fullTableData }
                                        setFilterIDs = { this.props.setFilterIDs }
                                        setTableData = { this.props.setTableData }
                                        setFilterBusy = { this.props.setFilterBusy }
                                    /> 
                                    :  
                                    "TODO: Add Date Range Here"
                                ) 
                            }
                            
                        </div>
                    </SwipeableViews>
                </div>
            </div>
        );
    }
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    settings: state.filterState.Settings
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(FilterTabs);


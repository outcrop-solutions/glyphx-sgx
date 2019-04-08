import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import { makeServerCall } from './ServerCallHelper.js';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import Badge from 'material-ui/Badge';
import Flexbox from 'flexbox-react';
import Tooltip from 'rc-tooltip';
import Promise from 'bluebird';
import 'rc-tooltip/assets/bootstrap.css';
import './css/General.css';


/**
 * Overview of filter selections made, can jump to a selection and remove all selections from a column.
 * @param onScroll: -ADCMT
 **/
class FilterSummaryView extends React.Component {

    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    // shouldComponentUpdate(nextProps, nextState) {
    //     for (var colName in this.props.filterList) {
    //         console.log(nextProps.filterList[colName],'filter list in filterSummaryView', colName, 
    //         'filter list in filterSummaryView',nextProps.filterList[colName].selectedValues,
    //              'filter list in filterSummaryView');
    //         // debugger;
    //             if ((this.props.filterList[colName].selectedValues) != 
    //             (nextProps.filterList[colName].selectedValues) 
    //             || (this.props.filterList[colName].rangeList) != 
    //             (nextProps.filterList[colName].rangeList)) {
    //                 return true;
    //             }
    //     }
    //     if (this.props.settings != nextProps.settings || this.props.tableData != nextProps.tableData) {
    //         return true;
    //     }
        
        
    //     return false;
    // }


    /**
     * Unselects all ranges and removes all elastic selections from a column through a redux dispatch
     * @param colName: name of the corresponding column to be deleted
     **/
    handleRowDel(colName) {
        //this.props.dispatch(removeFilterView(colName));
        
        var context = this;

        // let pom = new Promise(function (resolve, reject) {
        //     context.props.dispatch(removeFilterView(colName));
        //     resolve('done');
        // });

        // pom.then(() => this.props.refreshParent()).then(() => this.applyFilter());

        let pom = new Promise(function (resolve, reject) {
                context.props.dispatch(removeFilterView(colName));
                resolve('done');
        });

        pom.then(() => context.props.refreshParent()).then(() => context.applyFilter()).then(() => context.addToHistory());
    };


    /**
     * Unselects all ranges and removes all elastic selections from a column through a redux dispatch
     * @param colName: name of the corresponding column to be deleted
     **/
    handleRowDelForClearAll(colName) {
        this.props.dispatch(removeFilterView(colName));
    };


    addToHistory() {
        var undoRedoHistory = {
            history: this.props.UndoRedoHistory.history.slice(0),
            position: this.props.UndoRedoHistory.position
        }

        // debugger;

        if (undoRedoHistory.position !== undoRedoHistory.history.length - 1) {
            undoRedoHistory.history = undoRedoHistory.history.slice(0, undoRedoHistory.position + 1);
        }

        undoRedoHistory.history.push({filterList: this.props.filterList, tableData: this.props.tableData});
        undoRedoHistory.position = undoRedoHistory.position + 1;

        this.props.dispatch(editUndoRedoHistory(undoRedoHistory));
    }


    /**
     * - ADCMT
     */
    applyFilter = () => {
        //console.log('Filter Applied');
        var iframe = document.getElementById('GlyphViewer').contentWindow;

        var context = this;

        makeServerCall('applyFilters',
            function(result, b) {
                var resultJson = JSON.parse(result);
                // debugger;
                var data = resultJson.data;
                var tempRowIds = [];
                
				if (data && Array.isArray(data)) {
					if (data.length > 0) {							
						for (var index = 0; index < data.length; index++) {
							tempRowIds.push(parseInt(Object.values(data[index]).toString(), 10));
						}
					}
					else {
						// No data was matched.
						console.log('NO MATCH');
					}
				}
				
                // Change this to be located in the store
                //context.setState({ filterIDs: tempRowIds, hideShowButtonTextFlag: true });

                iframe.filterGlyphs(tempRowIds);

                context.props.dispatch( setTimer(new Date().getTime()) );
            },
            {post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.filterList } 
            }
        );
    };


    render() {
        var rowDel = this.handleRowDel.bind(this);
        var rowDelForClearAll = this.handleRowDelForClearAll.bind(this);
        var filterList = this.props.filterList;
        var viewList = [];

        // Find the min and max values selected bsed on the type (Text, Number, Date)
        for (var colName in filterList) {
            var filterType = filterList[colName].type;
            var min, max, i, curNum;
            var rCount = 0;

            if (filterType === "Text") {
                if (filterList[colName].selectedValues.length) {
                    min = filterList[colName].selectedValues[0];
                    max = filterList[colName].selectedValues[0];
                }
                else {
                    continue;
                }

                if ( filterList[colName].selectedValues.length) {
                    for (i = 0; i < filterList[colName].selectedValues.length; i++) {
                        if (filterList[colName].selectedValues[i] < min) {
                            min = filterList[colName].selectedValues[i];
                        }
                        else if (filterList[colName].selectedValues[i] > max) {
                            max = filterList[colName].selectedValues[i];
                        }
                    }
                }

            }

            else if (filterType === "Number") {
                if (filterList[colName].selectedValues.length) {
                    min = parseFloat(filterList[colName].selectedValues[0], 10);
                    max = parseFloat(filterList[colName].selectedValues[0], 10);
                }
                else {
                    continue;
                }


                for (i = 0; i < filterList[colName].selectedValues.length; i++) {
                    curNum = parseFloat(filterList[colName].selectedValues[i], 10);
                    if (curNum < min) {
                        min = curNum;
                    }
                    else if (curNum > max) {
                        max = curNum;
                    }
                }
            }

            else if (filterType === "Date") {
                //console.log("Not implemented yet...");
                break;
            }

            else {
                console.log("Oh no, something went wrong when reading the filter type into the filterView:" + filterType);
                break;
            }

            // Find how many ranges are selected
            var rList = this.props.filterList[colName].rangeList;
            for (i = 0; i < rList.length; i ++) {
                if (rList[i][3] === true) {
                    rCount++;
                }
            }
            viewList.push([filterList[colName].displayName, filterType, min, max, filterList[colName].selectedValues.length, rCount, colName]);
        }

        var context = this;

        var view = viewList.map( function(view) {
            return (
                <FilterViewRow 
                    view = { view } 
                    onDelEvent = { rowDel.bind(this) } 
                    onDelEventForClearAll = { rowDelForClearAll.bind(this) }
                    key = { view[0] }
                    ref = { view[0] }
                    settings = { context.props.settings }
                    onScroll = { (element, elastic) => context.props.onScroll(element, elastic) }
                />
            )
        });

        return (
            <Card style = {{ overflow: 'auto', width: '100%', padding: "0px", maxHeight:'200px', borderRadius: "5px" }} containerStyle = {{ padding: "0px", borderRadius: "5px" }}>
                <CardText style = {{ padding: "3px 8px", borderRadius: "5px" }} >
                    <div>

                        <Flexbox flexDirection = "row" >
                            <Flexbox style = {{ width: "100%" }} > 
                                <span style = {{ color: "#000000", width: "123px" }} > Options </span>
                            </Flexbox>
                            <Flexbox style = {{ width: "100%" }} > 
                                <span style = {{ color: "#000000", width: "106px"}} > Filter </span>
                            </Flexbox>
                            <Flexbox style = {{ width: "100%" }} > 
                                <span style = {{ color: "#000000", width: "84px" }} > Min </span>
                            </Flexbox>
                            <Flexbox style = {{ width: "100%" }} > 
                                <span style = {{ color: "#000000" }} > Max </span>
                            </Flexbox>
                        </Flexbox>

                        <Divider style = {{ marginBottom: "0px", backgroundColor: "#000000" }} />

                        {/* Displays the mapped views*/}
                        {view.length > 0 ? view : <div className = "centerText cursorNormal"><h3 style = {{ margin: "13px 0px 12px" }} > No Filters Selected </h3></div>}

                    </div>
                </CardText>
            </Card>
        );
    }
}


/**
 * Defines a row for the FilterSummaryView.
 **/
class FilterViewRow extends React.Component {


    /**
     * -ADCMT
     **/
    onDelEvent() {
        this.props.onDelEvent(this.props.view[6]);
    }

    onDelEventForClearAll() {
        this.props.onDelEventForClearAll(this.props.view[6]);
    }


    /**
     * -ADCMT
     * @param evt: -ADCMT
     * @param context: -ADCMT
     * @param elastic: -ADCMT
     **/
    onClickIcon(evt, context, elastic){
        context.props.onScroll(context.props.view[6],elastic);
    }


    render() {

        // Shorten display text if they're too long
        var displayName = this.props.view[0];
        var min = this.props.view[2].toString();
        var max = this.props.view[3].toString();

        if (displayName.length > 13) {
            displayName = displayName.substring(0,12) + "...";
        }

        if (min.length > 10) {
            min = min.substring(0,9) + "...";
        }

        if (max.length > 10) {
            max = max.substring(0,9) + "...";
        }

        return (
            <div >
                <Flexbox flexDirection = "row" style = {{ minHeight: "32px", marginTop: "11.5px" }} >

                        <div style = {{ width: "31px" }} >
                            <FontIcon
                                onClick = { this.onDelEvent.bind(this) }
                                className = "fa fa-trash cursorHand"
                                hoverColor = { this.props.settings.colors.filterOverviewColor.deleteHover }
                            />
                        </div>

                    <div style = {{ width: "41px" }} >
                        <Badge
                            badgeContent = { this.props.view[4] }
                            primary = { true }
                            style = {{ padding: "0px 0px 0px 0px" }}
                            badgeStyle = {{ 
                                width: "20px", 
                                height: "20px", 
                                top: "-10px", 
                                right: "-13px", 
                                backgroundColor: this.props.settings.colors.filterOverviewColor.badgeBackground, 
                                color: this.props.settings.colors.filterOverviewColor.badgeText 
                            }}
                        >
                            <FontIcon
                                onClick = { (evt) => this.onClickIcon(evt, this, true) }
                                className = "fa fa-list-ul cursorHand"
                                hoverColor = { this.props.settings.colors.filterOverviewColor.elasticHover }
                            />
                        </Badge>
                    </div>

                    <div style = {{ width: "51px" }} >
                        <Badge
                            badgeContent = { this.props.view[5] }
                            primary = { true }
                            style = {{ padding: "0px 0px 0px 0px" }}
                            badgeStyle = {{ 
                                width: "20px", 
                                height: "20px", 
                                top: "-10px", 
                                right: "-13px", 
                                backgroundColor: this.props.settings.colors.filterOverviewColor.badgeBackground, 
                                color: this.props.settings.colors.filterOverviewColor.badgeText 
                            }}
                        >
                            <FontIcon
                                onClick = { (evt) => this.onClickIcon(evt, this, false) }
                                className = "fa fa-sliders cursorHand"
                                hoverColor = { this.props.settings.colors.filterOverviewColor.rangeHover }
                            />
                        </Badge>
                    </div>

                    <div style = {{ width: "106px" }} >
                        <Tooltip
                            placement = 'bottom'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { <div> {this.props.view[0]} </div> }
                        >
                            <span style = {{  wordWrap: "break-word", display: "block" }} >
                                {displayName}
                            </span> 
                        </Tooltip>
                    </div>

                   <div style = {{ width: "92.5px" }} >
                        <Tooltip
                            placement = 'bottom'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { <div> {this.props.view[2]} </div> }
                        >
                            <span style = {{  wordWrap: "break-word" }} >
                                {min}
                            </span>
                        </Tooltip>
                    </div>
      
                    <Tooltip
                        placement = 'bottom'
                        mouseEnterDelay = { 0.5 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlay = { <div> {this.props.view[3]} </div> }
                    >
                        <span style = {{  wordWrap: "break-word" }} >
                            {max}
                        </span>
                    </Tooltip>
         
                </Flexbox>
                <Divider />
            </div>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const removeFilterView = (colName) => ({
    type: 'REMOVE_FILTER_VIEW',
    colName
});

export const editUndoRedoHistory = (undoRedoHistory) => ({
  type: 'UPDATE_HISTORY',
  undoRedoHistory
});

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    settings: state.filterState.Settings,
    VizParams: state.filterState.VizParams,
    UndoRedoHistory: state.filterState.UndoRedoHistory
  }
}


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(FilterSummaryView);
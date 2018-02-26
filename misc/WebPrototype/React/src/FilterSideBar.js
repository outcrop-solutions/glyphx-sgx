import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { makeServerCall } from './ServerCallHelper.js';
import RaisedButton from 'material-ui/RaisedButton';
import Flexbox from 'flexbox-react';
import AlertContainer from 'react-alert';
import Collapsible from 'react-collapsible';
import FilterSideBarTopView from './FilterSideBarTopView.js';
import FilterSideBarBottomView from './FilterSideBarBottomView.js';
import UndoRedoChangeListener from './UndoRedoChangeListener.js';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './FilterSideBar.css';


/**
 * This is the side bar that you see on the right side of the glyph viewer screen.
 */
class FilterSideBar extends React.Component {

    constructor(props) {
        super(props);
        
        this.loadVisualization = this.loadVisualization.bind(this);
        this.refreshTableDataOnRowSelection = this.refreshTableDataOnRowSelection.bind(this);
        //Store the states of all the elements inside this data structure.
        this.state  = {
            tableData: {},
         	topViewInitParams: {
                scrollToElement: this.scrollToCollapsibleElement
			},
            topViewVisible: true,
            filterIDs: null
        };
    };

    componentDidMount() {
        var collapsibles = document.getElementsByClassName('Collapsible__trigger');
        var context = this;
        for (var i = 0; i < collapsibles.length; i++) {
            collapsibles[i].style.setProperty('--collapsible-text-color-main', this.props.settings.colors.collapsibleColor.mainText);
            collapsibles[i].style.setProperty('--collapsible-text-color-sub', this.props.settings.colors.collapsibleColor.subText);
        }
        this.loadVisualization();
   };

   loadVisualization() {
        var context = this;
        makeServerCall('loadVisualization',
            function(res,b,c) {
            // Hide the loadmask.
                
                if (typeof res == 'string') {
                    res = JSON.parse(res);
                }
                
                if (Array.isArray(res.data) && res.data.length > 0) {
                    var result = context.convertToCompatibleDataObject(res.data);
                    context.makeFilterStructure(result);
                    context.setState({ tableData: result });
                    context.props.dispatch(setStatData(result));
                    context.props.updateViz(res.glyphViewerKey);
                }
                else {
                    // 0 records matched.
                    console.log('none matched');
                }
            },
            {
                post: true, 
                data:  { 
                    tableName: context.props.VizParams.tableName, 
                    query: context.props.VizParams.query, 
                    sdtPath: context.props.VizParams.sdtPath, 
                    datasourceId: context.props.VizParams.datasourceId ,
                    filterAllowedColumnList: context.props.VizParams.filterAllowedColumnList
                }
            }
        )
   }

   
   refreshTableDataOnRowSelection = (colName, selections) => {
        var fcolList = this.props.VizParams.filterAllowedColumnList.toString();
        var query = "SELECT rowid," + fcolList + " from " + this.props.VizParams.tableName;
        
        var context = this;
        var temp = "";
        var flag = false;

        // Create the query to pass
        var filterObj = this.props.filterObj;
        for (var column in filterObj) {
            if (filterObj[column].selectedValues.length > 0) {
                if (flag) {
                    query = query + " AND ";
                }
                else {
                    query = query + " WHERE ";
                }
                temp = JSON.stringify(filterObj[column].selectedValues);
                temp = temp.replace('[', '(').replace(/]$/, ")");
                query += column + " IN " + temp;
                flag = true;
            }
        }

        var URL = "fetchSelectedRowData?filterQuery=" + query; //"&selectedValues=" + sel
        debugger;

        return new Promise(function(resolve, reject) {
            var result = 'A is done'        

            // Get the data corresponding to the URL
            makeServerCall(URL,
                function (responseText) { 
                    var response = responseText;
                    if (typeof responseText === 'string') {
                        response = JSON.parse(response);
                    }
                    debugger;
                    if (response.data.length > 0) {
                        var result = context.convertToCompatibleDataObject(response.data);
                        context.setState({ tableData: result }, () => resolve(result));
                        context.props.dispatch(setStatData(result));
                    }
                    
                }
            );
        });
   }

    /**
	* This method shows a little popup alert on the left bottom screen
    * @param {string} strMsg: the message you want to be displayed.
	*/
    showAlert = (strMsg) => {
        this.msg.show(strMsg, {
            time: 5000,
            type: 'success',
            icon: <i className="fa fa-check-circle" style={{fontSize: '2.5em',color:'green'}}></i>
        })
    };


    /**
     * This method converts the server response to the way the client can consume.
     * @param {*} obj : array of records [{col1:,col2:,col3:,..},{col1:,col2:,col3:,..},...]
     * @return: 
     * {
     *  colName: {
     *      values:{
     *              v1:{
     *                  value:v1,
     *                  count: repetition of values,
     *                  recId:for future disable rows feature}},
     *              ...
     *              },
     *      totalCount: number         
     *  },   
     *  ....
     * }
     */
    convertToCompatibleDataObject(obj) {
        var resObj = (typeof obj == 'string' ? JSON.parse(obj) : obj);
        var returnObj = {};
        var resObjLen = resObj.length;
        var columnObj = null;
        var key = null;
        var record = null;
        var value = "";
        var keys = Object.keys(resObj[0]);
        var allowedColumns = this.props.VizParams.filterAllowedColumnList ? this.props.VizParams.filterAllowedColumnList : null;

        keys.forEach(
            function(val) {
                if (allowedColumns.indexOf(val) > -1) {
                    returnObj[val] = {
                        values: {},
                        totalCount: 0,
                        flatValues : []
                    }
                }
            }
        );
        
        var newKeys = Object.keys(returnObj);

        for (var recIndex = 0; recIndex < resObjLen; recIndex++) {
            for (var colIndex = 0; colIndex < newKeys.length; colIndex++) {
                key = newKeys[colIndex];    // Current column name
                record = resObj[recIndex];  // Record inside response data
                columnObj = returnObj[key]; // Column object of the returnObj
                value = record[key];        // Value inside server response data obj
                columnObj.totalCount++;

                if (columnObj.values.hasOwnProperty(value)) {
                    ++columnObj.values[value].count; 
                }
                else {
                    columnObj.flatValues.push(value);
                    columnObj.values[value] = {
                        value: value,
                        count: 1,
                        recId: record.rowid
                    };
                }
            }
        }

        return returnObj;
    }


    /**
     * This method constructs the structure of the Filter that is saved in the store and accessed everywhere throughout the applciation.
     * @param {Object} Obj: {col1:{values:{},totalCount:,flatValues:[]},col2:[data],...}
     */
    makeFilterStructure = (Obj) => {
        var returnObj = {};
        
        for (var property in Obj) {
           
            var columnObj = Obj[property];
            var columnName = property;
            var range;
            //var type = isNaN(columnObj.flatValues[0]) ||  columnObj.flatValues[0]=='' ? 'Text' : 'Number';
            var type = typeof columnObj.flatValues[0] == 'string' ? 'Text' : 'Number';

            if (type === "Number") {
                var minMax = this.findMinMax( columnObj.flatValues );
                range = [minMax.min, minMax.max, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false];
            }
            
            else if (type === "Text") {
                range = [0, 25, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false, 1, ""];
            }

            else if(columnObj.flatValues[0] instanceof Date){
                // Date range
            }

            returnObj[property] = {
                rangeList: [range],
                highlightedValues: [],
                bounds: [range[0], range[1]],
                selectedValues: [],
                pinned: false,
                type: type,
                displayName: this.generateDisplayName(columnName)
            }
        }

        this.props.dispatch(init(returnObj));
    };


    /**
     * This function replaces "_" with "<space>" and replaces "<Capital letter>" with "<space><Capital letter>" to make the display name!
     * @param str: The column name as it is.
     * @return display name string.
     */
    generateDisplayName = (str) => {
        return str.replace(new RegExp("_", 'g'), " ");

        /*
        var newString = str.charAt(0);
        var len = str.length;
        for (var i = 1; i < len; i++) {
            var ch = str.charAt(i);
            if (isNaN(ch)) {
                if (ch == "_") {
                    newString = newString + " ";
                }
                else if (ch == ch.toUpperCase()) {
                    newString = newString + " " + ch;
                }
                else {
                    newString = newString + ch;
                }
            }
        }
        return newString;
        */
    };


    /**
     * This function caculates the min and max of the value in an array.
     * @param arrValues: an array of column data.
     * @return Object: {min: <MinValue>, max:<MaxValue>}
     */
    findMinMax = (arrValues) => {
        var len = arrValues.length;
        
        var obj = {
            min: arrValues[0],
            max: arrValues[0]
        };

        for (var i = 0; i < len; i++) {
            if (arrValues[i] === '') {
                continue;
            }

            if (arrValues[i] > obj.max) {
                obj.max = arrValues[i];
            }

            if (arrValues[i] < obj.min) {
                obj.min = arrValues[i];
            }
        }

        return obj;
    };


    /**
     * This is just a link function that links the icon of the topview to the function in the bottom view.
     */
    scrollToCollapsibleElement = (element, Elastic) => {
        if (this.refs['bottom'] && this.refs['bottom'].getWrappedInstance()) {
            this.refs['bottom'].getWrappedInstance().scrollToCollapsibleElement(element, Elastic);
        }   
    }


   /**
    * This method is called when the user clicks on the 'arrow' to hide/show the top view of the filter
    * @param {object} event: the event object.
	*/
    toggleTopView = (event) => {
		var collapseTopViewButton = document.getElementById("collapseTopViewButton");
		
        if (this.refs['topCollapisble'].state.isClosed) {
			collapseTopViewButton.style.transform = '';
		    this.refs['topCollapisble'].openCollapsible();
		}

        else {
			collapseTopViewButton.style.transform = 'rotateZ(180deg)';
		    this.refs['topCollapisble'].closeCollapsible();
		}
    };


    setFilterIDs(filterIDs) {
        this.setState({ filterIDs: filterIDs });
    }

    setTableData(tableData) {
        this.setState({ tableData: tableData });
    }

    render = () => {
        var colList = Object.keys(this.state.tableData);
        return (
            <Flexbox 
                flexDirection = "column"
                flexGrow = {1}
                id = "FilterWindowOuterContiner"
                style = {{ 
                    height: "100%",
                    overflow:'hidden',
                    transition: '1s'
                }}
            >
                {/* TOP SECTION */}
                <div>
                    <AlertContainer ref = { a => this.msg = a } />
                </div>

                {/*<UndoRedoChangeListener tableData = { this.state.tableData } /> */}

                <Collapsible
                    transitionTime = {200} 
                    open = { true }
                    triggerDisabled = { true }
                    contentInnerClassName  = "Flex__layout-column topViewContent"
                    ref = 'topCollapisble'
                    triggerClassName = 'noHeaderTrigger cursorNormal'
                    triggerOpenedClassName = 'noHeaderTrigger cursorNormal'
                    contentOuterClassName = "cursorNormal"
                    overflowWhenOpen = "visible"
                >

                    <FilterSideBarTopView 
                        initParams = { this.state.topViewInitParams } 
                        colList = { colList } 
                        showAlert = { (strMsg) => this.showAlert(strMsg) }
                        reloadParent = { this.loadVisualization }
                        refreshParent = { this.refreshTableDataOnRowSelection }
                        showHideLoadingMask = { this.props.showHideLoadingMask }
                        filterIDs = { this.state.filterIDs }
                        setFilterIDs = { this.setFilterIDs.bind(this) }
                        tableData = { this.state.tableData } 
                        setTableData = { this.setTableData.bind(this) }
                    />

                </Collapsible>
            
                {/* COLLAPSE TOP SECTION BUTTON */}
                <RaisedButton 
                    fullWidth = { true } 
                    primary = { true } 
                    onClick = { this.toggleTopView.bind(this) }
                    buttonStyle = {{ backgroundColor: this.props.settings.colors.hideTopViewButtonColor.background, width: "448px" }}
                    style = {{ height: '20px', margin: "0px 0px 1px 1px" }}
                >
                    <i 
                        id = "collapseTopViewButton" 
                        className = "fa fa-caret-up" 
                        style = {{
                            fontSize: '1.6em',
                            color: this.props.settings.colors.hideTopViewButtonColor.icon,
                            transition: 'transform 500ms'
                        }}
                    /> 
                </RaisedButton>

                {/* BOTTOM SECTION */}
                
                <FilterSideBarBottomView 
                    ref = "bottom" 
                    tableData = { this.state.tableData } 
                    refreshTableDataOnRowSelection={(colName,selections) => this.refreshTableDataOnRowSelection(colName,selections)} 
                    setFilterIDs = { this.setFilterIDs.bind(this) }
                    setTableData = { this.setTableData.bind(this) }
                />

            </Flexbox>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const init = (storeFilterStruc) => ({
  type: 'INIT',
  storeFilterStruc
});

export const setStatData = (statData) => ({
    type: 'SET_STATISTICS_DATA',
    statData,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    storedViews: state.filterState.StoredViews,
    VizParams: state.filterState.VizParams,
    filterObj: state.filterState.Filter
  }
};


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(FilterSideBar));
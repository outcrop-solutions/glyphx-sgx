import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import RaisedButton from 'material-ui/RaisedButton';
import {Flex} from 'react-flex-material';
import IconButton from 'material-ui/IconButton';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import AlertContainer from 'react-alert';
import FilterTabs from './FilterTab.js';
import DualListBox from 'react-dual-listbox';
import Collapsible from 'react-collapsible';
import TopView from './TopView.js';
import { connect } from 'react-redux';
import {Scroll,Element,Events,scroller} from 'react-scroll';
import 'font-awesome/css/font-awesome.css';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './filterNav.css';

class FilterNav extends Component {

    constructor(props) {
        super(props);
        
		var tableData = this.fetchData();

        //Make columns and global store structure
        this.makeFilterStructure(tableData);
        
        //Store the states of all the elements inside this data structure.
        this.state  = {
            pinDailog: {
                open: false,
            },
            tableData: tableData,
			topViewInitParams: {
				viewSelectItems: ['view 0', 'view 1', 'view 4', 'view 5'],
				tableSelectItems: ['table 0', 'table 1', 'table 4', 'table 5'],
                scrollToElement: this.scrollToElement
			},
            activeColumns: [],
            pinnedDialogOptionValues: [],
            pinnedDialogSelectedValues: [],
            topViewVisible: true,
			pinnedDialogPrevState: [],
            viewNameTextFieldError: ""
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
        
        var collapsibles = document.getElementsByClassName('Collapsible__trigger');
        for (var i = 0; i < collapsibles.length; i++) {
            collapsibles[i].style.setProperty('--text-color', this.props.settings.textColor);
        }
    };

    componentWillUnmount() {
        /*Events.scrollEvent.remove('begin');
        Events.scrollEvent.remove('end');*/
    };

    fetchData = () => {
        var data = require('../src/Data/TempData.json');
        return data.Data;
    };
 
    /**
	* This method is called when
	*/
    showAlert = (strMsg) => {
        this.msg.show(strMsg, {
            time: 5000,
            type: 'success',
            icon: <i className="fa fa-check-circle" style={{fontSize: '2.5em',color:'green'}}></i>
        })
    };

    /**
     * 
     */
    makeFilterStructure = (Obj,Options) => {
        //var rangeStructure = {};
        //var elasticStructure = {};
        var returnObj = {};

        for(var property in Obj){
            var column = property;
            var range;
            var type = isNaN(Obj[property][0]) ? 'Text' : 'Number';

            if (type === "Number") {
                var minMax = this.findMinMax( Obj[property] );

                range = [minMax.min, minMax.max, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false];
            }
            else if (type === "Text") {
                range = [0, 25, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false, 1, ""];
            }
            else {
                // Date range
            }

            /* rangeStructure[column] = {
                rangeList: [[minMax.min,minMax.max,( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),false]],
                highlightedValues:[],
                bounds:[minMax.min,minMax.max]
            };
            
            elasticStructure[column] = {
                selectedValues: [],
                applied: false,
                pinned: false,
                type: type,
                displayName: this.generateDisplayName(column)
            }; */

            returnObj[property] = {
                rangeList: [range],
                highlightedValues: [],
                bounds: [range[0], range[1]],
                selectedValues: [],
                pinned: false,
                type: type,
                displayName: this.generateDisplayName(column)
            }


        }
        //this.props.dispatch(init({Ranges:rangeStructure,Elastic:elasticStructure}));
        this.props.dispatch(init(returnObj));
    };

    /**
     * This function will make columns and filterStructure
     * @param {Object} data - pass object of objects
     *  Eg: {
     *     colName: {
     *        data: []
     *      }      
     * }
     */
     makeColumns = (data,extra) => {
        var temp = null;
        var columnsFilterStructure = this.props.GLOBAL;
        var arrColumnsReturn = [];
        var arrPinnedColumnsReturn = [];
        var arrPinDialogOptions = [];
        var arrPinDialogSelected = [];
        var objReturn = {};


        
        var style = (function() {
            // Create the <style> tag
            var style = document.createElement("style");

            // WebKit hack
            style.appendChild(document.createTextNode(""));

            // Add the <style> element to the page
            document.head.appendChild(style);

            return style;
        })();

        style.sheet.insertRule('.Collapsible__trigger { display: block; font-weight: 400; text-decoration: none; color: #333333; position: relative; border: 1px solid white; padding: 15px; background: ' + this.props.settings.primaryColor + '; color: white; font-size: 1rem; }', 0);
        style.sheet.insertRule('.Collapsible__trigger.is-open { background: ' + this.props.settings.primaryColorDark + '; }', 1);
        style.sheet.insertRule('.Collapsible__trigger:hover { background: ' + this.props.settings.primaryColorLight + '; }', 2);
        style.sheet.insertRule('.unpinned { font-size: 20px !important; transform: rotateZ(35deg) !important; color: ' + this.props.settings.textColor + '!important; }', 3);
        style.sheet.insertRule('.pinned { font-size: 20px !important; transform: rotateZ(0deg) !important; color: ' + this.props.settings.secondaryColorLight + '!important; }', 4);


        for (var property in data) {
            var columnName = property;
            var colElasticFilterStruc = columnsFilterStructure[property] ? columnsFilterStructure[property] : columnsFilterStructure;
            var context = this;
            var displayName = colElasticFilterStruc.displayName;

            arrColumnsReturn.push(
                <Element name = { columnName } key = { columnName }>
                    <Collapsible 
                        transitionTime = { 200 }
                        key = { columnName }
                        ref = { columnName }
                        triggerOpenedClassName = "columnNameHeader"
                        handleTriggerClick = { this.onTriggerClick.bind(this,columnName) }
                        contentOuterClassName = "cursorNormal"
                        trigger = {
                            <div>
                                <IconButton 
                                    id = { "btn_" + columnName } 
                                    onClick = { context.onPinClick.bind(context) } 
                                    iconClassName = { colElasticFilterStruc.pinned ? "fa fa-thumb-tack pinned " + columnName : "fa fa-thumb-tack unpinned " + columnName }
                                    style = {{
                                        padding: '0px',
                                        width: 'inherit',
                                        height: 'inherit' 
                                    }} 
                                />
                                <span 
                                    style={{
                                        paddingLeft: '10px',
                                        fontSize: '1rem',
                                        color: this.props.settings.textColor
                                    }}
                                >
                                    {displayName}
                                </span>
                            </div>
                        } 
                        triggerClassName = 'columnNameHeader'
                    >
                        <FilterTabs 
                            ref = { 'tab-' + columnName } 
                            internalColName = { columnName } 
                            id = { columnName } 
                            displayName = { displayName } 
                            data = { data[columnName] } 
                        />
                    </Collapsible>
                </Element>
            );

            arrPinDialogOptions.push( {value: columnName, label: displayName} );


            if (colElasticFilterStruc.pinned) {
                arrPinnedColumnsReturn.push(
                    <Element name = { columnName + "_pinned" } key = { columnName + "_pinned" }>
                        <Collapsible 
                            transitionTime = { 200 } 
                            key = { columnName + "_pinned" }
                            ref = { columnName + "_pinned" }
                            triggerOpenedClassName = "columnNameHeader"
                            handleTriggerClick = { this.onTriggerClick.bind(this,columnName + "_pinned") }
                            contentOuterClassName = "cursorNormal"
                            trigger = {
                                <div>
                                    <IconButton 
                                        id = { "btn_" + columnName } 
                                        onClick = { context.onPinClick.bind(context) } 
                                        iconClassName = { colElasticFilterStruc.pinned ? "fa fa-thumb-tack pinned " + columnName : "fa fa-thumb-tack unpinned " + columnName }
                                        style = {{ 
                                            padding: '0px',
                                            width: 'inherit',
                                            height: 'inherit' 
                                        }} 
                                    />
                                    <span 
                                        style = {{
                                            paddingLeft: '10px',
                                            fontSize: '1rem',
                                            color: this.props.settings.textColor
                                        }}
                                    >
                                        {displayName}
                                    </span>
                                </div>
                            } 
                            triggerClassName = 'columnNameHeader'
                        >
                            <FilterTabs 
                                ref = { 'tab-' + columnName + "_pinned" } 
                                internalColName = { columnName + "_pinned" } 
                                id = { columnName } 
                                displayName = { displayName } 
                                data = { data[columnName] } 
                            />
                        </Collapsible>
                    </Element>
                );
                arrPinDialogSelected.push( {value: columnName, label: displayName} );
            }
        }
        objReturn = {columns:arrColumnsReturn,pinnnedColumns:arrPinnedColumnsReturn,pinnedOptions:arrPinDialogOptions,pinSelected:arrPinDialogSelected};
        return objReturn;
    };

    /**
     * This function replaces "_" with "<space>" and replaces "<Capital letter>" with "<space><Capital letter>" to make the display name!
     * @param str: The column name as it is.
     * @return display name string.
     */
    generateDisplayName = (str) => {
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
     * This function is called when the "Elastic/Range" Icon is clicked on from the filterView.
     * It will expand the correspoding column and bring the scroll to that element.
     * @param {string} element: this is the name of the column
     * @param {boolean} Elastic: the value will be true if the elastic icon is clicked or false if Range Icon is clicked. 
     */
    scrollToElement = (element, Elastic) => {
        var filterCollapisble = this.refs.filterCollapisble;
        var pinnedCollapisble = this.refs.pinnedCollapisble;
        var columnCollapisble = this.refs[element + "_pinned"] ? this.refs[element + "_pinned"] : this.refs[element];
        var tab = this.refs['tab-' + element + "_pinned"] ? this.refs['tab-'+ element + "_pinned"].getWrappedInstance() : this.refs['tab-' + element].getWrappedInstance();


        if (this.refs[element + "_pinned"]) {
            if (pinnedCollapisble.state.isClosed) {
                //filterCollapisble.prepareToOpen();
                pinnedCollapisble.openCollapsible();
            }

            if (columnCollapisble.state.isClosed) {
                //columnCollapisble.prepareToOpen();
                columnCollapisble.openCollapsible();
            }

            scroller.scrollTo(element + "_pinned", {
                duration: 1000,
                delay: 100,
                smooth: true,
                containerId: 'filterNav',
            });
        }
        else {
            if (filterCollapisble.state.isClosed) {
                //filterCollapisble.prepareToOpen();
                filterCollapisble.openCollapsible();
            }

            if (columnCollapisble.state.isClosed) {
                //columnCollapisble.prepareToOpen();
                columnCollapisble.openCollapsible();
            }

            scroller.scrollTo(element, {
                duration: 1000,
                delay: 100,
                smooth: true,
                containerId: 'filterNav',
            });
        }

        if (Elastic && tab.state.slideIndex != 0) {
            tab.handleChange(0, tab);
        }
        else if (!Elastic && tab.state.slideIndex != 1) {
            tab.handleChange(1, tab);
        }
    }


    /**
     * 
     */
    handleOpenClose = (strName,open,extra) => {
        switch(strName) {
            case 'pin':
                if (open) {
					this.setState( {pinnedDialogPrevState: this.state.pinnedDialogSelectedValues} );
                    this.setState( {pinDailog: { open: true }} ); 
                }
                else {
					if (extra ? extra.cancel : false) {
						this.setState({pinnedDialogSelectedValues: this.state.pinnedDialogPrevState});
					}
					
                    this.setState({pinDailog:{
                        open: false
                    }});
                }
                break;

            default:
                return null;
        }
    };
    
   

    /**
     * 
     */
    onPinnedOkDailog = (context) => {
        var pinnedValues = context.state.pinnedDialogSelectedValues.slice();
		var prevSelectedValues = context.state.pinnedDialogPrevState;
        var unpinnedArray = [];
		var len=prevSelectedValues.length;
        var len2;
		
		//find values that were unpinned
		for (var i = 0; i < len; i++) {
			if (pinnedValues.indexOf(prevSelectedValues[i]) == -1) {
				unpinnedArray.push(prevSelectedValues[i]);
			}
		}
		
        len2 = unpinnedArray.length;
            
        //unpin previously pinned together.
        for (var k = 0; k < len2; k++) {
            this.props.dispatch( {type: 'Update_Pin', details: {colName: unpinnedArray[k], pinned: false}} );
        }

        len2 = pinnedValues.length;
        //pin new values
        for (var j = 0; j < len2; j++) {
            if (!this.props.GLOBAL[pinnedValues[j]].pinned) {
                this.props.dispatch( {type: 'Update_Pin', details: {colName: pinnedValues[j], pinned: true}} );
            }
        }

        this.handleOpenClose('pin',false);
    };

    /**
     * 
     */
    onPinClick = (event, extra) => {
        console.log("pinned to active!");
        var but, colName;
        var pinned;
        var selectedValues = this.state.pinnedDialogSelectedValues;
        
        if (event) {
            event.stopPropagation();
            but = event.currentTarget;
            colName = but.id.split("btn_")[1];
            pinned = this.props.GLOBAL[colName].pinned;
            if (pinned) {
                selectedValues.splice(selectedValues.indexOf(colName),1);
            }
            else {
                selectedValues.push(colName);
            }
                
            this.setState({pinnedDialogSelectedValues: selectedValues});
            this.props.dispatch({type:'Update_Pin', details:{colName: colName,pinned: !pinned}});
        }
    };


   /**
	* This method is called when the user clicks on the 'arrow' to hide/show the top view of the filter
	*/
    toggleTopView = (event) => {
        var collapseTopViewButton = document.getElementById("collapseTopViewButton");
        var topView = document.getElementById("TopView");
        var filterWindow = document.getElementById("FilterWindowOuterContiner");
        
        if (this.state.topViewVisible == true) {
            filterWindow.style.transform = "translate(0px,-"+topView.clientHeight+"px)"
            //collapseTopViewButton.classList.remove('fa-caret-up');
            //collapseTopViewButton.classList.add('.Collapsible__trigger.is-open:after');
            collapseTopViewButton.style.transform = 'rotateZ(180deg)';
            this.setState({topViewVisible : false});
        }
        else {
            filterWindow.style.transform = "translate(0px,0px)"
            //collapseTopViewButton.classList.remove('fa-caret-down');
            //collapseTopViewButton.classList.add('fa-caret-up');
            collapseTopViewButton.style.transform = 'none';
            this.setState({topViewVisible : true});
        }
            
    };

    onTriggerClick = (element) => {
        var elem = this.refs[element];

        if (elem.state.isClosed) {
            elem.openCollapsible();

            scroller.scrollTo(element, {
                duration: 1000,
                delay: 100,
                smooth: true,
                containerId: 'filterNav',
            });
        }
        else {
            elem.closeCollapsible();
        }
    }

    render = () => {
         var pinnedEmptyString = <div className="centerText cursorNormal"><h3> Nothing Pinned! </h3><label> Anything you pin shows up here, so <br/> you can keep track of filters you <br/> need to get back to. </label></div>;
         var columnsObj = this.makeColumns(this.state.tableData);

        return (
            <div 
                className = "TopNav" 
                id = "FilterWindowOuterContiner" 
                style = {{
                    height: '100%',
                    transition: '1s',
                    paddingLeft: '1%',
                    paddingRight: '1%'
                }}
            >
                <div>
                    <AlertContainer ref = { a => this.msg = a } />
                </div>
                <Flex layout="column" style = {{ height: '100%' }}>
                     {/* TOP SECTION */}
                    
					<TopView initParams = { this.state.topViewInitParams } showAlert = { (strMsg) => this.showAlert(strMsg) }/>

                    <RaisedButton 
                        fullWidth = { true } 
                        primary = { true } 
                        onClick = { this.toggleTopView }
                        buttonStyle = {{ backgroundColor: this.props.settings.secondaryColor }}
                        style = {{ height: '20px' }}
                    >
                        <i 
                            id = "collapseTopViewButton" 
                            className = "fa fa-caret-up" 
                            style = {{
                                fontSize: '1.6em',
                                color: this.props.settings.textColor,
                                transition: 'transform 500ms'
                            }}
                        /> 
                    </RaisedButton>


                    {/* BOTTOM SECTION */}

                    <Flex flex="50">
                        <Collapsible 
                            transitionTime = {200} 
                            ref = 'pinnedCollapisble'
                            contentOuterClassName = "cursorNormal"
                            handleTriggerClick = { this.onTriggerClick.bind(this,'pinnedCollapisble') }
                            trigger = {
                                <div>
                                    <i className = "fa fa-thumb-tack" style = {{ fontSize: '1.3rem', color: this.props.settings.textColor }} />
                                    <span 
                                        style={{
                                            paddingLeft: '10px',
                                            fontSize: '1rem',
                                            color: this.props.settings.textColor
                                        }}
                                    >
                                        Pinned
                                    </span>
                                </div>
                            }
                        >

                            <Dialog
                                title = "Pinning Views!"
                                actions = {
                                    [
                                        <FlatButton
                                            label = "Ok"
                                            primary = { true }
                                            onClick = { () => this.onPinnedOkDailog(this) }
                                        />,
                                        <FlatButton
                                            label = "Cancel"
                                            primary = { true }
                                            onClick = { () => this.handleOpenClose('pin',false,{'cancel':true}) }
                                        />
                                    ]
                                }
                                modal = { true }
                                open = { this.state.pinDailog.open }
                            >
                                <DualListBox
                                    canFilter
                                    preserveSelectOrder 
                                    ref = "pinnedDialog"
                                    options = { columnsObj.pinnedOptions }
                                    selected = { this.state.pinnedDialogSelectedValues }
                                    onChange = { 
                                        (selected) => {
                                            this.setState({pinnedDialogSelectedValues: selected});
                                            //this.onPinClick(null,{pinnedValues: []})
                                        }
                                    }
                                />
                            </Dialog>
                                
                            <IconButton 
                                onClick = { () => this.handleOpenClose('pin',true) }
                                iconClassName = "fa fa-plus-square"
                                iconStyle = {{ color: this.props.settings.primaryColor }} 
                            />

                            {columnsObj.pinnnedColumns.length > 0 ? columnsObj.pinnnedColumns : pinnedEmptyString}

                        </Collapsible>

                        <Collapsible 
                            transitionTime = {200} 
                            ref = 'filterCollapisble'
                            contentOuterClassName = "cursorNormal"
                            handleTriggerClick = { this.onTriggerClick.bind(this,'filterCollapisble') }
                            trigger = {
                                <div>
                                    <i className = "fa fa-filter" style = {{ fontSize: '1.3rem', color: this.props.settings.textColor }} />
                                    <span 
                                        style = {{
                                            paddingLeft: '10px',
                                            fontSize: '1rem',
                                            color: this.props.settings.textColor
                                        }}
                                    >
                                        Filters
                                    </span>
                                </div>
                            }
                        >

                            {columnsObj.columns}

                        </Collapsible>
                    </Flex>
                </Flex>
            </div>
        );
    }
}


export const init = (storeFilterStruc) => ({
  type: 'INIT',
  storeFilterStruc
});

const mapStateToProps = function(state){
  return {
    GLOBAL: state.filterState.Filter,
    settings: state.filterState.Settings
  }
};

export default connect(mapStateToProps)(FilterNav);
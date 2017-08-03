import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import RaisedButton from 'material-ui/RaisedButton';
import {Flex} from 'react-flex-material';
import IconButton from 'material-ui/IconButton';
import FontIcon from 'material-ui/FontIcon';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import TextField from 'material-ui/TextField';
import AlertContainer from 'react-alert';
import FilterTabs from './FilterTab.js';
import DualListBox from 'react-dual-listbox';
import Collapsible from 'react-collapsible';
import TopView from './TopView.js';
import SearchBox from './SearchBox.js';
import {List, ListItem} from 'material-ui/List';
import { connect } from 'react-redux';
import ScrollIntoView from 'scroll-into-view-if-needed';
import 'font-awesome/css/font-awesome.min.css';
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
                scrollToElement: this.scrollToCollapsibleElement
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
        var collapsibles = document.getElementsByClassName('Collapsible__trigger');
        for (var i = 0; i < collapsibles.length; i++) {
            collapsibles[i].style.setProperty('--collapsible-text-color-main', this.props.settings.collapsibleColor.mainText);
            collapsibles[i].style.setProperty('--collapsible-text-color-sub', this.props.settings.collapsibleColor.subText);
        }
    };

    fetchData = () => {
        var data = require('../src/Data/TempData.json');
        return data.Data;
    };
 
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
     * This method constructs the structure of the Filter that is saved in the store and accessed everywhere throughout the applciation.
     * @param {Object} Obj:
     */
    makeFilterStructure = (Obj,Options) => {
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
		
		this.insertStyles();

        for (var property in data) {
            var columnName = property;
            var colElasticFilterStruc = columnsFilterStructure[property] ? columnsFilterStructure[property] : columnsFilterStructure;
            var context = this;
            var displayName = colElasticFilterStruc.displayName;

            arrColumnsReturn.push(
                <div id = { columnName } key = { columnName } name= {displayName} className="searchableColumn">
                    <Collapsible 
                        transitionTime = { 200 }
                        key = { columnName }
                        ref = { columnName }
                        id = { columnName }
                        triggerOpenedClassName = "columnNameHeader"
                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this,columnName) }
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
                                        color: this.props.settings.collapsibleColor.subText
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
                </div>
            );

            arrPinDialogOptions.push( {value: columnName, label: displayName} );


            if (colElasticFilterStruc.pinned) {
                arrPinnedColumnsReturn.push(
                    <div id = { columnName + "_pinned" } name= { displayName } key = { columnName + "_pinned" } className="searchableColumnPinned">
                        <Collapsible 
                            transitionTime = { 200 } 
                            key = { columnName + "_pinned" }
                            ref = { columnName + "_pinned" }
                            triggerOpenedClassName = "columnNameHeader"
                            handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this,columnName + "_pinned") }
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
                                            color: this.props.settings.collapsibleColor.subText
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
                    </div>
                );
                arrPinDialogSelected.push( {value: columnName, label: displayName} );
            }
        }
        objReturn = {columns:arrColumnsReturn,pinnnedColumns:arrPinnedColumnsReturn,pinnedOptions:arrPinDialogOptions,pinSelected:arrPinDialogSelected};
        return objReturn;
    };
	
	insertStyles = () => {
		var style = document.getElementById('themeStyles');
		
		if(style != null)
		{
			style.parentElement.removeChild(style);
			console.log('deleting old rules');
		}
		
		style = document.createElement("style");

		// WebKit hack
		style.appendChild(document.createTextNode(""));
		style.setAttribute('id','themeStyles');

		// Add the <style> element to the page
		document.head.appendChild(style);
		
		style.sheet.insertRule('.Collapsible__trigger { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; display: block; font-weight: 400; text-decoration: none; color: #333333; position: relative; border: 1px solid white; padding: 15px; background: ' + this.props.settings.collapsibleColor.mainBackground + '; color: white; font-size: 1rem; }', 0);
		style.sheet.insertRule('.Collapsible__trigger.is-open { background: ' + this.props.settings.collapsibleColor.mainCollapsed + '; }', 1);
		style.sheet.insertRule('.Collapsible__trigger:hover { background: ' + this.props.settings.collapsibleColor.mainHover + '; }', 2);
		style.sheet.insertRule('.unpinned { font-size: 20px !important; transform: rotateZ(35deg) !important; color: ' + this.props.settings.collapsibleColor.unpinned + '!important; }', 3);
		style.sheet.insertRule('.pinned { font-size: 20px !important; transform: rotateZ(0deg) !important; color: ' + this.props.settings.collapsibleColor.pinned + '!important; }', 4);
		style.sheet.insertRule('.columnNameHeader { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; font-size: 1rem !important; padding: 10px !important; background: ' + this.props.settings.collapsibleColor.subBackground + '!important; }', 5);
		style.sheet.insertRule('.columnNameHeader.is-open { background: ' + this.props.settings.collapsibleColor.subCollapsed + '!important; }', 6);
		style.sheet.insertRule('.columnNameHeader:hover {  background: ' + this.props.settings.collapsibleColor.subHover + '!important; }', 7);
		
		// For the selectboxes  
		style.sheet.insertRule('.Select-placeholder {  color: ' + this.props.settings.tableSelectColor.text + ' !important; }', 8);
		style.sheet.insertRule('.Select-arrow {  border-color: ' + this.props.settings.tableSelectColor.text + ' transparent transparent !important; }', 9);
        style.sheet.insertRule('.Select-control { background-color: ' + this.props.settings.tableSelectColor.background + '!important; }', 10);
		style.sheet.insertRule('.Select-value {  fill: ' + this.props.settings.tableSelectColor.selectedText + '!important; font-size: 13px !important;}', 11);
		style.sheet.insertRule('.Select-option.is-selected {  background-color: ' +  this.props.settings.tableSelectColor.selectedBackground + '!important; font-size: 13px !important; color: ' +  this.props.settings.tableSelectColor.selectedText +'}', 12);
		style.sheet.insertRule('.Select-option.is-focused {  background-color: ' +  this.props.settings.tableSelectColor.background + '!important; color: ' +  this.props.settings.tableSelectColor.text + '}', 13);
        style.sheet.insertRule('.Select--multi .Select-value {  background-color: ' + this.props.settings.tableSelectColor.chipBackground + ' !important; border: 1px solid ' + this.props.settings.tableSelectColor.chipBackground + ' !important; color: ' + this.props.settings.tableSelectColor.selectedText + ' !important; margin: 3px 0px 0px 8px;}', 14);
		style.sheet.insertRule('.Select--multi .Select-value-icon {  border-right: 1px solid ' + this.props.settings.tableSelectColor.deleteHover + ' !important; padding: 0px 3px 0px 3px; margin: 1px 0px 0px 0px; font-size: 18px;}', 15);
        style.sheet.insertRule('.Select--multi .Select-value-icon:hover, .Select--multi .Select-value-icon:focus { color: ' + this.props.settings.tableSelectColor.deleteHover + '!important; }', 16); 

            
	}

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
     * This is called when the collapsibles are clicked.
     * @param {string} element- this is the name of the element that surrounds the collapsible. Used to scrollTo.
     */
    onCollapsibleTriggerClick = (element) => {
        var elem = this.refs[element];

        if (elem.state.isClosed) {
            elem.openCollapsible();
            var context = this;
            
            this.checkToScroll(element,elem,this);
        }
        else {
            elem.closeCollapsible();
        }
    }

    /**
     * This function is called when the "Elastic/Range" Icon is clicked on from the filterView.
     * It will expand the correspoding column and bring the scroll to that element.
     * @param {string} element: this is the name of the column
     * @param {boolean} Elastic: the value will be true if the elastic icon is clicked or false if Range Icon is clicked. 
     */
    scrollToCollapsibleElement = (element, Elastic) => {
        var filterCollapisble = this.refs.filterCollapisble;
        var pinnedCollapisble = this.refs.pinnedCollapisble;
        var columnCollapisble = this.refs[element + "_pinned"] ? this.refs[element + "_pinned"] : this.refs[element];
        var tab = this.refs['tab-' + element + "_pinned"] ? this.refs['tab-'+ element + "_pinned"].getWrappedInstance() : this.refs['tab-' + element].getWrappedInstance();
        var context = this;

        if (this.refs[element + "_pinned"]) {
            element = element + "_pinned";
			context.clearSearchBox(null,'pinnedCollapisbleSearchBox');

            if (pinnedCollapisble.state.isClosed) {
                //filterCollapisble.prepareToOpen();
                pinnedCollapisble.openCollapsible();
            }

            if (columnCollapisble.state.isClosed) {
                //columnCollapisble.prepareToOpen();
                columnCollapisble.openCollapsible();
            }

        }
        else {
			context.clearSearchBox(null,'filterCollapisbleSearchBox');

            if (filterCollapisble.state.isClosed) {
                //filterCollapisble.prepareToOpen();
                filterCollapisble.openCollapsible();
            }

            if (columnCollapisble.state.isClosed) {
                //columnCollapisble.prepareToOpen();
                columnCollapisble.openCollapsible();
            }

        }

        if (Elastic && tab.state.slideIndex != 0) {
            tab.handleChange(0, tab);
        }
        else if (!Elastic && tab.state.slideIndex != 1) {
            tab.handleChange(1, tab);
        }
		
		this.checkToScroll(element,columnCollapisble,this);
    }

    /**
     * This function will bring the scroll to that element.
     * @param {String/Object} element: this is the name of the column
     */
    scroll = (element) => {
        if(typeof element == 'string')
            element = document.getElementById(element);

        if(element)
            /*element.scrollIntoView({ 
                behavior: 'smooth' 
            });*/
            ScrollIntoView(element, false, {
                            duration: 150
                            });
        else
            return false;
    }
	
	/**
	 * Sometimes scrollbars only appear after the collapisbles have expanded. So we wait for them to expand and then scroll
	 * @param {string} elementName: the name of the element(usually the internalColName).
	 * @param {Obj} element: the actual element object.
	 * @param {Obj} context: the context of FilterNav. 
	 */
	checkToScroll = (elementName,element,context) => {
        clearInterval(context['timeout']);
        context['counter'] = 0;
        context['timeout'] = window.setInterval(function() {
                context['counter']++;
                console.log('timeout');
                 if (!element.state.isClosed || context['counter'] > 10)
                 {
                    context.scroll(elementName);
                    clearInterval(context['timeout']);
                 }
        }, 250);
	}
	
	/**
	 * 
	 */
	clearSearchBox = (evt,strSearchBoxId) => {
		var sb = this.refs[strSearchBoxId];
		if(sb){
			evt = sb.clearText(evt);
            this.searchMultipleColumns(evt,sb.props.pinned,{filterViewClick:true});
		}
	}
	
	/**
	 * This function is called on key up on the search bars inside pinned and filters collapisbles.
	 * This will perform multiple search for the underlying collapisbles and show only those that match the textfield.
	 * @param {Obj} evt: actual event object
	 * @param {Bool} pinned: if true then search on pinned collapsible columns else on filter collapisbles.
	 */
	searchMultipleColumns = (evt,pinned,extra) => {
		var inputValue = evt.currentTarget.value ? evt.currentTarget.value.toUpperCase() : "";
		var divList = pinned ? document.getElementsByClassName('searchableColumnPinned') : document.getElementsByClassName('searchableColumn');
		var len = divList.length;
		var name,i;
		var shouldBeVisible;
        var ref;
		var inputValues = inputValue.split(',');
		
		for (i = 0; i < len; i++) {
            name = divList[i].getAttribute('name');
            ref = this.refs[divList[i].getAttribute('id')] 
            
            if(ref && (extra ? !extra.filterViewClick : true) )
                ref.state.isClosed ? console.log('closed') : ref.closeCollapsible();
			
            shouldBeVisible = false;
            
            if (name) {
				
				for(var j=0; j < inputValues.length; j++)
				{
					if (name.toUpperCase().indexOf(inputValues[j]) > -1) {
						shouldBeVisible = true;
					}
				}
				
				!shouldBeVisible ? divList[i].style.display = "none" : divList[i].style.display = "" ;
            } 
        }
	}
	
	/**
	 * This function is called on click of the "collapse All" button. All the collapsibles in the selected tab will collapse.
	 * @param {Obj} evt: actual event object
	 * @param {Bool} pinned: if true then search on pinned collapsible columns else on filter collapisbles.
	 */
	collapseAll = (evt,pinned) => {
		var divList = pinned ? document.getElementsByClassName('searchableColumnPinned') : document.getElementsByClassName('searchableColumn');
		var len = divList.length;
		var i,ref;
		
		for (i = 0; i < len; i++) {
            ref = this.refs[divList[i].getAttribute('id')] 
            
            if(ref)
                ref.state.isClosed ? console.log('closed') : ref.closeCollapsible();
		}
	}
	
	/**
	 * This function is called on key up on the search bars inside pinned and filters collapisbles.
	 * This will search for the underlying collapisbles and show only those that match the textfield.
	 * @param {Obj} evt: actual event object
	 * @param {Bool} pinned: if true then search on pinned collapsible columns else on filter collapisbles.
	 */
	searchColumns = (evt,pinned) => {
		var inputValue = evt.currentTarget.value.toUpperCase();
		var divList = pinned ? document.getElementsByClassName('searchableColumnPinned') : document.getElementsByClassName('searchableColumn');
		var len = divList.length;
		var name,i;
		
		for (i = 0; i < len; i++) {
            name = divList[i].getAttribute('name');
            if (name) {
                if (name.toUpperCase().indexOf(inputValue) > -1) {
                    divList[i].style.display = "";
                } 
                else {
                    divList[i].style.display = "none";
                }
            } 
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
		
        if(this.refs['topCollapisble'].state.isClosed)
		{
			collapseTopViewButton.style.transform = '';
		    this.refs['topCollapisble'].openCollapsible();
		}
        else{
			collapseTopViewButton.style.transform = 'rotateZ(180deg)';
		    this.refs['topCollapisble'].closeCollapsible();
		}
        
    };

    render = () => {
         var pinnedEmptyString = <div className="centerText cursorNormal"><h3> Nothing Pinned! </h3><label> Anything you pin shows up here, so <br/> you can keep track of filters you <br/> need to get back to. </label><br/><br/></div>;
         var columnsObj = this.makeColumns(this.state.tableData);
		 var pinnedSearchBar = <div style = {{ margin: "1px -3px -6px 1px" }}>
                                    <SearchBox 
                                        ref="pinnedCollapisbleSearchBox"
                                        hintText="Search For Column"
                                        settings={{
                                            SearchBoxClearHover: this.props.settings.pinFilterColor.SearchBoxClearHover, 
                                            searchBoxUnderline: this.props.settings.pinFilterColor.searchBoxUnderline,
                                            overviewButtonsColorBg: this.props.settings.overviewButtonsColor.background,
                                            overviewButtonsColorText: this.props.settings.overviewButtonsColor.text
                                        }}
                                        onTextFieldValueChange= {(evt,pinned) => this.searchMultipleColumns(evt,pinned)}
                                        onCollapseAllClick={(evt,pinned) => this.collapseAll(evt,pinned)}
                                        id="pinnedCollapisbleSearchBox"
                                        pinned={true}
                                        collapseButton={true}
                                    />
                                <br />
                                </div>;

        return (
                <Flex 
                    layout = "column" 
                    id = "FilterWindowOuterContiner"
                    style = {{ 
                        height: '100%',
                        overflow:'hidden',
                        transition: '1s',
                        //paddingLeft: '1%',
                        //paddingRight: '1%'
                    }}
                >
                     {/* TOP SECTION */}
                     <div>
                        <AlertContainer ref = { a => this.msg = a } />
                    </div>

                    <Collapsible
                        transitionTime = {200} 
                        open = { true }
                        contentInnerClassName  = "Flex__layout-column"
                        ref = 'topCollapisble'
						triggerClassName = 'noHeaderTrigger cursorNormal'
						triggerOpenedClassName = 'noHeaderTrigger cursorNormal'
                        contentOuterClassName = "cursorNormal"
                        overflowWhenOpen = "visible"
                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this,'topCollapisble') }
                    >

                        <TopView initParams = { this.state.topViewInitParams } showAlert = { (strMsg) => this.showAlert(strMsg) }/>

                    </Collapsible>
                
					
                    <RaisedButton 
                        fullWidth = { true } 
                        primary = { true } 
                        onClick = { this.toggleTopView.bind(this) }
                        buttonStyle = {{ backgroundColor: this.props.settings.hideTopViewButtonColor.background, width: "448px" }}
                        style = {{ height: '20px', margin: "0px 0px 1px 1px" }}
                    >
                        <i 
                            id = "collapseTopViewButton" 
                            className = "fa fa-caret-up" 
                            style = {{
                                fontSize: '1.6em',
                                color: this.props.settings.hideTopViewButtonColor.icon,
                                transition: 'transform 500ms'
                            }}
                        /> 
                    </RaisedButton>

                    {/* BOTTOM SECTION */}
                    <Flex  
                        style = {{
                            'overflow':'auto',
                            'overflowX': 'hidden'
                            }} 
                        id = 'BottomView'
                        className = "sidenavbar"
                    >
                        <div id = 'pinnedCollapisble'>
                            <Collapsible 
                                transitionTime = {200} 
                                ref = 'pinnedCollapisble'
								key = 'pinnedCollapisble'
                                contentOuterClassName = "cursorNormal"
                                handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this,'pinnedCollapisble') }
                                trigger = {
                                    <div>
                                        <i className = "fa fa-thumb-tack" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                                        <span 
                                            style={{
                                                paddingLeft: '10px',
                                                fontSize: '1rem',
                                                color: this.props.settings.collapsibleColor.mainText
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
                                                style = {{ color: this.props.settings.pinFilterColor.okButton }}
                                            />,
                                            <FlatButton
                                                label = "Cancel"
                                                primary = { true }
                                                onClick = { () => this.handleOpenClose('pin',false,{'cancel':true}) }
                                                style = {{ color: this.props.settings.pinFilterColor.cancelButton }}
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
                                
                                {columnsObj.pinnnedColumns.length > 0 ? null : pinnedEmptyString}

                                <Flex layout = "row" >
                                    <RaisedButton
                                        primary = { true } 
                                        label = "Pin Filters"
                                        style = {{
                                            width: "100%",
                                            margin: "0px 0px 6px 0px"
                                        }}
                                        buttonStyle={{
                                            height: '28px',
                                            lineHeight: '28px',
                                            backgroundColor: this.props.settings.overviewButtonsColor.background
                                        }} 
                                        labelStyle= {{
                                            fontSize: '13px',
                                            color: this.props.settings.overviewButtonsColor.text
                                        }}
                                        overlayStyle = {{
                                            height: '28px',
                                            lineHeight: '28px'
                                        }}
                                        onClick = { () => this.handleOpenClose('pin', true) }
                                        icon = {
                                            <FontIcon
                                                className = "fa fa-plus"
                                                style = {{ 
                                                    color: this.props.settings.overviewButtonsColor.text,
                                                    fontSize: "20px",
                                                    margin: "0px 0px 1px 14px"
                                                }}
                                            />
                                        }
                                    />
                                </Flex>

                                {columnsObj.pinnnedColumns.length > 0 ? pinnedSearchBar : null}
                                {columnsObj.pinnnedColumns.length > 0 ? columnsObj.pinnnedColumns : null}

                            </Collapsible>
                        </div>
                    
                        <div id = 'filterCollapisble'>

                            <Collapsible 
                                transitionTime = {200} 
                                ref = 'filterCollapisble'
								key = 'filterCollapisble'
                                contentOuterClassName = "cursorNormal"
                                handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this,'filterCollapisble') }
                                trigger = {
                                    <div>
                                        <i className = "fa fa-filter" style = {{ fontSize: '1.3rem', color: this.props.settings.collapsibleColor.mainIcon }} />
                                        <span 
                                            style = {{
                                                paddingLeft: '10px',
                                                fontSize: '1rem',
                                                color: this.props.settings.collapsibleColor.mainText
                                            }}
                                        >
                                            Filters
                                        </span>
                                    </div>
                                }
                            >

                                <div style = {{ margin: "1px -3px -6px 1px" }} >
                                    <SearchBox 
                                        settings = {{
                                            SearchBoxClearHover: this.props.settings.pinFilterColor.SearchBoxClearHover, 
                                            searchBoxUnderline: this.props.settings.pinFilterColor.searchBoxUnderline,
                                            overviewButtonsColorBg: this.props.settings.overviewButtonsColor.background,
                                            overviewButtonsColorText: this.props.settings.overviewButtonsColor.text
                                        }}
                                        hintText = "Search For Column"
                                        ref = "filterCollapisbleSearchBox"
                                        onTextFieldValueChange = { (evt) => this.searchMultipleColumns(evt) }
                                        onCollapseAllClick = { (evt, pinned) => this.collapseAll(evt, pinned) }
                                        id = "filterCollapisbleSearchBox"
                                        pinned = { false }
                                        collapseButton = { true }
                                    />
                                </div>
								
                                <br/>
                                {columnsObj.columns}
                            </Collapsible>
                        </div>
                    </Flex>
                </Flex>
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
import React, { Component } from 'react';
import ScrollIntoView from 'scroll-into-view-if-needed';
import Flexbox from 'flexbox-react';
import Collapsible from 'react-collapsible';
import IconButton from 'material-ui/IconButton';
import FilterTabs from './FilterTab.js';
import SearchBox from './SearchBox.js';
import PinningDialog from './FilterSideBarPinningViewsDialog.js';
import { connect } from 'react-redux';
import 'font-awesome/css/font-awesome.min.css';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './FilterSideBar.css';

/**
 * This is the bottom view of the side bar that you see on the right side of the screen.
 * @param tableData: - Data formatted into form needed to load filters
 */
class FilterSideBarBottomView extends Component {

    constructor(props) {
        super(props);
        
        // Store the states of all the elements inside this data structure.
        this.state  = {
            activeColumns: []
        };
    };


    /**
     * This function will make columns and filterStructure
     * @param {Object} data - pass object of objects
     *  Eg: {
     *     colName1: {
     *        values:{},
     *        totalCount:'',
     *        flatValues: []
     *      },
     *     colName2: {
     *        values:{},
     *        totalCount:'',
     *        flatValues: []
     *      },
     *     ...      
     *  }
     */
    makeColumns = (data, extra) => {
        var columnsFilterStructure = this.props.GLOBAL;
        var arrColumnsReturn = [], arrPinnedColumnsReturn = [], arrPinDialogOptions = [], arrPinDialogSelected = [], objReturn = {};
		var config;

        for (var property in data) {
            var columnName = property;
            var colElasticFilterStruc = columnsFilterStructure[property] ? columnsFilterStructure[property] : columnsFilterStructure;
            var displayName = colElasticFilterStruc.displayName;
            var pinnedClsName = colElasticFilterStruc.pinned ? "fa fa-thumb-tack pinned " + columnName : "fa fa-thumb-tack unpinned " + columnName;

            config = {
                outerDivClassName: 'searchableColumn',
                internalColName: columnName,
                displayName: displayName,
                globalStoreName: columnName,
                data: data[columnName],
                iconClassName: pinnedClsName
            };

            // Normal Columns in the filters collapsible.
            arrColumnsReturn.push(
                this.getColumnHTML(config)
            );
            arrPinDialogOptions.push({ value: columnName, label: displayName });

            // Pinned Columns in the pinned collapsible.
            if (colElasticFilterStruc.pinned) {
                config = {
                    outerDivClassName: 'searchableColumnPinned',
                    internalColName: columnName + "_pinned",
                    displayName: displayName,
                    globalStoreName: columnName,
                    data: data[columnName],
                    iconClassName: "fa fa-thumb-tack pinned " + columnName
                }
                arrPinnedColumnsReturn.push(
                    this.getColumnHTML(config)
                );
                arrPinDialogSelected.push( {value: columnName, label: displayName} );
            }
        }
        
        objReturn = {
            columns: arrColumnsReturn,
            pinnnedColumns: arrPinnedColumnsReturn,
            pinnedOptions: arrPinDialogOptions,
            pinSelected: arrPinDialogSelected
        };
        
        return objReturn;
    };


    /**
     * This function creates and returns the markup for the column.
     * @param {object} config: 
     *  {
     *      outerDivClassName: '',
     *      internalColName: "",
     *      displayName: '',
     *      globalStoreName: '',
     *      data: [],
     *      iconClassName: ""
     *  }
     * @return {object} The HTML of the column
     */
    getColumnHTML = (config) => {
        return (
            <div id = { config.internalColName } name = { config.displayName } key = { config.internalColName } className = { config.outerDivClassName } style = {{ marginBottom: "2px" }} >
                <Collapsible 
                    transitionTime = { 200 } 
                    key = { config.internalColName }
                    ref = { config.internalColName }
                    triggerOpenedClassName = "columnNameHeader"
                    handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this, config.internalColName) }
                    contentOuterClassName = "cursorNormal"
                    contentInnerClassName = "subCollapsibleInner"
                    trigger = {
                        <div>
                            <IconButton 
                                id = { "btn_" + config.globalStoreName } 
                                onClick = { this.onPinClick.bind(this) } 
                                iconClassName = { config.iconClassName }
                                style = {{ 
                                    padding: '0px',
                                    width: 'inherit',
                                    height: 'inherit' 
                                }}
                            />
                            <span 
                                style = {{
                                    paddingLeft: '10px',
                                    fontSize: '1.1rem',
                                    color: this.props.settings.colors.collapsibleColor.subText
                                }}
                            >
                                {config.displayName}
                            </span>
                        </div>
                    } 
                    triggerClassName = 'columnNameHeader'
                >
                    <FilterTabs 
                        ref = { 'tab-' + config.internalColName } 
                        internalColName = { config.internalColName } 
                        id = { config.globalStoreName } 
                        displayName = { config.displayName } 
                        data = { config.data } 
                    />
                </Collapsible>
            </div>
        );
    }

    
    /**
     * This function is called when the pin is clicked on the table collapsible element.
     * It pins the columns
     * @param {obj} event: The event object.
     * @param {obj} extra: Other parameters you would like to pass.
     */
    onPinClick = (event, extra) => {
        var but, colName;
        var pinned;
        var selectedValues = this.refs['PinningDialog'] ? this.refs['PinningDialog'].state.pinnedDialogSelectedValues : [];
        
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
                
            if (this.refs['PinningDialog']) {
                this.refs['PinningDialog'].setState({pinnedDialogSelectedValues: selectedValues});
            }
        
            this.props.dispatch({type:'Update_Pin', details:{colName: colName,pinned: !pinned}});
        }
    };


    /**
     * This is called when the collapsibles are clicked.
     * @param {string} element- this is the name of the element that surrounds the collapsible. Used to scrollTo.
     */
    onCollapsibleTriggerClick = (element) => {
        var elem = this.refs[element];

        if (elem.state.isClosed) {
            elem.openCollapsible();
            this.checkToScroll(element, elem, this);
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
        if (typeof element == 'string') {
            element = document.getElementById(element);
        }

        if (element) {
            ScrollIntoView(element, false, { duration: 150 });
        }
        else
            return false;
    }

	
	/**
	 * Sometimes scrollbars only appear after the collapisbles have expanded. So we wait for them to expand and then scroll
	 * @param {string} elementName: the name of the element(usually the internalColName).
	 * @param {Obj} element: the actual element object.
	 * @param {Obj} context: the context of FilterSideBar. 
	 */
	checkToScroll = (elementName, element, context) => {
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
	 * This is clears the top level search box inside pinned and filter collapsibles.
     * @param evt: - ADCMT
	 * @param strSearchBoxId: - ADCMT
	 */
	clearSearchBox = (evt, strSearchBoxId) => {
		var sb = this.refs[strSearchBoxId];
		if (sb) {
			evt = sb.clearText(evt);
            this.searchMultipleColumns(evt, sb.props.pinned, {filterViewClick: true} );
		}
	}
	
	/**
	 * This function is called on key up on the search bars inside pinned and filters collapisbles.
	 * This will perform multiple search for the underlying collapisbles and show only those that match the textfield.
	 * @param {Obj} evt: actual event object
	 * @param {Bool} pinned: if true then search on pinned collapsible columns else on filter collapisbles.
     * @param extra: - ADCMT
	 */
	searchMultipleColumns = (evt, pinned, extra) => {
		var inputValue = evt.currentTarget.value ? evt.currentTarget.value.toUpperCase() : "";
		var divList = pinned ? document.getElementsByClassName('searchableColumnPinned') : document.getElementsByClassName('searchableColumn');
		var len = divList.length;
		var name, i;
		var shouldBeVisible;
        var ref;
		var inputValues = inputValue.split(',');
		
		for (i = 0; i < len; i++) {
            name = divList[i].getAttribute('name');
            ref = this.refs[divList[i].getAttribute('id')] 
            
            if (ref && (extra ? !extra.filterViewClick : true) ) {
                ref.state.isClosed ? console.log('closed') : ref.closeCollapsible();
            }
			
            shouldBeVisible = false;
            
            if (name) {
				for (var j = 0; j < inputValues.length; j++) {
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
	collapseAll = (evt, pinned) => {
		var divList = pinned ? document.getElementsByClassName('searchableColumnPinned') : document.getElementsByClassName('searchableColumn');
		var len = divList.length;
		var i, ref;
		
		for (i = 0; i < len; i++) {
            ref = this.refs[divList[i].getAttribute('id')];
            
            if (ref) {
                ref.state.isClosed ? console.log('closed') : ref.closeCollapsible();
            }
		}
    }
	
	/**
	 * This function is called on key up on the search bars inside pinned and filters collapisbles.
	 * This will search for the underlying collapisbles and show only those that match the textfield.
	 * @param {Obj} evt: actual event object
	 * @param {Bool} pinned: if true then search on pinned collapsible columns else on filter collapisbles.
	 */
	searchColumns = (evt, pinned) => {
		var inputValue = evt.currentTarget.value.toUpperCase();
		var divList = pinned ? document.getElementsByClassName('searchableColumnPinned') : document.getElementsByClassName('searchableColumn');
		var len = divList.length;
		var name, i;
		
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

    render() {
        var pinnedEmptyString = <div className = "centerText cursorNormal" style = {{ margin: "0px 60px 7px", borderRadius: "5px" }} >
                                    <div style = {{ fontSize: "17px", fontWeight: "bold", paddingTop: "6px", color: "#000000" }} > Nothing Pinned </div>
                                    <div style = {{ fontWeight: "bold", fontSize: "14px", margin: "8px 0px -32px", color: "#000000" }} > Anything you pin shows up here for <br/> keeping track of important filters. </div>
                                    <br/><br/>
                                </div>;
        var columnsObj = this.makeColumns(this.props.tableData);
        var pinnedSearchBar =   <div style = {{ margin: "1px -3px -6px 1px" }} >
                                    <SearchBox 
                                        ref = "pinnedCollapisbleSearchBox"
                                        hintText = "Search for column..."
                                        settings = {{
                                            SearchBoxClearHover: this.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
                                            searchBoxUnderline: this.props.settings.colors.pinFilterColor.searchBoxUnderline,
                                            overviewButtonsColorBg: this.props.settings.colors.overviewButtonsColor.background,
                                            overviewButtonsColorText: this.props.settings.colors.overviewButtonsColor.text,
                                            tableSelectColor: this.props.settings.colors.tableSelectColor.background
                                        }}
                                        onTextFieldValueChange = { (evt, pinned) => this.searchMultipleColumns(evt, pinned) }
                                        onCollapseAllClick = { (evt, pinned) => this.collapseAll(evt, pinned) }
                                        id = "pinnedCollapisbleSearchBox"
                                        pinned = { true }
                                        collapseButton = { true } 
                                    />
                                    <br />
                                </div>;

        return (

            <Flexbox 
                flexDirection = "column" 
                flexGrow = {1}
                style = {{ 
                    overflow: 'auto',
                    overflowX: "hidden",
                    transition: '1s',
                    padding: "6px"
                }}
                id = "BottomView"
                className = "sidenavbar"
            >
                <div id = 'pinnedCollapisble' style = {{ marginBottom: "3px" }} >
                    <Collapsible 
                        transitionTime = {200} 
                        ref = 'pinnedCollapisble'
                        key = 'pinnedCollapisble'
                        contentOuterClassName = "cursorNormal"
                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this,'pinnedCollapisble') }
                        trigger = {
                            <div>
                                <i className = "fa fa-thumb-tack" style = {{ fontSize: '1.3rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                                <span 
                                    style = {{
                                        paddingLeft: '10px',
                                        fontSize: '1.3rem',
                                        color: this.props.settings.colors.collapsibleColor.mainText
                                    }}
                                >
                                    Pinned
                                </span>
                            </div>
                        }
                    >   
                        {columnsObj.pinnnedColumns.length > 0 ? null : pinnedEmptyString}

                        <PinningDialog 
                            ref = "PinningDialog"
                            pinnedOptions = { columnsObj.pinnedOptions }
                            settings = {{
                                overviewButtonsColor: this.props.settings.colors.overviewButtonsColor,
                                pinFilterColor: this.props.settings.colors.pinFilterColor
                            }}
                            GLOBAL = { this.props.GLOBAL }
                            dispatch = { this.props.dispatch }
                            fullWidth = { columnsObj.pinnnedColumns.length > 0 ? true : false }
                        />   

                        {columnsObj.pinnnedColumns.length > 0 ? pinnedSearchBar : null}
                        {columnsObj.pinnnedColumns.length > 0 ? columnsObj.pinnnedColumns : null}

                    </Collapsible>
                </div>
            
                <div id = 'filterCollapisble' style = {{ marginBottom: "-6px" }} >

                    <Collapsible 
                        transitionTime = {200} 
                        ref = 'filterCollapisble'
                        key = 'filterCollapisble'
                        contentOuterClassName = "cursorNormal"
                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this, 'filterCollapisble') }
                        trigger = {
                            <div>
                                <i className = "fa fa-filter" style = {{ fontSize: '1.3rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                                <span 
                                    style = {{
                                        paddingLeft: '9px',
                                        fontSize: '1.3rem',
                                        color: this.props.settings.colors.collapsibleColor.mainText
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
                                    SearchBoxClearHover: this.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
                                    searchBoxUnderline: this.props.settings.colors.pinFilterColor.searchBoxUnderline,
                                    overviewButtonsColorBg: this.props.settings.colors.overviewButtonsColor.background,
                                    overviewButtonsColorText: this.props.settings.colors.overviewButtonsColor.text,
                                    tableSelectColor: this.props.settings.colors.tableSelectColor.background
                                }}
                                hintText = "Search for column..."
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
            </Flexbox>
        );
    }
}

const mapStateToProps = function(state){
  return {
    GLOBAL: state.filterState.Filter,
    settings: state.filterState.Settings
  }
};

export default connect(mapStateToProps,null,null,{withRef:true})(FilterSideBarBottomView);

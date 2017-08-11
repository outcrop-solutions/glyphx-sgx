import React, { Component } from 'react';
import RaisedButton from 'material-ui/RaisedButton';
import {Flex} from 'react-flex-material';
import AlertContainer from 'react-alert';
import Collapsible from 'react-collapsible';
import FilterSideBarTopView from './FilterSideBarTopView.js';
import FilterSideBarBottomView from './FilterSideBarBottomView.js';
import { connect } from 'react-redux';
import 'font-awesome/css/font-awesome.min.css';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './FilterSideBar.css';

/**
 * This is the side bar that you see on the right side of the screen.
 * This is made up of two other componenets:
 *  1) FilterSideBarTopView.
 *  2) FilterSideBarBottomView.
 */
class FilterSideBar extends Component {

    constructor(props) {
        super(props);
        
		var tableData = this.fetchData();

        //Make columns and global store structure
        this.makeFilterStructure(tableData);
        
        //Store the states of all the elements inside this data structure.
        this.state  = {
            tableData: tableData,
			topViewInitParams: {
				viewSelectItems: ['view 0', 'view 1', 'view 4', 'view 5'],
				tableSelectItems: ['table 0', 'table 1', 'table 4', 'table 5'],
                scrollToElement: this.scrollToCollapsibleElement
			},
            topViewVisible: true
        };
    };


    componentDidMount() {
        var collapsibles = document.getElementsByClassName('Collapsible__trigger');
        for (var i = 0; i < collapsibles.length; i++) {
            collapsibles[i].style.setProperty('--collapsible-text-color-main', this.props.settings.colors.collapsibleColor.mainText);
            collapsibles[i].style.setProperty('--collapsible-text-color-sub', this.props.settings.colors.collapsibleColor.subText);
        }
    };

    /**
     * Use to make server call to fetch data.
     */
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
     * This is just a link function that links the icon of the topview to the function in the bottom view.
     */
    scrollToCollapsibleElement = (element, Elastic) => {
        if(this.refs['bottom'] && this.refs['bottom'].getWrappedInstance())
            this.refs['bottom'].getWrappedInstance().scrollToCollapsibleElement(element,Elastic);
    }

   /**
    * This method is called when the user clicks on the 'arrow' to hide/show the top view of the filter
    * @param {object} event: the event object.
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
        var colList = Object.keys(this.state.tableData);
        return (
                <Flex 
                    layout = "column" 
                    id = "FilterWindowOuterContiner"
                    style = {{ 
                        height: '100%',
                        overflow:'hidden',
                        transition: '1s'
                    }}
                >
                     {/* TOP SECTION */}
                    <div>
                        <AlertContainer ref = { a => this.msg = a } />
                    </div>

                    <Collapsible
                        transitionTime = {200} 
                        open = { true }
                        triggerDisabled ={true}
                        contentInnerClassName  = "Flex__layout-column"
                        ref = 'topCollapisble'
						triggerClassName = 'noHeaderTrigger cursorNormal'
						triggerOpenedClassName = 'noHeaderTrigger cursorNormal'
                        contentOuterClassName = "cursorNormal"
                        overflowWhenOpen = "visible">

                        <FilterSideBarTopView 
                            initParams = { this.state.topViewInitParams } 
                            colList = { colList } 
                            showAlert = { (strMsg) => this.showAlert(strMsg) }
                        />

                    </Collapsible>
                
					{/* COLLAPSE TOP SECTION BUTTON */}
                    <RaisedButton 
                        fullWidth = { true } 
                        primary = { true } 
                        onClick = { this.toggleTopView.bind(this) }
                        buttonStyle = {{ backgroundColor: this.props.settings.colors.hideTopViewButtonColor.background, width: "448px" }}
                        style = {{ height: '20px', margin: "0px 0px 1px 1px" }}>
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
                        ref="bottom"
                        tableData={this.state.tableData}
                    />

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

export default connect(mapStateToProps)(FilterSideBar);
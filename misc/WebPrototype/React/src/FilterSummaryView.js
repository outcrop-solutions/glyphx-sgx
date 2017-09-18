import React from 'react';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import Badge from 'material-ui/Badge';
import Flexbox from 'flexbox-react';
import { Card, CardText } from 'material-ui/Card';
import { red500, blue500 } from 'material-ui/styles/colors';
import { connect } from 'react-redux';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import 'font-awesome/css/font-awesome.min.css';
import './General.css';


/**
 * Main FilterView parent class which gets exported
 **/
class FilterViewForm extends React.Component {

    shouldComponentUpdate(nextProps, nextState) {
        for (var colName in this.props.filterList) {
            if (this.props.filterList[colName].selectedValues != nextProps.filterList[colName].selectedValues) {
                return true;
            }

            if (this.props.filterList[colName].rangeList != nextProps.filterList[colName].rangeList) {
                return true;
            }
        }

        if (this.props.settings != nextProps.settings) {
            return true;
        }
        
        return false;
    }


    /**
     * Unselects all ranges and removes all elastic selections from a column
     * @param colName: name of the corresponding column to be deleted
     **/
    handleRowDel(colName) {
        this.props.dispatch(removeFilterView(colName));
    };

    render() {
        // Lose scope of 'this' in the map function
        var rowDel = this.handleRowDel.bind(this);
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
                    console.log(filterList[colName].selectedValues);
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

                console.log("min: " + min + ", max: " + max);

            }


            else if (filterType === "Date") {
                console.log("Not implemented yet...");
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
            return (<FilterViewRow 
                        view = { view } 
                        onDelEvent = { rowDel.bind(this) } 
                        key = { view[0] }
                        ref = { view[0] }
                        settings = { context.props.settings }
                        onScroll = { (element,elastic) => context.props.onScroll(element,elastic) }
                    />)
        });

        return (

            <Card style = {{ overflow: 'auto', width: '100%', padding: "0px", maxHeight:'200px', borderRadius: "5px" }} containerStyle = {{ padding: "0px", borderRadius: "5px" }}>
                <CardText
                    style = {{
                        padding: "3px 8px",
                        borderRadius: "5px"
                    }}
                >
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
                        {view.length > 0 ? view : <div className = "centerText cursorNormal"><h3> No Filters Selected </h3></div>}
                    </div>

                </CardText>
            </Card>
        );
    }
}


class FilterViewRow extends React.Component {

    onDelEvent() {
        this.props.onDelEvent(this.props.view[6]);
    }

    onClickIcon(evt,context,Elastic){
        context.props.onScroll(context.props.view[6],Elastic);
    }


    render() {

        // Shorten display text if theyre too long
        var displayName = this.props.view[0];
        var min = this.props.view[2].toString();
        var max = this.props.view[3].toString();

        if(displayName.length > 13) {
            displayName = displayName.substring(0,12) + "...";
        }

        if(min.length > 10) {
            min = min.substring(0,9) + "...";
        }

        if(max.length > 10) {
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
                            badgeStyle = {{ width: "20px", height: "20px", top: "-10px", right: "-13px", backgroundColor: this.props.settings.colors.filterOverviewColor.badgeBackground, color: this.props.settings.colors.filterOverviewColor.badgeText }}
                            >
                            <FontIcon
                                onClick = { (evt) => this.onClickIcon(evt,this,true) }
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
                            badgeStyle = {{ width: "20px", height: "20px", top: "-10px", right: "-13px", backgroundColor: this.props.settings.colors.filterOverviewColor.badgeBackground, color: this.props.settings.colors.filterOverviewColor.badgeText }}
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
 * Local styling
 **/
const styleSet = {
    iconStyles: {
        fontSize: 26,
        paddingLeft: 10,
        paddingRight: 13,
        margin: "12px 0px 0px -21px"
    },
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const removeFilterView = (colName) => ({
    type: 'REMOVE_FILTER_VIEW',
    colName
});


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
 * Connects the FilterViewForm component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(FilterViewForm);
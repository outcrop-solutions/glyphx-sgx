import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import { makeServerCall } from './ServerCallHelper.js';
import Flexbox from 'flexbox-react';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';
/* import FontIcon from 'material-ui/FontIcon'; */
import Divider from 'material-ui/Divider';
import Select from 'react-styled-select'

/**
 * -ADCMT
 **/
class XYZRemapModal extends React.Component {

	state = {
        XValues: [],
        XValue: null,
        XTemp: null,
        XLowerRange: -180,
        XUpperRange: 180,
        YValues: [],
        YValue: null,
        YTemp: null,
        YLowerRange: -180,
        YUpperRange: 180,
        ZValues: [],
        ZValue: null,
        ZTemp: null,
        ZLowerRange: 0,
        ZUpperRange: 360,
    };


    componentDidMount() {

        var shouldExec = true;

        if (window.performance) {
            if (performance.navigation.type === 1) {
                shouldExec = false;
                this.props.history.push('/home');
            }
        }

        if (shouldExec) {

            // debugger;

            var list = this.props.VizParams.filterAllowedColumnList;
            var XVals = [];
            var YVals = [];
            var ZVals = [];

            for (var i = 0; i < this.props.VizParams.filterAllowedColumnList.length; i++) {
                if (this.props.VizParams.filterAllowedColumnList[i].toLowerCase().indexOf("geo") !== -1) {
                    XVals.push({ label: "longitude", value: "longitude" });
                    YVals.push({ label: "latitude", value: "latitude" });
                    break;
                }
            }

            for (var i = 0; i < list.length; i++) {
                XVals.push({ label: list[i], value: list[i] });
                YVals.push({ label: list[i], value: list[i] });
                ZVals.push({ label: list[i], value: list[i] });
            }

            /*
            console.log("X");
            console.log(XVals);
            console.log("Y");
            console.log(YVals);
            console.log("Z");
            console.log(ZVals);
            */
            
            this.setState({ 
                XVals: XVals,
                YVals: YVals,
                ZVals: ZVals
            });
        
        }
    }

    /**
     * React built-in which acts as a listener for when props change
     * @param nextProps: The props the component would have after the change
     **/
	componentWillReceiveProps(nextProps) {

        //if (this.props.XYZDisplay != nextProps.XYZDisplay && (this.state.XTemp == null || this.state.YTemp == null || this.state.ZTemp == null) ) {
        if (this.props.initialX !== nextProps.initialX || 
            this.props.initialY !== nextProps.initialY || 
            this.props.initialZ !== nextProps.initialZ) {
            this.setState({ 
                XValue: nextProps.initialX,
                XTemp: nextProps.initialX, 
                YValue: nextProps.initialY,
                YTemp: nextProps.initialY,
                ZValue: nextProps.initialZ,
                ZTemp: nextProps.initialZ
            });
        }
        else {
            var context = this;

            setTimeout(function () {
                context.setState(context.state);
            }, 500);
        }
    }


    /**
     * Updates the select box to represent the selection option made
     * @param event: -ADCMT
     * @param index: -ADCMT
     * @param value: Value of the selected drop down option
     * @param target: String representing the select box that triggered this method
     **/
    onSelectChange(e, target) {
        if (target === "X") {
            this.setState({ XTemp: e });
        }

        else if (target === "Y") {
            this.setState({ YTemp: e });
        }

        else if (target === "Z") {
            this.setState({ ZTemp: e });
        }
    }

    linearInterpolation (x1, x2, y1, y2, x) {
        if (x2 - x1 === 0) {
            if (x >= y1 && x <= y2) {
                return x;
            }
            else {
                return y2;
            }
        }
        else {
            return (((x - x1) * (y2 - y1)) / (x2 - x1)) + y1;
        }
    }


    /**
     * When settings are saved, updates the redux store and local state to reflect the changes
     **/
    onSettingsSave() {
        /*
        this.setState({ themeSelection: this.state.themeTempSelection, overlapSelection: this.state.overlapTempSelection, hideScrollSelection: this.state.hideScrollTempSelection });
        this.props.dispatch(
            editSettings(
                this.state.colorThemes.indexOf(this.state.themeTempSelection), 
                (this.state.overlapTempSelection === "No" ? false : true), 
                false, 
                (this.state.hideScrollTempSelection === "Yes" ? true : false)
            )
        );
        */

        var cols = "" + (this.state.XTemp ? this.state.XTemp + ", " : "") + 
                    (this.state.YTemp === this.state.XTemp ? "" :  (this.state.YTemp ? this.state.YTemp + ", ": "")) + 
                    (this.state.ZTemp === this.state.XTemp || this.state.ZTemp === this.state.YTemp ? "" : (this.state.ZTemp ? this.state.ZTemp + ", ": ""));

        if (cols !== "") {
            this.setState({ XValue: this.state.XTemp, YValue: this.state.YTemp, ZValue: this.state.ZTemp });

            var query = "SELECT rowid, " + cols.substring(0, cols.length - 2) + " FROM " + this.props.VizParams.tableName;
            //var query = "SELECT rowid, * FROM " + this.props.VizParams.tableName;

            var URL = "getXYZData?searchQuery=" + query; 
            // debugger;

            var context = this;
        
            return new Promise(function(resolve, reject) {
                var result = 'A is done'        

                // Get the data corresponding to the URL
                makeServerCall(URL,
                    function (responseText) { 
                        var response = responseText;

                        if (typeof responseText === 'string') {
                            response = JSON.parse(response);
                        }

                        var responseDetails = {
                            X: context.state.XTemp,
                            Y: context.state.YTemp,
                            Z: context.state.ZTemp
                        }

                        // debugger;

                        if (responseDetails.X != null) {
                            if (isNaN(response.data[0][responseDetails.X])) {
                                responseDetails.Xtype = "STR";
                                responseDetails.XUnique = [];
                            }
                            else {
                                responseDetails.Xtype = "INT";
                                responseDetails.XMin = parseInt(response.data[0][responseDetails.X], 10);
                                responseDetails.XMax = parseInt(response.data[0][responseDetails.X], 10);
                            }
                        }

                        if (responseDetails.Y != null) {
                            if (isNaN(response.data[0][responseDetails.Y])) {
                                responseDetails.Ytype = "STR";
                                responseDetails.YUnique = [];
                            }
                            else {
                                responseDetails.Ytype = "INT";
                                responseDetails.YMin = parseInt(response.data[0][responseDetails.Y], 10);
                                responseDetails.YMax = parseInt(response.data[0][responseDetails.Y], 10);
                            }
                        }

                        if (responseDetails.Z != null) {
                            if (isNaN(response.data[0][responseDetails.Z])) {
                                responseDetails.Ztype = "STR";
                                responseDetails.ZUnique = [];
                            }
                            else {
                                responseDetails.Ztype = "INT";
                                responseDetails.ZMin = parseInt(response.data[0][responseDetails.Z], 10);
                                responseDetails.ZMax = parseInt(response.data[0][responseDetails.Z], 10);
                            }
                        }


                        
                        for (var i = 0; i < response.data.length; i++) {
                             if (responseDetails.Xtype === "INT") {
                                 var parseX = parseInt(response.data[i][responseDetails.X], 10);
                                 if (responseDetails.XMin > parseX) {
                                     responseDetails.XMin = parseX;
                                 }
                                 else if (responseDetails.XMax < parseX) {
                                     responseDetails.XMax = parseX;
                                 }
                             }
                             else {
                                if (responseDetails.XUnique.indexOf(response.data[i][responseDetails.X]) === -1) {
                                    responseDetails.XUnique.push(response.data[i][responseDetails.X]);
                                }
                             }

                             if (responseDetails.Y != responseDetails.X) {
                                if (responseDetails.Ytype === "INT") {
                                    var parseY = parseInt(response.data[i][responseDetails.Y], 10);
                                    if (responseDetails.YMin > parseY) {
                                        responseDetails.YMin = parseY;
                                    }
                                    else if (responseDetails.YMax < parseY) {
                                        responseDetails.YMax = parseY;
                                    }
                                }
                                else {
                                    if (responseDetails.YUnique.indexOf(response.data[i][responseDetails.Y]) === -1) {
                                        responseDetails.YUnique.push(response.data[i][responseDetails.Y]);
                                    }
                                }
                             }

                             if (responseDetails.Z != responseDetails.X && responseDetails.Z !== responseDetails.Y) {
                                if (responseDetails.Ztype === "INT") {
                                    var parseZ = parseInt(response.data[i][responseDetails.Z], 10);
                                    if (responseDetails.ZMin > parseZ) {
                                        responseDetails.ZMin = parseZ;
                                    }
                                    else if (responseDetails.ZMax < parseZ) {
                                        responseDetails.ZMax = parseZ;
                                    }
                                }
                                else {
                                    if (responseDetails.ZUnique.indexOf(response.data[i][responseDetails.Z]) === -1) {
                                        responseDetails.ZUnique.push(response.data[i][responseDetails.Z]);
                                    }
                                }
                             }
                        }

                        if (responseDetails.XUnique) {
                            responseDetails.XUnique.sort();
                        }
                        if (responseDetails.YUnique) {
                            responseDetails.YUnique.sort();
                        }
                        if (responseDetails.ZUnique) {
                            responseDetails.ZUnique.sort();
                        }

                        if (responseDetails.Y == responseDetails.X) {
                            if (responseDetails.Ytype === "INT") {
                                responseDetails.YMin = responseDetails.XMin;
                                responseDetails.YMax = responseDetails.XMax;
                            }
                            else {
                                responseDetails.YUnique = responseDetails.XUnique;
                            }
                        }

                        if (responseDetails.Z == responseDetails.X) {
                            if (responseDetails.Ztype === "INT") {
                                responseDetails.ZMin = responseDetails.XMin;
                                responseDetails.ZMax = responseDetails.XMax;
                            }
                            else {
                                responseDetails.ZUnique = responseDetails.XUnique;
                            }
                        }
                        else if (responseDetails.Z === responseDetails.Y) {
                            if (responseDetails.Ztype === "INT") {
                                responseDetails.ZMin = responseDetails.YMin;
                                responseDetails.ZMax = responseDetails.YMax;
                            }
                            else {
                                responseDetails.ZUnique = responseDetails.YUnique;
                            }
                        }

                        responseDetails;
                        // debugger;

                        
                        var XYZDataStructure = [];

                        for (var i = 0; i < response.data.length; i++) {

                            var node = { rowID: response.data[i].rowid };

                            if (responseDetails.Xtype === "INT") {
                                node.X = context.linearInterpolation(responseDetails.XMin, responseDetails.XMax, context.state.XLowerRange, context.state.XUpperRange, parseInt(response.data[i][responseDetails.X], 10));
                            }
                            else {
                                node.X = (responseDetails.XUnique.indexOf(response.data[i][responseDetails.X]) + 1) * (context.state.XUpperRange - context.state.XLowerRange) / (responseDetails.XUnique.length) + context.state.XLowerRange
                            }

                            if (responseDetails.Ytype === "INT") {
                                node.Y = context.linearInterpolation(responseDetails.YMin, responseDetails.YMax, context.state.YLowerRange, context.state.YUpperRange, parseInt(response.data[i][responseDetails.Y], 10));
                            }
                            else {
                                node.Y = (responseDetails.YUnique.indexOf(response.data[i][responseDetails.Y]) + 1) * (context.state.YUpperRange - context.state.YLowerRange) / (responseDetails.YUnique.length) + context.state.YLowerRange
                            }

                            if (responseDetails.Ztype === "INT") {
                                node.Z = context.linearInterpolation(responseDetails.ZMin, responseDetails.ZMax, context.state.ZLowerRange, context.state.ZUpperRange, parseInt(response.data[i][responseDetails.Z], 10));
                            }
                            else {
                                node.Z = (responseDetails.ZUnique.indexOf(response.data[i][responseDetails.Z]) + 1) * (context.state.ZUpperRange - context.state.ZLowerRange) / (responseDetails.ZUnique.length) + context.state.ZLowerRange
                            }

                            XYZDataStructure.push(node);
                        }

                        context.props.dispatch(editModalDisplay(false));

                        var iframe = document.getElementById('GlyphViewer').contentWindow;
                        iframe.updateXYZCoordinates(XYZDataStructure, responseDetails.X, responseDetails.Y, responseDetails.Z);

                        //console.log(XYZDataStructure);
                        // debugger;

                        context.props.dispatch( setTimer(new Date().getTime()) );

                        
                    }
                );
            });
        }

        

    }


    /**
     * Reverts to previous settings if any where changed and closes the modal
     **/
    onSettingsCancel() {
        //this.setState({ themeTempSelection: this.state.themeSelection, overlapTempSelection: this.state.overlapSelection, hideScrollTempSelection: this.state.hideScrollSelection });
        this.setState({ XTemp: this.state.XValue, YTemp: this.state.YValue, ZTemp: this.state.ZValue });
        this.props.dispatch(editModalDisplay(false));
    }

    render(){

        return(
            <Dialog
				title = { <div> Change Axes </div> }
                ref = "XYZRemap"
                modal = { true }
				open = { this.props.XYZDisplay }
                //open = { false }
				bodyStyle = {{ padding: "0px 24px 10px" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", lineHeight: "12px", padding: "10px 30px 14px"}}
				actions = {
					[
						<FlatButton
                            label = "Save"
                            primary = { true }
                            onClick = { () => this.onSettingsSave() }
                            style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                        />,
                        <FlatButton
                            label = "Cancel"
                            primary = { true }
                            onClick = { () => this.onSettingsCancel() }
                            style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
                        />
					]
				}
			>

                <Flexbox flexDirection = "row" >
                    <div style = {{ width: "32%", margin: "20px 0px 3px", color: "#000000", fontSize: "20px", fontWeight: "bold", textAlign: "center" }} > X </div>
                    <div style = {{ width: "32%", margin: "20px 0px 3px", color: "#000000", fontSize: "20px", fontWeight: "bold", textAlign: "center" }} > Y </div>
                    <div style = {{ width: "32%", margin: "20px 0px 3px", color: "#000000", fontSize: "20px", fontWeight: "bold", textAlign: "center" }} > Z </div>        
                </Flexbox>

                <div style = {{ marginBottom: "10px" }} >
                    <Divider style = {{ backgroundColor: "#acacac", height: "2px" }} />
                </div>

                <div>

                    <Flexbox flexDirection = "row" >

                        <Select 
                            className = "selectXYZ dark-theme"
                            simpleValue
                            value = { this.state.XTemp } 
                            placeholder = "Select a new X Axis" 
                            options = { this.state.XVals } 
                            onChange = { (e) => this.onSelectChange(e, "X") } 
                            style = {{
                                margin: "-11px 0px 0px 0px",
                                width: "25% !important"
                            }}
                        />
                        

                        <RaisedButton
                            onClick = { () => 
                                this.setState({ 
                                    XLowerRange: (this.state.XLowerRange === -180 ? 180 : -180), 
                                    XUpperRange: (this.state.XUpperRange == -180 ? 180 : -180)  }) }
                            label = { <i className = { this.state.XLowerRange === -180 ? "fa fa-sort-alpha-asc" : "fa fa-sort-alpha-desc"} 
                            style = {{ margin: "0px 0px 0px -5px" }} /> }
                            style = {{
                                margin: "2px 12px 0px 3px",
                                minWidth: "37px",
                                width: "5%",
                                height: "25px"
                            }}
                            buttonStyle = {{
                                height: '25px',
                                lineHeight: '25px',
                                //backgroundColor: ( this.canUndoRedo("undo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed
                            }} 
                            labelStyle = {{
                                fontSize: '16px',
                                color: this.props.settings.colors.overviewButtonsColor.text
                            }}
                            overlayStyle = {{
                                height: '25px',
                                lineHeight: '25px'
                            }}
                            primary = { true }
                        />

                        <Select 
                            className = "selectXYZ dark-theme"
                            simpleValue
                            value = { this.state.YTemp } 
                            placeholder = "Select a new Y Axis" 
                            options = { this.state.YVals } 
                            onChange = { (e) => this.onSelectChange(e, "Y") } 
                            style = {{
                                margin: "-11px 0px 0px 0px",
                                width: "25% !important"
                            }}
                        />

                        <RaisedButton
                            onClick = { () => 
                                this.setState({ 
                                    YLowerRange: (this.state.YLowerRange === -180 ? 180 : -180), 
                                    YUpperRange: (this.state.YUpperRange == -180 ? 180 : -180)  }) }
                            label = { <i className = { this.state.YLowerRange === -180 ? "fa fa-sort-alpha-asc" : "fa fa-sort-alpha-desc"} 
                            style = {{ margin: "0px 0px 0px -5px" }} /> }
                            style = {{
                                margin: "2px 12px 0px 3px",
                                minWidth: "37px",
                                width: "5%",
                                height: "25px"
                            }}
                            buttonStyle = {{
                                height: '25px',
                                lineHeight: '25px',
                                //backgroundColor: ( this.canUndoRedo("undo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed
                            }} 
                            labelStyle = {{
                                fontSize: '16px',
                                color: this.props.settings.colors.overviewButtonsColor.text
                            }}
                            overlayStyle = {{
                                height: '25px',
                                lineHeight: '25px'
                            }}
                            primary = { true }
                        />

                        <Select 
                            className = "selectXYZ dark-theme"
                            simpleValue
                            value = { this.state.ZTemp } 
                            placeholder = "Select a new Z Axis" 
                            options = { this.state.ZVals } 
                            onChange = { (e) => this.onSelectChange(e, "Z") } 
                            style = {{
                                margin: "-11px 0px 0px 0px",
                                width: "25% !important"
                            }}
                        />

                        <RaisedButton
                            onClick = { () => 
                                this.setState({ 
                                    ZLowerRange: (this.state.ZLowerRange === 0 ? 360 : 0), 
                                    ZUpperRange: (this.state.ZUpperRange == 360 ? 0 : 360)  }) }
                            label = { <i className = { this.state.ZLowerRange === 0 ? "fa fa-sort-alpha-asc" : "fa fa-sort-alpha-desc"} 
                            style = {{ margin: "0px 0px 0px -5px" }} /> }
                            style = {{
                                margin: "2px 12px 0px 3px",
                                minWidth: "37px",
                                width: "5%",
                                height: "25px"
                            }}
                            buttonStyle = {{
                                height: '25px',
                                lineHeight: '25px',
                                //backgroundColor: ( this.canUndoRedo("undo") ? this.props.settings.colors.collapsibleColor.subBackground : "#bebebe" )
                                backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed
                            }} 
                            labelStyle = {{
                                fontSize: '16px',
                                color: this.props.settings.colors.overviewButtonsColor.text
                            }}
                            overlayStyle = {{
                                height: '25px',
                                lineHeight: '25px'
                            }}
                            primary = { true }
                        />
                    </Flexbox>

                    <div style = {{ width: "100%" }} >
                        <div style = {{ width: "450px", margin: "40px 0px -20px 150px" }} >
                            <img src = { "./Res/Img/Axes.png" } className = "noselect" alt = { "Axes" } draggable = { false } />
                        </div>
                        <div style = {{ margin: "-362px 0px 0px 192px", fontWeight: "bold", color: "#000" }} >
                            Z: {this.state.ZTemp}
                        </div>
                        <div style = {{ margin: "150px 0px 0px 345px", fontWeight: "bold", color: "#000" }} >
                            Y: {this.state.YTemp}
                        </div>
                        <div style = {{ margin: "96px 0px 25px 350px", fontWeight: "bold", color: "#000" }} >
                            X: {this.state.XTemp}
                        </div>
                    </div>

                </div>
                
            </Dialog>             
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (XYZModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    XYZModal,
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
    settings: state.filterState.Settings,
    XYZDisplay : state.filterState.ModalDisplay.XYZModal,
    VizParams: state.filterState.VizParams,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(XYZRemapModal));
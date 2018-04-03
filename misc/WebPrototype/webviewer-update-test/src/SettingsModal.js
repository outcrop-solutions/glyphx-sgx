import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import Select from 'react-select';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './SettingsModal.css';


/**
 * -ADCMT
 **/
class SettingsModal extends React.Component {

	state = {
        colorThemes: ["SynGlyphX", "Gannon"],
        themeSelection: "SynGlyphX",
        themeTempSelection: "SynGlyphX",
        overlapSelection: "No",
        overlapTempSelection: "No",
        hideScrollSelection: "No",
        hideScrollTempSelection: "No",
        axesVisibilitySelection: "Visible",
        axesVisibilityTempSelection: "Visible",
    };


    /**
     * Updates the select box to represent the selection option made
     * @param event: -ADCMT
     * @param index: -ADCMT
     * @param value: Value of the selected drop down option
     * @param target: String representing the select box that triggered this method
     **/
    handleSelectChange(event, index, value, target) {
        if (target === "Theme") {
            this.setState({ themeTempSelection: value });
        }

        else if (target === "Overlap") {
            this.setState({ overlapTempSelection: value });
        }

        else if (target === "HideScroll") {
            this.setState({ hideScrollTempSelection: value });
        }
    }


    /**
     * When settings are saved, updates the redux store and local state to reflect the changes
     **/
    onSettingsSave() {

        if (this.state.axesVisibilityTempSelection != this.state.axesVisibilitySelection) {
            if (document.getElementById('GlyphViewer')) {
                document.getElementById('GlyphViewer').contentWindow.postMessage({action: 'toggleAxis'}, '*');
                this.state.axisVisible ? this.setState({ axisVisible: false }) : this.setState({ axisVisible: true });
            }
        }
        this.setState({ 
            themeSelection: this.state.themeTempSelection, 
            overlapSelection: this.state.overlapTempSelection, 
            hideScrollSelection: this.state.hideScrollTempSelection,
            axesVisibilitySelection: this.state.axesVisibilityTempSelection,
        });

        this.props.dispatch(
            editSettings(
                this.state.colorThemes.indexOf(this.state.themeTempSelection), 
                (this.state.overlapTempSelection === "No" ? false : true), 
                false, 
                (this.state.hideScrollTempSelection === "Yes" ? true : false)
            )
        );

        if (!this.props.homePage) {
            var gv = document.getElementById('GlyphViewerContainer');
            var filterNav = document.getElementById("filterNav");
            var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;

            if (this.state.overlapTempSelection === "No") {
                if (filterNavOpen) {
                    gv.style.width = "calc(100% - 450px)";
                }

                else {
                    gv.style.width = "100%";
                }   
            }

            else {
                gv.style.width = "100%";
            }
        }
    }


    /**
     * Reverts to previous settings if any where changed and closes the modal
     **/
    onSettingsCancel() {
        this.setState({ themeTempSelection: this.state.themeSelection, overlapTempSelection: this.state.overlapSelection, hideScrollTempSelection: this.state.hideScrollSelection });
        this.props.dispatch(editSettings(null, null, false, null));
    }

    render(){

        var yesNoSelectItems = ["Yes", "No"];
        yesNoSelectItems = yesNoSelectItems.map(function(value) {
			return({ label: value, value: value });
		});

        var axesSelectItems = ["Visible", "Hidden"];
        axesSelectItems = axesSelectItems.map(function(value) {
			return({ label: value, value: value });
		});

        var colorThemes = this.state.colorThemes.map(function(value) {
			return({ label: value, value: value });
		});

        return(
            <Dialog
				title = { <div> <FontIcon className = "fa fa-cogs fa-2x" color = '#ffffff' /> &nbsp;&nbsp;Settings </div> }
                ref = "Settings"
                modal = { true }
				open = { this.props.settingsDisplay }
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

                <div style = {{ margin: "20px 0px 3px", color: "#000000", fontSize: "20px", fontWeight: "bold" }} > General </div>

                <div style = {{ marginBottom: "20px" }} >
                    <Divider style = {{ backgroundColor: "#acacac", height: "2px" }} />
                </div>

                {/*
                <Flexbox flexDirection = "row" >

                    <Tooltip
                        placement = 'right'
                        mouseEnterDelay = { 0.5 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlay = { 
                            <div> 
                                This will change the colors across the GlyphEd site.
                            </div> 
                        }
                    >
                        <FontIcon style = {{ margin: "5px -6px 0px 0px", fontSize: "22px", height: "22px", color: "#535353" }} className = "fa fa-question-circle" color = '#000000' />
                    </Tooltip>

                    <label style = {{ margin: "-13px 0px 0px 15px" }} ><h4> Color Theme </h4></label>

                    <div style = {{  margin: "0px 0px 0px 300px", position: "fixed", zIndex: "2001" }} >
                        <Select 
                            simpleValue
                            value = { this.state.themeTempSelection } 
                            placeholder = "Select" 
                            options = { colorThemes } 
                            onChange = { (value) => this.setState({ themeTempSelection: value }) } 
                            style = {{  width: "150px" }}
                            clearable = { false }
                        />
                    </div>
                </Flexbox>
                */}

                <Flexbox flexDirection = "row" >

                    <Tooltip
                        placement = 'right'
                        mouseEnterDelay = { 0.5 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlay = { 
                            <div> 
                                This will change the visibility setting of the Axes within the visualization.
                            </div> 
                        }
                    >
                        <FontIcon style = {{ margin: "5px -6px 0px 0px", fontSize: "22px", height: "22px", color: "#535353" }} className = "fa fa-question-circle" color = '#000000' />
                    </Tooltip>

                    <label style = {{ margin: "-13px 0px 0px 15px" }} ><h4> Axes Visibility </h4></label>

                    <div style = {{  margin: "0px 0px 0px 300px", position: "fixed", zIndex: "2001" }} >
                        <Select 
                            simpleValue
                            value = { this.state.axesVisibilityTempSelection } 
                            placeholder = "Select" 
                            options = { axesSelectItems } 
                            onChange = { (value) => this.setState({ axesVisibilityTempSelection: value }) } 
                            style = {{  width: "150px" }}
                            clearable = { false }
                        />
                    </div>
                </Flexbox>

                <div style = {{ margin: "20px 0px 3px", color: "#000000", fontSize: "20px",fontWeight: "bold" }} > Filter Sidebar </div>

                <div style = {{ marginBottom: "20px" }} >
                    <Divider style = {{ backgroundColor: "#acacac", height: "2px" }} />
                </div>

                <Flexbox flexDirection = "row" >

                    <Tooltip
                        placement = 'right'
                        mouseEnterDelay = { 0.5 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlayStyle = {{ marginTop: "-100px" }}
                        overlay = { 
                            <div> 
                                When set to yes, the filter sidebar will 
                                <br />overlap the glyph viewer window when open.
                            </div> 
                        }
                    >
                        <FontIcon style = {{ margin: "5px -6px 0px 0px", fontSize: "22px", height: "22px", color: "#535353" }} className = "fa fa-question-circle" />
                    </Tooltip>

                    <label style = {{ margin: "-13px 0px 0px 15px" }} ><h4> Overlap Glyph Viewer </h4></label>

                    <div style = {{  margin: "0px 0px 0px 300px", position: "fixed", zIndex: "2002" }} >
                        <Select 
                            simpleValue
                            value = { this.state.overlapTempSelection } 
                            placeholder = "Select" 
                            options = { yesNoSelectItems } 
                            onChange = { (value) => this.setState({ overlapTempSelection: value }) } 
                            style = {{  width: "150px" }}
                            clearable = { false }
                        />
                    </div>
                </Flexbox>

                {/*
                <Flexbox flexDirection = "row" >

                    <Tooltip
                        placement = 'right'
                        mouseEnterDelay = { 0.5 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlay = { 
                            <div> 
                                When set to yes, mousing over a table that already has a scrollbar 
                                <br />will hide the main scrollbar until the mouse exits the table.
                            </div> 
                        }
                    >
                        <FontIcon style = {{ margin: "5px -6px 0px 0px", fontSize: "22px", height: "22px", color: "#535353" }} className = "fa fa-question-circle" />
                    </Tooltip>

                    <label style = {{ margin: "-13px 0px 0px 15px" }} ><h4> Mouse Hover Hides Scrollbar </h4></label>

                    <div style = {{  margin: "0px 0px 0px 300px", position: "fixed", zIndex: "2000" }} >
                        <Select 
                            simpleValue
                            value = { this.state.hideScrollTempSelection } 
                            placeholder = "Select" 
                            options = { yesNoSelectItems } 
                            onChange = { (value) => this.setState({ hideScrollTempSelection: value }) } 
                            style = {{  width: "150px" }}
                            clearable = { false }
                        />
                    </div>
                </Flexbox>
                */}
                
            </Dialog>             
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editSettings = (theme, overlap, display, hideScroll) => ({
    type: 'EDIT_SETTINGS',
    theme,
    overlap,
    display,
    hideScroll
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    settingsDisplay : state.filterState.ModalDisplay.settingsModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(SettingsModal);
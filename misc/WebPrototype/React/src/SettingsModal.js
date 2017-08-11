import React from 'react';
import { connect } from 'react-redux';
import MenuItem from 'material-ui/MenuItem';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import DropDownMenu from 'material-ui/DropDownMenu';


/**
 * @param glyphWindowWidth: width the glyph window should take when overlap setting is turned off
 **/
class SettingsModal extends React.Component {


    /**
     * Initial state of the component.
     **/
	state = {
        themeSelection: 0,
        themeTempSelection: 0,
        overlapSelection: 0,
        overlapTempSelection: 0,
    };


    /**
     * Updates the select box to represent the selection option made
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
    }


    /**
     * When settings are saved, updates the redux store and local state to reflect the changes
     **/
    onSettingsSave() {
        this.setState({ themeSelection: this.state.themeTempSelection });
        this.setState({ overlapSelection: this.state.overlapTempSelection });
        this.props.dispatch(editSettings(this.state.themeTempSelection, (this.state.overlapTempSelection === 0 ? false : true), false));

        var gv = document.getElementById('GlyphViewerContainer');
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;

        if (this.state.overlapTempSelection === 0) {
            if (filterNavOpen) {
                gv.style.width = this.props.glyphWindowWidth + "px";
            }
            else {
                gv.style.width = "100%";
            }   
        }
        else {
            gv.style.width = "100%";
        }
    }

    /**
     * Reverts to previous settings if any where changed and closes the modal
     **/
    onSettingsCancel() {
        this.setState({ themeTempSelection: this.state.themeSelection, overlapTempSelection: this.state.overlapSelection, });
        this.props.dispatch(editSettings(null, null, false));
    }

    render(){
        return(
            <Dialog
                title = "Settings"
                ref = "Settings"
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
                modal = { true }
                open = { this.props.settingsDisplay }
            >
                <label><h4> Theme Settings </h4></label>
                <DropDownMenu
                    value = { this.state.themeTempSelection }
                    onChange = { (event, index, value) => this.handleSelectChange(event, index, value, "Theme") }
                    iconStyle = {{ fill: this.props.settings.colors.settingsModalColor.text}}
                    underlineStyle = {{ borderColor: this.props.settings.colors.settingsModalColor.text }}
                    selectedMenuItemStyle = {{ 
                        backgroundColor: this.props.settings.colors.settingsModalColor.selectedBackground, 
                        color: this.props.settings.colors.settingsModalColor.selectedText
                    }}
                >
                    <MenuItem value = { 0 } primaryText = "SynGlyphX" />
                    <MenuItem value = { 1 } primaryText = "Gannon" />
                </DropDownMenu>

                <br />

                <label><h4> Overlap Settings </h4></label>
                <DropDownMenu
                    value = { this.state.overlapTempSelection }
                    onChange = { (event, index, value) => this.handleSelectChange(event, index, value, "Overlap") }
                    iconStyle = {{ fill: this.props.settings.colors.settingsModalColor.text}}
                    underlineStyle = {{ borderColor: this.props.settings.colors.settingsModalColor.text }}
                    selectedMenuItemStyle = {{ 
                        backgroundColor: this.props.settings.colors.settingsModalColor.selectedBackground, 
                        color: this.props.settings.colors.settingsModalColor.selectedText
                    }}
                >
                    <MenuItem value = { 0 } primaryText = "Sidebar pushes view" />
                    <MenuItem value = { 1 } primaryText = "Sidebar overlaps view" />
                </DropDownMenu>
            </Dialog>             
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editSettings = (theme, overlap, display) => ({
    type: 'EDIT_SETTINGS',
    theme,
    overlap,
    display
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
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(SettingsModal);
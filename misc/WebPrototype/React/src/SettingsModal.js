import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
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
        hideScrollSelection: 0,
        hideScrollTempSelection: 0,
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
        else if (target === "HideScroll") {
            this.setState({ hideScrollTempSelection: value });
        }
    }


    /**
     * When settings are saved, updates the redux store and local state to reflect the changes
     **/
    onSettingsSave() {
        this.setState({ themeSelection: this.state.themeTempSelection });
        this.setState({ overlapSelection: this.state.overlapTempSelection });
        this.setState({ hideScrollSelection: this.state.hideScrollTempSelection });
        this.props.dispatch(editSettings(this.state.themeTempSelection, (this.props.homePage ? null : (this.state.overlapTempSelection === 0 ? false : true)), false, (this.props.homePage ? null : (this.state.hideScrollTempSelection === 0 ? true : false))));

        if (!this.props.homePage) {
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
    }


    /**
     * Reverts to previous settings if any where changed and closes the modal
     **/
    onSettingsCancel() {
        this.setState({ themeTempSelection: this.state.themeSelection, overlapTempSelection: this.state.overlapSelection, hideScrollTempSelection: this.state.hideScrollSelection });
        this.props.dispatch(editSettings(null, null, false, null));
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
                <Flexbox flexDirection = "row" >
                    <label><h4> Theme </h4></label>
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
                </Flexbox>

                {this.props.homePage ? null :
                    <div>
                        <Flexbox flexDirection = "row" >
                            <label><h4> Filter Sidebar </h4></label>
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
                        </Flexbox>

                        <Flexbox flexDirection = "row" >
                            <label><h4> Mouse Hover </h4></label>
                            <DropDownMenu
                                value = { this.state.hideScrollTempSelection }
                                onChange = { (event, index, value) => this.handleSelectChange(event, index, value, "HideScroll") }
                                iconStyle = {{ fill: this.props.settings.colors.settingsModalColor.text}}
                                underlineStyle = {{ borderColor: this.props.settings.colors.settingsModalColor.text }}
                                selectedMenuItemStyle = {{ 
                                    backgroundColor: this.props.settings.colors.settingsModalColor.selectedBackground, 
                                    color: this.props.settings.colors.settingsModalColor.selectedText
                                }}
                            >
                                <MenuItem value = { 0 } primaryText = "Hover hides scroll" />
                                <MenuItem value = { 1 } primaryText = "Hover shows scroll" />
                            </DropDownMenu>
                        </Flexbox>
                    </div>
                }
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
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(SettingsModal);
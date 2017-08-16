import React from 'react';
import { connect } from 'react-redux';
import MenuItem from 'material-ui/MenuItem';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import DropDownMenu from 'material-ui/DropDownMenu';
import Collapsible from 'react-collapsible';
import './AllViewsModal.css';


class AllViewsModal extends React.Component {

    /**
     * When settings are saved, updates the redux store and local state to reflect the changes
     **/
    onSettingsSave() {
        this.setState({ themeSelection: this.state.themeTempSelection });
        this.setState({ overlapSelection: this.state.overlapTempSelection });
        //this.props.dispatch(editSettings(this.state.themeTempSelection, (this.state.overlapTempSelection === 0 ? false : true), false));

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
        //this.props.dispatch(editSettings(null, null, false));
    }

    render(){
        return(
            <Dialog
                title = "All Views"
                actions = {
                    [
                        <FlatButton
                            label = "Back"
                            primary = { true }
                            //onClick = { () =>  }
                            style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
                        />
                    ]
                }
                modal = { true }
                open = { this.props.allViewsDisplay }
            >

                <Collapsible 
                    transitionTime = {200} 
                    contentOuterClassName = "C-Size-1-body"
                    triggerClassName = "C-Size-1"
                    triggerOpenedClassName = "C-Size-1"
                    trigger = {
                        <div style = {{ margin: "-70px 0px 0px 114px" }} >
                            <span 
                                style = {{
                                    paddingLeft: '10px',
                                    fontSize: '1rem',
                                    color: this.props.settings.colors.collapsibleColor.mainText
                                }}
                            >
                                Marketing &amp; Recruiting
                            </span>
                        </div>
                    }
                >

                    <br /> <br /> HI
                    
                </Collapsible>


                <Collapsible 
                    transitionTime = {200} 
                    contentOuterClassName = "C-Size-2-body"
                    triggerClassName = "C-Size-2"
                    triggerOpenedClassName = "C-Size-2"
                    trigger = {
                        <div style = {{ margin: "-70px 0px 0px 114px" }} >
                            <span 
                                style = {{
                                    paddingLeft: '10px',
                                    fontSize: '1rem',
                                    color: this.props.settings.colors.collapsibleColor.mainText
                                }}
                            >
                                Admissions
                            </span>
                        </div>
                    }
                >

                    <br /> <br /> HI
                    
                </Collapsible>

                <Collapsible 
                    transitionTime = {200} 
                    contentOuterClassName = "C-Size-3-body"
                    triggerClassName = "C-Size-3"
                    triggerOpenedClassName = "C-Size-3"
                    trigger = {
                        <div style = {{ margin: "-70px 0px 0px 114px" }} >
                            <span 
                                style = {{
                                    paddingLeft: '10px',
                                    fontSize: '1rem',
                                    color: this.props.settings.colors.collapsibleColor.mainText
                                }}
                            >
                                Financial Aid
                            </span>
                        </div>
                    }
                >

                    <br /> <br /> HI
                    
                </Collapsible>

                <Collapsible 
                    transitionTime = {200} 
                    contentOuterClassName = "C-Size-4-body"
                    triggerClassName = "C-Size-4"
                    triggerOpenedClassName = "C-Size-4"
                    trigger = {
                        <div style = {{ margin: "-70px 0px 0px 114px" }} >
                            <span 
                                style = {{
                                    paddingLeft: '10px',
                                    fontSize: '1rem',
                                    color: this.props.settings.colors.collapsibleColor.mainText
                                }}
                            >
                                Custom
                            </span>
                        </div>
                    }
                >

                    <br /> <br /> HI
                    
                </Collapsible>
                
            </Dialog>             
        );
    }
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (allViewsModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    allViewsModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    allViewsDisplay : state.filterState.ModalDisplay.allViewsModal,
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(AllViewsModal);
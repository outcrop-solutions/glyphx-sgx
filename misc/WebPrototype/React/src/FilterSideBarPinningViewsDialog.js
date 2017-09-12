import React, { Component } from 'react';
import DualListBox from 'react-dual-listbox';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';
import Dialog from 'material-ui/Dialog';
import FontIcon from 'material-ui/FontIcon';
import 'font-awesome/css/font-awesome.min.css';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './FilterSideBar.css';

/**
 * This is the pop up dialog that you see when you click "pin filters" inside the pinned collapsible.
 * This class also encapsulates the dual list box.
 * @param pinnedOptions: - ADCMT
 */
class FilterSideBarPinningViewsDialog extends Component {

    constructor(props) {
        super(props);
        
        // Store the states of all the elements inside this data structure.
        this.state  = {
            pinDailog: {
                open: false,
            },
            pinnedDialogSelectedValues: [],
			pinnedDialogPrevState: []
        };
    };


    /**
     * This function handles the open/close of the dialog.
     * This also handles the cancel click on the dialog.
     * @param {bool} open: should the dialog be open or not
     * @param {obj} extra: To pass some extra objects
     */
    handlePinDialogOpenClose = (open, extra) => {
        if (open) {
            this.setState( {pinnedDialogPrevState: this.state.pinnedDialogSelectedValues} );
            this.setState( {pinDailog: { open: true } }); 
        }
        else {
            if (extra ? extra.cancel : false) {
                this.setState({ pinnedDialogSelectedValues: this.state.pinnedDialogPrevState });
            }
            this.setState({ pinDailog: { open: false } });
        }
    };


    /**
     * This gets called when "ok" is clicked dialog box.
     * @param {object} context: The context of FilterSideBarPinningDialog.
     */
    onPinnedOkDailog = (context) => {
        var pinnedValues = context.state.pinnedDialogSelectedValues.slice();
		var prevSelectedValues = context.state.pinnedDialogPrevState;
        var unpinnedArray = [];
		var len = prevSelectedValues.length;
        var len2;
		
		// Find values that were unpinned
		for (var i = 0; i < len; i++) {
			if (pinnedValues.indexOf(prevSelectedValues[i]) == -1) {
				unpinnedArray.push(prevSelectedValues[i]);
			}
		}
		
        len2 = unpinnedArray.length;
            
        // Unpin previously pinned together.
        for (var k = 0; k < len2; k++) {
            this.props.dispatch({ type: 'Update_Pin', details: {colName: unpinnedArray[k], pinned: false} });
        } 

        len2 = pinnedValues.length;

        // Pin new values
        for (var j = 0; j < len2; j++) {
            if (!this.props.GLOBAL[pinnedValues[j]].pinned) {
                this.props.dispatch({ type: 'Update_Pin', details: {colName: pinnedValues[j], pinned: true} });
            }
        }

        this.handlePinDialogOpenClose(false);
    };

    shouldComponentUpdate(newProps, newState){
        if (this.state != newState) {
            return true;
        }
        else if (this.props.settings != newProps.settings) {
            return true;
        }

        return false;
    };

    render() {
        return (
            <div>
                <div style = {{ width: (this.props.fullWidth ? "100%" : "72%"), margin: "0 auto 3px" }} >
                    <RaisedButton
                        primary = { true } 
                        label = "Pin Filters"
                        style = {{
                            width: "100%",
                            margin: "0px 0px 2px"
                        }}
                        buttonStyle = {{
                            height: '28px',
                            lineHeight: '28px',
                            backgroundColor: this.props.settings.overviewButtonsColor.background
                        }} 
                        labelStyle = {{
                            fontSize: '13px',
                            color: this.props.settings.overviewButtonsColor.text
                        }}
                        overlayStyle = {{
                            height: '28px',
                            lineHeight: '28px'
                        }}
                        onClick = { () => this.handlePinDialogOpenClose(true) }
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
                </div>

                <Dialog
                    title = "Pinning Views"
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
                                onClick = { () => this.handlePinDialogOpenClose(false,{'cancel':true}) }
                                style = {{ color: this.props.settings.pinFilterColor.cancelButton }}
                            />
                        ]
                    }
                    modal = { true }
                    open = { this.state.pinDailog.open } >
                        <DualListBox
                            canFilter
                            preserveSelectOrder 
                            ref = "pinnedDialog"
                            options = { this.props.pinnedOptions }
                            selected = { this.state.pinnedDialogSelectedValues }
                            onChange = { (selected) => { this.setState({ pinnedDialogSelectedValues: selected }); } }
                        />
                </Dialog>
            </div>
        );
    }
}

export default FilterSideBarPinningViewsDialog;
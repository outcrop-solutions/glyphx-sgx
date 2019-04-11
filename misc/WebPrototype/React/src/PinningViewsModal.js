import React from 'react';
import DualListBox from 'react-dual-listbox';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';
import Dialog from 'material-ui/Dialog';
import FontIcon from 'material-ui/FontIcon';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './css/PinningViewsModal.css';


/**
 * This is the pop up dialog that you see when you click "pin filters" inside the pinned collapsible.
 * This class also encapsulates the dual list box.
 * @param pinnedOptions: - ADCMT
 * @param settings: - ADCMT
 * @param GLOBAL: - ADCMT
 * @param dispatch: - ADCMT
 * @param fullWidth: - ADCMT
 */
class PinningViewsModal extends React.Component {

    constructor(props) {
        super(props);
        
        this.state  = {
            pinDailog: {
                open: false,
            },
            pinnedDialogSelectedValues: [],
			pinnedDialogPrevState: []
        };
    };


    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState){
        return (this.state !== nextState || this.props.settings !== nextProps.settings);

        /*
        if (this.state != nextState) {
            return true;
        }
        else if (this.props.settings != nextProps.settings) {
            return true;
        }

        return false;
        */
    };


    /**
     * This function handles the open/close of the dialog.
     * This also handles the cancel click on the dialog.
     * @param {bool} open: should the dialog be open or not
     * @param {obj} extra: To pass some extra objects
     */
    handlePinDialogOpenClose = (open, extra) => {
        if (open) {
            this.setState({ pinnedDialogPrevState: this.state.pinnedDialogSelectedValues });
            this.setState({ pinDailog: { open: true } }); 
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
			if (pinnedValues.indexOf(prevSelectedValues[i]) === -1) {
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
                            backgroundColor: this.props.settings.colors.overviewButtonsColor.background
                        }} 
                        labelStyle = {{
                            fontSize: '13px',
                            color: this.props.settings.colors.overviewButtonsColor.text
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
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    fontSize: "20px",
                                    margin: "0px 0px 1px 14px"
                                }}
                            />
                        }
                    />
                </div>

                <Dialog
                    title = { <div> <FontIcon className = "fa fa-thumb-tack fa-2x" color = '#ffffff' /> &nbsp;&nbsp;Pin Filters </div> }
                    modal = { true }
                    open = { this.state.pinDailog.open } 
				    titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", lineHeight: "12px", padding: "10px 30px 14px"}}
                    actions = {
                        [
                            <FlatButton
                                label = "Ok"
                                primary = { true }
                                onClick = { () => this.onPinnedOkDailog(this) }
                                style = {{ color: this.props.settings.colors.pinFilterColor.okButton }}
                            />,
                            <FlatButton
                                label = "Cancel"
                                primary = { true }
                                onClick = { () => this.handlePinDialogOpenClose(false, {'cancel': true}) }
                                style = {{ color: this.props.settings.colors.pinFilterColor.cancelButton }}
                            />
                        ]
                    }
                >
                    <div style = {{ margin: "20px 0px 0px" }} >
                        <DualListBox
                            canFilter
                            preserveSelectOrder 
                            ref = "pinnedDialog"
                            options = { this.props.pinnedOptions }
                            selected = { this.state.pinnedDialogSelectedValues }
                            onChange = { (selected) => { this.setState({ pinnedDialogSelectedValues: selected }); } }
                        />
                    </div>
                </Dialog>
            </div>
        );
    }
}

export default PinningViewsModal;
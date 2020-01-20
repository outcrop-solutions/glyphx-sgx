import React from 'react';
import Flexbox from 'flexbox-react';
import RaisedButton from 'material-ui/RaisedButton';
import FontIcon from 'material-ui/FontIcon';
import TextField from 'material-ui/TextField';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './css/SearchBox.css';


/**
 * @param onTextFieldValueChange: - ADCMT
 * @param pinned: - ADCMT
 * @param id: - ADCMT
 * @param hintText: - ADCMT
 * @param collapseButton: - ADCMT
 **/
class SearchBox extends React.Component {

    constructor (props) {
        super(props);
        this.state = { 
            textFieldValue: ""
        }
    }


    /**
     * - ADCMT
     * @param evt: - ADCMT
     **/
    onChange = (evt) => {
        var tf = this.refs['SearchTextField'];
        var innerTF = tf.getInputNode();
        evt = { currentTarget: innerTF };

        this.setState({ textFieldValue: innerTF.value });

        if (typeof this.props.onTextFieldValueChange === "function" && !this.props.shouldOnBlur) {
            this.props.onTextFieldValueChange(evt, this.props.pinned);
        }
    }


    /**
     * - ADCMT
     * @param evt: - ADCMT
     **/
    clearText = (evt) => {
        var tf = this.refs['SearchTextField'];
        var innerTF = tf.getInputNode();
        evt = { currentTarget: innerTF };

        innerTF.value = "";

        this.setState({ textFieldValue: innerTF.value });

        return evt;
    }

    blurOnEnter(e, id) {
        if (e.which === 13) {
            var input = document.getElementById(id);
            input.blur();
        }
    }

    render() {
        return (
            <Flexbox flexDirection = "row" >
                <Flexbox style = {{ 
                    width: "100%", 
                    borderRadius: "0.521vh", 
                    // backgroundColor: this.props.settings.tableSelectColor 
                    }} > 

                    <TextField
                        id = { this.props.id }
                        type = "search"
                        ref = "SearchTextField"
                        value = { this.state.textFieldValue }
                        style = {{
                            // borderColor: "#d9d9d9 #ccc #b3b3b3",
                            // borderRadius: "4px",
                            border: "1px solid #ccc",
                            width: "100%",
                            height: "3.1vh",
                            fontSize: "1.668vh",
                            lineHeight: "2.503vh"
                            // fontFamily: "ITCFranklinGothicStd-Med"
                        }}
                        inputStyle = {{
                            padding: "0px 0.521vh 0px 0.521vh",
                        }}
                        hintStyle = {{
                            paddingLeft:"0.730vh",
                            bottom: "-0.104vh",
                        }}
                        underlineStyle = {{
                            borderColor: "rgb(245, 245, 255)",
                            margin: "0px 0px -0.834vh",
                        }}
                        
                        onChange = { this.onChange } 
                        onKeyPress = { (e) => this.blurOnEnter(e, this.props.id) }
                        hintText = {
                            <span style = {{ fontSize: 'inherit', color: 'rgba(0, 0, 0, 0.5)', fontSize: "1.668vh" }} >
                                <FontIcon
                                    className = "fa fa-search" 
                                    style = {{
                                        padding: '0px', 
                                        width: '2.505vw',
                                        height: '2.503vh',
                                        fontSize: 'inherit',
                                        color: 'inherit'
                                    }}
                                />
                                {this.props.hintText}
                            </span>
                        }
                        underlineFocusStyle = {{ 
                            borderColor: this.props.settings.searchBoxUnderline, 
                            margin: "0px 0px -1.034vh 0px",
                        }}
                        onBlur = { this.props.shouldOnBlur ? (evt) => this.props.onTextFieldValueChange(evt,this.props.pinned) : null }
                    /> 

                </Flexbox>

                {/* Conditionally renders the collapse button */}
                {this.props.collapseButton ? 
                    <Flexbox style = {{ width: "25%", margin: "0px 0.417vh 0px 0.938vh" }} > 
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Close all open columns
                                </div> 
                            }
                        >
                            <RaisedButton 
                                label = "Collapse" 
                                primary = { true } 
                                buttonStyle = {{
                                    height: "3.128vh",
                                    lineHeight: "3.128vh",
                                    backgroundColor: this.props.settings.overviewButtonsColorBg,
                                }} 
                                labelStyle = {{
                                    fontSize: "1.356vh",
                                    color: this.props.settings.overviewButtonsColorText
                                }}
                                overlayStyle = {{
                                    height: "3.128vh",
                                    lineHeight: "3.128vh",
                                }}
                                onClick = { (evt) => this.props.onCollapseAllClick(evt,this.props.pinned) }
                            />
                        </Tooltip>
                    </Flexbox>
                    : 
                    null
                }

            </Flexbox>
        );
    }
}

export default SearchBox;
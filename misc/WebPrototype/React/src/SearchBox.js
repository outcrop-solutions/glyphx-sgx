import React from 'react';
import Flexbox from 'flexbox-react';
import RaisedButton from 'material-ui/RaisedButton';
import FontIcon from 'material-ui/FontIcon';
import TextField from 'material-ui/TextField';
import 'font-awesome/css/font-awesome.min.css';

/**
 * @param onTextFieldValueChange: - ADCMT
 * @param pinned: - ADCMT
 * @param id: - ADCMT
 * @param hintText: - ADCMT
 * @param collapseButton: - ADCMT
 **/
class SearchBox extends React.Component {

    constructor(props){
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

        this.setState({
            textFieldValue: innerTF.value
        });

        if (typeof this.props.onTextFieldValueChange === "function") {
            this.props.onTextFieldValueChange(evt,this.props.pinned);
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

        this.setState({
            textFieldValue: innerTF.value
        });

        return evt;
    }

    render() {
        return (
            <Flexbox flexDirection = "row" style = {{ width: "100%" }} >

                <Flexbox style = {{ width: "100%", borderRadius: "5px", backgroundColor: this.props.settings.tableSelectColor }} > 
                    <TextField
                        id = {this.props.id}
                        type = "search"
                        ref = "SearchTextField"
                        value = {this.state.textFieldValue}
                        style = {{
                            borderColor: "#d9d9d9 #ccc #b3b3b3",
                            borderRadius: "4px",
                            border: "1px solid #ccc",
                            width: "100%",
                            height: "30px"
                        }}
                        inputStyle = {{
                            paddingLeft:"5px",
                            paddingRight:"5px"
                        }}
                        hintStyle = {{
                            paddingLeft:"7px",
                            bottom: "-1px"
                        }}
                        underlineStyle = {{
                            margin: "0px 0px -8px"
                        }}
                        onChange = {this.onChange} 
                        hintText = {
                            <span 
                                style = {{
                                    fontSize: 'inherit',
                                    color: 'rgba(0, 0, 0, 0.5)',
                                }}
                            >
                                <FontIcon
                                    className = "fa fa-search" 
                                    style = {{
                                        padding: '0px',
                                        width: '24px',
                                        height: '24px',
                                        fontSize: 'inherit',
                                        color: 'inherit'
                                    }}
                                />
                                {this.props.hintText}
                            </span>
                        }
                        underlineFocusStyle = {{ borderColor: this.props.settings.searchBoxUnderline, margin: "0px 0px -8px 0px" }}
                    /> 
                </Flexbox>

                {/* Conditionally renders the collapse button */}
                {this.props.collapseButton ? 
                    <Flexbox style = {{ width: "25%", margin: "0px 4px 0px 9px" }} > 
                        <RaisedButton 
                            label = "Collapse" 
                            primary = { true } 
                            buttonStyle = {{
                                height: '30px',
                                lineHeight: '30px',
                                backgroundColor: this.props.settings.overviewButtonsColorBg,
                            }} 
                            labelStyle = {{
                                fontSize: '13px',
                                color: this.props.settings.overviewButtonsColorText
                            }}
                            overlayStyle = {{
                                height: '30px',
                                lineHeight: '30px',
                            }}
                            onClick = { (evt) => this.props.onCollapseAllClick(evt,this.props.pinned) }
                        />
                    </Flexbox>
                : null
                }

            </Flexbox>
        );
    }
}

export default SearchBox;
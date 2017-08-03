import React, { Component } from 'react';
import {Flex} from 'react-flex-material';
import RaisedButton from 'material-ui/RaisedButton';
import FontIcon from 'material-ui/FontIcon';
import TextField from 'material-ui/TextField';
import 'font-awesome/css/font-awesome.min.css';


class SearchBox extends Component {

    constructor(props){
        super(props);
        this.state ={ 
            textFieldValue: ""
        }
    }

    onChange = (evt) => {
        var tf = this.refs['SearchTextField'];
        var innerTF = tf.getInputNode();
        evt = { currentTarget: innerTF };

        this.setState({
            textFieldValue: innerTF.value
        });

        if(typeof this.props.onTextFieldValueChange == "function")
            this.props.onTextFieldValueChange(evt,this.props.pinned);
    }

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
            <Flex layout = "row">
                <Flex 
                    flex = {this.props.collapseButton ? "75": "100"} 
                    align="space-between center" 
                >
                    <TextField
                        type = "text" 
                        id = {this.props.id}
                        type="search"
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
                            bottom: "-3px",
                            paddingLeft:"7px"
                        }}
                        underlineStyle = {{
                            margin: "0px 0px -8px 0px"
                        }}
                        onChange = {this.onChange} 
                        hintText = {
                            <span 
                                style = {{
                                        fontSize: 'inherit',
                                        color: 'rgba(0, 0, 0, 0.3)'
                                    }}
                            >
                                <FontIcon
                                    className="fa fa-search" 
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
                </Flex>
                {this.props.collapseButton ? 
                    <Flex flex = "25" style = {{ margin: "0px 0px 0px 9px" }}>
                        <RaisedButton 
                            label="Collapse" 
                            primary={true} 
                            buttonStyle={{
                                height: '30px',
                                lineHeight: '30px',
                                backgroundColor: this.props.settings.overviewButtonsColorBg,
                            }} 
                            labelStyle= {{
                                fontSize: '13px',
                                color: this.props.settings.overviewButtonsColorText
                            }}
                            overlayStyle = {{
                                height: '30px',
                                lineHeight: '30px',
                            }}
                            onClick={(evt) => this.props.onCollapseAllClick(evt,this.props.pinned)}
                        />
                    </Flex>
                : null
                }
            
            </Flex>
        );
    }
}

export default SearchBox;
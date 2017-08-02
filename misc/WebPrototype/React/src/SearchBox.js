import React, { Component } from 'react';
import {Flex} from 'react-flex-material';
import RaisedButton from 'material-ui/RaisedButton';
import FontIcon from 'material-ui/FontIcon';
import TextField from 'material-ui/TextField';
import 'font-awesome/css/font-awesome.css';


class SearchBox extends Component {

    constructor(props){
        super(props);
    }

    render() {
        return (
            <Flex layout = "row">
                <Flex flex = "80" style = {{ margin: "8px -18px 0px 0px" }} >
                    <TextField
                        type = "text" 
                        id = {this.props.id}
                        style = {{
                            borderColor: "#d9d9d9 #ccc #b3b3b3",
                            borderRadius: "4px",
                            border: "1px solid #ccc",
                            width: "100%",
                            height: "30px",
                            margin: "-7px 0px -8px 3px"
                        }}
                        inputStyle = {{
                            margin: "0px 0px 0px 8px"
                        }}
                        hintStyle = {{
                            margin: "0px 0px -12px 6px"
                        }}
                        underlineStyle = {{
                            margin: "0px 0px -8px 0px"
                        }}
                        onKeyUp = { (evt) => this.props.searchMultipleColumns(evt,this.props.pinned) } 
                        hintText = {
                            <span 
                                style = {{
                                        fontSize: 'inherit',
                                        margin: "5px -3px 0px 3px",
                                        color: 'rgba(0, 0, 0, 0.3)'
                                    }}
                            >
                                <FontIcon
                                    className="fa fa-search cursorHand" 
                                    style = {{
                                        padding: '0px',
                                        width: '24px',
                                        height: '24px',
                                        fontSize: 'inherit',
                                        //margin: "5px -3px 0px 3px",
                                        color: 'rgba(0, 0, 0, 0.3)'
                                    }}
                                />
                                Search for columns...
                            </span>
                        }
                        underlineFocusStyle = {{ borderColor: this.props.settings.searchBoxUnderline, margin: "0px 0px -8px 0px" }}
                    /> 
                </Flex>

                <Flex flex = "6">
                    <FontIcon
                        className="fa fa-times cursorHand" 
                        style = {{
                            padding: '0px',
                            fontSize: "16px",
                            margin: "8px 0px 0px 4px"
                        }}
                        hoverColor = {this.props.settings.SearchBoxClearHover}
                        onClick = { (evt) => this.props.clearSearchBox(evt,this.props.id) }
                    />
                </Flex>
                <Flex flex = "25" style = {{ margin: "2px 0px -2px 15px" }}>
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
                            color: this.props.settings.overviewButtonsColorText,
                            margin: "0px 0px 0px -5px"
                        }}
                        overlayStyle = {{
                            height: '30px',
                            lineHeight: '30px',
                        }}
                        onClick={(evt) => this.props.collapseAll(evt,this.props.pinned)}
                    />
                </Flex>
            </Flex>
        );
    }
}

export default SearchBox;
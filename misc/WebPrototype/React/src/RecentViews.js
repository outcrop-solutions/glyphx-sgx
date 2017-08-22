import React from 'react';
import { connect } from 'react-redux';
import { Flex } from 'react-flex-material';
import { Card, CardText } from 'material-ui/Card';
import { withRouter } from 'react-router-dom'
import injectTapEventPlugin from 'react-tap-event-plugin';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import RaisedButton from 'material-ui/RaisedButton';
import FontIcon from 'material-ui/FontIcon';
import './General.css';
import 'font-awesome/css/font-awesome.min.css';


class RecentViews extends React.Component {

    render() {
        return (
            <Flex layout = "column">
                <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                    >
                        <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Unsaved Session
                                    </Flex>

                                    <Flex flex = "35">
                                        2:19pm
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Some View
                                    </Flex>

                                    <Flex flex = "35">
                                        1:15pm
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Glyph 123
                                    </Flex>

                                    <Flex flex = "35">
                                        1:01pm
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Test Glyph
                                    </Flex>

                                    <Flex flex = "35">
                                        11:59am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Look At Me
                                    </Flex>

                                    <Flex flex = "35">
                                        11:19am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Who Am I
                                    </Flex>

                                    <Flex flex = "35">
                                        10:30am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Guy Above is Lame
                                    </Flex>

                                    <Flex flex = "35">
                                        10:10am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        How Many
                                    </Flex>

                                    <Flex flex = "35">
                                        9:45am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        Do I Need
                                    </Flex>

                                    <Flex flex = "35">
                                        9:19am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "grey-hover"
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%', margin: "1px 2px 3px 4px" }} >
                                    <Flex flex = "40" >
                                        To Make
                                    </Flex>

                                    <Flex flex = "35">
                                        8:00am
                                    </Flex>

                                    <Flex flex = "25">
                                        8/15/2017
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>

                    </CardText>
                </Card>
                
            </Flex>
        );
    }
}


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
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(RecentViews);
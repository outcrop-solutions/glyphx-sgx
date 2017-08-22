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

        var context = this;

        var recentList = [["Unsaved Session", "2:19pm", "8/15/2017"], ["Some View", "1:15pm", "8/15/2017"], ["Glyph 123", "1:01pm", "8/15/2017"], 
                            ["Test Glyph", "11:59am", "8/15/2017"], ["Look At Me", "11:19am", "8/15/2017"], ["Who Am I", "10:30am", "8/15/2017"]];

        var recentViews = recentList.map( function(view) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                                className = "inherit-hover"
                                onClick = { () => context.props.history.push("/glyph-viewer") }
                            >
                                <Flex layout = "row" style = {{ width: '100%', height: '100%' }} >
                                    <Flex flex = "40" >
                                        { view[0] }
                                    </Flex>

                                    <Flex flex = "35">
                                        { view[1] }
                                    </Flex>

                                    <Flex flex = "25">
                                        { view[2] }
                                    </Flex>  
                                </Flex>
                            </CardText>
                        </Card>
            )
        });

        return (
            <Flex layout = "column">
                <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                    >
                        {recentViews}
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
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(RecentViews));
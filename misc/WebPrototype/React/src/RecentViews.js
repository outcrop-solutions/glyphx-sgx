import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
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
                            ["Look At Me", "11:19am", "8/15/2017"], ["Test Glyph", "11:59am", "8/15/2017"], ["Who Am I", "10:30am", "8/15/2017"]];

        var recentViews = recentList.map( function(view) {
            return (
                <Card className = "inherit-hover noselect" containerStyle = {{ padding: "0px", borderRadius: "5px", width: "100%" }} style = {{ backgroundColor: context.props.settings.colors.general.lightBubble, height: "35px", width: "100%", borderRadius: "5px", marginTop: (view === recentList[0] ? "0px" : "5px") }} key = { view } >
                    <CardText
                        style = {{
                            padding: "7px",
                            borderRadius: "5px",
                            width: "100%"
                        }}
                        onClick = { () => context.props.history.push("/glyph-viewer") }
                    >
                        <Flexbox flexDirection = "row" minWidth = "100%" >
                            <Flexbox style = {{ width: "100%" }} > 
                                { view[0] }
                            </Flexbox>

                            <Flexbox style = {{ width: "60%" }} > 
                                { view[1] }
                            </Flexbox>

                            <Flexbox style = {{ width: "60%" }} > 
                                { view[2] }
                            </Flexbox>  
                        </Flexbox>
                    </CardText>
                </Card>
            )
        });

        return (
            <div>
                <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, marginBottom: "3px", paddingBottom: "4px", borderRadius: "2px" }} >
                    <div 
                        style = {{ 
                            color: this.props.settings.colors.overviewButtonsColor.text, 
                            margin: "0 auto",
                            width: "111px", 
                            paddingTop: "4px",
                            fontSize: "18px",
                            fontWeight: "normal"
                        }}
                        className = "noselect"
                        
                    > 
                        Recent Views
                    </div>
                </div>
                <div style = {{ padding: "7px" }} >
                    {recentViews}
                </div>
            </div>
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
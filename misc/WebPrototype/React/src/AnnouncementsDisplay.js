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


class AnnouncementsDisplay extends React.Component {
    
    render() {
        var context = this;
        var announcementList = [["Maintenance", "We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "Click to vote on which features you would like to see in the next release."], 
                                ["Shout", "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to read article."],
                                ["Maintenance", "filler text"], 
                                ["Release", "filler text"],
                                ["Poll", "filler text"], 
                                ["Shout", "filler text"]];

        var announcements = announcementList.map( function(announcement) {
            return (

                (announcement[0] === "Maintenance" ? <MaintenanceAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } /> : 
                    (announcement[0] === "Release" ? <ReleaseAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } /> :
                        (announcement[0] === "Poll" ? <PollAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } /> :
                            (announcement[0] === "Shout" ? <ShoutAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } /> :
                                "Error! Announcement Type Not Recognized!!"
                ))))
            )
        });

        return (
            <Flex layout = "column">
                <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} >
                    <CardText
                        style = {{
                            padding: "7px 7px 5px 7px",
                        }}
                    >
                        <div style = {{ maxHeight: (this.props.cardHeight - 555 + "px"), overflow: "auto" }} >
                            {announcements}
                        </div>
                    </CardText>
                </Card>
                
            </Flex>
        );
    }
}

class MaintenanceAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px" }}  >
                <CardText
                    style = {{
                        padding: "7px",
                        marginBottom: "2px",
                        backgroundColor: this.props.settings.colors.announcementColors.maintenance
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >
                    <Flex layout = "row" style = {{ width: '100%', height: '100%' }} >
                        <Flex flex = "10" >
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "30px" }} >
                                        <i className = "fa fa-exclamation-triangle" style = {{ fontSize: "20px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flex>

                        <Flex flex = "90">
                            { this.props.announcement }
                        </Flex>

                    </Flex>
                </CardText>
            </Card>
        );
    }
}

class ReleaseAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px" }}  >
                <CardText
                    style = {{
                        padding: "7px",
                        marginBottom: "2px",
                        backgroundColor: this.props.settings.colors.announcementColors.release
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >
                    <Flex layout = "row" style = {{ width: '100%', height: '100%' }} >
                        <Flex flex = "10" >
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "25px" }} >
                                        <i className = "fa fa-code-fork" style = {{ fontSize: "25px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flex>

                        <Flex flex = "90">
                            { this.props.announcement }
                        </Flex>

                    </Flex>
                </CardText>
            </Card>
        );
    }
}

class PollAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px" }}  >
                <CardText
                    style = {{
                        padding: "7px",
                        marginBottom: "2px",
                        backgroundColor: this.props.settings.colors.announcementColors.poll
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >
                    <Flex layout = "row" style = {{ width: '100%', height: '100%' }} >
                        <Flex flex = "10" >
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "30px" }} >
                                        <i className = "fa fa-gavel" style = {{ fontSize: "20px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flex>

                        <Flex flex = "90">
                            { this.props.announcement }
                        </Flex>

                    </Flex>
                </CardText>
            </Card>
        );
    }
}

class ShoutAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px" }}  >
                <CardText
                    style = {{
                        padding: "7px",
                        marginBottom: "2px",
                        backgroundColor: this.props.settings.colors.announcementColors.shout
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >
                    <Flex layout = "row" style = {{ width: '100%', height: '100%' }} >
                        <Flex flex = "10" >
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "30px" }} >
                                        <i className = "fa fa-bullhorn" style = {{ fontSize: "20px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flex>

                        <Flex flex = "90">
                            { this.props.announcement }
                        </Flex>

                    </Flex>
                </CardText>
            </Card>
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
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(AnnouncementsDisplay));
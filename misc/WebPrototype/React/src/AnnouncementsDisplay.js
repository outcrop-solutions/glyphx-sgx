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
import VotingModal from './VotingModal.js';
import './General.css';
import './AnnouncementsDisplay.css';
import 'font-awesome/css/font-awesome.min.css';


class AnnouncementsDisplay extends React.Component {
    
    render() {
        var context = this;
        var announcementList = [["Maintenance", "We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "Click to vote on which features you would like to see first in the next release."], 
                                ["Shout", "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article."],
                                ["Maintenance", "1 We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "1 Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "1 Click to vote on which features you would like to see first in the next release."], 
                                ["Shout", "1 John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article."],
                                ["Maintenance", "2 We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "2 Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "2 Click to vote on which features you would like to see first in the next release."], 
                                ["Shout", "2 John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article."]];

        var announcements = announcementList.map( function(announcement) {
            return (

                (announcement[0] === "Maintenance" ? <MaintenanceAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } last = { announcement === announcementList[announcementList.length - 1] } first = { announcement === announcementList[0] } /> : 
                    (announcement[0] === "Release" ? <ReleaseAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } last = { announcement === announcementList[announcementList.length - 1] } first = { announcement === announcementList[0] } /> :
                        (announcement[0] === "Poll" ? <PollAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } last = { announcement === announcementList[announcementList.length - 1] } first = { announcement === announcementList[0] } /> :
                            (announcement[0] === "Shout" ? <ShoutAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } last = { announcement === announcementList[announcementList.length - 1] } first = { announcement === announcementList[0] } /> :
                                "Error! Announcement Type Not Recognized!!"
                ))))
            )
        });

        return (
            <Flexbox flexGrow = {1} style = {{ height: "100%", minHeight: "0" }} >
                <Card containerStyle = {{ height: "100%", width: "100%", padding: "0px", borderRadius: "2px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} style = {{ borderRadius: "2px", width: "100%", height: "100%" }} >
                    <CardText
                        style = {{
                            padding: "7px 7px 0px 7px",
                            height: "100%",
                            width: "100%",
                            borderRadius: "2px",
                            overflowY: "auto"
                        }}
                        className = "announcementsScroll"
                    >
                        {announcements}

                        
    
                    </CardText>
                </Card>
            </Flexbox>
        );
    }
}

class MaintenanceAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ borderRadius: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "3px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        backgroundColor: this.props.settings.colors.announcementColors.maintenance,
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >
                    <Flexbox flexDirection="row"  >
                        <Flexbox style = {{ width: "15%" }} > 
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "30px" }} >
                                        <i className = "fa fa-exclamation-triangle" style = {{ fontSize: "20px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flexbox>

                        <Flexbox style = {{ width: "100%" }} > 
                            { this.props.announcement }
                        </Flexbox>

                    </Flexbox>
                </CardText>
            </Card>
        );
    }
}

class ReleaseAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ borderRadius: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "3px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        backgroundColor: this.props.settings.colors.announcementColors.release,
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >

                    <Flexbox flexDirection="row" >
                        <Flexbox style = {{ width: "15%" }} > 
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "25px" }} >
                                        <i className = "fa fa-code-fork" style = {{ fontSize: "25px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flexbox>

                        <Flexbox style = {{ width: "100%" }} > 
                            { this.props.announcement }
                        </Flexbox>

                    </Flexbox>
                </CardText>
            </Card>
        );
    }
}

class PollAnnouncement extends React.Component {

    state = {
        display: false
    }

    toggleDisplay = () => {
        if (this.state.display) {
            this.setState({ display: false });
        }
        else {
            this.setState({ display: true });
        }
        
    }

    render() {
        return (
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ borderRadius: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "3px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        backgroundColor: this.props.settings.colors.announcementColors.poll,
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    onClick = { this.toggleDisplay }
                >
                    <VotingModal display = { this.state.display } toggleDisplay = { this.toggleDisplay.bind(this)} />

                    <Flexbox flexDirection="row" >
                        <Flexbox style = {{ width: "15%" }} > 
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "30px" }} >
                                        <i className = "fa fa-gavel" style = {{ fontSize: "20px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flexbox>

                        <Flexbox style = {{ width: "100%" }} > 
                            { this.props.announcement }
                        </Flexbox>

                    </Flexbox>
                </CardText>
            </Card>
        );
    }
}

class ShoutAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ borderRadius: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "3px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        backgroundColor: this.props.settings.colors.announcementColors.shout,
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >

                    <Flexbox flexDirection="row" >
                        <Flexbox style = {{ width: "15%" }} > 
                            <div style = {{ display: "table", height: "100%", width: "100%" }} >
                                <div style = {{ display: "table-cell", verticalAlign: "middle" }} >
                                    <div style = {{ margin: "0 auto", width: "30px" }} >
                                        <i className = "fa fa-bullhorn" style = {{ fontSize: "20px" }} />
                                    </div>
                                </div>
                            </div>
                        </Flexbox>

                        <Flexbox style = {{ width: "100%" }} > 
                            { this.props.announcement }
                        </Flexbox>

                    </Flexbox>
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
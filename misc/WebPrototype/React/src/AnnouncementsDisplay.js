import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import Flexbox from 'flexbox-react';
import VotingModal from './VotingModal.js';
import ComponentLoadMask from './ComponentLoadMask.js';
import './General.css';


/**
 * -ADCMT
 */
class AnnouncementsDisplay extends React.Component {

    state = {
        loadMask: true,
        announcements: [] //restructure announcements to be = [{date: date, type: type, message: message}, ...]
    }


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

        var context = this;


        // Make server call to grab announcements
        //makeServerCall(something here),
        //    function (responseText) { 

                // Post the new data to the state and hide the window load-mask
                context.setState({ loadMask: false });
        //   }
        //);
	}

    
    render() {
        var context = this;
        var announcementList = [["Maintenance", "We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "Click to vote on which features you would like to see first."], 
                                ["Shout", "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article."],
                                ["Maintenance", "1 We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "1 Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "1 Click to vote on which features you would like to see first."], 
                                ["Shout", "1 John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article."],
                                ["Maintenance", "2 We will be performing several software updates and maintenance on our servers this Saturday, August 26th, at 9pm EST (2:00 GMT). Servers will be down for 2 hours."], 
                                ["Release", "2 Release 1.x.x is going to be live this Saturday, August 26th! Click to view the summary on new features."],
                                ["Poll", "2 Click to vote on which features you would like to see first."], 
                                ["Shout", "2 John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article."]];

        var announcements = announcementList.map( 
            function(announcement) {
                return (
                    (announcement[0] === "Maintenance" ? <MaintenanceAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } first = { announcement === announcementList[0] } /> : 
                        (announcement[0] === "Release" ? <ReleaseAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } first = { announcement === announcementList[0] } /> :
                            (announcement[0] === "Poll" ? <PollAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } first = { announcement === announcementList[0] } /> :
                                (announcement[0] === "Shout" ? <ShoutAnnouncement announcement = { announcement[1] } settings = { context.props.settings } key = { announcement } first = { announcement === announcementList[0] } /> :
                                    "Error! Announcement Type Not Recognized!!"
                                )
                            )
                        )         
                    )
                )
            }
        );

        return (
            <Flexbox flexGrow = {1} style = {{ height: "100%", minHeight: "0" }} >
                
                <div style = {{ width: "100%", display: (this.state.loadMask ? "" : "none") }} >
                    <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                </div>

                <div style = {{ padding: "7px", display: (this.state.loadMask ? "none" : "") }} >
                    <div style = {{ height: "100%", overflowY: "scroll", borderRadius: "3px" }} className = "customScroll" >
                        {announcements}
                    </div>
                </div>

            </Flexbox>
        );
    }
}


class MaintenanceAnnouncement extends React.Component {

    render() {
        return (
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ backgroundColor: this.props.settings.colors.general.lightBubble, borderRadius: "5px", marginRight: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "6px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >
                    <Flexbox flexDirection = "row" className = "noselect" >

                        <Flexbox flexDirection = "column" alignSelf = "center" >
                            <Flexbox style = {{ height: "50%" }} >
                                <i 
                                    className = "fa fa-exclamation-triangle" 
                                    style = {{ 
                                        fontSize: "20px",
                                        margin: "0px 9px 0px 0px",
                                        padding: "10px 0px 0px 9px",
                                        backgroundColor: this.props.settings.colors.announcementColors.maintenance,
                                        height: "40px",
                                        width: "40px",
                                        borderRadius: "3px"
                                    }} 
                                />
                            </Flexbox>
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
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ backgroundColor: this.props.settings.colors.general.lightBubble, borderRadius: "5px", marginRight: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "6px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >

                    <Flexbox flexDirection = "row" className = "noselect" >

                        <Flexbox flexDirection = "column" alignSelf = "center" >
                            <Flexbox style = {{ height: "50%" }} >
                                <i 
                                    className = "fa fa-code-fork" 
                                    style = {{ 
                                        fontSize: "29px",
                                        margin: "0px 9px 0px 0px",
                                        padding: "6px 0px 0px 13px",
                                        backgroundColor: this.props.settings.colors.announcementColors.release,
                                        height: "40px",
                                        width: "40px",
                                        borderRadius: "3px"
                                    }} 
                                />
                            </Flexbox>
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
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ backgroundColor: this.props.settings.colors.general.lightBubble, borderRadius: "5px", marginRight: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "6px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    onClick = { this.toggleDisplay }
                >
                    <VotingModal display = { this.state.display } toggleDisplay = { this.toggleDisplay.bind(this)} />

                    <Flexbox flexDirection = "row" className = "noselect" >

                        <Flexbox flexDirection = "column" alignSelf = "center" >
                            <Flexbox style = {{ height: "50%" }} >
                                <i 
                                    className = "fa fa-gavel" 
                                    style = {{ 
                                        fontSize: "20px",
                                        margin: "0px 9px 0px 0px",
                                        padding: "10px 0px 0px 9px",
                                        backgroundColor: this.props.settings.colors.announcementColors.poll,
                                        height: "40px",
                                        width: "40px",
                                        borderRadius: "3px"
                                    }} 
                                />
                            </Flexbox>
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
            <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ backgroundColor: this.props.settings.colors.general.lightBubble, borderRadius: "5px", marginRight: "5px" }} >
                <CardText
                    style = {{
                        padding: "7px",
                        marginTop: (this.props.first ? "0px" : "6px"),
                        marginBottom: (this.props.last ? "7px" : "0px"),
                        borderRadius: "5px"
                    }}
                    className = "cursorHand"
                    //onClick = {  }
                >

                    <Flexbox flexDirection = "row" className = "noselect" >

                        <Flexbox flexDirection = "column" alignSelf = "center" >
                            <Flexbox style = {{ height: "50%" }} >
                                <i 
                                    className = "fa fa-bullhorn" 
                                    style = {{ 
                                        fontSize: "20px",
                                        margin: "0px 9px 0px 0px",
                                        padding: "10px 0px 0px 9px",
                                        backgroundColor: this.props.settings.colors.announcementColors.shout,
                                        height: "40px",
                                        width: "40px",
                                        borderRadius: "3px"
                                    }} 
                                />
                            </Flexbox>
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
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(AnnouncementsDisplay);
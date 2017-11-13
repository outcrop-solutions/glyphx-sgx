import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import { Tabs, Tab } from 'material-ui/Tabs';
import Flexbox from 'flexbox-react';
import VotingModal from './VotingModal.js';
import ComponentLoadMask from './ComponentLoadMask.js';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import FontIcon from 'material-ui/FontIcon';
import SwipeableViews from 'react-swipeable-views';
import AnnouncementsDisplay from './AnnouncementsDisplay.js';
import DatePicker from 'material-ui/DatePicker';
import TimePicker from 'material-ui/TimePicker';
import RaisedButton from 'material-ui/RaisedButton';
import TextField from 'material-ui/TextField';
import './General.css';

const days = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"];
const months = ["January", "February", "March", "April", "May", "August", "June", "July", "September", "October", "November", "December"];

/**
 * -ADCMT
 */
class AdminWizardModal extends React.Component {

    constructor(props){
    super(props);

    this.state = {
        oldAnnouncements: [],
        newAnnouncements: [],
        maxAnnouncements: 20,
        oldFAQ: [],
        newFAQ: [],
        slideIndex: 0,
        announcementSelection: 0,
        maintenanceDuration: "",
        versionNumber: "",
        date: null,
        time: null,
    }

    this.handleDatePick = this.handleDatePick.bind(this);
    this.handleTimePick = this.handleTimePick.bind(this);
  };


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

        //var context = this;

        // Make server call to grab admin data (announcements and FAQ)
        //makeServerCall(something here),
        //    function (responseText) { 

                // Process data to proper format
                //var announcements = responseText.announcements
                //var FAQ = responseText.FAQ

                // Post the new data to the state
                //context.setState({ oldAnnouncements: announcements, newAnnouncements: announcements, oldFAQ: FAQ, newFAQ: FAQ });
        //   }
        //);

        this.setState({ oldAnnouncements: announcementList, newAnnouncements: announcementList });
	}


    deleteAnnouncement(toDelete) {
        var newAnnouncements = this.state.newAnnouncements;

        for (var i = 0; i < newAnnouncements.length; i++) {
            if (JSON.stringify(newAnnouncements[i]) == toDelete) {
                newAnnouncements.splice(i, 1);
            }
        }

        this.setState({ newAnnouncements: newAnnouncements });
    }


     /**
     * Updates local state values of min and max based on text field input
     * @param e: the event instance of the text field, html element
     **/
    onTextChange(e) {
        var value = e.target.value;

        if (e.target.name === "maintenanceDuration") {
            this.setState({ maintenanceDuration: value });
        }

        else if (e.target.name === "versionNumber") {
            this.setState({ versionNumber: value });
        }
        
    }

    handleTimePick(e, time) {
        var formatTime = time.toLocaleString().split(", ")[1];
        this.setState({ time: formatTime.slice(0, 5) + formatTime.slice(8) });
    }

    handleDatePick(e, date) {
        this.setState({ date:
            (days[date.getDay() - 1] + ", " + months[date.getMonth()] + " " + date.getDate() + 
            (date.getDate() % 10 === 1 ? "st, " : (date.getDate() % 10 === 2 ? "nd, " : (date.getDate() % 10 === 3 ? "rd, " : "th, "))) 
            + date.getFullYear())
        });
    }

    formatDate(date) {
        return (
            days[date.getDay() - 1] + ", " + months[date.getMonth()] + " " + date.getDate() + 
            (date.getDate() % 10 === 1 ? "st, " : (date.getDate() % 10 === 2 ? "nd, " : (date.getDate() % 10 === 3 ? "rd, " : "th, "))) 
            + date.getFullYear()
        );
    }

    onClickCreate() {
        var newAnnouncement = {};

        var pDate = new Date();
        var postDate = (pDate.getMonth() + 1) + "/" + pDate.getDate() + "/" + pDate.getFullYear();

        debugger;

        if (this.state.announcementSelection === 0) {
            var content = {};

            content.date = this.state.date;
            content.time = this.state.time;
            content.length = this.state.maintenanceDuration + " hours";

            newAnnouncement.type = "Maintenance";
            newAnnouncement.content = content;
            newAnnouncement.postDate = postDate;
        }

        var newAnnouncements = this.state.newAnnouncements;
        newAnnouncements.unshift(newAnnouncement);

        this.setState({ newAnnouncements: newAnnouncements });
    }
    
    render() {

        return (
            <Dialog
				title = { <div> <FontIcon className = "fa fa-pencil-square-o fa-2x" color = '#ffffff' /> &nbsp;&nbsp;Admin Wizard </div> }
                ref = "Settings"
                modal = { true }
				open = { this.props.adminDisplay }
				bodyStyle = {{ padding: "0px 24px 10px" }}
                contentStyle = {{ width: "85%", maxWidth: "none" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", lineHeight: "12px", padding: "10px 30px 14px"}}
				actions = {
					[
                        <FlatButton
                            label = "Save"
                            primary = { true }
                            onClick = { null }
                            style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                        />,
                        <FlatButton
                            label = "Cancel"
                            primary = { true }
                            onClick = { () => this.props.dispatch(editModalDisplay(false)) }
                            style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
                        />
					]
				}
			>
                <div style = {{ height: "75vh", marginTop: "10px" }} >
                    <Tabs onChange = { (value) => this.setState({ slideIndex: value }) } value = { this.state.slideIndex } >
                        <Tab 
                            label = "Announcements" 
                            value = { 0 }
                            icon = { <FontIcon style = {{ color: (this.state.slideIndex === 0 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") }} className = "fa fa-bullhorn" ></FontIcon> }
                            buttonStyle = { {height: "49px", backgroundColor: (this.state.slideIndex === 0 ? "#272958" : this.props.settings.colors.filterTabColor.tabBackground), color: (this.state.slideIndex === 0 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") } }
                        />
                        <Tab 
                            label = "FAQ" 
                            value = { 1 }
                            icon = { <FontIcon style = {{ color: (this.state.slideIndex === 1 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") }} className="fa fa-question-circle-o"></FontIcon> }
                            buttonStyle = { {height: "49px", backgroundColor: (this.state.slideIndex === 1 ? "#272958" : this.props.settings.colors.filterTabColor.tabBackground), color: (this.state.slideIndex === 1 ? this.props.settings.colors.filterTabColor.titleText : "#adadad") } }
                        />
                    </Tabs>

                    <SwipeableViews
                        index = { this.state.slideIndex }
                        onChangeIndex = { this.handleChange }
                        style = {{
                            overflowY: "hidden",
                            padding: "0px"
                        }}           
                    >
                        <Flexbox flexDirection = "row" style = {{ height: "100%" }} >
                            <Flexbox flexDirection = "column" style = {{ height: "70vh", minHeight: "0", width: "40%" }} >
                                <AnnouncementsDisplay adminEdit = { true } announcementList = { this.state.newAnnouncements } deleteAnnouncement = { this.deleteAnnouncement.bind(this) } />
                            </Flexbox>

                            <Flexbox flexDirection = "column" style = {{ width: "60%" }} >

                                <br />

                                <div style = {{ width: "235px", margin: "-4px auto -14px", fontWeight: "bold", fontSize: "18px" }} > Create New Announcement </div>

                                <br />

                                <Flexbox flexDirection = "row" style = {{ height: "40px", margin: "0px auto 30px" }} >
                                    <i 
                                        className = "fa fa-exclamation-triangle cursorHand" 
                                        style = {{ 
                                            fontSize: "20px",
                                            margin: "0px 25px 0px 0px",
                                            padding: "10px 0px 0px 9px",
                                            backgroundColor: this.state.announcementSelection === 0 ? this.props.settings.colors.announcementColors.maintenance : "#c7c8ca",
                                            height: "40px",
                                            width: "40px",
                                            color: this.state.announcementSelection === 0 ? "#000000" : "",
                                            borderRadius: "3px"
                                        }}
                                        onClick = { () => this.setState({ announcementSelection: 0, content: {} }) }
                                    />
                                    <i 
                                        className = "fa fa-code-fork cursorHand" 
                                        style = {{ 
                                            fontSize: "29px",
                                            margin: "0px 25px 0px 0px",
                                            padding: "6px 0px 0px 13px",
                                            backgroundColor: this.state.announcementSelection === 1 ? this.props.settings.colors.announcementColors.release : "#c7c8ca",
                                            height: "40px",
                                            width: "40px",
                                            color: this.state.announcementSelection === 1 ? "#000000" : "",
                                            borderRadius: "3px"
                                        }} 
                                        onClick = { () => this.setState({ announcementSelection: 1, content: {} }) }
                                    />
                                    <i 
                                        className = "fa fa-gavel cursorHand" 
                                        style = {{ 
                                            fontSize: "20px",
                                            margin: "0px 25px 0px 0px",
                                            padding: "10px 0px 0px 9px",
                                            backgroundColor: this.state.announcementSelection === 2 ? this.props.settings.colors.announcementColors.poll : "#c7c8ca",
                                            height: "40px",
                                            width: "40px",
                                            color: this.state.announcementSelection === 2 ? "#000000" : "",
                                            borderRadius: "3px"
                                        }} 
                                        onClick = { () => this.setState({ announcementSelection: 2, content: {} }) }
                                    />
                                    <i 
                                        className = "fa fa-bullhorn cursorHand" 
                                        style = {{ 
                                            fontSize: "20px",
                                            padding: "10px 0px 0px 9px",
                                            backgroundColor: this.state.announcementSelection === 3 ? this.props.settings.colors.announcementColors.shout : "#c7c8ca",
                                            height: "40px",
                                            width: "40px",
                                            color: this.state.announcementSelection === 3 ? "#000000" : "",
                                            borderRadius: "3px"
                                        }} 
                                        onClick = { () => this.setState({ announcementSelection: 3, content: {} }) }
                                    />
                                </Flexbox>
                                
                                {this.state.announcementSelection === 0 ?
                                    <Flexbox flexDirection = "column" >
                                        <Flexbox flexDirection = "row" style = {{ paddingLeft: "30px" }} >
                                            <Flexbox style = {{ width: "50%" }} >
                                                <div style = {{ margin: "16px 10px 0px 0px", fontWeight: "bold", fontSize: "16px" }} > Start date of maintenance: </div>
                                            </Flexbox>

                                            <Flexbox style = {{ width: "50%" }} >
                                                <DatePicker 
                                                    hintText = "Date"
                                                    dialogContainerStyle = {{ color: "red" }}
                                                    onChange = { this.handleDatePick }
                                                    formatDate = { this.formatDate }
                                                />
                                            </Flexbox>
                                        </Flexbox>

                                        <Flexbox flexDirection = "row" style = {{ paddingLeft: "30px" }} >
                                            <Flexbox style = {{ width: "50%" }} >
                                                <div style = {{ margin: "16px 10px 0px 0px", fontWeight: "bold", fontSize: "16px" }} > Time of maintenance (EST): </div>
                                            </Flexbox>
                                            
                                            <Flexbox style = {{ width: "50%" }} >
                                                <TimePicker
                                                    hintText = "Time"
                                                    minutesStep = {15}
                                                    onChange = { this.handleTimePick }
                                                />
                                            </Flexbox>
                                        </Flexbox>

                                        <Flexbox flexDirection = "row" style = {{ paddingLeft: "30px" }} >
                                            <Flexbox style = {{ width: "50%" }} >
                                                <div style = {{ margin: "16px 10px 0px 0px", fontWeight: "bold", fontSize: "16px" }} > Estimated downtime (Hours): </div>
                                            </Flexbox>
                                            
                                            <Flexbox style = {{ width: "50%" }} >
                                                <TextField 
                                                    type = 'number'
                                                    name = "maintenanceDuration" 
                                                    value = { this.state.maintenanceDuration }
                                                    hintText = "Duration"
                                                    onChange = { (e) => this.onTextChange(e) }
                                                    underlineFocusStyle = {{ borderColor: this.props.settings.colors.rangeColor.textFieldUnderline }}
                                                    underlineStyle = {{ borderColor: "#d2d2d2" }}
                                                />
                                            </Flexbox>
                                        </Flexbox>
                                    </Flexbox>
                                    :
                                    <Flexbox flexDirection = "column" >

                                        <Flexbox style = {{ margin: "0 auto" }} >
                                            <TextField 
                                                name = "versionNumber" 
                                                value = { this.state.versionNumber }
                                                hintText = "Version Number"
                                                hintStyle = {{ color: "#707192", marginLeft: "6px" }}
                                                onChange = { (e) => this.onTextChange(e) }
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.rangeColor.textFieldUnderline }}
                                            />
                                        </Flexbox>

                                        <Flexbox flexDirection = "row" style = {{ padding: "0px 30px" }} >
                                            <Flexbox flexDirection = "column" style = {{ width: "45%" }} >
                                                <TextField 
                                                    hintText = "Feature description"
                                                    style = {{ width: "100%" }}
                                                    multiLine = { true }
                                                    rows = { 1 }
                                                    rowsMax = { 4 }
                                                    hintStyle = {{ color: "#707192", marginLeft: "6px" }}
                                                    textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                    underlineStyle = {{ borderColor: "grey" }}
                                                    underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                />

                                                <RaisedButton 
                                                    label = "Add Feature"
                                                    style = {{ width: "50%", margin: "10px auto 0px" }}
                                                    buttonStyle = {{
                                                        height: '35px',
                                                        lineHeight: '35px',
                                                        backgroundColor: "#272958"
                                                    }} 
                                                    labelStyle = {{
                                                        fontSize: '14px',
                                                        color: this.props.settings.colors.overviewButtonsColor.text,
                                                        margin: "0px 0px 0px -3px",
                                                        paddingLeft: "0px",
                                                        paddingRight: "0px"
                                                    }}
                                                    overlayStyle = {{ height: '35px', lineHeight: '35px' }}
                                                    //onClick = { () => this.onClickCreate() }
                                                    primary = { true } 
                                                />
                                            </Flexbox>

                                            <Flexbox style = {{ width: "10%" }} />

                                            <Flexbox flexDirection = "column" style = {{ width: "45%" }} >
                                                <TextField 
                                                    hintText = "Bugfix description"
                                                    style = {{ width: "100%" }}
                                                    multiLine = { true }
                                                    rows = { 1 }
                                                    rowsMax = { 4 }
                                                    hintStyle = {{ color: "#707192", marginLeft: "6px" }}
                                                    textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                    underlineStyle = {{ borderColor: "grey" }}
                                                    underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                />

                                                <RaisedButton 
                                                    label = "Add Bugfix"
                                                    style = {{ width: "50%", margin: "10px auto 0px" }}
                                                    buttonStyle = {{
                                                        height: '35px',
                                                        lineHeight: '35px',
                                                        backgroundColor: "#272958"
                                                    }} 
                                                    labelStyle = {{
                                                        fontSize: '14px',
                                                        color: this.props.settings.colors.overviewButtonsColor.text,
                                                        margin: "0px 0px 0px -3px",
                                                        paddingLeft: "0px",
                                                        paddingRight: "0px"
                                                    }}
                                                    overlayStyle = {{ height: '35px', lineHeight: '35px' }}
                                                    //onClick = { () => this.onClickCreate() }
                                                    primary = { true } 
                                                />
                                            </Flexbox>
                                        </Flexbox>
                                    </Flexbox>
                                }
                                



                                <RaisedButton 
                                    label = "Create"
                                    style = {{ width: "50%", margin: "50px auto 0px" }}
                                    buttonStyle = {{
                                        height: '35px',
                                        lineHeight: '35px',
                                        backgroundColor: "#272958"
                                    }} 
                                    labelStyle = {{
                                        fontSize: '14px',
                                        color: this.props.settings.colors.overviewButtonsColor.text,
                                        margin: "0px 0px 0px -3px",
                                        paddingLeft: "0px",
                                        paddingRight: "0px"
                                    }}
                                    overlayStyle = {{ height: '35px', lineHeight: '35px' }}
                                    onClick = { () => this.onClickCreate() }
                                    primary = { true } 
                                />

                            </Flexbox>
                        </Flexbox>

                        <div>
                            2
                        </div>
                    </SwipeableViews>
                </div>
            </Dialog>     
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (adminModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    adminModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
    adminDisplay : state.filterState.ModalDisplay.adminModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(AdminWizardModal);



var announcementList = [
    {
        type: "Maintenance",
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/8/2017"
    },
    {
        type: "Release",
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/8/2017"
    },
    {
        type: "Poll",
        content: {
            date: "August 26th, 2017",
            poll: {
                options: ["Have Aditya stop playing music.", "Have Brad stop messing with peoples desks.", "Have lunch on Mark", "Have the day off"],
                shortOptions: ["Aditya stop music", "Brad stop desks", "Lunch on Mark", "Have day off" ],
                colors: ['#43A19E', '#7B43A1', '#F2317A', '#FF9824']
            }
        },
        postDate: "11/8/2017"
    },
    {
        type: "Shout",
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/8/2017"
    },
    {
        type: "Maintenance",
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/7/2017"
    },
    {
        type: "Release",
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/7/2017"
    },
    {
        type: "Poll",
        content: {
            date: "August 26th, 2017",
            poll: {
                options: ["Have Aditya stop playing music.", "Have Brad stop messing with peoples desks.", "Have lunch on Mark", "Have the day off"],
                shortOptions: ["Aditya stop music", "Brad stop desks", "Lunch on Mark", "Have day off" ],
                colors: ['#43A19E', '#7B43A1', '#F2317A', '#FF9824']
            }
        },
        postDate: "11/7/2017"
    },
    {
        type: "Shout",
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/7/2017"
    },
    {
        type: "Maintenance",
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/6/2017"
    },
    {
        type: "Release",
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/6/2017"
    },
    {
        type: "Poll",
        content: {
            date: "August 26th, 2017",
            poll: {
                options: ["Have Aditya stop playing music.", "Have Brad stop messing with peoples desks.", "Have lunch on Mark", "Have the day off"],
                shortOptions: ["Aditya stop music", "Brad stop desks", "Lunch on Mark", "Have day off" ],
                colors: ['#43A19E', '#7B43A1', '#F2317A', '#FF9824']
            }
        },
        postDate: "11/6/2017"
    },
    {
        type: "Shout",
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/6/2017"
    }
];
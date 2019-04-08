import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import { Tabs, Tab } from 'material-ui/Tabs';
import { guidGenerator } from './GeneralFunctions.js';
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
import PieChart from './PieChart.js';
import AdminAnnouncementEditor from './AdminAnnouncementEditor.js';
import './css/General.css';


/**
 * -ADCMT
 */
class AdminWizardModal extends React.Component {

    constructor(props){
    super(props);

    this.state = {
        slideIndex: 0,
    }
  };
    
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
                            label = "Close"
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
                        <AdminAnnouncementEditor />

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
        id: guidGenerator(),
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/8/2017"
    },
    {
        type: "Release",
        id: guidGenerator(),
        content: {
            release: "1.0.0",
            features: ["cool feature 1wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww", "cool feature 2", "cool feature 3" ],
            bugfixes: []
        },
        postDate: "11/8/2017"
    },
    {
        type: "Poll",
        id: guidGenerator(),
        content: {
            date: "August 26th, 2017",
            question: "Which features would you like to see first?",
            options: ["Have Aditya stop playing music.", "Have Brad stop messing with peoples desks.", "Have lunch on Mark", "Have the day off"],
            shortOptions: ["Aditya stop music", "Brad stop desks", "Lunch on Mark", "Have day off" ],
            colors: ['#43A19E', '#7B43A1', '#F2317A', '#FF9824']
        },
        postDate: "11/6/2017"
    },
    {
        type: "Shout",
        id: guidGenerator(),
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/8/2017"
    },
    {
        type: "Maintenance",
        id: guidGenerator(),
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/7/2017"
    },
    {
        type: "Release",
        id: guidGenerator(),
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/7/2017"
    },
    {
        type: "Poll",
        id: guidGenerator(),
        content: {
            date: "August 26th, 2017",
            question: "Which features would you like to see first?",
            options: ["Have Aditya stop playing music.", "Have Brad stop messing with peoples desks.", "Have lunch on Mark", "Have the day off"],
            shortOptions: ["Aditya stop music", "Brad stop desks", "Lunch on Mark", "Have day off" ],
            colors: ['#43A19E', '#7B43A1', '#F2317A', '#FF9824']
        },
        postDate: "11/6/2017"
    },
    {
        type: "Shout",
        id: guidGenerator(),
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/7/2017"
    },
    {
        type: "Maintenance",
        id: guidGenerator(),
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/6/2017"
    },
    {
        type: "Release",
        id: guidGenerator(),
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/6/2017"
    },
    {
        type: "Poll",
        id: guidGenerator(),
        content: {
            date: "August 26th, 2017",
            question: "Which features would you like to see first?",
            options: ["Have Aditya stop playing music.", "Have Brad stop messing with peoples desks.", "Have lunch on Mark", "Have the day off"],
            shortOptions: ["Aditya stop music", "Brad stop desks", "Lunch on Mark", "Have day off" ],
            colors: ['#43A19E', '#7B43A1', '#F2317A', '#FF9824']
        },
        postDate: "11/6/2017"
    },
    {
        type: "Shout",
        id: guidGenerator(),
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/6/2017"
    }
];
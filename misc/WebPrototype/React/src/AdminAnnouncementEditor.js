import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import { Tabs, Tab } from 'material-ui/Tabs';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import FontIcon from 'material-ui/FontIcon';
import DatePicker from 'material-ui/DatePicker';
import TimePicker from 'material-ui/TimePicker';
import RaisedButton from 'material-ui/RaisedButton';
import TextField from 'material-ui/TextField';
import SwipeableViews from 'react-swipeable-views';
import Flexbox from 'flexbox-react';
import { guidGenerator } from './GeneralFunctions.js';
import VotingModal from './VotingModal.js';
import ComponentLoadMask from './ComponentLoadMask.js';
import AnnouncementsDisplay from './AnnouncementsDisplay.js';
import PieChart from './PieChart.js';
import './css/General.css';

const days = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"];
const months = ["January", "February", "March", "April", "May", "August", "June", "July", "September", "October", "November", "December"];
const pieColors = ["#3949ab", "#00acc1", "#43a047", "#fdd835", "#fb8c00", "#f4511e"];

/**
 * -ADCMT
 */
class AdminAnnouncementEditor extends React.Component {

    constructor(props){
    super(props);

    this.state = {
        oldAnnouncements: [],
        newAnnouncements: [],
        maxAnnouncements: 20,

        announcementSelection: 0,

        maintenanceDuration: "",
        date: null,
        time: null,

        versionNumber: "",
        featureText: "",
        bugfixText: "",
        features: [],
        bugfixes: [],

        question: "",
        fullAnswer: "",
        shortAnswer: "",
        fullAnswers: [],
        shortAnswers: [],
        colors: [],
        data: [],

        shoutMessage: "",
        shoutLink: "",

        doIHoldControl: false
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

                // Post the new data to the state
                //context.setState({ oldAnnouncements: announcements, newAnnouncements: announcements, oldFAQ: FAQ, newFAQ: FAQ });
                //context.props.dispatch( setTimer(new Date().getTime()) );
        //   }
        //);

        this.setState({ oldAnnouncements: announcementList, newAnnouncements: announcementList });
	}


    /**
     * Will remove an announcement locally (doesnt remove from server)
     * @param idToDelete: id of the announcement to delete
     */
    deleteAnnouncement(idToDelete) {
        var newAnnouncements = this.state.newAnnouncements;

        for (var i = 0; i < newAnnouncements.length; i++) {
            if (newAnnouncements[i].id == idToDelete) {
                newAnnouncements.splice(i, 1);
            }
        }

        this.setState({ newAnnouncements: newAnnouncements });
    }


    /**
     * Adds a feature or bugfix to the corresponding list
     * @param type: feature / bugfix
     */
    onClickFeatureBug(type) {
        if (type === "feature") {
            if (this.state.featureText.trim() !== "") {
                var features = this.state.features;
                features.push(this.state.featureText);
                this.setState({ features: features, featureText: "" });
            }
        }

        else {
            if (this.state.bugfixText.trim() !== "") {
                var bugfixes = this.state.bugfixes;
                bugfixes.push(this.state.bugfixText);
                this.setState({ bugfixes: bugfixes, bugfixText: "" });
            }
        }
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

        else if (e.target.name === "featureText") {
            this.setState({ featureText: value });
        }

        else if (e.target.name === "bugfixText") {
            this.setState({ bugfixText: value });
        }

        else if (e.target.name === "versionNumber") {
            this.setState({ versionNumber: value });
        }

        else if (e.target.name === "question") {
            this.setState({ question: value });
        }

        else if (e.target.name === "fullAnswer") {
            this.setState({ fullAnswer: value });
        }

        else if (e.target.name === "shortAnswer") {
            if (value.split(" ").length < 5) {
                this.setState({ shortAnswer: value });
            }
        }

        else if (e.target.name === "shoutLink") {
            this.setState({ shoutLink: value });
        }

        else if (e.target.name === "shoutMessage") {
            this.setState({ shoutMessage: value });
        }
    }


    /**
     * Sets the local state to track the time chosen
     * @param e: null event instance
     * @param time: object which contains information about the time selected
     */
    handleTimePick(e, time) {
        var formatTime = time.toLocaleString().split(", ")[1];
        this.setState({ time: formatTime.slice(0, 5) + formatTime.slice(8) });
    }


    /**
     * Sets the local state to track the date chosen
     * @param e: null event instance
     * @param date: object which contains information about the date selected
     */
    handleDatePick(e, date) {
        this.setState({ date:
            (days[date.getDay() - 1] + ", " + months[date.getMonth()] + " " + date.getDate() + 
            (date.getDate() % 10 === 1 ? "st, " : (date.getDate() % 10 === 2 ? "nd, " : (date.getDate() % 10 === 3 ? "rd, " : "th, "))) 
            + date.getFullYear())
        });
    }


    /**
     * Function passed as a param to the date picker
     * Regulates the appearance of the date selected in the text field
     * @param date: object which contains information about the date selected
     */
    formatDate(date) {
        return (
            days[date.getDay() - 1] + ", " + months[date.getMonth()] + " " + date.getDate() + 
            (date.getDate() % 10 === 1 ? "st, " : (date.getDate() % 10 === 2 ? "nd, " : (date.getDate() % 10 === 3 ? "rd, " : "th, "))) 
            + date.getFullYear()
        );
    }


    /**
     * Handles deleting a feature or bugfix from the preview
     * @param type: feature / bugfix
     * @param toRemove: string indicating the feature / bugfix
     */
    removeElement(type, toRemove) {
        if (type === "feature") {
            var features = this.state.features;
            var index = features.indexOf(toRemove);

            if (index !== -1){
                features.splice(index, 1);
                this.setState({ features: features });
            }
        }

        else {
            var bugfixes = this.state.bugfixes;
            var index = bugfixes.indexOf(toRemove);

            if (index !== -1){
                bugfixes.splice(index, 1);
                this.setState({ bugfixes: bugfixes });
            }
        }
    }


    /**
     * Updates the state to add an answer to the poll announcement
     */
    onAddAnswer() {
        var fullAnswers = this.state.fullAnswers;
        var shortAnswers = this.state.shortAnswers;
        var colors = this.state.colors;

        fullAnswers.push(this.state.fullAnswer);
        shortAnswers.push(this.state.shortAnswer);
        colors.push(pieColors[fullAnswers.length - 1]);

        var data = [];
        for (var i = 0; i < fullAnswers.length; i++) {
            data.push(100 / fullAnswers.length);
        }

        this.setState({ fullAnswers: fullAnswers, shortAnswers: shortAnswers, colors: colors, data: data, fullAnswer: "", shortAnswer: "" });
    }


    /**
     * Blurs the text field when the enter key is pressed
     * @param e: key press event
     * @param id: id of the text field to blur
     */
    blurOnEnter(e, id) {
        if (e.which === 13) {
            if (id === "featureText") {
                this.onClickFeatureBug("feature");
            }
            else if (id === "bugfixText") {
                this.onClickFeatureBug("bugfix");
            }

            var input = document.getElementById(id);
            input.blur();
        }
    }


    /**
     * Handles creation of an announcement based on fields saved in the state
     */
    onClickCreate() {
        var newAnnouncement = {
            id: guidGenerator()
        };

        var pDate = new Date();
        var postDate = (pDate.getMonth() + 1) + "/" + pDate.getDate() + "/" + pDate.getFullYear();

        // debugger;

        if (this.state.announcementSelection === 0) {
            var content = {};

            content.date = this.state.date;
            content.time = this.state.time;
            content.length = this.state.maintenanceDuration + " hours";

            newAnnouncement.type = "Maintenance";
            newAnnouncement.content = content;
            newAnnouncement.postDate = postDate;
        }
        
        else if (this.state.announcementSelection === 1) {
            var content = {};

            content.release = this.state.versionNumber;
            content.features = this.state.features;
            content.bugfixes = this.state.bugfixes;

            newAnnouncement.type = "Release";
            newAnnouncement.content = content;
            newAnnouncement.postDate = postDate;
        }

        else if (this.state.announcementSelection === 2) {
            var content = {};

            content.date = this.state.date;
            content.question = this.state.question;
            content.options = this.state.fullAnswers;
            content.shortOptions = this.state.shortAnswers;
            content.colors = this.state.colors;

            newAnnouncement.type = "Poll";
            newAnnouncement.content = content;
            newAnnouncement.postDate = postDate;
        }

        else {
            var content = {};
            
            var link = this.state.shoutLink;
            if (!link.startsWith("http") ) {
                if (link.startsWith("www")) {
                    link = "http://" + link;
                }
                else {
                    link = "http://www." + link;
                }
            }

            content.message = this.state.shoutMessage;
            content.link = link;

            newAnnouncement.type = "Shout";
            newAnnouncement.content = content;
            newAnnouncement.postDate = postDate;
        }


        var newAnnouncements = this.state.newAnnouncements;
        newAnnouncements.unshift(newAnnouncement);

        this.setState({ 
            newAnnouncements: newAnnouncements, 

            maintenanceDuration: "", 
            date: null,
            time: null,

            versionNumber: "",
            featureText: "",
            bugfixText: "",
            features: [],
            bugfixes: [],

            question: "",
            fullAnswer: "",
            shortAnswer: "",
            fullAnswers: [],
            shortAnswers: [],
            colors: [],
            data: [],

            shoutMessage: "",
            shoutLink: "",
        });
    }

    
    render() {
        var context = this;

        var features = this.state.features.map( 
            function(feature) {
                return (
                    <li 
                        key = { feature } 
                        className = "redHover cursorHand" 
                        onClick = { context.removeElement.bind(context, "feature", feature) } 
                        style = {{ wordBreak: "break-word" }}
                    >
                        {feature}<br />
                    </li>
                )
            }
        );

        var bugfixes = this.state.bugfixes.map( 
            function(bugfix) {
                return (
                    <li 
                        key = { bugfix } 
                        className = "redHover cursorHand" 
                        onClick = { context.removeElement.bind(context, "bugfix", bugfix) }
                        style = {{ wordBreak: "break-word" }}
                    >
                        {bugfix}<br />
                    </li>
                )
            }
        );

        var shortOptions = this.state.shortAnswers.map( 
            function(option) {
                return (
                    <div key = { option } >
                        <dt style = {{ borderRadius: "3px", display: "inline", backgroundColor: context.state.colors[context.state.shortAnswers.indexOf(option)] }} > &nbsp; &nbsp;  </dt>
                        <dd style = {{ marginLeft: "15px", display: "inline" }} > {option} </dd>
                    </div>
                )
            }
        );

        return (
            <Flexbox flexDirection = "row" style = {{ height: "100%" }} >

                {/* Announcements Admin Dislay */}
                <Flexbox flexDirection = "column" style = {{ height: "70vh", minHeight: "0", width: "40%" }} >
                    <AnnouncementsDisplay adminEdit = { true } announcementList = { this.state.newAnnouncements } deleteAnnouncement = { this.deleteAnnouncement.bind(this) } />
                </Flexbox>

                <Flexbox flexDirection = "column" style = {{ width: "60%" }} >

                    <br />
                    <div style = {{ width: "235px", margin: "-4px auto -14px", fontWeight: "bold", fontSize: "18px" }} > Create New Announcement </div>
                    <br />

                    {/* Announcement Type Selection Buttons */}
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
                            onClick = { () => this.setState({ announcementSelection: 0 }) }
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
                            onClick = { () => this.setState({ announcementSelection: 1 }) }
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
                            onClick = { () => this.setState({ announcementSelection: 2 }) }
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
                            onClick = { () => this.setState({ announcementSelection: 3 }) }
                        />
                    </Flexbox>
                    
                    {/* Warning Announcement Creation */}
                    {this.state.announcementSelection === 0 ?
                        <Flexbox flexDirection = "column" >

                            <Flexbox flexDirection = "row" style = {{ paddingLeft: "30px", marginBottom: "10px" }} >
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

                            <Flexbox flexDirection = "row" style = {{ paddingLeft: "30px", marginBottom: "10px" }} >
                                <Flexbox style = {{ width: "50%" }} >
                                    <div style = {{ margin: "16px 10px 0px 0px", fontWeight: "bold", fontSize: "16px" }} > Time of maintenance (EST): </div>
                                </Flexbox>
                                
                                <Flexbox style = {{ width: "50%" }} >
                                    <TimePicker
                                        hintText = "Time"
                                        minutesStep = { 15 }
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
                                        id = "maintenanceDuration" 
                                        value = { this.state.maintenanceDuration }
                                        hintText = "Duration"
                                        onChange = { (e) => this.onTextChange(e) }
                                        onKeyPress = { (e) => this.blurOnEnter(e, "maintenanceDuration") }
                                        underlineFocusStyle = {{ borderColor: this.props.settings.colors.rangeColor.textFieldUnderline }}
                                        underlineStyle = {{ borderColor: "#d2d2d2" }}
                                    />
                                </Flexbox>
                            </Flexbox>

                        </Flexbox>
                        :
                        (this.state.announcementSelection === 1 ?
                            <Flexbox flexDirection = "column" style = {{ marginTop: "-20px" }} >
                                {/* Release Announcement Creation */}

                                {/* Release Preview */}
                                <Flexbox flexDirection = "row" style = {{ marginTop: "30px", paddingLeft: "30px" }} >
                                    <Flexbox flexDirection = "column" style = {{ width: "50%", color: "#000000", paddingRight: "20px" }} >
                                        <div style = {{ fontWeight: "bold" }} > New Features: </div>
                                        <ul>
                                            {features}
                                        </ul>
                                    </Flexbox>

                                    <Flexbox flexDirection = "column" style = {{ width: "50%", color: "#000000", paddingRight: "20px" }} >
                                        <div style = {{ fontWeight: "bold" }} > Bug Fixes: </div>
                                        <ul>
                                            {bugfixes}
                                        </ul>
                                    </Flexbox>
                                </Flexbox>

                                <Flexbox flexDirection = "row" style = {{ padding: "0px 30px", marginTop: "20px" }} >
                                    <Flexbox style = {{ width: "45%" }} >
                                        <Flexbox 
                                            style = {{ margin: "5px 10px 0px 0px", width: "36px", height: "36px", borderRadius: "18px", backgroundColor: "#272958" }}
                                            onClick = { () => this.onClickFeatureBug("feature") }
                                        >
                                            <i 
                                                className = "fa fa-plus cursorHand" 
                                                style = {{
                                                    fontSize: '1.5em',
                                                    color: this.props.settings.colors.collapsibleColor.mainIcon,
                                                    margin: "6px 0px 0px 7px"
                                                }}
                                            /> 
                                        </Flexbox>

                                        <div style = {{ width: "90%", backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "Feature description"
                                                name = "featureText"
                                                id = "featureText"
                                                value = { this.state.featureText }
                                                style = {{ width: "100%" }}
                                                multiLine = { true }
                                                rows = { 1 }
                                                rowsMax = { 4 }
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                                onKeyPress = { (e) => this.blurOnEnter(e, "featureText") }
                                            />
                                        </div>
                                    </Flexbox>

                                    <Flexbox style = {{ width: "10%" }} />

                                    <Flexbox style = {{ width: "45%" }} >
                                        <div 
                                            style = {{ margin: "5px 10px 0px 0px", width: "36px", height: "36px", borderRadius: "18px", backgroundColor: "#272958" }}
                                            onClick = { () => this.onClickFeatureBug("bigfix") } 
                                        >
                                            <i 
                                                className = "fa fa-plus cursorHand" 
                                                style = {{
                                                    fontSize: '1.5em',
                                                    color: this.props.settings.colors.collapsibleColor.mainIcon,
                                                    margin: "6px 0px 0px 7px"
                                                }}
                                            /> 
                                        </div>

                                        <div style = {{ width: "90%", backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "Bugfix description"
                                                name = "bugfixText"
                                                id = "bugfixText"
                                                value = { this.state.bugfixText }
                                                style = {{ width: "100%" }}
                                                multiLine = { true }
                                                rows = { 1 }
                                                rowsMax = { 4 }
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                                onKeyPress = { (e) => this.blurOnEnter(e, "bugfixText") }
                                            />
                                        </div>

                                    </Flexbox>
                                </Flexbox>

                                <Flexbox style = {{ margin: "35px auto -30px" }} >
                                    <TextField 
                                        name = "versionNumber"
                                        id = "versionNumber"
                                        value = { this.state.versionNumber }
                                        hintText = "Version Number"
                                        hintStyle = {{ marginLeft: "6px" }}
                                        onChange = { (e) => this.onTextChange(e) }
                                        onKeyPress = { (e) => this.blurOnEnter(e, "versionNumber") }
                                        underlineStyle = {{ borderColor: "grey" }}
                                        underlineFocusStyle = {{ borderColor: this.props.settings.colors.rangeColor.textFieldUnderline }}
                                    />
                                </Flexbox>
                            </Flexbox>
                            :
                            (this.state.announcementSelection === 2 ?
                                <Flexbox flexDirection = "column" >
                                    {/* Poll Announcement Creation */}

                                    {/* Poll Announcement Preview */}
                                    {this.state.colors.length !== 0 ?
                                        <Flexbox flexDirection = "row" style = {{ height: "20vh" }} >
                                            {this.state.colors.length > 1 ?
                                                <PieChart
                                                    data = { this.state.data }
                                                    radius = { 150 }
                                                    hole = { 50 }
                                                    colors = { this.state.colors }
                                                    labels = { true }
                                                    percent = { true }
                                                    strokeWidth = { 3 }
                                                    stroke = { '#fff' }
                                                />
                                                :
                                                <PieChart
                                                    data = { [10000, 1] }
                                                    radius = { 150 }
                                                    hole = { 50 }
                                                    colors = { ["#3949ab", "#00acc1"] }
                                                    labels = { true }
                                                    percent = { true }
                                                    strokeWidth = { 3 }
                                                    stroke = { '#fff' }
                                                />
                                            }

                                            <dl style = {{ width: "200px", background: "#fff", padding: "5px 15px", marginLeft: "10px" }} >
                                                {shortOptions}
                                            </dl>
                                        </Flexbox>
                                        :
                                        null
                                    }

                                    <Flexbox flexDirection = "row" style = {{ paddingLeft: "30px", margin: "10px 0px -5px" }} >
                                        <Flexbox style = {{ width: "53%" }} >
                                            <div style = {{ margin: "16px 10px 0px 0px", fontWeight: "bold", fontSize: "16px" }} > Voting end date: </div>
                                        </Flexbox>

                                        <Flexbox style = {{ width: "47%" }} >
                                            <DatePicker 
                                                hintText = "Date"
                                                dialogContainerStyle = {{ color: "red" }}
                                                onChange = { this.handleDatePick }
                                                formatDate = { this.formatDate }
                                            />
                                        </Flexbox>
                                    </Flexbox>

                                    <Flexbox flexDirection = "row" style = {{ padding: "0px 30px", marginTop: "20px" }} >
                                        <Flexbox flexGrow = { 1 } style = {{ backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "Question"
                                                name = "question"
                                                value = { this.state.question }
                                                style = {{ width: "100%" }}
                                                multiLine = { true }
                                                rows = { 1 }
                                                rowsMax = { 4 }
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                            />
                                        </Flexbox>
                                    </Flexbox>

                                    <Flexbox flexDirection = "row" style = {{ padding: "0px 30px", marginTop: "20px" }} >
                                        
                                        <Flexbox style = {{ marginTop: "6px", width: "36px", height: "36px", borderRadius: "18px", backgroundColor: "#272958" }} >
                                            <i 
                                                className = "fa fa-plus cursorHand" 
                                                style = {{
                                                    fontSize: '1.5em',
                                                    color: this.props.settings.colors.collapsibleColor.mainIcon,
                                                    margin: "6px 0px 0px 8px"
                                                }}
                                                onClick = { () => this.onAddAnswer() }
                                            /> 
                                        </Flexbox>

                                        <Flexbox style = {{ width: "20px" }} />

                                        <Flexbox flexGrow = { 3 } style = {{ backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "Full Answer"
                                                name = "fullAnswer"
                                                value = { this.state.fullAnswer }
                                                style = {{ width: "100%" }}
                                                multiLine = { true }
                                                rows = { 1 }
                                                rowsMax = { 4 }
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                            />
                                        </Flexbox>

                                        <Flexbox style = {{ width: "20px", fontWeight: "bold", fontSize: "23px", margin: "10px 0px 0px 5px" }} > + </Flexbox>

                                        <Flexbox flexGrow = { 1 } style = {{ backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "Legend (Max 4 Words)"
                                                name = "shortAnswer"
                                                value = { this.state.shortAnswer }
                                                style = {{ width: "100%" }}
                                                multiLine = { true }
                                                rows = { 1 }
                                                rowsMax = { 4 }
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                            />
                                        </Flexbox>

                                    </Flexbox>
                                </Flexbox>
                                :
                                <Flexbox flexDirection = "column" >
                                    {/* Shout Announcement Creation */}
                                    <Flexbox flexDirection = "row" style = {{ padding: "0px 30px", marginTop: "20px" }} >
                                        <Flexbox flexGrow = { 1 } style = {{ backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "Shout Message"
                                                name = "shoutMessage"
                                                value = { this.state.shoutMessage }
                                                style = {{ width: "100%" }}
                                                multiLine = { true }
                                                rows = { 1 }
                                                rowsMax = { 4 }
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                            />
                                        </Flexbox>
                                    </Flexbox>

                                    <Flexbox flexDirection = "row" style = {{ padding: "0px 30px", marginTop: "20px" }} >
                                        <Flexbox flexGrow = { 1 } style = {{ backgroundColor: "#e7e7ff", borderRadius: "5px" }} >
                                            <TextField 
                                                hintText = "(Optional) Link to Open on Click"
                                                name = "shoutLink"
                                                value = { this.state.shoutLink }
                                                style = {{ width: "100%" }}
                                                hintStyle = {{ marginLeft: "6px" }}
                                                textareaStyle = {{ marginLeft: "6px", width: "98%" }}
                                                underlineStyle = {{ borderColor: "grey" }}
                                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.buttons.general }}
                                                onChange = { (e) => this.onTextChange(e) }
                                            />
                                        </Flexbox>
                                    </Flexbox>

                                </Flexbox>
                            )
                        )
                    }

                    {/* Create Announcement Button */}
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
        );
    }
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings
  }
}

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(AdminAnnouncementEditor);



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
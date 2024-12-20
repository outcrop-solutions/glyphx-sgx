 /*eslint-env jquery*/
import React from 'react';
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import Flexbox from 'flexbox-react';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Divider from 'material-ui/Divider';
import VotingModal from './VotingModal.js';
import ComponentLoadMask from './ComponentLoadMask.js';
import { guidGenerator } from './GeneralFunctions.js';
import './css/General.css';


/**
 * -ADCMT
 */

let announcementList = [
    // {
    //     type: "Release",
    //     id: "a1",
    //     content: {
    //         release: "1.0.0",
    //         features: [
    //                 "New 'Filter Overview' section located in the top of the filter side panel allows for easy monitoring and clearing of applied filters per column. You can also quickly navigate to an applied filter by clicking on the elastic or range icons.",
    //                 "Small objects on glyphs are hidden depending on the zoom level, the distance can be adjusted from floating toolbar.",
    //                 "Statistics can now be monitored per numeric column and will update each time a filter is applied.",
    //                 "Undo and Redo filter functionality.",
    //                 "Ranges can be disabled without being deleted allowing to toggle quickly between different ranges.",
    //                 "More than one range per column can be applied.",
    //                 "Text ranges have a variety of types to choose from.",
    //                 "Ability to pin filter columns and view separately.",
    //                 "Changes made to a visualization can be saved and loaded from the View Manager on the Home Page, or the view drop-down from within a visualization.",
    //                 "Change axes has been remodeled and now takes about one second without having to reload the visualization.",
    //                 "Userfeed section has been added to the Home Page allowing for communication within your institution.",
    //                 "Announcements section added to Home Page to keep users up to date on our progress.",
    //                 "More options available for front-end filters such as select all and deselect all."
    //                 ],
    //         bugfixes: []
    //     },
    //     postDate: "5/15/2018"
    // },
    // {
    //     type: "Shout",
    //     id: "a2",
    //     content: {
    //         message: "GlyphEd has been selected to speak with John Carroll University at Ruffalo Noel Levitz National Conference this summer.",
    //         linkType: "text",
    //         link: ""
    //     },
    //     postDate: "3/20/2018"
    // },
    // {
    //     type: "Shout",
    //     id: "a3",
    //     content: {
    //         message: "Gannon University improves their retention rate by 5% in under a year using GlyphEd. Click to learn more.",
    //         linkType: "link",
    //         link: "https://s3.amazonaws.com/sgxshared/gannon-case-study.pdf"
    //     },
    //     postDate: "1/12/2018"
    // },
    // {
    //     type: "Shout",
    //     id: "a4",
    //     content: {
    //         message: "Kent state chooses GlyphEd to explore insights in it's freshman class data in an innovative new proof of concept. Click to learn more.",
    //         linkType: "link",
    //         link: "http://www.globenewswire.com/news-release/2017/10/03/1140109/0/en/Kent-State-University-to-use-GlyphEd-to-Discover-Insights-in-Freshman-Student-Data.html"
    //     },
    //     postDate: "10/3/2017"
    // }
];

class AnnouncementsDisplay extends React.Component {

    state = {
        loadMask: true,
        announcements: [], //restructure announcements to be = [{date: date, type: type, message: message}, ...]
        allAnnouncements: false,
        announcementsAppended: []
    }


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
        let context = this;
        $.ajax({
            type: "GET",
            url: "https://api.rss2json.com/v1/api.json?rss_url=https://www.glyphed.com/news?format=rss",
            dataType: 'jsonp',
            success: function(result) {
                let news_arr = result.items;
                let arr = [];

                for(let i = 0; i < news_arr.length; i++){
                    let publish_date = news_arr[i].pubDate.slice(0, 10);
                    publish_date = publish_date.slice(5) + "/" + publish_date.slice(0,4);
                    let article_content = news_arr[i].title + ' - ' + news_arr[i].description;

                    if(article_content.length > 150) article_content = article_content.substring(0, 147) + "...";

                    let obj = {
                        type: "Shout",
                        id: guidGenerator(),
                        content: {
                            message: article_content,
                            linkType: "link",
                            link: news_arr[i].link
                        },
                        postDate: publish_date.replace('-', '/')
                    };
                    arr.push(obj);
                }
                announcementList = arr.concat(announcementList);
                // context.annoucementUpdate();
              }
        });

        $.ajax({
            type: "GET",
            url: "https://api.rss2json.com/v1/api.json?rss_url=https://www.glyphed.com/blog?format=rss",
            dataType: 'jsonp',
            success: function(result) {
                let blog_arr = result.items;
                let arr = [];

                for(let i = 0; i < blog_arr.length; i++){
                    let publish_date = blog_arr[i].pubDate.slice(0, 10);
                    publish_date = publish_date.slice(5) + "/" + publish_date.slice(0,4);
                    let article_content = blog_arr[i].title + ' - ' + blog_arr[i].description;
                    if(article_content.length > 150) article_content = article_content.substring(0, 147) + "...";

                    let obj = {
                        type: "Shout",
                        id: guidGenerator(),
                        content: {
                            message: article_content,
                            linkType: "link",
                            link: blog_arr[i].link
                        },
                        postDate: publish_date.replace('-', '/')
                    };
                    arr.push(obj);
                }
                announcementList = arr.concat(announcementList);
                announcementList.sort((a,b) => {
                    return new Date(b.postDate.slice(b.postDate.lastIndexOf('/') + 1), 
                        b.postDate.slice(0, b.postDate.indexOf('/')), 
                        b.postDate.slice(b.postDate.indexOf('/') + 1, b.postDate.lastIndexOf('/'))) 
                        - new Date(a.postDate.slice(a.postDate.lastIndexOf('/') + 1), 
                        a.postDate.slice(0, a.postDate.indexOf('/')), 
                        a.postDate.slice(a.postDate.indexOf('/') + 1, a.postDate.lastIndexOf('/')));
                });
                context.annoucementUpdate();
              }
        });
        //if (this.props.adminEdit) {
            //this.setState({ loadMask: false, announcements: this.props.announcementList });
        //}

        //else {
            //var context = this;
            // Make server call to grab announcements
            //makeServerCall(something here),
            //    function (responseText) { 

                    // Post the new data to the state and hide the window load-mask
                    this.setState({ loadMask: false, announcements: announcementList });
                    //context.props.dispatch( setTimer(new Date().getTime()) );
                    
            //   }
            //);
        //}
	}


    /**
     * React built-in which acts as a listener for when props change
     * @param nextProps: The props the component would have after the change
     **/
	componentWillReceiveProps(nextProps) {
        this.setState({ announcements: nextProps.announcementList });
    }

    annoucementUpdate(){
        let context = this;
        let announcements = announcementList.map( 
            function(announcement) {
                return (
                    (announcement.type === "Maintenance" ? 
                        <MaintenanceAnnouncement 
                            announcement = { announcement } 
                            settings = { context.props.settings } 
                            key = { announcement.id } 
                            first = { announcement === announcementList[0] } 
                            deleteAnnouncement = { context.props.deleteAnnouncement } 
                            adminEdit = { context.props.adminEdit }
                        /> : 
                        (announcement.type === "Release" ? 
                            <ReleaseAnnouncement 
                                announcement = { announcement } 
                                settings = { context.props.settings } 
                                key = { announcement.id }  
                                first = { announcement === announcementList[0] } 
                                deleteAnnouncement = { context.props.deleteAnnouncement } 
                                adminEdit = { context.props.adminEdit }
                            /> :
                            (announcement.type === "Poll" ? 
                                <PollAnnouncement 
                                    announcement = { announcement } 
                                    settings = { context.props.settings } 
                                    key = { announcement.id } 
                                    first = { announcement === announcementList[0] }
                                    deleteAnnouncement = { context.props.deleteAnnouncement } 
                                    adminEdit = { context.props.adminEdit }
                                /> :
                                (announcement.type === "Shout" ? 
                                    <ShoutAnnouncement 
                                        uid = {context.props.uid}
                                        webSocket = {context.props.webSocket}
                                        announcement = { announcement } 
                                        settings = { context.props.settings } 
                                        key = { announcement.id } 
                                        first = { announcement === announcementList[0] } 
                                        deleteAnnouncement = { context.props.deleteAnnouncement } 
                                        adminEdit = { context.props.adminEdit }
                                    /> :
                                    "Error! Announcement Type Not Recognized!!"
                                )
                            )
                        )         
                    )
                )
            }
        );
        context.setState({announcementsAppended: announcements})
    }

    
    render() {
        var context = this;

        return (
            <div style = {{height: "27.45vh", display: "block"}}> 
                <div style={{
                    height: "5.2vh",
                    width: "25vw",
                    backgroundColor: "#0c1836",
                    display: "flex"
                }}>
                    <h2
                        className='noselect'
                        style={{
                            float: "left",
                            fontFamily: "ITCFranklinGothicStd-Demi",
                            letterSpacing: "0.92px",
                            color: "white",
                            fontSize: "2.294vh",
                            textTransform: "uppercase",
                            margin: "auto auto auto 1.877vh"
                        }}
                        >Announcements
                    </h2>
                    <span 
                        onClick={() => this.setState({allAnnouncements: true})}
                        style={{
                            color: "white", 
                            fontSize: "2vh", 
                            float: "right", 
                            fontFamily: "ITCFranklinGothicStd-DmCd",
                            cursor: "pointer",
                            margin: "auto 1.4vh auto auto"
                            }}
                    >
                        See All&ensp;>
                    </span>

                    <Dialog 
                        contentStyle = {{ maxWidth: "40vw" }}
                        titleStyle = {{fontSize: "2.313vh", lineHeight: "3.365vh"}}
                        actionsContainerStyle = {{ padding: "0.841vh" }}
                        bodyStyle = {{
                            overflowY: "auto", 
                            fontSize: "1.682vh", 
                            maxHeight: "74.974vh", 
                            padding: "0px 1.824vh 1.524vh"}}
                        title = { <div style = {{ fontWeight: "bold" }} > All Announcements <Divider /></div> }
                        modal = { true }
                        open = { this.state.allAnnouncements }
                        actions = {
                            [
                                <FlatButton
                                    label = "Close"
                                    primary = { true }
                                    onClick = { () => this.setState({ allAnnouncements: false }) }
                                    style = {{ 
                                        height: "3.785vh", 
                                        lineHeight: "3.785vh", 
                                        minWidth: "7vw", 
                                        color: this.props.settings.colors.settingsModalColor.saveButton }}
                                    labelStyle = {{ lineHeight: "4.171vh", fontSize: "1.472vh" }}
                                />
                            ]
                        }
                    >
                        <Flexbox flexDirection = "row" className = "noselect" 
                            style = {{ marginTop: "3vh", minHeight: "44vh", minWidth: "38vw" }} >
                            <Flexbox 
                                flexDirection = "column" 
                                style = {{ 
                                    width: "75vw",
                                    color: "#000000", 
                                    paddingRight: "2.086vh"
                                }} 
                            >
                                {/* <div style = {{ fontWeight: "bold" }} > New Features: </div> */}
                                {/* <ul>
                                    {features}
                                </ul> */}
                                    <div style = {{ display: (this.state.loadMask ? "none" : "") }} >
                                    <div style = {{ borderRadius: "3px", width: "37vw" }}>
                                        {this.state.announcementsAppended ? this.state.announcementsAppended : null}
                                    </div>
                                </div>
                            </Flexbox>
                            {/* {this.props.announcement.content.bugfixes.length > 0 ?
                                <Flexbox flexDirection = "column" style = {{ width: "50%", color: "#000000", paddingRight: "20px" }} >
                                    <div style = {{ fontWeight: "bold" }} > Bug Fixes: </div>
                                    <ul>
                                        {bugfixes}
                                    </ul>
                                </Flexbox>
                                :
                                null
                            } */}
                        </Flexbox>
                    </Dialog>

                </div>

                {/* body */}
                <Flexbox flexGrow = {1} style = {{ height: "22.2vh", display: "block" }} >
                    
                    <div style = {{ width: "100%", marginTop: "60px", display: (this.state.loadMask ? "" : "none") }} >
                        <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                    </div>

                    <div style = {{ display: (this.state.loadMask ? "none" : "") }} >
                        <div style = {{ 
                            height: "22.2vh", 
                            overflowY: "auto",
                            width: "25vw"
                        }} 
                            className = "customScroll">
                           {this.state.announcementsAppended ? this.state.announcementsAppended.slice(0,5) : null}
                        </div>
                    </div>

                </Flexbox>

            </div> 
        );
    }
}


class MaintenanceAnnouncement extends React.Component {

    render() {
        return (
            <Flexbox flexDirection = "row" /*  style = {{ marginTop: (this.props.first ? "0px" : "6px")*/ >
                {this.props.adminEdit? 
                    <Flexbox flexDirection = "column" alignSelf = "center" >
                        <Flexbox style = {{ height: "50%" }} >
                            <i 
                                className = "fa fa-minus cursorHand" 
                                style = {{ 
                                    fontSize: "2.085vh",
                                    margin: "0px 0.938vh 0px 0px",
                                    padding: "0.626vh 0px 0px 0.730vh",
                                    color: "#ffffff",
                                    backgroundColor: "#aa0d0d",
                                    height: "5.2vh",
                                    width: "3.128vh",
                                    borderRadius: "15px"
                                }} 
                                onClick = { () => this.props.deleteAnnouncement(this.props.announcement.id) }
                            />
                        </Flexbox>
                    </Flexbox>
                    :
                    null
                }

                <Card containerStyle = {{ 
                        padding: "0px", 
                        borderRadius: "5px" }} 
                    style = {{ 
                        width: "100%", 
                        textShadow: "0",
                        // backgroundColor: this.props.settings.colors.general.lightBubble, 
                        boxShadow: "0px 0px 0px"
                 }} >
                    <CardText
                        style = {{
                            padding: "1.043vh",
                            // marginTop: (this.props.first ? "0px" : "6px"),
                            marginBottom: (this.props.last ? "0.730vh" : "0px"),
                            backgroundColor: "#e6e7e8",
                            fontFamily: "ITCFranklinGothicStd-Med",
                            fontSize: "1.668vh",
                            minHeight: "7.1vh"
                        }}
                        /* className = "cursorHand" */
                        //onClick = {  }
                    >
                        <Flexbox flexDirection = "row" className = "noselect" >

                            <Flexbox flexDirection = "column" alignSelf = "center" >
                                <Flexbox style = {{ height: "50%" }} >
                                    <i 
                                        className = "fa fa-exclamation-triangle" 
                                        style = {{ 
                                            fontSize: "2.086vh",
                                            margin: "0px 0.938vh 0px 0px",
                                            padding: "1.043vh 0px 0px 0.938vh",
                                            backgroundColor: this.props.settings.colors.announcementColors.maintenance,
                                            height: "4.15vh",
                                            width: "2.09vw",
                                            borderRadius: "3px"
                                        }} 
                                    />
                                </Flexbox>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <div style = {{ width: "100%" }} > 
                                    We will be performing software updates and maintenance on our servers on {this.props.announcement.content.date}, 
                                    at {this.props.announcement.content.time} EST (UTC-5). Servers will be down for {this.props.announcement.content.length}. 
                                    <div style = {{ textAlign: "right", marginTop: "1.043vh" }} >{this.props.announcement.postDate}</div>
                                </div>
                            </Flexbox>

                        </Flexbox>
                    </CardText>
                    <div style={{backgroundColor: "#e6e7e8"}}>
                        <div style={{height: "1px", width: "95.73%", borderBottom: "1px solid #9ea3af", margin: "auto"}}>
                        </div>
                    </div>
                </Card>
            </Flexbox>
        );
    }
}

class ReleaseAnnouncement extends React.Component {

    state = {
        notesDisplay: false
    }

    render() {

        var features = this.props.announcement.content.features.map( 
            function(feature) {
                return (
                    <li key = { feature } style = {{ wordBreak: "break-word" }} >{feature}<br /></li>
                )
            }
        );

        var bugfixes = this.props.announcement.content.bugfixes.map( 
            function(bugfix) {
                return (
                    <li key = { bugfix } style = {{ wordBreak: "break-word" }} >{bugfix}<br /></li>
                )
            }
        );

        return (
            <Flexbox flexDirection = "row" style = {{marginBottom: (this.props.last ? "0.730vh" : "0px") }} >
                {this.props.adminEdit? 
                    <Flexbox flexDirection = "column" alignSelf = "center" >
                        <Flexbox>
                            <i 
                                className = "fa fa-minus cursorHand" 
                                style = {{ 
                                    fontSize: "2.085vh",
                                    margin: "0px 0.938vh 0px 0px",
                                    padding: "0.626vh 0px 0px 0.730vh",
                                    color: "#ffffff",
                                    backgroundColor: "#aa0d0d",
                                    minHeight: "5.2vh",
                                    width: "3.128vh",
                                    borderRadius: "15px"
                                }} 
                                onClick = { () => this.props.deleteAnnouncement(this.props.announcement.id) }
                            />
                        </Flexbox>
                    </Flexbox>
                    :
                    null
                }

                <Card containerStyle = {{ 
                        padding: "0px", 
                        borderRadius: "5px" }} 
                    style = {{ 
                        width: "100%", 
                        boxShadow: "0px 0px 0px" 
                 }} >
                    <CardText
                        style = {{
                            padding: "10px",
                            marginBottom: (this.props.last ? "0.730vh" : "0px"),
                            backgroundColor: "#e6e7e8",
                            fontFamily: "ITCFranklinGothicStd-Med",
                            fontSize: "1.668vh",
                            minHeight: "7.1vh"
                        }}
                        className = "cursorHand"
                        onClick = { () => this.setState({ notesDisplay: true }) }
                    >
                        <Dialog
                            title = { <div style = {{ fontWeight: "bold" }} > Release Notes <Divider /></div> }
                            modal = { true }
                            open = { this.state.notesDisplay }
                            actions = {
                                [
                                    <FlatButton
                                        label = "Close"
                                        primary = { true }
                                        onClick = { () => this.setState({ notesDisplay: false }) }
                                        style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                                        labelStyle = {{ lineHeight: "4.171vh" }}
                                    />
                                ]
                            }
                        >
                            <Flexbox 
                                flexDirection = "row" className = "noselect" 
                                style = {{ marginTop: "3vh", minHeight: "44vh", minWidth: "38vw" }} >
                                <Flexbox 
                                    flexDirection = "column" 
                                    style = {{ 
                                        width: (this.props.announcement.content.bugfixes.length > 0 ? "50%" : "100%"), 
                                        color: "#000000", 
                                        paddingRight: "2.086vh" 
                                    }} 
                                >
                                    <div style = {{ fontWeight: "bold" }} > New Features: </div>
                                    <ul>
                                        {features}
                                    </ul>
                                </Flexbox>
                                {this.props.announcement.content.bugfixes.length > 0 ?
                                    <Flexbox flexDirection = "column" style = {{ width: "50%", color: "#000000", paddingRight: "2.086vh" }} >
                                        <div style = {{ fontWeight: "bold" }} > Bug Fixes: </div>
                                        <ul>
                                            {bugfixes}
                                        </ul>
                                    </Flexbox>
                                    :
                                    null
                                }
                            </Flexbox>
                        </Dialog>
                        
                        <Flexbox flexDirection = "row" className = "noselect" >

                            <Flexbox flexDirection = "column" alignSelf = "center" >
                                <Flexbox style = {{ height: "50%" }} >
                                    <i 
                                        className = "fa fa-code-fork" 
                                        style = {{ 
                                            fontSize: "3.02vh",
                                            margin: "0px 0.938vh 0px 0px",
                                            padding: "0.626vh 0px 0px 1.356vh",
                                            backgroundColor: this.props.settings.colors.announcementColors.release,
                                            height: "4.15vh",
                                            width: "2.09vw",
                                            borderRadius: "3px"
                                        }} 
                                    />
                                </Flexbox>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} >
                                <div style = {{ width: "100%" }} > 
                                    Release {this.props.announcement.content.release} is now live! Click to view release notes.
                                    <div style = {{ textAlign: "right", marginTop: "1.043vh" }} >{this.props.announcement.postDate}</div>
                                </div>
                            </Flexbox>

                        </Flexbox>
                    </CardText>
                    <div style={{backgroundColor: "#e6e7e8"}}>
                        <div style={{height: "1px", width: "95.73%", borderBottom: "1px solid #9ea3af", margin: "auto"}}>
                        </div>
                    </div>
                </Card>
            </Flexbox>
        );
    }
}

class PollAnnouncement extends React.Component {

    state = {
        display: false
    }

    toggleDisplay = () => {
        this.setState({ display: !this.state.display });
    }

    votingClosed(date, time) {
        return false;
    }

    render() {
        return (
            <Flexbox flexDirection = "row" style = {{ marginBottom: (this.props.last ? "0.730vh" : "0px") }} >
                {this.props.adminEdit? 
                    <Flexbox flexDirection = "column" alignSelf = "center" >
                        <Flexbox style = {{ height: "50%" }} >
                            <i 
                                className = "fa fa-minus cursorHand" 
                                style = {{ 
                                    fontSize: "2.085vh",
                                    margin: "0px 0.938vh 0px 0px",
                                    padding: "0.626vh 0px 0px 0.730vh",
                                    color: "#ffffff",
                                    backgroundColor: "#aa0d0d",
                                    height: "5.2vh",
                                    width: "3.128vh",
                                    borderRadius: "15px"
                                }} 
                                onClick = { () => this.props.deleteAnnouncement(this.props.announcement.id) }
                            />
                        </Flexbox>
                    </Flexbox>
                    :
                    null
                }
                
                <Card containerStyle = {{ padding: "0px", borderRadius: "5px" }} style = {{ 
                    width: "100%", 
                    textShadow: "0",
                    boxShadow: "0px 0px 0px"
                    }} >
                    <CardText
                        style = {{
                            padding: "1.043vh",
                            marginBottom: (this.props.last ? "0.730vh" : "0px"),
                            backgroundColor: "#e6e7e8",
                            fontFamily: "ITCFranklinGothicStd-Med",
                            fontSize: "1.668vh",
                            minHeight: "7.1vh"
                        }}
                        className = "cursorHand"
                        onClick = { this.toggleDisplay }
                    >
                        <VotingModal display = { this.state.display } votingClosed = { this.votingClosed() } toggleDisplay = { this.toggleDisplay.bind(this)} poll = { this.props.announcement.content } />

                        <Flexbox flexDirection = "row" className = "noselect" >

                            <Flexbox flexDirection = "column" alignSelf = "center" >
                                <Flexbox style = {{ height: "50%" }} >
                                    <i 
                                        className = "fa fa-gavel" 
                                        style = {{ 
                                            fontSize: "2.085vh",
                                            margin: "0px 0.938vh 0px 0px",
                                            padding: "1.043vh 0px 0px 0.938vh",
                                            backgroundColor: this.props.settings.colors.announcementColors.poll,
                                            height: "4.15vh",
                                            width: "2.09vw",
                                            borderRadius: "3px"
                                        }} 
                                    />
                                </Flexbox>
                            </Flexbox>


                            <Flexbox style = {{ width: "100%" }} > 
                                <div style = {{ width: "100%" }} >
                                    { this.votingClosed(this.props.announcement.content.date, this.props.announcement.content.time) ? 
                                        this.props.announcement.content.question + " Voting now closed, click to view results." 
                                        :  
                                        this.props.announcement.content.question + " Click to vote, voting will be open until " + this.props.announcement.content.date +  "."
                                    }
                                    <div style = {{ textAlign: "right", marginTop: "1.043vh"}} >{this.props.announcement.postDate}</div>
                                </div>
                                
                            </Flexbox>

                        </Flexbox>
                    </CardText>
                    <div style={{backgroundColor: "#e6e7e8"}}>
                        <div style={{height: "1px", width: "95.73%", borderBottom: "1px solid #9ea3af", margin: "auto"}}>
                        </div>
                    </div>
                </Card>
            </Flexbox>
        );
    }
}

class ShoutAnnouncement extends React.Component {

    webSocketSend(link){
        if(this.props.uid){ 
            this.props.webSocket.send(JSON.stringify({
                url_uid: this.props.uid,
                open_url: link}));
		}
    }

    render() {
        let context = this;
        return (
            <Flexbox flexDirection = "row" style = {{ marginBottom: (this.props.last ? "0.730vh" : "0px") }} >
                {this.props.adminEdit? 
                    <Flexbox flexDirection = "column" alignSelf = "center" >
                        <Flexbox style = {{ height: "50%" }} >
                            <i 
                                className = "fa fa-minus cursorHand" 
                                style = {{ 
                                    fontSize: "2.085vh",
                                    margin: "0px 0.938vh 0px 0px",
                                    padding: "0.626vh 0px 0px 0.730vh",
                                    color: "#ffffff",
                                    backgroundColor: "#aa0d0d",
                                    minHeight: "5.2vh",
                                    width: "3.128vh",
                                    borderRadius: "15px"
                                }} 
                                onClick = { () => this.props.deleteAnnouncement(this.props.announcement.id) }
                            />
                        </Flexbox>
                    </Flexbox>
                    :
                    null
                }

                <Card containerStyle = {{ 
                        padding: "0px", 
                        borderRadius: "5px" }} 
                    style = {{ 
                        width: "100%", 
                        boxShadow: "0px 0px 0px"
                }} >
                    <CardText
                        style = {{
                            padding: "1.043vh",
                            marginBottom: (this.props.last ? "0.730vh" : "0px"),
                            backgroundColor: "#e6e7e8",
                            fontFamily: "ITCFranklinGothicStd-Med",
                            fontSize: "1.668vh",
                            minHeight: "7.1vh"
                        }}
                        className = "cursorHand"
                        onClick = { () => this.props.announcement.content.linkType === "link" ? 
                        (this.props.uid ? this.webSocketSend(this.props.announcement.content.link) 
                            : window.open(this.props.announcement.content.link)) : null }
                    >

                        <Flexbox flexDirection = "row" className = "noselect" >

                            <Flexbox flexDirection = "column" alignSelf = "center" >
                                <Flexbox style = {{ height: "50%" }} >
                                    <i 
                                        className = "fa fa-bullhorn" 
                                        style = {{ 
                                            fontSize: "2.085vh",
                                            margin: "0px 0.938vh 0px 0px",
                                            padding: "1.043vh 0px 0px 0.938vh",
                                            backgroundColor: this.props.settings.colors.announcementColors.shout,
                                            height: "4.15vh",
                                            width: "2.09vw",
                                            borderRadius: "3px"
                                        }} 
                                    />
                                </Flexbox>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <div style = {{ width: "100%" }} > 
                                    { this.props.announcement.content.message }
                                    <div style = {{ textAlign: "right", marginTop: "1.043vh" }} >{this.props.announcement.postDate}</div>
                                </div>
                            </Flexbox>

                        </Flexbox>
                    </CardText>
                    <div style={{backgroundColor: "#e6e7e8"}}>
                        <div style={{height: "1px", width: "95.73%", borderBottom: "1px solid #9ea3af", margin: "auto"}}>
                        </div>
                    </div>
                </Card>
            </Flexbox>
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

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});

/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
    webSocket: state.filterState.webSocket,
    uid: state.filterState.uid,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(AnnouncementsDisplay);

/*
var announcementList = [
    {
        type: "Maintenance",
        id: "a1",
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/8/2017"
    },
    {
        type: "Release",
        id: "a12",
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/8/2017"
    },
    {
        type: "Poll",
        id: "artyui23",
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
        id: "a14",
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/8/2017"
    },
    {
        type: "Maintenance",
        id: "a15",
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/7/2017"
    },
    {
        type: "Release",
        id: "a16",
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/7/2017"
    },
    {
        type: "Poll",
        id: "a2tyuik3",
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
        id: "a18",
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/7/2017"
    },
    {
        type: "Maintenance",
        id: "a19",
        content: {
            date: "August 26th, 2017",
            time: "9pm",
            length: "2 hours"
        },
        postDate: "11/6/2017"
    },
    {
        type: "Release",
        id: "a2",
        content: {
            release: "1.0.0",
            features: ["cool feature 1", "cool feature 2", "cool feature 3" ],
            bugfixes: ["yay bugfix 1", "yay bugfix 2", "yay bugfix 3" ]
        },
        postDate: "11/6/2017"
    },
    {
        type: "Poll",
        id: "a23asd",
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
        id: "a23",
        content: {
            message: "John Carroll University Selects GlyphEd™ Software to Support Ongoing Commitment to Student Success, click to view article.",
            linkType: "link",
            link: "http://www.GlyphEd.co"
        },
        postDate: "11/6/2017"
    }
];
*/
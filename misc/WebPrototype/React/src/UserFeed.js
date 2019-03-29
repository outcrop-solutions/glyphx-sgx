import React from 'react';
import { connect } from 'react-redux';
import { makeServerCall } from './ServerCallHelper.js';
import { guidGenerator } from './GeneralFunctions.js';
import Flexbox from 'flexbox-react';
import { Card, CardText } from 'material-ui/Card';
import Select from 'react-select';
import Divider from 'material-ui/Divider';
import ComponentLoadMask from './ComponentLoadMask.js';
import Twilio from 'twilio-chat';
import './General.css';


/**
 * Enables messaging and sharing visualizations between instatution teams.
 **/
class UserFeed extends React.Component {

    state = {
        loadMask: true,
        token: null,
        chatClient: null,
        channelClient: null,
        chatReady: false,
        messages: [],
        //teamSelectValue: "University of Notre Dame",
        channelName: "TestingChannel",
        messageBody: ""
    }


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

        var institution = this.props.userInfo.institutionDir.split("/");
        
        if (institution.length > 1) {
            this.setState({ channelName: institution[institution.length -2] });
        }
        else {
            this.setState({ channelName: "Institution" + this.props.userInfo.Institution });
        }

        var context = this;

        makeServerCall("token?userInfo=" + context.props.userInfo.Name + "|SPLITTER|" + context.props.userInfo.Email,
            function (responseText) { 
                var chat = new Twilio(responseText);

                // debugger;

                chat.initialize().then(context.clientInitiated.bind(context));
                
                context.setState({ token: responseText, chatClient: chat });
            }
        );
	}

    clientInitiated = () => {
        this.setState({ chatReady: true }, () => {
        this.state.chatClient.getChannelByUniqueName(this.state.channelName)
            .then(channel => {
                // debugger;
                if (channel) {
                    this.setState({ channelClient: channel });
                    return channel;
                }
            })
            .catch(err => {
                // debugger;
                if (err.body.code === 50300) {
                    return this.state.chatClient.createChannel({
                        uniqueName: this.state.channelName
                    });
                }
            })
            .then(channel => {
                //console.log("\n\n\n\n\nMEMBERS:");
                //channel.getMembers().then(members => { console.log(members) });
                // debugger;
                this.setState({ channelClient: channel });
                channel.getMembers()
                .then(members => {
                    // debugger;
                    var shouldJoin = true;
                    for (var i = 0; i < members.length; i++) {
                        if (members[i].state.identity == (this.props.userInfo.Name + "|SPLITTER|" + this.props.userInfo.Email)) {
                            shouldJoin = false;
                        }
                    }

                    if (shouldJoin) {
                        return channel.join();
                    }
                })
            })
            .then(() => {
                // debugger;
                this.state.channelClient.getMessages().then(this.messagesLoaded);
                this.state.channelClient.on('messageAdded', this.messageAdded);
            });
        });
    };


    scrollToBottomChat() {
        var objDiv = document.getElementById("chatArea");
        objDiv.scrollTop = objDiv.scrollHeight;
    }
    

    messagesLoaded = messagePage => {
        // debugger;
        var messages = messagePage.items;
        /*
        if (messages.length > 100) {
            messages = messages.slice(messages.length - 100);
        }
        */
        this.setState({ messages: messages, loadMask: false }, () => this.scrollToBottomChat());
        //console.log("MESSAGES:");
        //console.log(messages);
    };

    messageAdded = message => {
        // debugger;
        this.setState((prevState, props) => ({
            messages: [...prevState.messages, message]
        }), () => this.scrollToBottomChat());
    };

    checkEnter(event) {
        if (event.keyCode == 13) {
            event.preventDefault();

            // debugger;
            
            this.state.channelClient.sendMessage(this.state.messageBody);

            this.setState({ messageBody: "" });
        }
    }


    /**
     * Handles changing teams from the drop-down
     * @param value: value of the team to change to
     **/
    onTeamSelectChange = (value) => {
        this.setState({ teamSelectValue: value });
    };
    

    render() {

        var teamList = ["University of Notre Dame", "Marketing & Recruiting", "Admissions", "Financial Aid", "Custom Team 1"];
        teamList = teamList.map( function(value) {
			return({ label: value, value: value });
		});

        var context = this;

        var posts = this.state.messages.map( function(post) {
            return (
                <Card 
                    containerStyle = {{ padding: "0px", borderRadius: "10px" }} 
                    style = {{ 
                        backgroundColor: context.props.settings.colors.general.lightBubble, 
                        borderRadius: "10px", 
                        paddingBottom: "2px", 
                        marginBottom: "7px"
                    }} 
                    key = { guidGenerator() } 
                >
                    <CardText style = {{ padding: "5px", borderRadius: "10px" }} >
                        <Flexbox flexDirection = "row" minWidth = "100%" >

                            {/* <img src = { post[1] } className = "img-circle noselect" style = {{ marginRight: "10px" }} alt = { post[0] } draggable = { false } /> */}
 
                            <Flexbox flexDirection = "column" style = {{ width: "100%" }} >
                                <Flexbox style = {{ height: "100%" }} > 
                                    <div style = {{ wordBreak: "break-word" }} ><span style = {{ fontWeight: "bold" }} >{post.state.author.split("|SPLITTER|")[0]}:</span> &nbsp; {post.state.body}</div>
                                </Flexbox>

                                <Divider style = {{ marginBottom: "3px", backgroundColor: "#b9b9b9" }} />

                                <Flexbox flexDirection = "row" minWidth = "100%" className = "noselect" style = {{ margin: "-2px 0px -1px 0px" }} >

                                    {/* <i className = "fa fa-comments" style = {{ fontSize: "17px", marginRight: "15px" }} /> */}
                                    {/* <i className = "fa fa-thumbs-up" style = {{ fontSize: "17px",marginRight: "25px" }} /> */}
                                    <div style = {{ margin: "0px 15px 0px 0px" }} > {(post.state.dateUpdated.getMonth() + 1) + "/" + post.state.dateUpdated.getDate() + "/" + post.state.dateUpdated.getFullYear()} </div>

                                    <Flexbox style = {{ width: "100%" }} > 
                                        <div style = {{ width: "100%", textAlign: "right" }} > 
                                            {post.state.dateUpdated.getHours() > 12 ? 
                                                (post.state.dateUpdated.getHours() - 12) + ":" + (post.state.dateUpdated.getMinutes() < 10 ? '0' + post.state.dateUpdated.getMinutes() : post.state.dateUpdated.getMinutes()) + 'pm'
                                                :
                                                (post.state.dateUpdated.getHours() == 12 ? 
                                                    12 + ":" + (post.state.dateUpdated.getMinutes() < 10 ? '0' + post.state.dateUpdated.getMinutes() : post.state.dateUpdated.getMinutes()) + 'pm'
                                                    :
                                                    post.state.dateUpdated.getHours() + ":" + (post.state.dateUpdated.getMinutes() < 10 ? '0' + post.state.dateUpdated.getMinutes() : post.state.dateUpdated.getMinutes()) + 'am'
                                                )
                                            }

                                        </div>
                                    </Flexbox>

                                </Flexbox>

                            </Flexbox>
                        </Flexbox>
                        
                    </CardText>
                </Card>
            )
        });

        return (
            <Flexbox flexDirection = "column" style = {{ height: "100%", minHeight: "0" }} >

                <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, borderRadius: "2px", marginBottom: "3px", paddingBottom: "4px" }} >
                    <div 
                        style = {{ 
                            color: this.props.settings.colors.overviewButtonsColor.text, 
                            margin: "0 auto",
                            width: "137px",
                            paddingTop: "4px",
                            fontSize: "19px",
                            letterSpacing: "0.23em",
                            textTransform: "uppercase"
                        }}
                        className = "noselect"
                    > 
                        User Feed
                    </div>
                </div>

                <div style = {{ height: "80%", display: (this.state.loadMask ? "" : "none") }} >
                    <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                </div>

                <Flexbox flexDirection = "column" style = {{ height: "100%", minHeight: "0", padding: "7px", display: (this.state.loadMask ? "none" : "") }}  > 

                    {/*
                    <Select 
                        simpleValue
                        clearable = { false }
                        value = { this.state.teamSelectValue } 
                        options = { teamList } 
                        onChange = { this.onTeamSelectChange } 
                        style = {{ margin: "0px 0px 7px 0px" }}
                        className = "noselect"
                    />
                    */}
                
                    <Flexbox flexGrow = {1} style = {{ height: "100%", minHeight: "0" }} >
                        <div
                            style = {{
                                padding: "7px 7px 0px 5px",
                                height: "100%",
                                width: "100%",
                                borderRadius: "2px",
                                overflowY: "scroll",
                            }}
                            className = "customScroll"
                            id = "chatArea"
                        >
                            {posts}

                        </div>
                    </Flexbox>

                    <textarea 
                        id = "messageBodyHolder" 
                        placeholder = "Type your message" 
                        rows = "5" 
                        value = { this.state.messageBody } 
                        onKeyDown = { (e) => this.checkEnter(e) } 
                        onChange = { (e) => this.setState({ messageBody: e.target.value}) } 
                        style = {{ borderRadius: "5px", padding: "10px", marginTop: "10px", backgroundColor: "#e7e7fd" }} 
                    ></textarea>
                    
                </Flexbox>

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
export default connect(mapStateToProps)(UserFeed);
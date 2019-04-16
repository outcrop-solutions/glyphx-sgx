import React from 'react';
import { connect } from 'react-redux';
import { makeServerCall } from './ServerCallHelper.js';
import { guidGenerator } from './GeneralFunctions.js';
import Flexbox from 'flexbox-react';
import { Card, CardText } from 'material-ui/Card';
/* import Select from 'react-select'; */
import Divider from 'material-ui/Divider';
import ComponentLoadMask from './ComponentLoadMask.js';
import Twilio from 'twilio-chat';
import './css/General.css';


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
        messageBody: "",
        privateMessageBody: "",
        noAccess: false,
        selfClick: "",
        privateChat: [],
        privChannels: []
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
        let channelArr = [];

        makeServerCall("token?userInfo=" + context.props.userInfo.Name + "|SPLITTER|" + context.props.userInfo.Email,
            function (responseText) { 
                var chat = new Twilio(responseText);
                console.log(chat.user)
                

                // debugger;

                chat.initialize().then(context.clientInitiated.bind(context));
                
                context.setState({ token: responseText, chatClient: chat});

                chat.getSubscribedChannels().then(res => {
                    if(res){
                        // console.log(res,res.items.slice(1))
                        if(res.items){
                            for(let i = 0; i < res.items.length; i++){
                                if(context.state.channelName !== res.items[i].uniqueName){
                                channelArr.push(res.items[i].uniqueName);
                                context.state.chatClient.getChannelByUniqueName(res.items[i].uniqueName)
                                    .then(channel => {
                                        if(channel){
                                            context.state.privChannels.push(channel);
                                            channel.getMessages().then(context.privateLoaded);
                                            channel.on('messageAdded', context.privateAdded);
                                        }
                                    })
                                    .catch(err => {
                                        console.log(err);
                                    });
                                }
                            }
                        }
                        console.log(channelArr)

                        /**
                         * WORK IN PROGRESS HERE
                         */
                    }
                });

            }
        );
	}

    clientInitiated = () => {
        this.setState({ chatReady: true }, () => {
        this.state.chatClient.getChannelByUniqueName(this.state.channelName /* 'notredame' */)
            .then(channel => {
                // debugger;
                if (channel) {
                    this.setState({ channelClient: channel });
                    console.log(channel, this.state.channelClient)
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
                /* this.setState({ channelClient: channel }); */
                channel.getMembers()
                .then(members => {
                    console.log(members, this.state.channelName)
                    // debugger;
                    var shouldJoin = false;
                    let count = 0;
                    let userStr = this.props.userInfo.Name + "|SPLITTER|" + this.props.userInfo.Email;
                    let institutionGrpPath = this.props.userInfo.institutionDir;
                    let institutionGrp = institutionGrpPath.slice(institutionGrpPath.indexOf("Institution")+13, institutionGrpPath.length-1);

                    while (count < members.length){
                        // console.log(count, members.length-1)
                        if(members[count].state.identity === userStr ){
                            // console.log(members[count].state.identity, userStr)
                            shouldJoin = true;
                            return;
                        }
                        else {
                            count++;
                        }
                    }
                    if (shouldJoin) {
                        return channel.join();
                    }

                    else if(institutionGrp === this.state.channelName){
                        return channel.join();
                    }
                    else if(shouldJoin === false){
                        this.setState({noAccess: true});
                        return;
                    }
                    
                })
            })
            .then(() => {
                // debugger;
                let oneOnoneChats;
                if(this.state.channelClient && this.state.noAccess === false){
                    this.state.channelClient.getMessages().then(this.messagesLoaded);
                    this.state.channelClient.on('messageAdded', this.messageAdded);
                }
                oneOnoneChats = this.state.chatClient.channels.channels._c;

                for (const [k, v] of oneOnoneChats.entries()) {
                    console.log(k, '---------------------------', v.uniqueName);
                    // if(v.uniqueName.includes(this.state.mySid)){
                    //     console.log(v.uniqueName)
                    //     v.getMessages().then(this.privateLoaded);
                    //     v.on('messageAdded', this.privateAdded);
                    // }
                    /* let name = v.state.identity.split("|SPLITTER|")[0]
                    if(name === this.props.userInfo.Name){
                        //my id
                        myId = k;
                        this.setState({mySid: myId})
                        // console.log(myId)
                        //MBf6c2f3b72cb9467786de95f9579ee6f1
                    }
                    if(text === name){
                        secondId = k;
                        //MBd274dd4d087d44ad8c72f0672d58ece6
                        // console.log(secondId)
                    } */
                  }
                /* if(this.state.chatClient){
                    this.state.chatClient.getChannelByUniqueName("MBf6c2f3b72cb9467786de95f9579ee6f1MBac4632b86e5c49119be733c771282821").then(res => {
                        if(res){
                        console.log(res);
                        res.getMessages().then(this.privateLoaded);
                        res.on('messageAdded', this.privateAdded);
                        }
                    }).catch(err => {
                        console.log(err);
                    })      
                } */
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
        // console.log(this.state.messages);
        //console.log("MESSAGES:");
        //console.log(messages);
    };

    messageAdded = message => {
        // debugger;
        this.setState((prevState, props) => ({
            messages: [...prevState.messages, message]
        }), () => this.scrollToBottomChat());
    };

    privateLoaded = messagePage => {
        console.log(messagePage)
        // debugger;
        var messages = messagePage.items;
        /*
        if (messages.length > 100) {
            messages = messages.slice(messages.length - 100);
        }
        */
        this.setState({ privateChat: messages, loadMask: false }, () => this.scrollToBottomChat());
        console.log(this.state.privateChat);
        //console.log("MESSAGES:");
        //console.log(messages);
    };

    privateAdded = message => {
        // debugger;
        this.setState((prevState, props) => ({
            privateChat: [...prevState.privateChat, message]
        }), () => this.scrollToBottomChat());
    };

    checkEnter(event) {
        if (event.keyCode === 13) {
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

    startPrivateChat(text){
        // console.log(this.state.channelClient,'=====', this.state.chatClient)
        let myId, secondId, channelString;
        if(this.props.userInfo.Name === text){
            //if its your name, do nothing
            // console.log('same')
            return;
        }
        else {
            // console.log(this.state.channelClient.members._c);
            //new ES6 data structure, MAP
            let mapV = this.state.channelClient.members._c;
            const a = [mapV.values()];
            // console.log(a)

            for (const [k, v] of mapV.entries()) {
                // console.log(k, '---------------------------', v);
                let name = v.state.identity.split("|SPLITTER|")[0]
                if(name === this.props.userInfo.Name){
                    //my id
                    myId = k;
                    // this.setState({mySid: myId})
                    // console.log(myId)
                    //MBf6c2f3b72cb9467786de95f9579ee6f1
                }
                if(text === name){
                    secondId = k;
                    //MBd274dd4d087d44ad8c72f0672d58ece6
                    // console.log(secondId)
                }
            }
            channelString = myId + secondId;
            // if(channelString) this.setState({privChannelStr: channelString})
            
            if(channelString){
                this.state.chatClient.getChannelByUniqueName(channelString)
                .then(res => {
                    // console.log(res, res.uniqueName);
                    if(res.uniqueName){
                    return;
                    }
                    else{
                        this.state.chatClient.createChannel({
                            uniqueName: channelString 
                        }).then(channel => {
                            /**
                             * this is where i would invite the guest to the channel. back-end server call with twilio to invite new user to ch.
                             * WORK IN PROGRESS
                             */
                            return channel.join();
                        });
                    }
                })
                .then(() => {
                    // console.log(this.state.chatClient.getChannelByUniqueName(channelString))
                    if(this.state.chatClient){
                        this.state.chatClient.getChannelByUniqueName(channelString).then(res => {
                            if(res){
                            // console.log(res);
                            res.getMessages().then(this.privateLoaded);
                            res.on('messageAdded', this.privateAdded);
                            }
                            return res;
                        }).then(res => {
                            res.invite(text).then(() => {
                                console.log('Your friend was invited')
                            })
                        })
                        /**
                         * WORK IN PROGRESS
                         * 
                         */
                    }
                })
                .catch((err) => {
                    console.log(err);
                });
            }
        }
        // console.log([myId, secondId])
    }

    getAllYourChannels(){
        this.state.chatClient.getUserChannelDescriptors()
            .then((paginator) => {
                console.log(paginator.state.items)
                for (let i = 0; i < paginator.items.length; i++) {
                  var channel = paginator.items[i];
                  console.log('Channel: ' + channel.friendlyName);
                }
              });    
    }

    render() {

        var teamList = ["University of Notre Dame", "Marketing & Recruiting", "Admissions", "Financial Aid", "Custom Team 1"];
        teamList = teamList.map( function(value) {
			return({ label: value, value: value });
        });

        var context = this;

        var posts = context.state.messages.map( function(post) {
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
                    <CardText style = {{ padding: "5px", borderRadius: "10px", fontSize: "16px" }} >
                        <Flexbox flexDirection = "row" minWidth = "100%" >

                            {/* <img src = { post[1] } className = "img-circle noselect" style = {{ marginRight: "10px" }} alt = { post[0] } draggable = { false } /> */}
 
                            <Flexbox flexDirection = "column" style = {{ width: "100%" }} >
                                <Flexbox style = {{ height: "100%" }} > 
                                    <div style = {{ wordBreak: "break-word" }} >
                                        <span
                                        id= "names"
                                        className= "user-feed-name"
                                        style = {{ fontWeight: "bold" }}
                                        title = "Start a private chat with..."
                                        /* role= "dialog" */
                                        onClick={(e) => {
                                            console.log(e.target.textContent, context.state.chatClient); 
                                            context.startPrivateChat(e.target.textContent);
                                            context.getAllYourChannels()} }>
                                        {post.state.author.split("|SPLITTER|")[0]}</span>
                                        <span>:</span>
                                    {/*  &nbsp; */} {post.state.body}
                                    </div>
                                </Flexbox>

                                <Divider style = {{ marginBottom: "1px", backgroundColor: "#b9b9b9" }} />

                                <Flexbox flexDirection = "row" minWidth = "100%" className = "noselect" style = {{ margin: "-2px 0px -1px 0px" }} >

                                    {/* <i className = "fa fa-comments" style = {{ fontSize: "17px", marginRight: "15px" }} /> */}
                                    {/* <i className = "fa fa-thumbs-up" style = {{ fontSize: "17px",marginRight: "25px" }} /> */}
                                    <div style = {{ margin: "0px 15px 0px 0px" }} > {(post.state.dateUpdated.getMonth() + 1) + "/" + post.state.dateUpdated.getDate() + "/" + post.state.dateUpdated.getFullYear()} </div>

                                    <Flexbox style = {{ width: "100%" }} > 
                                        <div style = {{ width: "100%", textAlign: "right" }} > 
                                            {post.state.dateUpdated.getHours() > 12 ? 
                                                (post.state.dateUpdated.getHours() - 12) + ":" + (post.state.dateUpdated.getMinutes() < 10 ? '0' + post.state.dateUpdated.getMinutes() : post.state.dateUpdated.getMinutes()) + 'pm'
                                                :
                                                (post.state.dateUpdated.getHours() === 12 ? 
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

        var privatePosts = context.state.privateChat.map( function(post) {
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
                    <CardText style = {{ padding: "5px", borderRadius: "10px", fontSize: "16px" }} >
                        <Flexbox flexDirection = "row" minWidth = "100%" >

                            {/* <img src = { post[1] } className = "img-circle noselect" style = {{ marginRight: "10px" }} alt = { post[0] } draggable = { false } /> */}
 
                            <Flexbox flexDirection = "column" style = {{ width: "100%" }} >
                                <Flexbox style = {{ height: "100%" }} > 
                                    <div style = {{ wordBreak: "break-word" }} >
                                        <span
                                        id= "names"
                                        className= "user-feed-name"
                                        style = {{ fontWeight: "bold" }}
                                        /* title = "Start a private chat with... "*/
                                        /* role= "dialog" */
                                        /* onClick={(e) => {
                                            console.log(e.target.textContent, context.state.chatClient); 
                                            context.startPrivateChat(e.target.textContent);
                                            context.getAllYourChannels()} } */>
                                        {post.state.author.split("|SPLITTER|")[0]}</span>
                                        <span>:</span>
                                    {/*  &nbsp; */} {post.state.body}
                                    </div>
                                </Flexbox>

                                <Divider style = {{ marginBottom: "1px", backgroundColor: "#b9b9b9" }} />

                                <Flexbox flexDirection = "row" minWidth = "100%" className = "noselect" style = {{ margin: "-2px 0px -1px 0px" }} >

                                    {/* <i className = "fa fa-comments" style = {{ fontSize: "17px", marginRight: "15px" }} /> */}
                                    {/* <i className = "fa fa-thumbs-up" style = {{ fontSize: "17px",marginRight: "25px" }} /> */}
                                    <div style = {{ margin: "0px 15px 0px 0px" }} > {(post.state.dateUpdated.getMonth() + 1) + "/" + post.state.dateUpdated.getDate() + "/" + post.state.dateUpdated.getFullYear()} </div>

                                    <Flexbox style = {{ width: "100%" }} > 
                                        <div style = {{ width: "100%", textAlign: "right" }} > 
                                            {post.state.dateUpdated.getHours() > 12 ? 
                                                (post.state.dateUpdated.getHours() - 12) + ":" + (post.state.dateUpdated.getMinutes() < 10 ? '0' + post.state.dateUpdated.getMinutes() : post.state.dateUpdated.getMinutes()) + 'pm'
                                                :
                                                (post.state.dateUpdated.getHours() === 12 ? 
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
                <div>
                    <h2
                        className='noselect'
                        style={{
                            color: this.props.settings.colors.overviewButtonsColor.text,
                            backgroundColor: '#018cbb'/* this.props.settings.colors.homePageColors.headerBackground  */, 
                            borderRadius: "2px", 
                           /*  marginTop: "15px", 
                            marginBottom: "3px", */ 
                            paddingBottom: "4px", 
                            margin: "0 auto", 
                            paddingTop: "4px",
                            fontSize: "19px",
                            letterSpacing: "0.14em",
                            textTransform: "uppercase",
                            textAlign: "center"}}
                        >User Feed
                    </h2>
                </div>

                <div style = {{ height: "80%", display: (this.state.loadMask ? "" : "none") }} >
                    <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                </div>

                <Flexbox flexDirection = "column" style = {{ 
                    fontFamily: "helvetica", 
                    height: "100%", 
                    minHeight: "0", 
                    padding: "7px 7px 0px 7px", 
                    display: (this.state.loadMask ? "none" : "") }}  > 

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
                    {/* <Flexbox className="user-feed-tab-defaultOpen" style={{display: ""}}> */}
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
                                {this.state.noAccess ? <div style={{fontSize: "20px", marginTop: "40px", textAlign: "center"}}>
                                NO ACCESS TO THIS ORGANIZATION'S USER FEED. PLEASE CONTACT THE HELP DESK.</div> : posts}
                                
                            </div>
                        </Flexbox>
                    {/* </Flexbox> */}
                    <Flexbox>
                    {/* <Flexbox flexGrow = {1} style = {{ height: "100%", minHeight: "0", display: `${this.state.privChannels ? "" : "none"}` }} >
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
                            {privatePosts}
                        </div>
                    </Flexbox> */}
                    </Flexbox>
                    

                    <textarea 
                        id = "messageBodyHolder" 
                        placeholder = "Type your message." 
                        rows = "5" 
                        value = { this.state.messageBody } 
                        onKeyDown = { (e) => this.checkEnter(e) } 
                        onChange = { (e) => this.setState({ messageBody: e.target.value}) } 
                        style = {{ borderRadius: "5px", padding: "10px", marginTop: "10px", backgroundColor: "#e7e7fd", fontSize: "16px"/* , height: "30%" */ }} 
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
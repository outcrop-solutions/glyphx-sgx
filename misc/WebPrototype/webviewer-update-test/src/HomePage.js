import React from 'react';
import { connect } from 'react-redux';
import { hideSplashScreen } from './LoadMaskHelper.js';
import { withRouter } from 'react-router-dom';
import { makeServerCall } from './ServerCallHelper.js';
import Flexbox from 'flexbox-react';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import TopNavBar from './TopNavBar.js';
import RecentViews from './RecentViews.js';
import ViewsManager from './ViewsManager.js';
import UserFeed from './UserFeed.js';
import AnnouncementsDisplay from './AnnouncementsDisplay.js';
import TutorialWindow from './TutorialWindow.js';
import getMuiTheme from 'material-ui/styles/getMuiTheme';
import './topNav.css';
import './General.css';

const muiTheme = getMuiTheme({
    datePicker: {
        selectColor: "#272958",
        headerColor: "#272958",
    },
    palette:{
        primary1Color: "#272958"
    },
    timePicker: {
        headerColor: "#272958",
    },
});


/**
 * Main wrapper for the home page, gets placed by the router
 */
class HomePage extends React.Component {
	
	constructor(props){
		super(props);
		this.loadRecentView = this.loadRecentView.bind(this);
		this.goToVizView = this.goToVizView.bind(this);
	}
	
    // 0 for start, "done" for end
    state = {
        tutorialStage: "done"
    }
	
	goToVizView(success) {
		if (success) {
            this.props.history.push('/glyph-viewer');
        }
		else {
            console.log("SOMETHING WENT WRONG!");
        }
	}

    /**
	 * React built-in which is called when component mounts.
     * Places some css rules that depend on variable color.
	 */
    componentDidMount() {

        //window.location.reload();

        // Removes the spinning load mask
        hideSplashScreen();

        this.props.dispatch( setRecentVizDropdown( null ));

        this.setState({ height: window.innerHeight });

        if (this.props.timeoutTimer == null) {

            this.props.dispatch( setTimer(new Date().getTime()) );

            var context = this;
            
            var x = setInterval(function() {
                var now = new Date().getTime();
                var distance = now - context.props.timeoutTimer;

                //debugger;

                var minutes = Math.floor((distance % (1000 * 60 * 60)) / (1000 * 60));


                if (minutes == 24) {
                    alert("Your session will expire in 5 minutes due to inactivity.");
                }

                else if (minutes > 28) {
                    clearInterval(x);
                    alert("Your session has expired due to inactivity.");
                    context.props.history.push("/logout");
                }
            }, 60000);

        }

        var style = document.getElementById('themeStyles');

		if (style != null) {
			style.parentElement.removeChild(style);
		}

		style = document.createElement("style");

		// WebKit hack
		style.appendChild(document.createTextNode(""));
		style.setAttribute('id','themeStyles');

		// Add the <style> element to the page
		document.head.appendChild(style);
		
		style.sheet.insertRule('.Collapsible__trigger { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; display: block; text-decoration: none; color: #333333; position: relative; border: none; padding: 15px; background: ' + this.props.settings.colors.collapsibleColor.mainBackground + '; color: white; font-size: 1rem; }', 0);
		style.sheet.insertRule('.Collapsible__trigger.is-open { background: ' + this.props.settings.colors.collapsibleColor.mainCollapsed + '; }', 1);
		style.sheet.insertRule('.Collapsible__trigger:hover { background: ' + this.props.settings.colors.collapsibleColor.mainHover + '; }', 2);
		style.sheet.insertRule('.columnNameHeader { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; font-size: 1rem !important; padding: 10px !important; background: ' + this.props.settings.colors.collapsibleColor.subBackground + '!important; }', 3);
		style.sheet.insertRule('.columnNameHeader.is-open { background: ' + this.props.settings.colors.collapsibleColor.subCollapsed + '!important; }', 4);
		style.sheet.insertRule('.columnNameHeader:hover {  background: ' + this.props.settings.colors.collapsibleColor.subHover + '!important; }', 5);

        style.sheet.insertRule('.C-Size-1 { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelTop + ' !important; }', 6);
        style.sheet.insertRule('.C-Size-1:Hover { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelTopHover + ' !important; }', 7);
        style.sheet.insertRule('.C-Size-2 { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelMiddle + ' !important; }', 8);
        style.sheet.insertRule('.C-Size-2:Hover { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelMiddleHover + ' !important; }', 9);
        style.sheet.insertRule('.C-Size-3 { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelBottom + ' !important; }', 10);
        style.sheet.insertRule('.C-Size-3:Hover { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelBottomHover + ' !important; }', 11);

        style.sheet.insertRule('.funnel-top-body:Hover { background-color: ' + this.props.settings.colors.homePageColors.funnelTopLine + ' !important; }', 12);
        style.sheet.insertRule('.funnel-middle-body:Hover { background-color: ' + this.props.settings.colors.homePageColors.funnelMiddleLine + ' !important; }', 13);
        style.sheet.insertRule('.funnel-bottom-body:Hover { background-color: ' + this.props.settings.colors.homePageColors.funnelBottomLine + ' !important; }', 14);

        style.sheet.insertRule('.Select-placeholder {  color: ' + this.props.settings.colors.tableSelectColor.text + ' !important; }', 15);
		style.sheet.insertRule('.Select-arrow {  border-color: ' + this.props.settings.colors.tableSelectColor.text + ' transparent transparent !important; }', 16);
		style.sheet.insertRule('.Select-value {  fill: ' + this.props.settings.colors.tableSelectColor.selectedText + '!important; font-size: 14px !important;}', 17);
		style.sheet.insertRule('.Select-option.is-selected {  background-color: ' +  this.props.settings.colors.tableSelectColor.selectedBackground + '!important; font-size: 13px !important; color: ' +  this.props.settings.colors.tableSelectColor.selectedText +'}', 18);
		style.sheet.insertRule('.Select-option.is-focused {  background-color: ' +  this.props.settings.colors.tableSelectColor.background + '!important; color: ' +  this.props.settings.colors.tableSelectColor.text + '}', 19);
        style.sheet.insertRule('.Select-control { background-color: ' + this.props.settings.colors.general.lighterBubble + ' !important; }', 20);

        style.sheet.insertRule('.faqCollapse { background: #caccff !important; }', 21);
        style.sheet.insertRule('.faqCollapse:after { color: #000000 !important }', 22);
        style.sheet.insertRule('.faqCollapse:hover { background: #dcdeff !important; }', 23);
        style.sheet.insertRule('.faqCollapse.is-open { background: #9397d1 !important; }', 24);

        style.sheet.insertRule('.Collapsible__contentInner { border: none; }', 25);
        style.sheet.insertRule('.inherit-hover:Hover { background-color: ' + this.props.settings.colors.homePageColors.hoverBackground + ' !important; }', 26);


        var funnelAccess = ""
        for (var key in this.props.funnelData) {
            for (var i = 0; i < this.props.funnelData[key].length; i++) {
                funnelAccess = funnelAccess + this.props.funnelData[key][i][0] + " | ";
            }
        }
        funnelAccess = funnelAccess.slice(0, -3);
        

        window.__lc = window.__lc || {};
        window.__lc.license = 9235035;

        // Visitor information
        window.__lc.visitor = {
            name: this.props.userInfo.Name,
            email: this.props.userInfo.Email
        };

        // Additional information presented to the agent about the user
        window.__lc.params = [
            { name: 'Institution', value: this.props.userInfo.institutionDir.split("/")[this.props.userInfo.institutionDir.split("/").length - 2] },
            { name: 'Funnel Access', value: funnelAccess },
        ];

        (function() {
            var lc = document.createElement('script'); lc.type = 'text/javascript'; lc.async = true;
            lc.src = ('https:' == document.location.protocol ? 'https://' : 'http://') + 'cdn.livechatinc.com/tracking.js';
            var s = document.getElementsByTagName('script')[0]; 
            s.parentNode.insertBefore(lc, s);
        })();

        document.title = "GlyphEd - Home";

        this.fetchSavedViews();
        this.fetchAnnouncements();
        this.fetchSaveUserSettings();
    }

    componentDidUpdate(){
        //debugger;
        console.log('update!');
    }


    /**
     * This function is because on the homepage load we want to fetch the saved views.
     */
    fetchSavedViews(){
        var context = this;
		makeServerCall("fetchSavedViews",
            function (responseText) {
                var response = JSON.parse(responseText);
                
                // Post the new data to the state and hide the window load-mask
               context.props.dispatch( setCurrentSavedViews( response.savedViews ) );
               context.props.dispatch( setTimer(new Date().getTime()) );
            }
        );
    }

    fetchSaveUserSettings(){
            /*makeServerCall("fetchUserSettings",
            function (responseText) {
                var response = JSON.parse(responseText);
                debugger;
            //do-stuff
            }
        );

        makeServerCall("saveUserSettings",
            function (responseText) {
                var response = JSON.parse(responseText);
                debugger;
                //do-stuff

                context.props.dispatch( setTimer(new Date().getTime()) );
            },
            {
                post:true,
                data: { 
                    userSettings: {a:'b','c':'d'}//{the actual object you want to be stored in the file}
                }
            }
        );*/
    }
    
    fetchAnnouncements(){
        var context = this;

        /*
        
        //Fetch Announcements.
        makeServerCall("manageAnnouncements",
                    function (responseText) {
                        var response = JSON.parse(responseText);
                        debugger;
                        //do-stuff
                        context.props.dispatch( setTimer(new Date().getTime()) );
                    },
                    {
                        post:true,
                        data: { 
                            operation:'fetch', //create/delete/fetch/pollResult
                            config:{
                                //type: MultiPoll/SinglePoll/Release/Maintenance/Shout
                                //content: {whatever to put in the description}
                                //AnnouncementId: (if delete/pollResult request)
                                //options:["",""...] options for poll
                                //selectedOptionIds:[]
                            } 
                        }
                    }
                );


        //Poll Creation.
        makeServerCall("manageAnnouncements",
                    function (responseText) {
                        var response = JSON.parse(responseText);
                        debugger;
                        //do-stuff
                        context.props.dispatch( setTimer(new Date().getTime()) );
                    },
                    {
                        post:true,
                        data: { 
                            operation:'create',
                            config:{
                                type: 'SinglePoll',
                                content: "Question1",
                                options:["asdbc","asdfcv"]
                            } 
                        }
                    }
                );

        //delete
        makeServerCall("manageAnnouncements",
                    function (responseText) {
                        var response = JSON.parse(responseText);
                        debugger;
                        //do-stuff
                        context.props.dispatch( setTimer(new Date().getTime()) );
                    },
                    {
                        post:true,
                        data: { 
                            operation:'delete',
                            config:{
                                AnnouncementId:'13'
                            } 
                        }
                    }
                );

        //pollresult with save of user selection.
        makeServerCall("manageAnnouncements",
                    function (responseText) {
                        var response = JSON.parse(responseText);
                        debugger;
                        //do-stuff
                        context.props.dispatch( setTimer(new Date().getTime()) );
                    },
                    {
                        post:true,
                        data: { 
                            operation:'pollResult',
                            config:{
                                AnnouncementId:'12',
                                selectedOptionIds:[4]
                            } 
                        }
                    }
                );
        
        */
    }
	
	//[]
	loadRecentView(rowArray) {
		console.log("recentViewloaded");
		console.log(rowArray);
		var ref = this.refs.viewsManager.getWrappedInstance();
		
		// This means that this is a funnel view click
		if (rowArray.length > 4) {
			//todo work out something for funnel click.
			ref.onLaunch(rowArray,this.goToVizView);
		}
		else { // This means that this is a saved view click
			var savedViewObj;
			
			this.props.storedViews.savedViews.forEach(function(savedView) {
				if (rowArray[3] == savedView.ID) {
					savedViewObj = savedView
                }
			});
			
			if (savedViewObj) {
				ref.onSavedViewSelect(savedViewObj, this.goToVizView, true);
			}
		}
	}

    /**
	 * Updates tutorial stage
     * @param stage: stage to change to
	 */
    updateStage(stage) {
        if (stage === 10) {
            this.setState({ tutorialStage: "done" });
        }

        else {
            this.setState({ tutorialStage: stage });
        }
    }

    render() {
        var imgsrc = window.SERVER_URL + "customerImg/" + window.encodeURIComponent(this.props.userInfo.institutionDir);
		var context = this;
        return (
            <MuiThemeProvider muiTheme = { muiTheme } style = {{ height: "100%" }} >

                <Flexbox flexDirection = "column" minHeight = "100vh" style = {{ height: "100vh" }} >
                    <TopNavBar homePage = { true } tutorialStage = { this.state.tutorialStage } />

                    {this.state.tutorialStage !== "done" ?
                        <div style = {{ height: "100vh", width: "100vw", backgroundColor: "rgba(0,0,0,0.6)", zIndex: "100", position: "fixed" }} />
                        :
                        null
                    }

                    { this.state.tutorialStage !== "done" ?
                            <TutorialWindow tutorialStage = { this.state.tutorialStage } updateStage = { this.updateStage.bind(this) } />
                            :
                            null
                    }
                    
                    <Flexbox flexGrow = {1} style = {{ height: "100%", minHeight: "0" }} >
                        <Flexbox flexDirection = "row" minWidth = "100%" >

                            <Flexbox style = {{ width: "30%", minHeight: "0", zIndex: (this.state.tutorialStage === 1 ? "300" : "5") }} >
                                <div style = {{ padding: "12px 6px 12px 12px", height: "100%", width: "100%", overflow: "auto", backgroundColor: "#ffffff" }} >
                                    <UserFeed />
                                </div>
                             </Flexbox>

                             <Flexbox flexDirection = "column" style = {{ width: "40%" }} >
                                <div style = {{ backgroundColor: "#ffffff" }} >
                                    <div style = {{ height: '120px', margin: "15px 0px -6px" }} >
                                            <img 
                                                src = { imgsrc } 
                                                style = {{ display: "block", margin: "0 auto", maxWidth: "90%", maxHeight: "90%" }} 
                                                alt = "University Logo" 
                                                draggable = { false } 
                                                className = "noselect" 
                                            />
                                    </div>
                                </div>

                                <Flexbox flexGrow = {1} style = {{ height: "100%", zIndex: (this.state.tutorialStage === 3 ? "300" : "5") }} >
                                   <div style = {{ padding: "5px 6px", height: "100%", width: "100%", overflow: "auto", backgroundColor: "#ffffff" }} >
                                        <ViewsManager ref="viewsManager"/>
                                    </div>
                                </Flexbox>
                             </Flexbox> 

                             <Flexbox style = {{ width: "30%", minHeight: "0", zIndex: (this.state.tutorialStage === 2 ? "300" : "5") }} >
                                <div style = {{ padding: "12px 6px 12px 12px", height: "100%", width: "100%", overflow: "auto", backgroundColor: "#ffffff" }} >
                                    <Flexbox flexDirection = "column" style = {{ height: "100%", minHeight: "0" }} >

                                        <RecentViews loadRecentView={(rowObj) => this.loadRecentView(rowObj)}/>

                                        <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, borderRadius: "2px", marginTop: "15px", marginBottom: "3px", paddingBottom: "4px" }} >
                                            <div 
                                                style = {{ 
                                                    color: this.props.settings.colors.overviewButtonsColor.text, 
                                                    margin: "0 auto",
                                                    width: "138px", 
                                                    paddingTop: "4px",
                                                    fontSize: "18px"
                                                }}
                                                className = "noselect"
                                            > 
                                                Announcements
                                            </div>
                                        </div>

                                        <AnnouncementsDisplay />
            
                                    </Flexbox>
                                </div>
                             </Flexbox>   

                        </Flexbox>
                    </Flexbox>

                </Flexbox>
          </MuiThemeProvider>
        );
    }
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
	storedViews: state.filterState.StoredViews,
    funnelData: state.filterState.FunnelData,
    timeoutTimer: state.filterState.TimeoutTimer
  }
}

export const setCurrentSavedViews = (savedViewsList) => ({
    type: 'UPDATE_SAVED_VIEWS',
    savedViewsList,
 });

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});

export const setRecentVizDropdown = (recentVizDropdown) => ({
    type: 'SET_RECENT_VIZ_DROPDOWN',
    recentVizDropdown,
});


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(HomePage));
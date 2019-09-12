import React from 'react';
import { connect } from 'react-redux';
import { hideSplashScreen } from './LoadMaskHelper.js';
import { deleteCookie, getLoginCookieName, makeServerCall } from './ServerCallHelper.js';
import { withRouter } from 'react-router';
import Flexbox from 'flexbox-react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import FloatingActionButton from 'material-ui/FloatingActionButton';
import ComponentLoadMask from './ComponentLoadMask.js';
import Tooltip from 'rc-tooltip';
import TimeoutAlert from './TimeoutAlert.js';
import 'rc-tooltip/assets/bootstrap.css';
import './css/TopNav.css';
import './css/General.css';


injectTapEventPlugin();


/**
 * -ADCMT
 */
class VisualizationView extends React.Component {

	state = {
        glyphViewLoaded: false,
        topNavBarHeight: 0,
        showCorrection: false,
        hideOnClick: false,
        setTimer: false,
        vizKey: ''
    };


	/**
	 * This function is called right after the react component is mounted.
	 * It decides whether to show the login form and calls the init().
	 */
    componentDidMount() {

        // debugger;
        // if (window.performance) {
        //     if (performance.navigation.type === 1) {
        //         this.props.history.push('/home');
        //     }
        // }

        // if (this.state.setTimer == false) {

        //     this.setState({ setTimer: true });

        //     var context = this;
            
        //     var x = setInterval(function() {
        //         context.setState(context.state);
        //     }, 60000);

        // }

        this.init();

        var style = document.getElementById('themeStyles');
		
		if (style != null) {
			style.parentElement.removeChild(style);
			//console.log('deleting old rules');
		}
		
		style = document.createElement("style");

		// WebKit hack
		style.appendChild(document.createTextNode(""));
		style.setAttribute('id','themeStyles');

		// Add the <style> element to the page
		document.head.appendChild(style);
		
		style.sheet.insertRule('.Collapsible__trigger { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; border-radius: 3px; display: block; text-decoration: none; color: #333333; position: relative; border: none; padding: 8px; background: ' + this.props.settings.colors.collapsibleColor.mainBackground + '; color: white; font-size: 1rem; }', 0);
		style.sheet.insertRule('.Collapsible__trigger.is-open { background: ' + this.props.settings.colors.collapsibleColor.mainCollapsed + '; }', 1);
		style.sheet.insertRule('.Collapsible__trigger:hover { background: ' + this.props.settings.colors.collapsibleColor.mainHover + '; }', 2);
		style.sheet.insertRule('.unpinned { font-size: 20px !important; transform: rotateZ(35deg) !important; color: ' + this.props.settings.colors.collapsibleColor.unpinned + '!important; }', 3);
		style.sheet.insertRule('.pinned { font-size: 20px !important; transform: rotateZ(0deg) !important; color: ' + this.props.settings.colors.collapsibleColor.pinned + '!important; }', 4);
		style.sheet.insertRule('.columnNameHeader { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; font-size: 1rem !important; padding: 4px !important; background: ' + this.props.settings.colors.collapsibleColor.subBackground + '!important; }', 5);
		style.sheet.insertRule('.columnNameHeader.is-open { background: ' + this.props.settings.colors.collapsibleColor.subCollapsed + '!important; border-bottom-left-radius: 0px; border-bottom-right-radius: 0px; }', 6);
		style.sheet.insertRule('.columnNameHeader:hover {  background: ' + this.props.settings.colors.collapsibleColor.subHover + '!important; }', 7);
		
		// For the selectboxes  
		style.sheet.insertRule('.Select-placeholder {  color: ' + this.props.settings.colors.tableSelectColor.text + ' !important; }', 8);
		style.sheet.insertRule('.Select-arrow {  border-color: ' + this.props.settings.colors.tableSelectColor.text + ' transparent transparent !important; }', 9);
        style.sheet.insertRule('.Select-control { background-color: #ffffff !important; }', 10);
		style.sheet.insertRule('.Select-value {  fill: ' + this.props.settings.colors.tableSelectColor.selectedText + '!important; font-size: 13px !important;}', 11);
		style.sheet.insertRule('.Select-option.is-selected {  background-color: ' +  this.props.settings.colors.tableSelectColor.selectedBackground + '!important; font-size: 13px !important; color: ' +  this.props.settings.colors.tableSelectColor.selectedText +'}', 12);
		style.sheet.insertRule('.Select-option.is-focused {  background-color: ' +  this.props.settings.colors.tableSelectColor.background + '!important; color: ' +  this.props.settings.colors.tableSelectColor.text + '}', 13);
        style.sheet.insertRule('.Select--multi .Select-value {  background-color: ' + this.props.settings.colors.tableSelectColor.chipBackground + ' !important; border: 1px solid ' + this.props.settings.colors.tableSelectColor.chipBackground + ' !important; color: ' + this.props.settings.colors.tableSelectColor.selectedText + ' !important; margin: 3px 0px 0px 8px; height: 28px}', 14);
		style.sheet.insertRule('.Select--multi .Select-value-icon {  border-right: 1px solid ' + this.props.settings.colors.tableSelectColor.deleteHover + ' !important; padding: 0px 3px 0px 3px; margin: 1px 0px 0px 0px; font-size: 18px;}', 15);
        style.sheet.insertRule('.Select--multi .Select-value-icon:hover, .Select--multi .Select-value-icon:focus { color: ' + this.props.settings.colors.tableSelectColor.deleteHover + '!important; }', 16);

        style.sheet.insertRule('.faqCollapse { background: #caccff !important; }', 17);
        style.sheet.insertRule('.faqCollapse:after { color: #000000 !important }', 18);
        style.sheet.insertRule('.faqCollapse:hover { background: #dcdeff !important; }', 19);
        style.sheet.insertRule('.faqCollapse.is-open { background: #9397d1 !important; }', 20);
        style.sheet.insertRule('.Collapsible__contentInner { border: none; border-bottom-left-radius: 3px; border-bottom-right-radius: 3px; }', 21);

        style.sheet.insertRule('.subCollapsibleInner { background-color: ' + this.props.settings.colors.homePageColors.subBackground + '; }', 22);

        style.sheet.insertRule('html { overflow: hidden; }', 23);



        var sidebarButton = document.getElementById("showSideBar");
        sidebarButton.style.display = "none";


        document.title = "GlyphEd - Viewer";

    }

    componentDidUpdate(){
        //debugger;
        let context = this;
        var now = new Date().getTime();
        var distance = now - this.props.timeoutTimer;

        //console.log("VIZ-SESSION: " + this.props.timeoutTimer);

        var minutes = Math.floor((distance % (1000 * 60 * 60)) / (1000 * 60));

        if (minutes === 23) {
            this.props.dispatch(editModalDisplay(true));
        }

        else if (minutes > 27) {
            let context = this;
            return new Promise((resolve, reject) => {
                makeServerCall("logout",
                function (responseText) { 
                    resolve(true);
                }
            );
            }).then(res =>{
                if(res === true){
                    alert("Your session has expired due to inactivity.");
                    window.location.reload();
                    deleteCookie(getLoginCookieName());
                    context.props.dispatch(logoutClear());
                    hideSplashScreen();
                }
            }).catch(err =>{
                // console.log(err);
            });
        }
        
    }

    componentWillUnmount(){
        this.props.dispatch(editModalDisplay_2(false));
        this.props.dispatch(clearStatisticsData());
    }

    
    /**
	 * -ADCMT
	 */
    updateViz (key) {
        //This is to refresh the iframe.
        this.setState({ vizKey: "" }, function() {
            this.setState({vizKey: key})
        });
        //console.log('state set: ' + key);
        /*var iframe = document.getElementById('GlyphViewer')
        var temp = iframe.src;
        iframe.src = "";
        iframe.src = temp;*/
    }


	/**
	 * This function does all the initialization that is needed.
	 */
    init() {
        var context = this;

        context.timeout = window.setInterval(
            function() {
                if (context.state.glyphViewLoaded) {
                    hideSplashScreen();
                    clearInterval(context['timeout']);
                }
            }, 
        250);
        
        this.setState({ vizKey: '' });
        
        var gv = document.getElementById('GlyphViewerContainer');
        /* var filterNav = document.getElementById("filterNav"); */
        var topNav = document.getElementById("TopNav");

        if (!this.props.settings.sideBarOverlap) {
            gv.style.width = "calc(100% - 450px)";
        }

        else {
            gv.style.width = "100%";
        }

        this.setState({ topNavBarHeight: topNav.clientHeight });
    }
	

	/**
	 * OnLoad of the iframehide the loadmask
	 */
    onLoadGlyphView(){
        this.setState({ glyphViewLoaded: true });
    }


    /**
	 * Displays an invisible div over the iframe when dragging a draggable window for it to not bug out
     * @param action: true/false indicating display of the invisible div
	 */
    handleDraggableCorrection(action, onClick) {
        if (onClick) {
            this.setState({ showCorrection: action, hideOnClick: onClick });
        }
        else {
            this.setState({ showCorrection: action });
        }
    }


    /**
	 * Hides the filter side nav by translating it off the screen so it doesnt resize and 
     * Wont have to be reloaded after it is "closed"
	 */ 
    toggleNav() {
        var filterNav = document.getElementById("filterNav");
        var filterNavOpen = filterNav.style.transform === "translate(460px, 0px)" ? false : true;
        var gv = document.getElementById('GlyphViewerContainer');

        var sidebarButton = document.getElementById("showSideBar");
        sidebarButton.style.display = "none";

        if (!filterNavOpen) {
            //open the filterNav sidebar
            filterNav.style.transform = "translate(0px, 0px)";
            if (!this.props.settings.sideBarOverlap) {
                gv.style.width = "calc(100% - 450px)";
            }

            else {
                gv.style.width = "100%";
            }
        }

        else {
            filterNav.style.transform = "translate(460px, 0px)";
            gv.style.width = "100%";
        }
    }


    showHideLoadingMask(state) {
        this.setState({ glyphViewLoaded: state });
    }
    
    returnHome() {
        if (document.getElementById('GlyphViewer')) {
            var iframe = document.getElementById('GlyphViewer').contentWindow;
            iframe.closeSceneView();
        }
        this.props.dispatch(clearFilterOptions());
        this.props.history.push('/home')
    }


    render() {
        return (
            <MuiThemeProvider>
                <div style = {{ width: '100%', height: '100%' }}>
						
					{/* Circular Load Mask to show when server calls made. */}
                    <div 
                        id = "LoadMask1"  
                        style = {{ 
                            position: 'absolute',    
                            width: '100%',
                            height: '100%',
                            zIndex: 1500,
                            backgroundColor: '#000000',
                            opacity: 0.5,
                            textAlign: 'center',
                            visibility: 'hidden',
                            display: 'table'
                        }} 
                    >
                        <CircularProgress id = "innerLoadMask"
                            size = { 60 } 
                            style = {{
                                display: 'table-cell',
                                verticalAlign: 'middle'
                            }}
                            thickness = { 7 } 
                        />
                    </div>

                    <TimeoutAlert />

                    <div id  = "filterNav" className = "sidenav" style = {{ height: "100%" }} >
                        <Flexbox flexDirection = "column" minHeight = "100vh" style = {{ height: "100vh", overflow: 'hidden' }}>
                            <div className = "TopNav" id = "TopNav" style = {{ width: '100%', height: '36px', transition: '0.37s' }}>
                                <TopNavBar />
                            </div>

                            <FilterSideBar 
                                updateViz = { (key) => {this.updateViz(key), console.log(key)} } 
                                showHideLoadingMask = { this.showHideLoadingMask.bind(this) } 
                                handleDraggableCorrection = { this.handleDraggableCorrection.bind(this) } 
                            />
                        </Flexbox>
                    </div>

                    <div id = "showSideBar" >
                        <Tooltip
                            placement = 'left'
                            mouseEnterDelay = { 0.5 }
                            mouseLeaveDelay = { 0.15 }
                            destroyTooltipOnHide = { false }
                            trigger = { Object.keys( {hover: 1} ) }
                            overlay = { 
                                <div> 
                                    Display Filter Side Bar
                                </div> 
                            }
                        >
                            <FloatingActionButton 
                                backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                                style = {{
                                    top: '5px',
                                    right: '5px',
                                    position: 'absolute',
                                    zIndex: '10'
                                }}
                                mini = { true }
                                //iconStyle = {{ height: "36px", width: "36px" }}
                                onClick = { () => this.toggleNav() }
                            >
                                <i 
                                    className = "fa fa-caret-down" 
                                    style = {{
                                        fontSize: '1.8em',
                                        color: this.props.settings.colors.collapsibleColor.mainIcon,
                                        transform: 'rotateZ(90deg)',
                                        margin: "0px 0px 0px -2px"
                                    }}
                                /> 
                            </FloatingActionButton>
                        </Tooltip>
                    </div>

                    <Tooltip
                        placement = 'right'
                        mouseEnterDelay = { 0.5 }
                        mouseLeaveDelay = { 0.15 }
                        destroyTooltipOnHide = { false }
                        trigger = { Object.keys( {hover: 1} ) }
                        overlay = { 
                            <div> 
                                Return to Home Page
                            </div> 
                        }
                    >
                        <FloatingActionButton 
                            backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                            style = {{
                                top: '5px',
                                left: '5px',
                                position: 'absolute',
                                zIndex: '10',
                                display: (this.props.uid ? "none" : "")
                            }}
                            mini = { true }
                            //iconStyle = {{ height: "36px", width: "36px" }}
                            onClick = { () => this.returnHome() }
                        >
                            <i 
                                className = "fa fa-home"
                                style = {{
                                    fontSize: '1.8em',
                                    color: this.props.settings.colors.collapsibleColor.mainIcon,
                                }}
                            /> 
                        </FloatingActionButton>
                    </Tooltip>
					
					{/* Actual Application body that you see */}
                    <Flexbox flexDirection = "column" minHeight = "100vh" style = {{ height: "100vh", overflow: 'hidden' }}>

                        <Flexbox flexGrow = {1} id = "iframeDiv" style = {{ height: "100%", minHeight: "0", overflow: 'hidden' }} >

                            <div id = "GlyphViewerContainer" 
                                style = {{ 
                                    transition: '0.37s', 
                                    width: '100%', 
                                    height: '100%'}}>

                                {/* Draggable windows */}
                                <StatisticModal handleCorrection = { this.handleDraggableCorrection.bind(this) } />
                                <GlyphLegend handleCorrection = { this.handleDraggableCorrection.bind(this) } />

                                {/* Invisible div over the iframe to allow draggable windows to not bug out */}
                                <div 
                                    style = {{ height: "100vh", width: "calc(100vw - 450px)", zIndex: "500", position: "fixed", display: (this.state.showCorrection ? "" : "none") }}
                                    onClick = { () => this.state.hideOnClick ? this.setState({ showCorrection: false, hideOnClick: false  }) : null }
                                />


                                <div style = {{ width: "100%", height: "100%", display: ((this.state.glyphViewLoaded || this.props.uid )? "none" : "") }} >
                                    <ComponentLoadMask 
                                        stopLoop = { this.state.glyphViewLoaded ? true : false } 
                                        bgColor = "#c6c6c6" 
                                        color = { this.props.settings.colors.buttons.general } 
                                        imgLink = "./Res/Img/GlyphED.png" />
                                </div>

                                {(this.state.vizKey === '' || this.props.uid) ? 
                                    <div></div>
                                    : 
                                    <iframe 
                                        id = "GlyphViewer" 
                                        onLoad = { this.onLoadGlyphView.bind(this) } 
                                        title = "3D rendering engine" 
                                        style = {{ 
                                            width:'100%', 
                                            height:'100%', 
                                            border: 'none'}} 
                                        src = {"./Res/Viz/demo.html#" + this.state.vizKey}
                                    />
                                } 

                            </div>

                            {(!this.props.uid ? <FloatingToggleButtons
                                iframeLoaded = { this.state.glyphViewLoaded } 
                                topNavBarHeight = { this.state.topNavBarHeight } 
                                handleDraggableCorrection = { this.handleDraggableCorrection.bind(this) } 
                            /> : null)}

                        </Flexbox>
                    </Flexbox>
                </div>

          </MuiThemeProvider>
        );
    }
}


export const editModalDisplay = (timeoutModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    timeoutModal,
});

export const clearFilterOptions = () => ({
    type: 'CLEAR_FILTER',
    
});

export const editModalDisplay_2 = (legendModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    legendModal,
});

export const clearStatisticsData = () => ({
    type: 'CLEAR_STATISTICS_DATA'
});

export const logoutClear = () => ({
    type: 'LOGOUT_CLEAR',
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
    VizParams: state.filterState.VizParams,
    timeoutTimer: state.filterState.TimeoutTimer,
    uid: state.filterState.uid
  }
}


/**
 * Connects the redux store to get access to global states. withRouter allows component to change navigation route.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(VisualizationView));
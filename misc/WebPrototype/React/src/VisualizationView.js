import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import {hideSplashScreen} from './LoadMaskHelper.js';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import UndoRedoChangeListener from './UndoRedoChangeListener.js';
import './topNav.css';
import 'font-awesome/css/font-awesome.min.css';

injectTapEventPlugin();


class VisualizationView extends React.Component {
    

	// Initial state of the component.
	state = {
        glyphViewLoaded: false,
        glyphWindowWidth: 0,
        topNavBarHeight: 0,
        showCorrection: false
    };

	/**
	 * This function is called right after the react component is mounted.
	 * It decides whether to show the login form and calls the init().
	 */
    componentDidMount() {
        this.init();

        var style = document.getElementById('themeStyles');
		
		if (style != null) {
			style.parentElement.removeChild(style);
			console.log('deleting old rules');
		}
		
		style = document.createElement("style");

		// WebKit hack
		style.appendChild(document.createTextNode(""));
		style.setAttribute('id','themeStyles');

		// Add the <style> element to the page
		document.head.appendChild(style);
		
		style.sheet.insertRule('.Collapsible__trigger { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; border-radius: 3px; display: block; font-weight: 400; text-decoration: none; color: #333333; position: relative; border: none; padding: 15px; background: ' + this.props.settings.colors.collapsibleColor.mainBackground + '; color: white; font-size: 1rem; }', 0);
		style.sheet.insertRule('.Collapsible__trigger.is-open { background: ' + this.props.settings.colors.collapsibleColor.mainCollapsed + '; }', 1);
		style.sheet.insertRule('.Collapsible__trigger:hover { background: ' + this.props.settings.colors.collapsibleColor.mainHover + '; }', 2);
		style.sheet.insertRule('.unpinned { font-size: 20px !important; transform: rotateZ(35deg) !important; color: ' + this.props.settings.colors.collapsibleColor.unpinned + '!important; }', 3);
		style.sheet.insertRule('.pinned { font-size: 20px !important; transform: rotateZ(0deg) !important; color: ' + this.props.settings.colors.collapsibleColor.pinned + '!important; }', 4);
		style.sheet.insertRule('.columnNameHeader { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; font-size: 1rem !important; padding: 10px !important; background: ' + this.props.settings.colors.collapsibleColor.subBackground + '!important; }', 5);
		style.sheet.insertRule('.columnNameHeader.is-open { background: ' + this.props.settings.colors.collapsibleColor.subCollapsed + '!important; border-bottom-left-radius: 0px; border-bottom-right-radius: 0px; }', 6);
		style.sheet.insertRule('.columnNameHeader:hover {  background: ' + this.props.settings.colors.collapsibleColor.subHover + '!important; }', 7);
		
		// For the selectboxes  
		style.sheet.insertRule('.Select-placeholder {  color: ' + this.props.settings.colors.tableSelectColor.text + ' !important; }', 8);
		style.sheet.insertRule('.Select-arrow {  border-color: ' + this.props.settings.colors.tableSelectColor.text + ' transparent transparent !important; }', 9);
        style.sheet.insertRule('.Select-control { background-color: ' + this.props.settings.colors.tableSelectColor.background + '!important; }', 10);
		style.sheet.insertRule('.Select-value {  fill: ' + this.props.settings.colors.tableSelectColor.selectedText + '!important; font-size: 13px !important;}', 11);
		style.sheet.insertRule('.Select-option.is-selected {  background-color: ' +  this.props.settings.colors.tableSelectColor.selectedBackground + '!important; font-size: 13px !important; color: ' +  this.props.settings.colors.tableSelectColor.selectedText +'}', 12);
		style.sheet.insertRule('.Select-option.is-focused {  background-color: ' +  this.props.settings.colors.tableSelectColor.background + '!important; color: ' +  this.props.settings.colors.tableSelectColor.text + '}', 13);
        style.sheet.insertRule('.Select--multi .Select-value {  background-color: ' + this.props.settings.colors.tableSelectColor.chipBackground + ' !important; border: 1px solid ' + this.props.settings.colors.tableSelectColor.chipBackground + ' !important; color: ' + this.props.settings.colors.tableSelectColor.selectedText + ' !important; margin: 3px 0px 0px 8px;}', 14);
		style.sheet.insertRule('.Select--multi .Select-value-icon {  border-right: 1px solid ' + this.props.settings.colors.tableSelectColor.deleteHover + ' !important; padding: 0px 3px 0px 3px; margin: 1px 0px 0px 0px; font-size: 18px;}', 15);
        style.sheet.insertRule('.Select--multi .Select-value-icon:hover, .Select--multi .Select-value-icon:focus { color: ' + this.props.settings.colors.tableSelectColor.deleteHover + '!important; }', 16);

        style.sheet.insertRule('.faqCollapse { background: #caccff !important; }', 17);
        style.sheet.insertRule('.faqCollapse:after { color: #000000 !important }', 18);
        style.sheet.insertRule('.faqCollapse:hover { background: #dcdeff !important; }', 19);
        style.sheet.insertRule('.faqCollapse.is-open { background: #9397d1 !important; }', 20);
        style.sheet.insertRule('.Collapsible__contentInner { border: none; border-bottom-left-radius: 3px; border-bottom-right-radius: 3px; }', 21);

        style.sheet.insertRule('.subCollapsibleInner { background-color: ' + this.props.settings.colors.homePageColors.subBackground + '; }', 22);

        style.sheet.insertRule('html { overflow: hidden; }', 23);

    }

	
	/**
	 * This function does all the initialization that is needed.
	 */
    init() {
        var context = this;
        context.timeout = window.setInterval(
            function(){
                if (context.state.glyphViewLoaded) {
                    hideSplashScreen();
                    clearInterval(context['timeout']);
                }
            }, 250);

        var gv = document.getElementById('GlyphViewerContainer');
        var filterNav = document.getElementById("filterNav");
        var topNav = document.getElementById("TopNav");
        var width = gv.clientWidth - filterNav.clientWidth;

        if (!this.props.settings.sideBarOverlap) {
            gv.style.width = width + "px";
        }
        else {
            gv.style.width = "100%";
        }

        this.setState({ glyphWindowWidth: width, topNavBarHeight: topNav.clientHeight });
    }
	

	/**
	 * OnLoad of the glyphviewer (iframe) we set this to true so that the loadmask can hide.
	 */
    onLoadGlyphView(){
        this.setState({
            glyphViewLoaded: true
        });
    }

    handleDraggableCorrection(action) {
        this.setState({ showCorrection: action });
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

                    <div id  = "filterNav" className = "sidenav" style = {{ fontWeight: "normal", height: "calc(100% - 56px)", marginTop: "56px" }} >
                        <FilterSideBar/>
                    </div>
					
					{/* Actual Application body that you see */}
                    <Flexbox flexDirection = "column" minHeight = "100vh" style = {{ height: "100vh", overflow: 'hidden' }}>

                        <div className = "TopNav" id = "TopNav" style = {{ width: '100%', height: '56px', transition: '1s' }}>
                            <TopNavBar glyphWindowWidth = { this.state.glyphWindowWidth } />
                        </div>

                        <Flexbox flexGrow = {1} id = "iframeDiv" style = {{ height: "100%", minHeight: "0", overflow: 'hidden' }} >

                            

                            <div id = "GlyphViewerContainer" style = {{ transition: '0.37s', width: '100%', height: '100%' }} >
                                <StatisticModal handleCorrection = { this.handleDraggableCorrection.bind(this) } />
                                <GlyphLegend handleCorrection = { this.handleDraggableCorrection.bind(this) } />
                                
                                <UndoRedoChangeListener />

                                <div style = {{ height: "100vh", width: "100vw", zIndex: "500", position: "fixed", display: (this.state.showCorrection ? "" : "none") }} />

                                <iframe 
                                    id = "GlyphViewer" 
                                    onLoad = { this.onLoadGlyphView.bind(this) } 
                                    title = "3D rendering engine" 
                                    style = {{ width:'100%', height:'100%', border: 'none' }} 
                                    src = "https://s3.amazonaws.com/synglyphx/demo.html" 
                                /> 
                            </div>
                            <FloatingToggleButtons topNavBarHeight = { this.state.topNavBarHeight } /> 
                        </Flexbox>
                    </Flexbox>
                </div>
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
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(VisualizationView);
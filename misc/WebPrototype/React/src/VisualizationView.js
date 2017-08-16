import React from 'react';
import { connect } from 'react-redux';
import { Flex } from 'react-flex-material';
import injectTapEventPlugin from 'react-tap-event-plugin';
import {hideSplashScreen} from './LoadMaskHelper.js';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import './topNav.css';
import 'font-awesome/css/font-awesome.min.css';

injectTapEventPlugin();


class VisualizationView extends React.Component {
    

	// Initial state of the component.
	state = {
        glyphViewLoaded: false,
        glyphWindowWidth: 0,
        topNavBarHeight: 0,
    };

	/**
	 * This function is called right after the react component is mounted.
	 * It decides whether to show the login form and calls the init().
	 */
    componentDidMount() {
        this.init();
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
	 * OnLoad of the glyphviewer(iframe) we set this to true so that the loadmask can hide.
	 */
    onLoadGlyphView(){
        this.setState({
            glyphViewLoaded: true
        });
    }


    render() {
        return (
            <MuiThemeProvider>
                <div style = {{ width:'100%', height:'100%' }}>
						
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
					
					{/* Actual Application body that you see */}
                    <Flex layout = "column" style = {{ position:'absolute', width:'100%', height:'100%' }}>

                        <Flex >
                            <div className = "TopNav" id="TopNav" style = {{ width:'100%', height:'56px',transition: '1s' }}>

                                <TopNavBar glyphWindowWidth = { this.state.glyphWindowWidth } />
								
                                <div id  = "filterNav" className = "sidenav" >
                                    <FilterSideBar/>
                                </div>
					
                            </div>
                        </Flex>

                        <Flex id = "iframeDiv" flex = "100" style = {{ overflow: 'hidden' }}>
                            <div id = "GlyphViewerContainer" style = {{ transition:'0.37s', width:'100%', height:'100%' }} >

                                <StatisticModal />
                                <GlyphLegend />

                                <iframe 
                                    id = "GlyphViewer" 
                                    onLoad={this.onLoadGlyphView.bind(this)} 
                                    title = "3D rendering engine" 
                                    style = {{ width:'100%', height:'100%', border: 'none' }} 
                                    src = "https://s3.amazonaws.com/synglyphx/demo.html" 
                                /> 

                            </div>
                            <FloatingToggleButtons topNavBarHeight = { this.state.topNavBarHeight } /> 
                        </Flex>
                    </Flex>
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
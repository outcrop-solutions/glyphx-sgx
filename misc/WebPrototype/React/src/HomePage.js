import React from 'react';
import { connect } from 'react-redux';
import { Flex } from 'react-flex-material';
import {hideSplashScreen} from './LoadMaskHelper.js';
import { Card, CardText } from 'material-ui/Card';
import injectTapEventPlugin from 'react-tap-event-plugin';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import ViewsManager from './ViewsManager.js';
import './topNav.css';
import 'font-awesome/css/font-awesome.min.css';

class HomePage extends React.Component {
    

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
        hideSplashScreen();
    }


    render() {
        return (
            <MuiThemeProvider>
                    <Flex layout = "column" style = {{ position:'absolute', width:'100%', height:'100%' }}>

                        <Flex>
                            <div style = {{ height:'56px' }}>
                                <TopNavBar />
                            </div>
                        </Flex>

                        <Flex style = {{ overflow: 'hidden', height: '15%', padding: "10px 10px 0px 10px" }}>
                            <Card containerStyle = {{ padding: "0px" }} style = {{ height: "100%" }} >
                                <CardText
                                    style = {{
                                        padding: "0px"
                                    }}
                                >
                                    <div style = {{ width: '100%', height: '100%' }} >
                                        <div style = {{  width: "350px", margin: "0 auto" }}>
                                            <h1 style = {{ margin: "0px 0px -10px 84px", paddingTop: "18px" }} > Welcome to </h1>
                                            <img src = "./Res/Img/GlyphED.png" style = {{ width: '350px' }} alt = "GlyphEd"/>
                                        </div>
                                    </div>
                                </CardText>
                            </Card>
                        </Flex>
                        
                        <Flex layout = "row" style = {{ width: '100%', height: '100%', padding: "10px" }}>
                            <Flex flex = "33" style = {{ backgroundColor: "purple", height: "100%" }} >
                                <Card containerStyle = {{ padding: "2px" }} style = {{ height: "100%" }} >
                                    <CardText
                                        style = {{
                                            padding: "2px",
                                        }}
                                    >
                                        <div style = {{ width: '100%', height: '100%' }} >
                                            User Feed goes here
                                        </div>
                                    </CardText>
                                </Card>
                            </Flex>

                            <Flex flex = "66">
                                <Flex style = {{  height: "15%", padding: "0px 0px 0px 10px" }} >
                                    <Card containerStyle = {{ padding: "0px" }} style = {{ height: "100%" }} >
                                        <CardText
                                            style = {{
                                                padding: "0px",
                                                height: "100%"
                                            }}
                                        >
                                            <div style = {{ width: '100%', height: '100%' }} >
                                                <div style = {{ width: "450px", margin: "0 auto" }}>
                                                    <img src = "./Res/Img/notredame.png" style = {{ width: 'inherit', margin: "9px 0px 0px 0px" }} alt = "University Logo"/>
                                                </div>
                                            </div>
                                        </CardText>
                                    </Card>
                                </Flex>

                                <Flex layout = "row" style = {{ height: "85%", padding: "10px 0px 0px 10px" }} >
                                    <Flex flex = "50" style = {{ padding: "0px 10px 0px 0px" }}>
                                        <Card containerStyle = {{ padding: "8px" }} style = {{ height: "100%" }} >
                                            <CardText
                                                style = {{
                                                    padding: "8px",
                                                }}
                                            >
                                                <div>
                                                    <ViewsManager />
                                                </div>
                                            </CardText>
                                        </Card>
                                    </Flex>

                                    <Flex flex = "50">
                                        <Card containerStyle = {{ padding: "8px" }} style = {{ height: "100%" }} >
                                            <CardText
                                                style = {{
                                                    padding: "8px",
                                                }}
                                            >
                                                <div style = {{  textAlign: "justify", textJustify: "inter-word" }} >
                                                    GlyphEd is a data visualization software designed to help the Enrollment Management Office benefit from the breadth 
                                                    and depth of available Enrollment Management data. We welcome your interest and input on product direction.
                                                </div>
                                                    <br/>For <i style = {{ textDecoration: "underline" }} >Help &amp; Support</i>, please contact us at: <a href="mailto:Support@GlyphEd.co">Support@GlyphEd.co</a>

                                                    <br/><br/>For <i style = {{ textDecoration: "underline" }} > Sales and Product Direction </i>, please contact us at: <a href="mailto:Mark@GlyphEd.co">Mark@GlyphEd.co</a>

                                                    <br/><br/>If you prefer to call, we can be reached at: <i style = {{ textDecoration: "underline" }} > 571-733-9469</i>  (During normal business hours, Eastern Time).

                                            </CardText>
                                        </Card>
                                    </Flex>
                                </Flex>
                            </Flex>
                        </Flex>

                    </Flex>
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
export default connect(mapStateToProps)(HomePage);
import React from 'react';
import { connect } from 'react-redux';
import { Flex } from 'react-flex-material';
import injectTapEventPlugin from 'react-tap-event-plugin';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import { Link } from 'react-router-dom';
import './NotFoundPage.css';
import 'font-awesome/css/font-awesome.min.css';

class NotFoundPage extends React.Component {
    

	// Initial state of the component.
	state = {
        glyphViewLoaded: false,
        glyphWindowWidth: 0,
        topNavBarHeight: 0,
    };

	
	/**
	 * This function shows the in app loadmask(cicular waiting).
	 */
    showLoadMask = () => {
        document.getElementById("LoadMask").style.visibility = "visible";
    };

	
	/**
	 * This function hides the initial loadmask/splash screen.
	 */
    hideLoadMask = () => {
        var lm = document.getElementById("ipl-progress-indicator");
        if (lm) {
            setTimeout(() => {
                document.getElementById("ipl-progress-indicator").classList.add('available');
                setTimeout(() => {
                document.getElementById("ipl-progress-indicator").outerHTML = '';
                }, 2000)
            }, 1000)
        }
    };
	

	/**
	 * This function is called right after the react component is mounted.
	 * It decides whether to show the login form and calls the init().
	 */
    componentDidMount() {
        this.hideLoadMask();
    }


    render() {
        return (
            <MuiThemeProvider>
				<div className="error-page-wrap">
                    <article className="error-page gradient">
                        <hgroup>
                            <h1>404</h1>
                            <h2>Can you even type?</h2>
                        </hgroup>
                        <Link to = "/home" style = {{ color: "#ffffff" }} >Back to Home</Link>
                    </article>
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
export default connect(mapStateToProps)(NotFoundPage);
import React from 'react';
import { connect } from 'react-redux';
import { Link } from 'react-router-dom';
import { hideSplashScreen } from './LoadMaskHelper.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import './NotFoundPage.css';


class NotFoundPage extends React.Component {


	/**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {
        hideSplashScreen();
    }


    render() {
        return (
            <MuiThemeProvider>
                <div style = {{ width: "100vw", height: "100vh", backgroundColor: "#ffffff" }} >
                    <div className = "error-page-wrap">
                        <article className = "error-page gradient">
                            <div style = {{ margin: "-90px 0px 0px" }} >
                                <hgroup>
                                    <h1>404</h1>
                                    <h2 style = {{ color: "#ffffff" }} >Can you even type?</h2>
                                </hgroup>
                                <Link to = "/home" style = {{ color: "#ffffff" }} >Back to Home</Link>
                            </div>
                        </article>
                    </div>
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
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(NotFoundPage);
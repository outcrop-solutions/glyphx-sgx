import React from 'react';
import { connect } from 'react-redux';
import { hideSplashScreen } from '../LoadMaskHelper.js';
// import { makeServerCall } from '../ServerCallHelper.js';
import { withRouter } from 'react-router-dom';
import 'rc-tooltip/assets/bootstrap.css';
import '../css/General.css';
import '../css/Views.css';


class EmptyDiv extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            loadMask: false,
        }
    }
    
    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
        hideSplashScreen();
        if(this.props.vars){
            console.log(this.props.vars);
            this.checkSharedHash(this.props.vars);
        }
    }

    render() {
        return (
            <div style = {{
                height: "100%", 
                width: "100%", 
                fontSize: "35px", 
                backgroundColor: "white"}}
            >
            </div>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
// export const sharedLink = (falsy) => ({
//     type: 'SHARE_LINK_REDIRECT',
//     falsy,
// });

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
export default withRouter(connect(mapStateToProps,null,null)(EmptyDiv));
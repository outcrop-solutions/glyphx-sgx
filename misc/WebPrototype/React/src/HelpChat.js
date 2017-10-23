import React from 'react';
import { connect } from 'react-redux';
import './General.css';


class HelpChat extends React.Component {

    render(){
        return (
            "Chat Here"
      );
    }
};


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(HelpChat);
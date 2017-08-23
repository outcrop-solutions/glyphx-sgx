import React from 'react';
import { connect } from 'react-redux';
import { Flex } from 'react-flex-material';
import { Card, CardText } from 'material-ui/Card';
import { withRouter } from 'react-router-dom'
import injectTapEventPlugin from 'react-tap-event-plugin';
import FilterSideBar from './FilterSideBar.js';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import CircularProgress from 'material-ui/CircularProgress';
import StatisticModal from './StatisticModal.js'
import TopNavBar from './TopNavBar.js';
import FloatingToggleButtons from './FloatingToggleButtons.js';
import GlyphLegend from './GlyphLegend.js';
import RaisedButton from 'material-ui/RaisedButton';
import FontIcon from 'material-ui/FontIcon';
import './General.css';
import 'font-awesome/css/font-awesome.min.css';


class UserFeed extends React.Component {
    

    render() {
        return (
            <Flex layout = "column">
                <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                    >
                        User Feed
                    </CardText>
                </Card>
                
            </Flex>
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
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(UserFeed);
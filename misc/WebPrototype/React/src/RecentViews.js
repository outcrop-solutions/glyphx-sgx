import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import { Card, CardText } from 'material-ui/Card';
import { makeServerCall } from './ServerCallHelper.js';
import Flexbox from 'flexbox-react';
import ComponentLoadMask from './ComponentLoadMask.js';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';
import './General.css';


/**
* -ADCMT
*/
class RecentViews extends React.Component {

    state = {
        loadMask: true,
		recents: []
    }


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

        var context = this;


        // Make server call to grab recent views
        makeServerCall("fetchRecentViews",
            function (responseText) { 
				if (typeof responseText == 'string' && responseText != null && responseText != "") {
					responseText = JSON.parse(responseText);
					// Post the new data to the state and hide the window load-mask
					context.setState({ loadMask: false, recents: responseText.reverse()});
				}
				else {
					context.setState({ loadMask: false });
				}
                
				context.props.dispatch( setTimer(new Date().getTime()) );
                
           }
        );
	}
    

    render() {
        var context = this;

        var newList = this.state.recents.slice(0, 5);

        var recentViews = newList.map( function(view) {
			if (typeof view[1] == 'string' && !isNaN(view[1])) {
				view[1] = parseInt(view[1]);
			}
				
			var viewTimeStamp = new Date(view[1]);

			var viewDate =  (viewTimeStamp.getMonth() + 1) + "/" + viewTimeStamp.getDate() + "/" + viewTimeStamp.getFullYear();

			var viewTime;
			if (viewTimeStamp.getHours() > 12) {
				viewTime =  viewTimeStamp.getHours() - 12 + ":" + (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : viewTimeStamp.getMinutes()) + 'pm';
            }
            else if (viewTimeStamp.getHours() == 12) {
				viewTime =  12 + ":" + (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : viewTimeStamp.getMinutes()) + 'pm';
            }
			else {
				viewTime = viewTimeStamp.getHours() + ":" + (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : viewTimeStamp.getMinutes()) + 'am';
            }
			
            return (
                <Card 
                    className = "inherit-hover noselect" 
                    containerStyle = {{ padding: "0px", borderRadius: "5px", width: "100%" }} 
                    style = {{ 
                        backgroundColor: context.props.settings.colors.general.lightBubble, 
                        height: "35px", 
                        width: "100%", 
                        borderRadius: "5px", 
                        marginTop: (view === newList[0] ? "0px" : "5px") 
                    }} 
                    key = { view } 
                >
                    <CardText
                        style = {{ padding: "7px", borderRadius: "5px", width: "100%" }}
                        onClick = { () => context.props.loadRecentView(view) }
                    >

                        <Flexbox flexDirection = "row" minWidth = "100%" >
                            <Flexbox style = {{ width: "100%", whiteSpace: "nowrap", overflow: "hidden", 
                            fontSize: "16px", letterSpacing: "0.05em" }} > 
                                <Tooltip
                                    placement = 'left'
                                    mouseEnterDelay = { 0.5 }
                                    mouseLeaveDelay = { 0.15 }
                                    destroyTooltipOnHide = { false }
                                    trigger = { Object.keys( {hover: 1} ) }
                                    overlay = { 
                                        <div> 
                                            { view[0] }
                                        </div> 
                                    }
                                >
                                    <div> 
                                        { view[0] }
                                    </div> 
                                </Tooltip>
                            </Flexbox>

                            <Flexbox style = {{ width: "40%" }} > 
                                <div style = {{ width: "100%", textAlign: "right" }} >
                                    { viewTime }
                                </div>
                            </Flexbox>

                            <Flexbox style = {{ width: "40%" }} > 
                                <div style = {{ width: "100%", textAlign: "right", margin: "0px 1px 0px" }} >
                                    { viewDate }
                                </div>
                            </Flexbox>  
                        </Flexbox>

                    </CardText>
                </Card>
            )
        });

        return (
            <div>
                <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, marginBottom: "3px", paddingBottom: "4px", borderRadius: "2px" }} >
                    <div 
                        className = "noselect"
                        style = {{ 
                            color: this.props.settings.colors.overviewButtonsColor.text, 
                            margin: "0 auto",
                            width: "70px", 
                            paddingTop: "4px",
                            fontSize: "19px",
                            letterSpacing: "0.23em",
                            textTransform: "uppercase"
                        }}
                    > 
                        Views
                    </div>
                </div>

                <div style = {{ height: "250px", display: (this.state.loadMask ? "" : "none") }} >
                    <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                </div>

                <div style = {{ padding: "7px", marginBottom: "5px", display: (this.state.loadMask ? "none" : "") }} >
                    {this.state.recents.length == 0 ? 
                    <div style = {{ margin: "30px 0px 15px 0px", fontSize: "18px", textAlign: "center" }}> No Recent Views </div> 
                    : recentViews}
                </div>
            </div>
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

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});


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
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(RecentViews));
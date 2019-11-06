import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import { Card, CardText } from 'material-ui/Card';
import { makeServerCall } from './ServerCallHelper.js';
import Flexbox from 'flexbox-react';
import ComponentLoadMask from './ComponentLoadMask.js';
import Tooltip from 'rc-tooltip';
import SavedViews from './SavedViews.js';
import 'rc-tooltip/assets/bootstrap.css';
import './css/General.css';
import './css/Views.css';


/**
* -ADCMT
*/
class ViewsTab extends React.Component {

    state = {
        loadMask: true,
        recents: [],
    }


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

        var context = this;


        // Make server call to grab recent views
        makeServerCall("fetchRecentViews",
            function (responseText) { 
				if (typeof responseText === 'string' && responseText !== null && responseText !== "") {
                    responseText = JSON.parse(responseText);
                    console.log(responseText);
                    // Post the new data to the state and hide the window load-mask
                    if(responseText.isUserLoggedIn !== false){
                        context.setState({ loadMask: false, recents: responseText.reverse()});
                    }
				}
				else {
					context.setState({ loadMask: false });
				}
                
				context.props.dispatch( setTimer(new Date().getTime()) );
                
           }
        );

        // makeServerCall("fetchRecentViewsEC2",
        //     function (responseText) { 
		// 		if (typeof responseText === 'string' && responseText !== null && responseText !== "") {
        // 			responseText = JSON.parse(responseText);
        //          console.log(responseText);
		// 			// Post the new data to the state and hide the window load-mask
		// 			context.setState({ loadMask: false, recents: responseText.reverse()});
		// 		}
		// 		else {
		// 			context.setState({ loadMask: false });
		// 		}
                
		// 		context.props.dispatch( setTimer(new Date().getTime()) );
                
        //    },
        //    {
        //        post: true,
        //        data: {
        //            fetch: true
        //        }
        //    }
        // );
        //On component mount, open Views Tab
        document.getElementById("tab-defaultOpen").click();
    }

    /**
	 * Updates tutorial stage
     * @param e: current target event
     * @param viewType: tab class to switch to
	 */
    
    openViewsTab(e, viewType) {
        // Declare all variables
        let i, tabcontent, tablinks;
      
        // Get all elements with class="tabcontent" and hide them
        tabcontent = document.getElementsByClassName("tabcontent");

        for (i = 0; i < tabcontent.length; i++) {
          tabcontent[i].style.display = "none";
        }
      
        // Get all elements with class="tablinks" and remove the class "active"
        tablinks = document.getElementsByClassName("tablinks");
        for (i = 0; i < tablinks.length; i++) {
          tablinks[i].className = tablinks[i].className.replace("active", "");
        }
      
        // Show the current tab, and add an "active" class to the button that opened the tab
        document.getElementById(viewType).style.display = "block";
        // console.log('checking', document.getElementById(viewType).display);
        e.currentTarget.className += " active";
      }
    

    render() {
        var context = this;

        var newList = this.state.recents.slice(0, 5);

        var recentViews = newList.map( function(view) {
			if (typeof view[1] === 'string' && !isNaN(view[1])) {
                view[1] = parseInt(view[1]);
			}
				
			var viewTimeStamp = new Date(view[1]);

			var viewDate =  (viewTimeStamp.getMonth() + 1) + "/" + viewTimeStamp.getDate() + "/" + viewTimeStamp.getFullYear();

			var viewTime;
			if (viewTimeStamp.getHours() > 12) {
				viewTime =  viewTimeStamp.getHours() - 12 + ":" + (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : viewTimeStamp.getMinutes()) + 'pm';
            }
            else if (viewTimeStamp.getHours() === 12) {
				viewTime =  12 + ":" + (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : viewTimeStamp.getMinutes()) + 'pm';
            }
			else {
				viewTime = viewTimeStamp.getHours() + ":" + (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : viewTimeStamp.getMinutes()) + 'am';
            }
			
            return (
                <div key = {`${view[0]}${view[1]}`}>
                    <Card 
                        className = "inherit-hover noselect hover-recent-select" 
                        containerStyle = {{ padding: "0px", width: "100%" }} 
                        style = {{ 
                            backgroundColor: "#e6e7e8", 
                            height: "50px", 
                            width: "100%", 
                            boxShadow: "0px 0px 0px",
                            padding: "10px",
                            // marginTop: (view === newList[0] ? "0px" : "5px"),
                        }} 
                        key = { view } 
                    >
                        <CardText
                        /* className="hover-recent-select" */
                            style = {{ padding: "7px", width: "100%" }}
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
                                    
                                <div style = {{ width: "100%", textAlign: "right" }} >
                                    { viewTime } &nbsp; &nbsp; { viewDate }
                                </div>
                            
                            </Flexbox>

                        </CardText>
                    </Card>
                    <div style={{height: "1px", borderBottom: "1px solid #9ea3af", margin: "auto"}}>

                    </div>
                </div>
            )
        });

        return (
            <div style = {{height: "247px", marginBottom: "4px", display: "inline-table"}}>
                <div style={{
                    height: "50px",
                    width: "25vw",
                    backgroundColor: "#0c1836"
                }}>
                    <h2
                        className='noselect'
                        style={{
                            // color: this.props.settings.colors.overviewButtonsColor.text,
                            width: "162px",
                            height: "19px",
                            fontFamily: "ITCFranklinGothicStd-Demi",
                            fontSize: "18px",
                            letterSpacing: "0.92px",
                            color: "white",
                            margin: "0px",
                            fontSize: "22px",
                            textTransform: "uppercase",
                            paddingLeft: "18px",
                            lineHeight: "52px"
                        }}
                        >My Views
                    </h2>
                </div>

                {/* tab content */}
                <div className="tab">
                    
                    <button className="tablinks" id="tab-defaultOpen" 
                    onClick= {(e) => this.openViewsTab(e,'Recent')}
                    style = {{padding: "10px 52.3320px 10px 52.3320px", width: "50%", marginLeft: "7px"}}>
                        Recent
                    </button>
                    
                    <button className="tablinks"
                    onClick= {(e) =>  this.openViewsTab(e, 'Saved')}
                    style = {{padding: "10px 52.3320px 10px 52.3320px", width: "50%", marginRight: "7px"}}>
                        Saved
                    </button>
                </div>

                <div style = {{ 
                    height: "70%", 
                    display: (this.state.loadMask ? "" : "none") }} >
                    <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                </div>

                {/* Recent Views Tab */}
                <div id="Recent" className="tabcontent">
                    <div className = "customScroll" style = {{ height: "253px", padding: "0px 7px 7px 7px", overflow: "auto", display: (this.state.loadMask ? "none" : "") }} >
                        {this.state.recents.length === 0 ? 
                        <div style = {{ margin: "30px 0px 15px 0px", fontSize: "18px", textAlign: "center" }}> No Recent Views </div> 
                        : recentViews}
                    </div>
                </div>
                {/* Saved Views Tab */}
                <div id="Saved" className="tabcontent customScroll" style={{ overflow: "auto", paddingTop: "7px", backgroundColor: "#e6e7e8"}}>
                    <SavedViews/> 
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
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(ViewsTab));
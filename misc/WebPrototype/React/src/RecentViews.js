import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom'
import { Card, CardText } from 'material-ui/Card';
import Flexbox from 'flexbox-react';
import ComponentLoadMask from './ComponentLoadMask.js';
import './General.css';


/**
* -ADCMT
*/
class RecentViews extends React.Component {

    state = {
        loadMask: true,
        recents:    [["Unsaved Session", "2:19pm", "8/15/2017"], ["Some View", "1:15pm", "8/15/2017"], ["Glyph 123", "1:01pm", "8/15/2017"], 
                    ["Look At Me", "11:19am", "8/15/2017"], ["Test Glyph", "11:59am", "8/15/2017"], ["Who Am I", "10:30am", "8/15/2017"]]
    }


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {

        var context = this;


        // Make server call to grab recent views
        //makeServerCall(something here),
        //    function (responseText) { 

                // Post the new data to the state and hide the window load-mask
                context.setState({ loadMask: false });
        //   }
        //);
	}
    

    render() {
        var context = this;

        var recentViews = this.state.recents.map( function(view) {
            return (
                <Card 
                    className = "inherit-hover noselect" 
                    containerStyle = {{ padding: "0px", borderRadius: "5px", width: "100%" }} 
                    style = {{ 
                        backgroundColor: context.props.settings.colors.general.lightBubble, 
                        height: "35px", 
                        width: "100%", 
                        borderRadius: "5px", 
                        marginTop: (view === context.state.recents[0] ? "0px" : "5px") 
                    }} 
                    key = { view } 
                >
                    <CardText
                        style = {{ padding: "7px", borderRadius: "5px", width: "100%" }}
                        //onClick = { () => context.props.history.push("/glyph-viewer") }
                    >

                        <Flexbox flexDirection = "row" minWidth = "100%" >
                            <Flexbox style = {{ width: "100%" }} > 
                                { view[0] }
                            </Flexbox>

                            <Flexbox style = {{ width: "60%" }} > 
                                { view[1] }
                            </Flexbox>

                            <Flexbox style = {{ width: "60%" }} > 
                                { view[2] }
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
                            width: "111px", 
                            paddingTop: "4px",
                            fontSize: "18px"
                        }}
                    > 
                        Recent Views
                    </div>
                </div>

                <div style = {{ height: "250px", display: (this.state.loadMask ? "" : "none") }} >
                    <ComponentLoadMask color = { this.props.settings.colors.buttons.general } />
                </div>

                <div style = {{ padding: "7px", display: (this.state.loadMask ? "none" : "") }} >
                    {recentViews}
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
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(RecentViews));
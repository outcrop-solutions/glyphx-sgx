import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';
import Flexbox from 'flexbox-react';
import './General.css';


class TutorialWindow extends React.Component {
	
	render(){
		return(
			<div 
                style = {{ 
                    width: "29%", 
                    left: (this.props.tutorialStage === 3 || this.props.tutorialStage === 8 ? "15.1%" : (this.props.tutorialStage > 3 ? "84.9%" : "50%")), 
                    marginLeft: "-14.5%", 
                    height: "auto", 
                    padding: "7px", 
                    borderRadius: "5px", 
                    backgroundColor: this.props.settings.colors.buttons.general, 
                    zIndex: "400", 
                    position: "fixed", 
                    marginTop: "100px" 
                }} 
            >
                <div style = {{ height: "100%", width: "100%", backgroundColor: "white", borderRadius: "5px", padding: "7px 7px 0px" }} >
                    <div onClick = { () => this.props.updateStage("done") } style = {{ width: "100%", textAlign: "right", display: (this.props.tutorialStage === 0? "none" : "") }} > <i className = "fa fa-times" style = {{ fontSize: "18px" }} /> </div>
                    { this.props.tutorialStage === 0 ? 
                        <div>
                            <div style = {{ fontSize: "22px", textAlign: "center", lineHeight: "30px" }} > Would you like to take a quick walkthrough of the site? </div>
                            <br />
                            <RaisedButton 
                                label = "Yes"
                                style = {{
                                    width: "100%",
                                    margin: "10px auto",
                                }}
                                buttonStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                    backgroundColor: this.props.settings.colors.buttons.general
                                }} 
                                labelStyle = {{
                                    fontSize: '14px',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    fontFamily: "Arial Black, Gadget, sans-serif",
                                    margin: "0px 0px 0px -3px",
                                    paddingLeft: "0px",
                                    paddingRight: "0px"
                                }}
                                overlayStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                }}
                                onClick = { () => this.props.updateStage(this.props.tutorialStage + 1) }
                                primary = {true } 
                            />
                            <RaisedButton 
                                label = "No"
                                style = {{
                                    width: "100%",
                                    margin: "0px auto -11px",
                                }}
                                buttonStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                    backgroundColor: "grey"
                                }} 
                                labelStyle = {{
                                    fontSize: '14px',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    fontFamily: "Arial Black, Gadget, sans-serif",
                                    margin: "0px 0px 0px -3px",
                                    paddingLeft: "0px",
                                    paddingRight: "0px"
                                }}
                                overlayStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                }}
                                onClick = { () => this.props.updateStage("done") }
                                primary = {true } 
                            />
                        </div>
                        : 
                        (this.props.tutorialStage === 1 ?
                             <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > Here is the User Feed where you can share saved views with different teams your admin has added you to. Use the <u><i>drop down</i></u> at the top to select a team and the <u><i>cube icon</i></u> located in the chat toolbar to share a view. </div>
                             :
                             (this.props.tutorialStage === 2 ?
                                <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > The <u><i>Recent Views</i></u> up top can be used to quickly return to a view you've recently visited. You can also return to your latest unsaved session if you've accidentally closed it. <br /><br />The <u><i>Announcements</i></u> section allows us to update you on our progress as well as get your input on which features you would like to see first through polls. </div>
                                :
                                (this.props.tutorialStage === 3 ?
                                    <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > The Views Manager is used to access your views. Your admin assigns your access to the different levels of the funnel. Clicking a funnel category will take you down the pipeline of narrowing down a view. <br /><br />The 'My Views' button below the funnel will take you to view your <u><i>Saved Views</i></u>, <u><i>Shared Views</i></u>, and <u><i>Custom Views</i></u></div>
                                    :
                                    (this.props.tutorialStage === 4 ?
                                        <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > This is the help window where you can report a bug, request a feature, view the user manual for more guidance, and read the FAQ. </div>
                                        :
                                        (this.props.tutorialStage === 5 ?
                                            <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > This is the system settings where you can personalize your experience through customizations such as color themes. </div>
                                            :
                                            (this.props.tutorialStage === 6 ?
                                                <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > This is the alerts window which will notify you if someone has shared a view with you, or if someone made a post in a team you were added to. </div>
                                                :
                                                (this.props.tutorialStage === 7 ?
                                                    <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > This is the user dropdown where you can logout or change account related settings. </div>
                                                    :
                                                    (this.props.tutorialStage === 8 ?
                                                        <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > Clicking on the GlyphEd logo will return you to the home page if you were to navigate away. </div>
                                                        :
                                                        <div style = {{ fontSize: "18px", textAlign: "center", lineHeight: "25px" }} > This chat can be used to interact with members of your teams or to contact us for emergency support. </div>      
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    }

                    <br /> 

                    <Flexbox flexDirection = "row" style = {{ marginBottom: "-2px" }} >
                        <Flexbox style = {{ width: "50%" }} >
                            <RaisedButton 
                                label = "Back"
                                style = {{
                                    width: "100%",
                                    margin: "10px auto",
                                    marginRight: "10px",
                                    display: (this.props.tutorialStage > 0 ? "" : "none")
                                }}
                                buttonStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                    backgroundColor: "grey"
                                }} 
                                labelStyle = {{
                                    fontSize: '14px',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    fontFamily: "Arial Black, Gadget, sans-serif",
                                    margin: "0px 0px 0px -3px",
                                    paddingLeft: "0px",
                                    paddingRight: "0px"
                                }}
                                overlayStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                }}
                                onClick = { () => this.props.updateStage(this.props.tutorialStage - 1) }
                                primary = {true } 
                            />
                        </Flexbox>

                        <Flexbox style = {{ width: "50%" }} >
                            <RaisedButton 
                                label = {this.props.tutorialStage === 9 ? "Finish" : "Next"}
                                style = {{
                                    width: "100%",
                                    margin: "10px auto",
                                    marginLeft: "10px",
                                    display: (this.props.tutorialStage > 0 ? "" : "none")
                                }}
                                buttonStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                    backgroundColor: this.props.settings.colors.buttons.general
                                }} 
                                labelStyle = {{
                                    fontSize: '14px',
                                    color: this.props.settings.colors.overviewButtonsColor.text,
                                    fontFamily: "Arial Black, Gadget, sans-serif",
                                    margin: "0px 0px 0px -3px",
                                    paddingLeft: "0px",
                                    paddingRight: "0px"
                                }}
                                overlayStyle = {{
                                    height: '50px',
                                    lineHeight: '50px',
                                }}
                                onClick = { () => this.props.updateStage(this.props.tutorialStage + 1) }
                                primary = {true } 
                            />
                        </Flexbox>
                    </Flexbox>

                </div>
            </div>
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
  }
}


/**
 * Connects the Announcements Dialog component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(TutorialWindow);
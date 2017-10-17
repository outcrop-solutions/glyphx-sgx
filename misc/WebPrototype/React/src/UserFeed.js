import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
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
import Select from 'react-select';
import ReactQuill from 'react-quill';
import theme from 'react-quill/dist/quill.snow.css';
import Divider from 'material-ui/Divider';
import './General.css';
import 'font-awesome/css/font-awesome.min.css';




class UserFeed extends React.Component {

    state = {
        teamSelectValue: "University of Notre Dame"
    }

    onTeamSelectChange = (value) => {
        this.setState({
            teamSelectValue: value
        });
        console.log(value);
    };
    

    render() {

        var teamList = ["University of Notre Dame", "Marketing & Recruiting", "Admissions", "Financial Aid", "Custom Team 1"];
        teamList = teamList.map(function(value){
			return(
                {
                    label: value, value: value
                }
            );
		});

        var postList = [["Mark Sloan", "./Res/Img/mark.png", "CEO", "Lunch is on me everyone!"], 
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "Marwane & Aditya are the best! Sorry for messing with your desks!"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "I think the office is haunted, the 'Ed' keeping moving by itself..."],
                        ["Mark Sloan", "./Res/Img/mark.png", "CEO", "I need to make a care package for my son, daily pictures of my dog to make sure he doesnt forget."],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "Day 56, still waiting for a day to pass without someone making awkward eye contact through the window."],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "1"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "2"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "3"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "4"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "5"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "3D Dev", "6"],
                        ];

        var context = this;

        var posts = postList.map( function(post) {
            return (
                <Card containerStyle = {{ padding: "0px", borderRadius: "10px" }} style = {{ backgroundColor: context.props.settings.colors.general.lightBubble, borderRadius: "10px", paddingBottom: "2px", marginTop: (post === postList[0] ? "0px" : "7px") }} key = { post } >
                    <CardText
                        style = {{
                            padding: "5px",
                            borderRadius: "10px"
                        }}
                    >
                        <Flexbox flexDirection="row" minWidth="100%" >

                                <img src = { post[1] } className = "img-circle noselect" style = {{ marginRight: "10px" }} alt = { post[0] } draggable = { false } />
 
                            <Flexbox flexDirection="column" style = {{ width: "100%" }} >
                                <Flexbox style = {{ height: "100%" }} > 
                                    {post[3]}
                                </Flexbox>

                                <Divider style = {{ marginBottom: "5px" }} />

                                <Flexbox flexDirection = "row" minWidth = "100%" className = "noselect" >

                                        <i className = "fa fa-comments" style = {{ fontSize: "17px", marginRight: "15px" }} />
                                        <i className = "fa fa-thumbs-up" style = {{ fontSize: "17px",marginRight: "25px" }} />
                                        <div> 9/5/2017 </div>

                                    <Flexbox style = {{ width: "100%" }} > 
                                        <div style = {{ width: "100%", textAlign: "right" }} > 
                                            - { post[0] }
                                        </div>
                                    </Flexbox>
                                </Flexbox>
                            </Flexbox>
                        </Flexbox>
                        
                        
                    </CardText>
                </Card>
            )
        });

        return (
            <Flexbox flexDirection="column" style = {{ height: "100%", minHeight: "0" }}  > 
                <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, borderRadius: "2px", marginBottom: "3px", paddingBottom: "4px" }} >
                    <div 
                        style = {{ 
                            color: this.props.settings.colors.overviewButtonsColor.text, 
                            margin: "0 auto",
                            width: "85px",
                            paddingTop: "4px",
                            fontSize: "18px"
                        }}
                        className = "noselect"
                    > 
                        User Feed
                    </div>
                </div>
                <Flexbox flexDirection="column" style = {{ height: "100%", minHeight: "0", padding: "7px" }}  > 
                    <Select 
                        simpleValue
                        clearable = { false }
                        value = { this.state.teamSelectValue } 
                        options = { teamList } 
                        onChange = { this.onTeamSelectChange } 
                        style = {{
                            margin: "0px 0px 7px 0px",
                        }}
                        className = "noselect"
                    />
                
                    <Flexbox flexGrow = {1} style = {{ height: "100%", minHeight: "0" }} >
                        <div
                            style = {{
                                padding: "7px 7px 0px 5px",
                                height: "100%",
                                width: "100%",
                                borderRadius: "2px",
                                overflowY: "auto",
                            }}
                            className = "customScroll"
                        >
                            {posts}

                        </div>
                    </Flexbox>

                    <div id = "toolbar" style = {{ marginTop: "7px", backgroundColor: this.props.settings.colors.general.darkerBubble, borderTopRightRadius: "3px", borderTopLeftRadius: "3px" }} >
                        <Flexbox flexDirection="row" >
                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-italic"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-underline"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-strike"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-list" value = "bullet"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-align"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-align" value = "center"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-align" value = "right"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "ql-link"></button>
                            </Flexbox>

                            <Flexbox style = {{ width: "100%" }} > 
                                <button className = "fa fa-cube" onClick = { () => console.log("custom clicked") } ></button>
                            </Flexbox>

                        </Flexbox>
                    </div>

                    <ReactQuill 
                        theme = "snow"
                        modules = {{ toolbar: '#toolbar' }}
                    >
                        <div className = "my-editing-area" style = {{ height: "150px", overflow: "auto", backgroundColor: this.props.settings.colors.general.lighterBubble, borderBottomRightRadius: "3px", borderBottomLeftRadius: "3px" }} />
                    </ReactQuill>
                </Flexbox>

            </Flexbox>
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
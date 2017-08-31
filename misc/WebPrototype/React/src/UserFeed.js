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
import Select from 'react-select';
import ReactQuill from 'react-quill';
import theme from 'react-quill/dist/quill.snow.css';
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

        var postList = [["Mark Sloan", "./Res/Img/mark.png", "Lunch is on me everyone!"], 
                        ["Bradley Lewis", "./Res/Img/brad.png", "Marwane & Aditya are the best! Sorry for messing with your desks!"],
                        ["Bradley Lewis", "./Res/Img/brad.png", "I think the office is haunted, the 'Ed' keeping moving by itself..."],
                        ["Mark Sloan", "./Res/Img/mark.png", "I need to make a care package for my son, daily pictures of my dog to make sure he doesnt forget."],
                        ["Bradley Lewis", "./Res/Img/brad.png", "Day 56, still waiting for a day to pass without someone making awkward eye contact through the window."]];

        var posts = postList.map( function(post) {
            return (
                <Card containerStyle = {{ padding: "0px", borderRadius: "10px" }} style = {{ borderRadius: "10px" }} key = { post } >
                    <CardText
                        style = {{
                            padding: "5px",
                            marginTop: (post === postList[0] ? "0px" : "7px"),
                            backgroundColor: "#f5f5ff",
                            borderRadius: "10px"
                        }}
                    >
                        <Flex layout = "column">
                            <Flex layout = "row" >
                                <Flex flex = "20" >
                                    <div style = {{ width: "50px", margin: "0 auto" }} >
                                        <img src = { post[1] } className = "img-circle" style = {{ margin: "0 auto" }} alt = { post[0] }/>
                                    </div>
                                </Flex>

                                <Flex flex = "80" style = {{ padding: "7px 0px 0px 7px" }}>
                                    <div style = {{ marginBottom: "3px" }} > {post[2]} </div>
                                    <Flex layout = "row" >
                                        <Flex flex = "15" >
                                            <div style = {{ margin: "0 auto", marginTop: "2px", width: "22px" }} >
                                                <i className = "fa fa-comments" style = {{ fontSize: "17px" }} />
                                            </div>
                                        </Flex>

                                        <Flex flex = "15"  style = {{ paddingLeft: "5px", marginTop: "2px" }} >
                                            <i className = "fa fa-thumbs-up" style = {{ fontSize: "17px" }} />
                                        </Flex>
                                        <Flex flex = "70" >
                                            <div style = {{ width: "100%", textAlign: "right", paddingRight: "5px" }} >
                                                - { post[0] }
                                            </div>
                                        </Flex>
                                    </Flex>
                                </Flex>
                            </Flex>

                            
                        </Flex>
                    </CardText>
                </Card>
            )
        });


        return (
            <Flex layout = "column" style = {{ height: "100%" }} >
                
                <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, marginBottom: "3px", paddingBottom: "4px" }} >
                    <div 
                        style = {{ 
                            color: this.props.settings.colors.overviewButtonsColor.text, 
                            margin: "0 auto",
                            width: "85px", 
                            paddingTop: "4px",
                            fontSize: "18px",
                            fontWeight: "normal"
                        }} 
                    > 
                        User Feed 
                    </div>
                </div>

                <Flex flex>
                    
                    <Flex 
                        layout = "column" 
                        style = {{ 
                            height: "100%", 
                            backgroundColor: this.props.settings.colors.homePageColors.subBackground, 
                            padding: "7px",
                            borderRadius: "3px"
                        }} 
                    >

                        <Select 
                                simpleValue
                                clearable = { false }
                                value = { this.state.teamSelectValue } 
                                options = { teamList } 
                                onChange = { this.onTeamSelectChange } 
                                style = {{
                                    margin: "0px 0px 7px 0px",
                                }}
                            />

                        <Flex flex style = {{ overflow: "auto", padding: "0px 10px" }} >
                            {posts}
                        </Flex>

                        <div id = "toolbar" style = {{ marginTop: "7px", backgroundColor: "#ddddf8" }} >
                            <Flex layout = "row">
                                <button className = "ql-italic"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "ql-underline"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "ql-strike"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />
                                
                                <button className = "ql-list" value = "bullet"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "ql-align"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "ql-align" value = "center"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "ql-align" value = "right"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "ql-link"></button>
                                <Flex divider />
                                <Flex divider />
                                <Flex divider />

                                <button className = "fa fa-cube" onClick = { () => console.log("custom clicked") } ></button>
                            </Flex>
                        </div>

                        <ReactQuill 
                            theme = "snow"
                            modules = {{ toolbar: '#toolbar' }}
                        >
                            <div className = "my-editing-area" style = {{ height: "120px", overflow: "auto", backgroundColor: "#f5f5ff" }} />
                        </ReactQuill>
                    </Flex>
                </Flex>

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
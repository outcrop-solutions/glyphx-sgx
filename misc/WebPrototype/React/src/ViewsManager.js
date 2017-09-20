import React from 'react';
import { connect } from 'react-redux';
import { Step, Stepper, StepLabel } from 'material-ui/Stepper';
import { Card, CardText } from 'material-ui/Card';
import RaisedButton from 'material-ui/RaisedButton';
import MenuItem from 'material-ui/MenuItem';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import DropDownMenu from 'material-ui/DropDownMenu';
import Collapsible from 'react-collapsible';
import ExpandTransition from 'material-ui/internal/ExpandTransition';
import FontIcon from 'material-ui/FontIcon';
import AllViewsModal from './AllViewsModal.js';
import MyViews from './MyViews.js';
import './ViewsManager.css';
import './General.css';


class ViewsManager extends React.Component {

    state = {
        loading: false,
        launchReady: false,
        stepIndex: 0,
        type: "Funnel",
        selectionType: "",
        flipped: false,
        clicked: false
    };

    dummyAsync = (cb) => {
        this.setState({loading: true}, () => {
        this.asyncTimer = setTimeout(cb, 500);
        });
    };


    handleNext = (type) => {
        const {stepIndex} = this.state;
        if (!this.state.loading) {
            console.log(type);
            if ( stepIndex >= 1 ) {
                this.dummyAsync(() => this.setState({
                    stepIndex: 0,
                    loading: false,
                }));
            }
            else {
                this.dummyAsync(() => {this.setState({
                    stepIndex: stepIndex + 1,
                    loading: false,
                    type: type
                    });
                });
            }
        }
    };

    handlePrev = () => {
        if (this.state.flipped) {
            this.flip();
        }
        const {stepIndex} = this.state;
        this.setState({ type: "Funnel"});
        if (!this.state.loading) {
            this.dummyAsync(() => this.setState({
                loading: false,
                stepIndex: stepIndex - 1,
            }));
        }
    };


    flip(type) {
        this.setState({
            flipped: !this.state.flipped,
            clicked: true,
        });
        var context = this;
        setTimeout(function () {
            context.setState({
                type: type
            });
        }, 200);
      
    }

    handleViewSelect = (type) => {
        this.props.dispatch(editModalDisplay(true));
        this.setState({
            selectionType: type
        });
    }


    render() {
        const contentStyle = {margin: '0 16px'};
        var context = this;

        var mandrList = ["Prospects", "High School Profiles", "Prospects & High Schools (Geospatial)"];

        var marketingAndRecruiting = mandrList.map( function(title) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ width: "80%", minWidth: "331px", margin: "0 auto", height: "35px", backgroundColor: context.props.settings.colors.homePageColors.funnelTopBody }} key = { title } >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                        className = "funnel-top-body noselect"
                        onClick = { context.handleViewSelect.bind(context, title) }
                    >
                        {title}
                    </CardText>
                </Card>
            )
        });

        var admList = ["Global Admissions", "Applicants", "Applicants by High School", "Review Committee", "Current Year RC with Prior Year Admits"];

        var admissions = admList.map( function(title) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ width: "80%", minWidth: "331px", margin: "0 auto", height: "35px", backgroundColor: context.props.settings.colors.homePageColors.funnelMiddleBody }} key = { title } >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                        className = "funnel-middle-body noselect"
                        onClick = { context.handleViewSelect.bind(context, title) }
                    >
                        {title}
                    </CardText>
                </Card>
            )
        });

        var faList = ["FA Package Summaries", "FA Package Summaries (Grid)"];

        var financialAid = faList.map( function(title) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ width: "80%", minWidth: "331px", margin: "0 auto", height: "35px", backgroundColor: context.props.settings.colors.homePageColors.funnelBottomBody }} key = { title } >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                        className = "funnel-bottom-body noselect"
                        onClick = { context.handleViewSelect.bind(context, title) }
                    >
                        {title}
                    </CardText>
                </Card>
            )
        });

        var flippedCSS = (this.state.flipped ? " Card-Back-Flip" : " Card-Front-Flip");
        if (!this.state.clicked) flippedCSS =  "";

        var backButton = <RaisedButton
                                label = "Back"
                                onClick = { () => this.flip("Funnel") }
                                style = {{ 
                                    width: "80%", 
                                    minWidth: "331px", 
                                    margin: "0 auto",
                                    display: (this.state.type === "MarketingAndRecruiting" || this.state.type === "Admissions" || this.state.type === "FinancialAid" ? "" : "none")
                                }}
                                buttonStyle = {{
                                    height: '30px',
                                    lineHeight: '30px',
                                    backgroundColor: "#818181"
                                }} 
                                labelStyle = {{
                                    fontSize: '13px',
                                    color: "#ffffff"
                                }}
                                overlayStyle = {{
                                    height: '30px',
                                    lineHeight: '30px'
                                }}
                            />;

        return(
            <div>
                <div style = {{ backgroundColor: this.props.settings.colors.homePageColors.headerBackground, marginBottom: "3px", borderRadius: "2px", paddingBottom: "4px" }} >
                    <div 
                        style = {{ 
                            color: this.props.settings.colors.overviewButtonsColor.text, 
                            margin: "0 auto",
                            width: "125px", 
                            paddingTop: "4px",
                            fontSize: "18px",
                            fontWeight: "normal"
                        }}
                        className = "noselect"
                    > 
                        Views Manager
                    </div>
                </div>

                <Stepper activeStep = { this.state.stepIndex } style = {{ borderRadius: "2px", height: "60px", marginBottom: "10px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} >
                    <Step>
                        <StepLabel
                            icon = { 
                                (this.state.stepIndex === 0 ?
                                    <span className = "fa-stack fa-lg noselect">
                                        <i 
                                            className = "fa fa-circle fa-stack-2x" 
                                            style = {{     
                                                fontSize: "26px",
                                                color: this.props.settings.colors.overviewButtonsColor.background,
                                                margin: "5px 0px 0px 0px"
                                            }} 
                                        />
                                        <div className = "fa-stack-1x" style = {{ fontWeight: "bold", fontSize: "14px", color: this.props.settings.colors.overviewButtonsColor.text, margin: "-1px 0px 0px" }} > 1 </div>
                                    </span> 
                                :
                                    <span className = "fa-stack fa-lg noselect">
                                        <i 
                                            className = "fa fa-circle fa-stack-2x" 
                                            style = {{     
                                                fontSize: "23px",
                                                color: "#ffffff",
                                                margin: "6px 0px 0px 0px",
                                            }} 
                                        />
                                        <i 
                                            className = "fa fa-check-circle fa-stack-1x" 
                                            style = {{     
                                                fontSize: "27px",
                                                color: this.props.settings.colors.overviewButtonsColor.background,
                                                margin: "-1px 7px 0px 0px"
                                            }} 
                                        />
                                    </span>
                                )
                            } 
                    >
                        <label className = "noselect" style = {{ color: "#ffffff", fontWeight: "normal" }} > Select a Category </label>
                    </StepLabel>
                    </Step>
                    <Step>
                        <StepLabel
                            icon = {
                                <span className = "fa-stack fa-lg noselect">
                                    <i 
                                        className = "fa fa-circle fa-stack-2x" 
                                        style = {{     
                                            fontSize: "26px",
                                            color: (this.state.stepIndex === 1 ? this.props.settings.colors.overviewButtonsColor.background : "grey" ),
                                            margin: "5px 0px 0px 0px",
                                        }} 
                                    />
                                    <div className = "fa-stack-1x" style = {{ fontWeight: "bold", fontSize: "14px", color: ( this.state.stepIndex === 0 ? "#bdbdbd" : "#ffffff" ), margin: "-1px 0px 0px" }} > 2 </div>
                                </span> 
                            }
                            style = {{ marginRight: "8px" }}
                        >
                            <label className = "noselect" style = {{ color: ( this.state.stepIndex === 0 ? "#bdbdbd" : "#ffffff" ), fontWeight: "normal" }} > Select a View </label>
                        </StepLabel>
                    </Step>
                </Stepper>

                    <ExpandTransition loading = { this.state.loading } open = { true } style = {{ overflow: "auto", height: "100%" }} >
                        <div>
                            <div style = {{ height: "100%", backgroundColor: this.props.settings.colors.homePageColors.subBackground, borderRadius: "2px", }} >
                                <div 
                                    className = "Card" 
                                    style = {{ 
                                        marginBottom: "0px", 
                                        width: "100%", 
                                        display: (this.state.stepIndex === 0 ? "" : "none"),
                                        marginTop: (this.state.type === "MarketingAndRecruiting" || this.state.type === "Admissions" || this.state.type === "FinancialAid" ? "-250px" : "0px")
                                    }} 
                                >
                                    <div className = { "Card-Front" + flippedCSS } style = {{ width: "100%" }} >


                                        <div onClick = { () => this.flip("MarketingAndRecruiting") } className = "C-Size-1 noselect" >
                                            <div style = {{ margin: "-45px auto 0px", width: "216px", fontSize: '1rem', fontFamily: "Arial Black, Gadget, sans-serif", color: "#000000" }} >
                                                MARKETING &amp; RECRUITING
                                            </div>
                                        </div>

                                        <div onClick = { () => this.flip("Admissions") } className = "C-Size-2 noselect" >
                                            <div style = {{ margin: "-45px auto 0px", width: "100px", fontSize: '1rem', fontFamily: "Arial Black, Gadget, sans-serif", color: "#000000" }} >
                                                ADMISSIONS
                                            </div>
                                        </div>

                                        <div onClick = { () => this.flip("FinancialAid") } className = "C-Size-3 noselect" >
                                            <div style = {{ margin: "-45px auto 0px", width: "118px", fontSize: '1rem', fontFamily: "Arial Black, Gadget, sans-serif", color: "#000000" }} >
                                                FINANCIAL AID
                                            </div>
                                        </div>

                                    </div>
                                    <div className = { "Card-Back" + flippedCSS } style = {{ padding: "0px 20px" }} >
                                            {this.state.type === "MarketingAndRecruiting" ? <div style = {{ margin: "63px 0px 62px" }} > {marketingAndRecruiting} {backButton} </div> 
                                                : (this.state.type === "Admissions" ? <div style = {{ margin: "30px 0px 25px" }} > {admissions} {backButton} </div> 
                                                    : (this.state.type === "FinancialAid" ? <div style = {{ margin: "80px 0px 80px" }} > {financialAid} {backButton} </div> : null)) 
                                            }
                                            
                                    </div>
                                </div>
                                    
                                    
                                
                                {this.state.type === "My Views" ? <MyViews /> : null}

                                <FlatButton
                                    label = "Back"
                                    backgroundColor = "#dcdcdc"
                                    onClick = { this.handlePrev }
                                    style = {{ marginRight: 12 }}
                                    style = {{ display: (this.state.stepIndex === 1 && this.state.type === "My Views" ? "auto" : "none"), margin: "5px 0px 0px 11px", bottom: "10px" }}
                                />
                                <AllViewsModal type = { this.state.selectionType } />
                            </div>
                        

                            {this.state.stepIndex === 0 ? 
                                <RaisedButton 
                                    label = "My Views"
                                    style = {{
                                        width: "100%",
                                        margin: "10px auto",
                                    }}
                                    buttonStyle = {{
                                        height: '50px',
                                        lineHeight: '50px',
                                        backgroundColor: this.props.settings.colors.homePageColors.myViewsButton
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
                                    onClick = { this.handleNext.bind(this, "My Views") }
                                    primary = {true } 
                                />
                                : 
                                null
                            }
                        </div>
                    </ExpandTransition>
            </div>
        );
    }
};


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
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(ViewsManager);
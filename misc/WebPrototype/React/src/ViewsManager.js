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
        type: ""
    };

    componentDidMount() {
		var style = document.getElementById('themeStyles');
		
		if(style != null)
		{
			style.parentElement.removeChild(style);
			console.log('deleting old rules');
		}
		
		style = document.createElement("style");

		// WebKit hack
		style.appendChild(document.createTextNode(""));
		style.setAttribute('id','themeStyles');

		// Add the <style> element to the page
		document.head.appendChild(style);
		
		style.sheet.insertRule('.Collapsible__trigger { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; display: block; font-weight: 400; text-decoration: none; color: #333333; position: relative; border: 1px solid white; padding: 15px; background: ' + this.props.settings.colors.collapsibleColor.mainBackground + '; color: white; font-size: 1rem; }', 0);
		style.sheet.insertRule('.Collapsible__trigger.is-open { background: ' + this.props.settings.colors.collapsibleColor.mainCollapsed + '; }', 1);
		style.sheet.insertRule('.Collapsible__trigger:hover { background: ' + this.props.settings.colors.collapsibleColor.mainHover + '; }', 2);
		style.sheet.insertRule('.unpinned { font-size: 20px !important; transform: rotateZ(35deg) !important; color: ' + this.props.settings.colors.collapsibleColor.unpinned + '!important; }', 3);
		style.sheet.insertRule('.pinned { font-size: 20px !important; transform: rotateZ(0deg) !important; color: ' + this.props.settings.colors.collapsibleColor.pinned + '!important; }', 4);
		style.sheet.insertRule('.columnNameHeader { -moz-transition: all .1s ease-in; -o-transition: all .1s ease-in; -webkit-transition: all .1s ease-in; font-size: 1rem !important; padding: 10px !important; background: ' + this.props.settings.colors.collapsibleColor.subBackground + '!important; }', 5);
		style.sheet.insertRule('.columnNameHeader.is-open { background: ' + this.props.settings.colors.collapsibleColor.subCollapsed + '!important; }', 6);
		style.sheet.insertRule('.columnNameHeader:hover {  background: ' + this.props.settings.colors.collapsibleColor.subHover + '!important; }', 7);

        style.sheet.insertRule('.C-Size-1 { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelTop + ' !important; }', 8);
        style.sheet.insertRule('.C-Size-1:Hover { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelTopHover + ' !important; }', 9);
        style.sheet.insertRule('.C-Size-2 { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelMiddle + ' !important; }', 10);
        style.sheet.insertRule('.C-Size-2:Hover { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelMiddleHover + ' !important; }', 11);
        style.sheet.insertRule('.C-Size-3 { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelBottom + ' !important; }', 12);
        style.sheet.insertRule('.C-Size-3:Hover { border-top: 70px solid ' + this.props.settings.colors.homePageColors.funnelBottomHover + ' !important; }', 13);

        style.sheet.insertRule('.funnel-top-body:Hover { background-color: ' + this.props.settings.colors.homePageColors.funnelTopBody + ' !important; }', 14);
        style.sheet.insertRule('.funnel-middle-body:Hover { background-color: ' + this.props.settings.colors.homePageColors.funnelMiddleBody + ' !important; }', 15);
        style.sheet.insertRule('.funnel-bottom-body:Hover { background-color: ' + this.props.settings.colors.homePageColors.funnelBottomBody + ' !important; }', 16);

        style.sheet.insertRule('.inherit-hover:Hover { background-color: ' + this.props.settings.colors.homePageColors.hoverBackground + ' !important; }', 17);
    }

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
                    this.props.dispatch(editModalDisplay(true));
                });
            }
        }
    };

    handlePrev = () => {
    const {stepIndex} = this.state;
    if (!this.state.loading) {
      this.dummyAsync(() => this.setState({
        loading: false,
        stepIndex: stepIndex - 1,
      }));
    }
  };

    /**
     * This is called when the collapsibles are clicked.
     * @param {string} element- this is the name of the element that surrounds the collapsible. Used to scrollTo.
     */
    onCollapsibleTriggerClick = (element) => {
        var elem = this.refs[element];

        if ( !elem.state.isClosed ) {
            elem.closeCollapsible();
        }
        else {
            if (!this.refs.MarketingAndRecruiting.state.isClosed) {
                this.refs.MarketingAndRecruiting.closeCollapsible();
            }
            if (!this.refs.FinancialAid.state.isClosed) {
                this.refs.FinancialAid.closeCollapsible();
            }
            if (!this.refs.Admissions.state.isClosed) {
                this.refs.Admissions.closeCollapsible();
            }

            setTimeout(function () { elem.openCollapsible(); }, 100);
        }
    }

    render(){
        const contentStyle = {margin: '0 16px'};
        var context = this;

        var mandrList = ["Prospects", "High School Profiles", "Prospects & High Schools (Geospatial)"];

        var marketingAndRecruiting = mandrList.map( function(title) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ height: "35px", backgroundColor: context.props.settings.colors.homePageColors.funnelTopLine }} key = { title } >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                        className = "funnel-top-body"
                        onClick = { context.handleNext.bind(context, title) }
                    >
                        {title}
                    </CardText>
                </Card>
            )
        });

        var admList = ["Global Admissions", "Applicants", "Applicants by High School", "Review Committee", "Current Year RC with Prior Year Admits"];

        var admissions = admList.map( function(title) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ height: (title.length > 36 ? "50px" : "35px"), backgroundColor: context.props.settings.colors.homePageColors.funnelMiddleLine }} key = { title } >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                        className = "funnel-middle-body"
                        onClick = { context.handleNext.bind(context, title) }
                    >
                        {title}
                    </CardText>
                </Card>
            )
        });

        var faList = ["FA Package Summaries", "FA Package Summaries (Grid)"];

        var financialAid = faList.map( function(title) {
            return (
                <Card containerStyle = {{ padding: "0px" }} style = {{ height: (title.length > 22 ? "50px" : "35px"), backgroundColor: context.props.settings.colors.homePageColors.funnelBottomLine }} key = { title } >
                    <CardText
                        style = {{
                            padding: "7px",
                        }}
                        className = "funnel-bottom-body"
                        onClick = { context.handleNext.bind(context, title) }
                    >
                        {title}
                    </CardText>
                </Card>
            )
        });

        return(
            <div>

                    
                    <Stepper activeStep = { this.state.stepIndex } style = {{ marginBottom: "10px", backgroundColor: this.props.settings.colors.homePageColors.subBackground }} >
                        <Step>
                            <StepLabel
                                icon = { 
                                    (this.state.stepIndex === 0 ?
                                        <span className = "fa-stack fa-lg">
                                            <i 
                                                className = "fa fa-circle fa-stack-2x" 
                                                style = {{     
                                                    fontSize: "26px",
                                                    color: this.props.settings.colors.overviewButtonsColor.background,
                                                    margin: "5px 0px 0px 0px"
                                                }} 
                                            />
                                            <strong className = "fa-stack-1x" style = {{ fontSize: "14px", color: this.props.settings.colors.overviewButtonsColor.text }} >1</strong>
                                        </span> 
                                    :
                                        <span className = "fa-stack fa-lg">
                                            <i 
                                                className = "fa fa-circle fa-stack-2x" 
                                                style = {{     
                                                    fontSize: "25px",
                                                    color: "#ffffff",
                                                    margin: "5px 0px 0px 0px",
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
                            <label style = {{ color: "#ffffff", fontWeight: "normal" }} > Select a Category </label>
                        </StepLabel>
                        </Step>
                        <Step>
                            <StepLabel
                                icon = {
                                    <span className = "fa-stack fa-lg">
                                        <i 
                                            className = "fa fa-circle fa-stack-2x" 
                                            style = {{     
                                                fontSize: "26px",
                                                color: (this.state.stepIndex === 1 ? this.props.settings.colors.overviewButtonsColor.background : "grey" ),
                                                margin: "5px 0px 0px 0px",
                                            }} 
                                        />
                                        <strong className = "fa-stack-1x" style = {{ fontSize: "14px", color: this.props.settings.colors.overviewButtonsColor.text }} >
                                            <label style = {{ color: ( this.state.stepIndex === 0 ? "#bdbdbd" : "#ffffff" ) }} > 2 </label>
                                        </strong>
                                    </span> 
                                }
                            >
                                <label style = {{ color: ( this.state.stepIndex === 0 ? "#bdbdbd" : "#ffffff" ), fontWeight: "normal" }} > Select a View </label>
                            </StepLabel>
                        </Step>
                    </Stepper>

                    <ExpandTransition loading = { this.state.loading } open = { true } style = {{ backgroundColor: "#f7f7f7" }} >
                        <div style = {{ width: "100%" }} >
                                <div style = {{ width: "100%" }} >
                                        {this.state.stepIndex === 0 ? 
                                            <div style = {{ width: "100%" }} >
                                                <div style = {{ margin: "0 auto", width: "370px" }}>
                                                    <Collapsible 
                                                        ref = "MarketingAndRecruiting"
                                                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this, 'MarketingAndRecruiting') }
                                                        transitionTime = {200} 
                                                        contentOuterClassName = "C-Size-1-body"
                                                        contentInnerClassName = "C-Size-inner"
                                                        triggerClassName = "C-Size-1"
                                                        triggerOpenedClassName = "C-Size-1"
                                                        trigger = {
                                                            <div style = {{ margin: "-60px auto 0px", width: "78%" }} >
                                                                <span style = { styles.textSpan } >
                                                                    MARKETING &amp; RECRUITING
                                                                </span>
                                                            </div>
                                                        }
                                                    >
                                                        <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.funnelTopBody }} >
                                                            <CardText
                                                                style = {{
                                                                    padding: "7px",
                                                                }}
                                                            >
                                                                {marketingAndRecruiting}
                                                            </CardText>
                                                        </Card>
                                                        
                                                    </Collapsible>

                                                    <Collapsible 
                                                        ref = "Admissions"
                                                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this, 'Admissions') }
                                                        transitionTime = {200} 
                                                        contentOuterClassName = "C-Size-2-body"
                                                        contentInnerClassName = "C-Size-inner"
                                                        triggerClassName = "C-Size-2"
                                                        triggerOpenedClassName = "C-Size-2"
                                                        trigger = {
                                                            <div style = {{ margin: "-60px auto 0px", width: "48%" }} >
                                                                <span style = { styles.textSpan } >
                                                                    ADMISSIONS
                                                                </span>
                                                            </div>
                                                        }
                                                    >
                                                        <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.funnelMiddleBody }} >
                                                            <CardText
                                                                style = {{
                                                                    padding: "7px",
                                                                }}
                                                            >
                                                                {admissions}
                                                            </CardText>
                                                        </Card>
                                                    </Collapsible>

                                                    <Collapsible
                                                        ref = "FinancialAid"
                                                        handleTriggerClick = { this.onCollapsibleTriggerClick.bind(this, 'FinancialAid') }
                                                        transitionTime = {200} 
                                                        contentOuterClassName = "C-Size-3-body"
                                                        contentInnerClassName = "C-Size-inner"
                                                        triggerClassName = "C-Size-3"
                                                        triggerOpenedClassName = "C-Size-3"
                                                        trigger = {
                                                            <div style = {{ margin: "-60px auto 0px", width: "71%" }} >
                                                                <span style = { styles.textSpan } >
                                                                    FINANCIAL AID
                                                                </span>
                                                            </div>
                                                        }
                                                    >
                                                        <Card containerStyle = {{ padding: "0px", backgroundColor: this.props.settings.colors.homePageColors.funnelBottomBody }} >
                                                            <CardText
                                                                style = {{
                                                                    padding: "7px",
                                                                }}
                                                            >
                                                               {financialAid}
                                                            </CardText>
                                                        </Card>
                                                    </Collapsible>

                                                </div>
                                                <div style = {{ margin: "0 auto", width: "370px" }}>
                                                    <RaisedButton 
                                                        label = "My Views"
                                                        style = {{
                                                            width: "220px",
                                                            margin: "10px 0px 0px 75px"
                                                        }}
                                                        buttonStyle = {{
                                                            height: '50px',
                                                            lineHeight: '50px',
                                                            backgroundColor: this.props.settings.colors.homePageColors.myViewsButton
                                                        }} 
                                                        labelStyle = {{
                                                            fontSize: '12px',
                                                            color: this.props.settings.colors.overviewButtonsColor.text,
                                                            fontFamily: "Arial Black, Gadget, sans-serif",
                                                            margin: "0px 0px 0px -3px",
                                                            paddingLeft: "0px",
                                                            paddingRight: "0px"
                                                        }}
                                                        overlayStyle = {{
                                                            height: '50px',
                                                            lineHeight: '50px'
                                                        }}
                                                        onClick = { this.handleNext.bind(this, "My Views") }
                                                        primary = {true } 
                                                    />
                                                </div>
                                            </div>
                                            : 
                                            (this.state.type === "My Views" ? <MyViews /> : <AllViewsModal type = { this.state.type } goBack = { this.handlePrev.bind(this) } /> )
                                        }
                                    <div style = {{ marginTop: 12 }} >
                                        <FlatButton
                                            label = "Back"
                                            backgroundColor = "#dcdcdc"
                                            onClick = { this.handlePrev }
                                            style = {{ marginRight: 12 }}
                                            style = {{ display: (this.state.stepIndex === 1 && this.state.type === "My Views" ? "auto" : "none"), margin: "0px 0px 0px 11px", bottom: "10px" }}
                                        />
                                    </div>
                                </div>
                        </div>
                    </ExpandTransition>
                </div>
                
                     
        );
    }
};

const styles = {
    textSpan: {
        paddingLeft: '10px',
        fontSize: '1rem',
        fontFamily: "Arial Black, Gadget, sans-serif",
        color: "#000000"
    }
}

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
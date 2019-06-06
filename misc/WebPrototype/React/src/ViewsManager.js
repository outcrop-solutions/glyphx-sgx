import React from 'react';
import { connect } from 'react-redux';
// import { Step, Stepper, StepLabel } from 'material-ui/Stepper';
// import { Card, CardText } from 'material-ui/Card';
import { makeServerCall } from './ServerCallHelper.js';
// import { withRouter } from 'react-router-dom';
// import RaisedButton from 'material-ui/RaisedButton';
// import FlatButton from 'material-ui/FlatButton';
// import ExpandTransition from 'material-ui/internal/ExpandTransition';
import AllViewsModal from './AllViewsModal.js';
// import SavedViews from './SavedViews.js';
import Flexbox from 'flexbox-react';
import './css/ViewsManager.css';
import './css/General.css';


/**
 * -ADCMT
 */
class ViewsManager extends React.Component {
	
	constructor(props){
		super(props);
		this.onLaunch =  this.onLaunch.bind(this);
		this.state = {
			loading: false,
			launchReady: false,
			stepIndex: 0,
			type: "Funnel",
			selectionType: "",
			selectionTypeURL: "",
			flipped: false,
            clicked: false,
            legendPng: "",
		}
    }
    
    componentDidMount(){
        if(document.getElementById("topic-defaultOpen")){
            Promise.all([document.getElementById("topic-defaultOpen").click()])
            .then(() => {
                if(document.getElementById("inquiry-defaultOpen")){
                    document.getElementById("inquiry-defaultOpen").firstChild.click();
                }
            });
        }
    }

    /**
     * Enables transition pause when navigating from regular view to my views and vice versa
     */
    dummyAsync = (cb) => {
        this.setState({ loading: true }, () => {
            this.asyncTimer = setTimeout(cb, 500);
        });
    };
	
	//Moving this here instead of SavedViews.js due to refs problem
	onSavedViewSelect(savedVizObj,callback,recentViewClick){
        // console.log(savedVizObj, callback, recentViewClick, 'seeing what is passed in');
        var originalVizName = savedVizObj.OriginalVizName; 
        var query = savedVizObj.QueryString; 
        var funnelData;
        var keys = Object.keys(this.props.funnelData);
        var path;
        var context = this;
        var flag = true;
    
        for(var keyIndex=0;keyIndex<keys.length && flag;keyIndex++){
            funnelData = this.props.funnelData[keys[keyIndex]];
    
            for(var i = 0;i < funnelData.length; i++)
            {
                if(funnelData[i][0] === originalVizName){
                    path = funnelData[i][1];
                    flag = false;
                    break;
                }
            }
        }
    
        var index = path.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");
        var currentDate = new Date();
		var sdtPath = path.substring(index + 1);
		
		if(recentViewClick){
			//dosomething
		}
	
		var tempPath = path.substring(index + 1) + "&&&"+currentDate.getTime()+","+originalVizName+","+savedVizObj.ID+","+savedVizObj.Name;
        makeServerCall(window.encodeURI('frontEndFilterData/' + tempPath ),
            function (responseText) {
                var response = JSON.parse(responseText);
                
                // Post the new data to the state and hide the window load-mask
                context.props.dispatch(
                    setCurrentVizParams(
                        {
                            tableName: response.tableName,
                            datasourceId: response.datasourceId ,
                            query: query,
                            originalVizName:originalVizName,
                            filterAllowedColumnList:  response.filterAllowedColumnList,
                            sdtPath: sdtPath,
                            savedViz: true,
                            vizID:savedVizObj.ID,
                            savedVizName: savedVizObj.Name,
                            frontEndFilterString: savedVizObj.frontEndFilterString,
                            initialX: response.initialX,
                            initialY: response.initialY,
                            initialZ: response.initialZ
                        }
                    )
                );

                if(typeof callback === 'function'){
					callback(true);
					//context.props.history.push('/glyph-viewer');
				}
				
            }
        );

        makeServerCall('frontEndFiltersEC2',
            function (responseText) {
                var response = JSON.parse(responseText);
                console.log(response, 'response from fetch front filters ec2 in views manager');
                // Post the new data to the state and hide the window load-mask
                // context.props.dispatch(
                //     setCurrentVizParams(
                //         {
                //             tableName: response.tableName,
                //             datasourceId: response.datasourceId ,
                //             query: query,
                //             originalVizName:originalVizName,
                //             filterAllowedColumnList:  response.filterAllowedColumnList,
                //             sdtPath: sdtPath,
                //             savedViz: true,
                //             vizID:savedVizObj.ID,
                //             savedVizName: savedVizObj.Name,
                //             frontEndFilterString: savedVizObj.frontEndFilterString,
                //             initialX: response.initialX,
                //             initialY: response.initialY,
                //             initialZ: response.initialZ
                //         }
                //     )
                // );

                // if(typeof callback === 'function'){
				// 	callback(true);
				// 	//context.props.history.push('/glyph-viewer');
				// }
				
            }, {
                post: true,
                data: {
                    key: tempPath
                }
            }
        );
    }
	
	// Moving this here instead of AllViewsModal.js due to refs problem
	onLaunch(extra, callback) {
        console.log(extra);
        // Handle launch when no selections made on a column (select all unless its not allowed to select all)
		var context = this;
		var tableName;
		var frontEndFilters;
		var originalVizName;
		var datasourceId;
		var filterAllowedColumnList;
		var sdtPath;
        var index;
        
        console.log("-----------------------Stage 1 reached");
		
		if (Array.isArray(extra)) { 
		//[.originalVizName,.time,.date,.frontEndFilters,.datasourceId,.filterAllowedColumnList,sdtPath,tableName]
			sdtPath = extra[6];
			tableName = extra[7];
			frontEndFilters = extra[3];
			originalVizName = extra[0];
			datasourceId = extra[4];
			filterAllowedColumnList = extra[5];
		}
		else {
			index = this.state.selectionTypeURL.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");
			sdtPath = this.state.selectionTypeURL.substring(index + 1);
			tableName = extra.tableName;
			frontEndFilters = extra.frontEndFilters;
			originalVizName = extra.originalVizName;
			datasourceId = extra.datasourceId;
			filterAllowedColumnList = extra.filterAllowedColumnList;
        }

        console.log("-----------------------Stage 2 reached");
        var currentDate = new Date();
        console.log(
            "TABLE:", tableName,
            "TIME:",currentDate.getTime(),
            "DATE:",currentDate.getTime(), 
            "FRONT END FILTS:",frontEndFilters, 
            "ORIGINAL VIZ NAME:",originalVizName, 
            "DATA SOURCE ID:",datasourceId, 
            "Allowed COlmn list:",filterAllowedColumnList, 
            "SDT Path:",sdtPath, "FRONTENDFILERQUERY" )
		
		 //If you dont want it to update on 
		makeServerCall('checkFrontEndFilterQuery',
			function(res){
                res = JSON.parse(res);
                
                console.log("-----------------------Stage 3 reached");

				// Check if match flag is true means that at least one row was returned using the query.
				if (res.match === true || res.match === "true") {
					// Set the params to the store and then goto viz page.
					context.props.dispatch(setCurrentVizParams({
							originalVizName: originalVizName, 
							tableName: tableName,
							datasourceId: datasourceId ,
							query: res.query, 
							filterAllowedColumnList: filterAllowedColumnList, 
							sdtPath: sdtPath, 
							frontEndFilters: frontEndFilters
						}));
					context.props.dispatch(editModalDisplay(false));
					//
				}
				
				if (typeof callback === 'function') {
					callback(res.match);
                }
            },
            {
                post: true, 
                data:  { 
					tableName: tableName,
					time: currentDate.getTime(),
					date: currentDate.getTime(), 
					frontEndFilters: frontEndFilters, 
					originalVizName: originalVizName, 
					datasourceId: datasourceId, 
					filterAllowedColumnList: filterAllowedColumnList, 
					sdtPath: sdtPath 
				}
            }
        );
    }
	
    /**
     * -ADCMT
     * @param type: -ADCMT
     */
    handleNext = (type) => {
        const {stepIndex} = this.state;
        if (!this.state.loading) {
            //console.log(type);
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


    /**
     * Returns from my views to funnel view
     */
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


    /**
     * Toggles flip from funnel view to sub-funnels and back
     * @param type: side that is being flipped to
     */
    flip (type, len) {
        if(len > 0){
        this.setState({ flipped: !this.state.flipped, clicked: true });
        var context = this;
    
        context.setState({ type: type });
        }
        else {
            return null;
        }
    }


    /**
     * Toggles flip from funnel view to my view and back
     * @param type: side that is being flipped to
     */
    handleViewSelect (type){

        this.props.dispatch(editModalDisplay(true));
        console.log(type[0], type[1]);

        return new Promise((resolve, reject) => {
            this.setState({ selectionType: type[0], selectionTypeURL: type[1] });
            resolve();
        }).then(() => {
            this.getLegend();
        });
    }

    /**
     * 
     * @param innerHtml added scalable clear - unHighlightTopicRows() & unHighlightInquieryRows() - 
     * and highlight -highlightTopicSelectedRow & highlightInquirySelectedRows - for easy of highlighting for UI/UX
     */

    /* highlightTopicSelectedRow(innerHtml){
        // console.log(document.getElementsByClassName('main-category-select'))
        for(let i = 0; i < document.getElementsByClassName('main-category-select').length; i++){
            if(document.getElementsByClassName('main-category-select')[i].innerHTML === innerHtml){
                document.getElementsByClassName('main-category-select')[i].style.backgroundColor = '#ddd';
            }
        }
    } */

    highlightTopicSelectedRow(innerHtml){
        for(let i = 0; i < document.getElementsByClassName('main-category-select').length; i++){
            if(document.getElementsByClassName('main-category-select')[i].innerHTML === innerHtml){
                let str = document.getElementsByClassName('main-category-select')[i].className;
                document.getElementsByClassName('main-category-select')[i].className = str.replace('light', 'dark');
            }
        }
    }

    unHighlightTopicRows(){
        // console.log(document.getElementsByClassName('main-category-select'))
        for(let i = 0; i < document.getElementsByClassName('main-category-select').length; i++){
                let str = document.getElementsByClassName('main-category-select')[i].className;
                document.getElementsByClassName('main-category-select')[i].className = str.replace('dark', 'light');
        }
    }

    highlightInquirySelectedRow(innerHtml){
        /* for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
            if(document.getElementsByClassName('second-view-selection')[i].innerHTML === innerHtml){
                document.getElementsByClassName('second-view-selection')[i].style.backgroundColor = '#ddd';
            }
        } */
        for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
            if(document.getElementsByClassName('second-view-selection')[i].innerHTML === innerHtml){
                let str = document.getElementsByClassName('second-view-selection')[i].className;
                document.getElementsByClassName('second-view-selection')[i].className = str.replace('light', 'dark');
            }
        }
    }

    unHighlightInquiryRows(){
        /* for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
            document.getElementsByClassName('second-view-selection')[i].style.backgroundColor = '#e7e7fd';
        } */
        for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
            let str = document.getElementsByClassName('second-view-selection')[i].className;
            document.getElementsByClassName('second-view-selection')[i].className = str.replace('dark', 'light');
        }
    }

    getLegend() {
        var context = this;
        // let imgPath = '';
        // let legendSrc = '';
        // let strArr = this.state.selectionType.split(" ");
        // let firstWord = strArr[0];
        
        // console.log('caled get legend');
        let index = context.state.selectionTypeURL.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");
        let sdtPath = context.state.selectionTypeURL.substring(index + 1);
        // console.log(window.encodeURI('getLegendURL/' + sdtPath), 'sdtpath');
        makeServerCall(window.encodeURI('getLegendURL/' + sdtPath),
            function (responseText) { 
                let response;
                if(typeof responseText === 'string') response = JSON.parse(responseText);
                // console.log(response.body, 'RESPONSETEXT');
                let imgPath = response.body;
                /**
                 * MUTATE BACKEND CALL TO RETURN MORE THAN 1 PNG
                 * WORK IN PROGRESS
                 */
                
                if (imgPath !== '' && (imgPath.includes(context.state.selectionType) || imgPath.includes(context.state.type))) {
                    context.setState({legendPng: imgPath});
                }
            }
        );
            
        // if(this.state.legendPng){
        //     return(
        //         <div style = {{
        //             marginLeft: "15px",
        //             width: "35%",}} >
                            
        //             <img
        //                 src = {'http://ec2-18-224-124-242.us-east-2.compute.amazonaws.com:8000/Legend/' + window.encodeURIComponent(this.state.legendPng)} 
        //                 style = {{ width: '99%', height: "100%", borderRadius: "3px" }} 
        //                 title = "Legend.png"
        //                 alt = "Legend" 
        //                 draggable = { false } 
        //             />
        //         </div>
        //     );
        // }
    }

    render() {
        let context = this;
        /* console.log(this) */
        var funnelData = this.props.funnelData;
        // console.log(funnelData);
        var mandrList, admList, faList, customList /* retentionList */;

        //checking to see if its there and if it is, copy and put it in a new array
        if (funnelData["Marketing and Recruiting"]) mandrList = funnelData["Marketing and Recruiting"];
        else mandrList = [];
        

        if (funnelData["Admissions"]) admList = funnelData["Admissions"];
        else admList = [];

        if (funnelData["Financial Aid"]) faList = funnelData["Financial Aid"];
        else faList = [];

        if (funnelData["Custom"]) customList = funnelData["Custom"];
        else customList = [];

        // console.log(mandrList);

        // console.log(admList);

        // console.log(faList);

        // console.log(customList);

        //splicing out things that aren't supposed to be in there. to double check
        for (var i = mandrList.length - 1; i > -1; i--) {
            if (mandrList[i][0] !== "Prospects" && mandrList[i][0] !== "High School Profiles") {
                mandrList.splice(i, 1);
            }
        }
        // console.log(mandrList);
        for (var x = admList.length - 1; x > -1; x--) {
            if (admList[x][0] !== "Current Year RC with Prior Year Admits" && 
            admList[x][0] !== "Global Admissions" && admList[x][0] !== "Applicants" 
            && admList[x][0] !== "Applicants by High School" && admList[x][0] !== "Review Committee") {
                admList.splice(x, 1);
            }
        }
        // console.log(admList);

        faList = [];

        /*
        for (var i = faList.length - 1; i > -1; i--) {
            if (faList[i][0] != "" && faList[i][0] != "" && faList[i][0] != "") {
                faList.splice(i, 1);
            }
        }

        console.log(faList);
        */

        for (var j = customList.length - 1; j > -1; j--) {
            if (customList[j][0] !== "Applicants" && customList[j][0] !== "Pre-College" &&
             customList[j][0] !== "Adversity" && customList[j][0] !== "First Source" && 
             customList[j][0] !== "Retention Geo" && customList[j][0] !== "Retention Non Geo") {
                customList.splice(j, 1);
            }
        }
        // console.log(customList);

        for (var k = mandrList.length - 1; k > -1; k--) {
            if (mandrList[k][0] !== "Prospects" && mandrList[k][0] !== "High School Profiles") {
                mandrList.splice(k, 1);
            }
        }

        const styleForFirstViewSelect = {
            fontSize:"19px", 
            padding: "7px",
            fontFamily: "Helvetica"
            /* borderBottom: "1px solid black" */
            /* backgroundColor: "white" */
        };

        const styleForSecViewSelect = {
            width: "100%", 
            margin: "0 auto", 
            height: "35px",
            fontSize: "19px", 
            fontFamily: "Helvetica"
            /* backgroundColor: "white", */
            /* borderBottom: "1px solid black"  */
        };

        const h3TableHeader = {
            fontSize: "22px", 
            borderBottom: "1px solid black", 
            padding: "9px 0 0 3px",
            margin: "0px",
            backgroundColor: "#e7e7fd",
            fontWeight: "300"
        };

        var marketingAndRecruiting = mandrList.map( function(title) {
            return (
                <div
                    // containerStyle = {{ padding: "0px" }} 
                    style = {styleForSecViewSelect} 
                    key = { title }
                    id= "inquiry-defaultOpen" 
                >
                    <div
                        style = {{ padding: "7px" }}
                        className = "funnel-top-body noselect second-view-selection light"
                        onClick = { (e) => {
                            context.handleViewSelect(title);
                            context.unHighlightInquiryRows();
                            context.highlightInquirySelectedRow(e.target.innerHTML); } }
                    >
                        {title[0]}
                    </div>
                </div>
            )
        });

        var admissions = admList.map( function(title) {
            return (
                <div
                    // containerStyle = {{ padding: "0px" }} 
                    style = {styleForSecViewSelect} 
                    key = { title } 
                >
                    <div
                        style = {{ padding: "7px" }}
                        className = "funnel-middle-body noselect second-view-selection light"
                        onClick = {(e) => {
                            context.handleViewSelect(title);
                            context.unHighlightInquiryRows();
                            context.highlightInquirySelectedRow(e.target.innerHTML); } }
                    >
                        {title[0]}
                    </div>
                </div>
            )
        });

        var financialAid = faList.map( function(title) {
            return (
                <div
                    // containerStyle = {{ padding: "0px" }} 
                    style = {styleForSecViewSelect} 
                    key = { title } 
                >
                    <div
                        style = {{ padding: "7px" }}
                        className = "funnel-bottom-body noselect second-view-selection light"
                        onClick = {(e) => {
                            context.handleViewSelect(title);
                            context.unHighlightInquiryRows();
                            context.highlightInquirySelectedRow(e.target.innerHTML); } }
                    >
                        {title[0]}
                    </div>
                </div>
            )
        });

        var custom = customList.map( function(title) {
            return (
                <div
                    // containerStyle = {{ padding: "0px" }} 
                    style = {styleForSecViewSelect} 
                    key = { title } 
                >
                    <div
                        style = {{ padding: "7px" }}
                        className = "funnel-bottom-body noselect second-view-selection light"
                        onClick = {(e) => {
                            context.handleViewSelect(title);
                            context.unHighlightInquiryRows();
                            context.highlightInquirySelectedRow(e.target.innerHTML); } }
                    >
                        {title[0]}
                    </div>
                </div>
            )
        });
        
       /*  var flippedCSS = (this.state.flipped ? " Card-Back-Flip" : " Card-Front-Flip");
        if (!this.state.clicked) flippedCSS =  "";

        var backButton = (
            <RaisedButton
                label = "Back"
                onClick = { () => this.flip("Funnel") }
                style = {{ 
                    width: "80%", 
                    minWidth: "331px", 
                    margin: "0 auto",
                    display: (this.state.type === "MarketingAndRecruiting" || this.state.type === "Admissions" || this.state.type === "FinancialAid" || this.state.type === "Custom" ? "" : "none")
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
            />
        ); */

        return(
            <div style={{height:"100%"}}>
                <div>
                    <h2
                        className='noselect'
                        style={{
                            color: this.props.settings.colors.overviewButtonsColor.text,
                            backgroundColor: '#018cbb'/* this.props.settings.colors.homePageColors.headerBackground  */, 
                            borderRadius: "2px", 
                            paddingBottom: "4px", 
                            margin: "auto auto 12px auto", 
                            padding: "10px 0 8px 0",
                            fontSize: "21px",
                            letterSpacing: "0.14em",
                            textTransform: "uppercase",
                            textAlign: "center"}}
                        >Visualization Manager
                    </h2>
                </div>

                    {/* <ExpandTransition loading = { this.state.loading } open = { true } style = {{ overflow: "auto", height: "100%" }} > */}
                <Flexbox style={{height: "25%", paddingBottom: "3px"}}>
                        <div style = {{ 
                            display: "table",
                            backgroundColor: 'white'/* this.props.settings.colors.homePageColors.subBackground */, 
                            border: "1px solid black",
                            height: "25%",
                            width: "65%",
                            }} >

                            <div style = {{ clear: "both", height:"100%"/* (this.state.type === "My Views" ? "" : *//* ) */ /* , display: "block" */ }} >
                               {/*  <div 
                                    className = "Card column-left" 
                                    style = {{ 
                                        marginBottom: "0px", 
                                        display: (this.state.stepIndex === 0 ? "" : "none"),
                                        //marginTop: (this.state.type === "MarketingAndRecruiting" || this.state.type === "Admissions" || this.state.type === "FinancialAid" || this.state.type === "Custom" ? "-250px" : "0px")
                                    }} 
                                > */}
                                    <div
                                    style = {{ 
                                        width: "50%",
                                        height: "100%", 
                                        float: "left", 
                                        borderRight: "2px solid black" }} > 
                                        <h3 style={h3TableHeader}><b style={{fontSize:"28px"}}>①</b> Select a Topic</h3>
                                        <div>
                                            <div
                                            onClick = { (e) => {
                                                this.flip("MarketingAndRecruiting", mandrList.length);
                                                this.unHighlightTopicRows();
                                                this.highlightTopicSelectedRow(e.target.innerHTML);
                                            ;} } 
                                            className = { `${(mandrList.length > 0 ? "noselect" : "cursorDefault noselect")} 
                                            main-category-select light` } 
                                            id="topic-defaultOpen"
                                            style = {styleForFirstViewSelect} >
                                            Marketing and Recruiting
                                            </div>

                                            <div className = "main-category-select"
                                            onClick = {(e) => {
                                                this.flip("Admissions", admList.length);
                                                this.unHighlightTopicRows();
                                                this.highlightTopicSelectedRow(e.target.innerHTML);
                                            } } 
                                            className = { `${(admList.length > 0 ? "noselect" : "cursorDefault noselect")} 
                                            main-category-select light`}
                                            style={styleForFirstViewSelect} >
                                            Admissions
                                        </div>

                                            <div className = "main-category-select"
                                            onClick = {(e) => { 
                                                this.flip("FinancialAid", faList.length); 
                                                this.unHighlightTopicRows();
                                                this.highlightTopicSelectedRow(e.target.innerHTML);
                                            }} 
                                            className = { `${(faList.length > 0 ? "noselect" : "cursorDefault noselect")} 
                                            main-category-select light`}
                                            style={styleForFirstViewSelect} >
                                            Financial Aid
                                            </div>

                                            <div className = "main-category-select"
                                            onClick = {(e) => {
                                                this.flip("Custom", customList.length);
                                                this.unHighlightTopicRows();
                                                this.highlightTopicSelectedRow(e.target.innerHTML); 
                                            }}
                                            className = { `${(customList.length > 0 ? "noselect" : "cursorDefault noselect")} main-category-select light`}
                                            style={styleForFirstViewSelect} >
                                            Custom
                                            </div>
                                        </div>

                                    </div>

                                    {/*this is where the 2nd modal pops up after initial selection */}
                                    <div
                                    style = {{ 
                                        height: "100%", 
                                        width: "50%", 
                                        float: "right" }}>
                                        <h3 style={h3TableHeader}><b style={{fontSize:"28px"}}>②</b> Select an Inquiry</h3>
                                        <Flexbox style = {{ /* alignItems: "center",  height: "100%",*/ /* borderBottom: "1px solid black"  */}} >
                                        {this.state.type === "MarketingAndRecruiting" ? 
                                        <div style={{width:"100%"}}> {marketingAndRecruiting} </div>
                                            /* {this.state.type === "MarketingAndRecruiting" ? <div> {marketingAndRecruiting} {backButton} </div>  */
                                                : (this.state.type === "Admissions" ? 
                                                <div style={{width:"100%"}}> {admissions} </div> 
                                                    : (this.state.type === "FinancialAid" ? 
                                                    <div style={{width:"100%"}}> {financialAid} </div> 
                                                        : (this.state.type === "Custom" ? 
                                                        <div style={{width:"100%"}}> {custom} </div> : null)
                                                    )
                                                )
                                            }
                                        </Flexbox>
                                    </div>
                                
                            </div>
                        </div>

                        {/*displays legend Png for newer users*/}
            
                        {/* {this.getLegend()} */}
                        
                        <div style = {{
                            display: this.state.legendPng ? "" : "none",
                            marginLeft: "15px",
                            width: "35%",}} >
                                    
                            <img
                                src = {'http://ec2-18-224-124-242.us-east-2.compute.amazonaws.com:8000/Legend/' + window.encodeURIComponent(this.state.legendPng)} 
                                style = {{ width: '99%', height: "100%", borderRadius: "3px" }} 
                                title = "Legend.png"
                                alt = "Legend" 
                                draggable = { false } 
                            />
                        </div>
                    
                </Flexbox>
                <AllViewsModal type = { this.state.selectionType } typeURL = { this.state.selectionTypeURL } 
                        onLaunch={(extra,callback) => {this.onLaunch(extra,callback)} }/>
                    {/* </ExpandTransition> */}
            </div>
        );
    }
};


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
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    funnelData: state.filterState.FunnelData,
    storedViews: state.filterState.StoredViews
  }
}

export const setCurrentVizParams = (vizParams) => ({
   type: 'SET_VIZ_PARAMS',
   vizParams,
});


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(ViewsManager);
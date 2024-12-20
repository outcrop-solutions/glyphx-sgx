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
            topicArray: [],
            //legendPng is legacy code
		}
    }
    
    // componentDidMount(){
    //     if(document.getElementById("topic-defaultOpen")){
    //         Promise.all([document.getElementById("topic-defaultOpen").click()])
    //         .then(() => {
    //             if(document.getElementById("inquiry-defaultOpen")){
    //                 document.getElementById("inquiry-defaultOpen").firstChild.click();
    //             }
    //         });
    //     }
    // }

    /**
     * Enables transition pause when navigating from regular view to my views and vice versa
     */
        dummyAsync = (cb) => {
            this.setState({ loading: true }, () => {
                this.asyncTimer = setTimeout(cb, 500);
        });
    };
	
	//Moving this here instead of SavedViews.js due to refs problem
	onSavedViewSelect = (savedVizObj,callback,recentViewClick) => {
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
	onLaunch(extra, callback, callback_2) {
        console.log(extra, callback_2);
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
        // console.log(
        //     "TABLE:", tableName,
        //     "TIME:",currentDate.getTime(),
        //     "DATE:",currentDate.getTime(), 
        //     "FRONT END FILTS:",frontEndFilters, 
        //     "ORIGINAL VIZ NAME:",originalVizName, 
        //     "DATA SOURCE ID:",datasourceId, 
        //     "Allowed COlmn list:",filterAllowedColumnList, 
        //     "SDT Path:",sdtPath, "FRONTENDFILERQUERY" )
		
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
        // console.log('called')
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

    // highlightTopicSelectedRow(innerHtml){
    //     for(let i = 0; i < document.getElementsByClassName('main-category-select').length; i++){
    //         if(document.getElementsByClassName('main-category-select')[i].innerHTML === innerHtml){
    //             let str = document.getElementsByClassName('main-category-select')[i].className;
    //             document.getElementsByClassName('main-category-select')[i].className = str.replace('light', 'dark');
    //         }
    //     }
    // }

    // unHighlightTopicRows(){
    //     // console.log(document.getElementsByClassName('main-category-select'))
    //     for(let i = 0; i < document.getElementsByClassName('main-category-select').length; i++){
    //             let str = document.getElementsByClassName('main-category-select')[i].className;
    //             document.getElementsByClassName('main-category-select')[i].className = str.replace('dark', 'light');
    //     }
    // }

    // highlightInquirySelectedRow(innerHtml){
    //     /* for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
    //         if(document.getElementsByClassName('second-view-selection')[i].innerHTML === innerHtml){
    //             document.getElementsByClassName('second-view-selection')[i].style.backgroundColor = '#ddd';
    //         }
    //     } */
    //     for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
    //         if(document.getElementsByClassName('second-view-selection')[i].innerHTML === innerHtml){
    //             let str = document.getElementsByClassName('second-view-selection')[i].className;
    //             document.getElementsByClassName('second-view-selection')[i].className = str.replace('light', 'dark');
    //         }
    //     }
    // }

    // unHighlightInquiryRows(){
    //     /* for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
    //         document.getElementsByClassName('second-view-selection')[i].style.backgroundColor = '#e7e7fd';
    //     } */
    //     for(let i = 0; i < document.getElementsByClassName('second-view-selection').length; i++){
    //         let str = document.getElementsByClassName('second-view-selection')[i].className;
    //         document.getElementsByClassName('second-view-selection')[i].className = str.replace('dark', 'light');
    //     }
    // }

    getLegend() {
        var context = this;
        // let strArr = this.state.selectionType.split(" ");
        // let firstWord = strArr[0];
        
        let index = context.state.selectionTypeURL.replace(/\\([^\\]*)$/,'!!!!$1').lastIndexOf("\\");
        let sdtPath = context.state.selectionTypeURL.substring(index + 1);
        // console.log(sdtPath, 'sdtPath');
        makeServerCall(window.encodeURI('getLegendURL/' + sdtPath),
            function (responseText) { 
                let response;
                if(typeof responseText === 'string') response = JSON.parse(responseText);
                // console.log(response)
                if(response.body){
                    let imgPath = response.body.legendUrl;
                    console.log(response.body.legendUrl, response.body.imgArr,'imgPath');
                    
                    context.props.dispatch(setLegendUrlArr(response.body.imgArr));

                    if (imgPath && imgPath.length > 0 && (imgPath.includes(context.state.selectionType) || imgPath.includes(context.state.type))) {
                        context.setState({ legendPng: 
                            `http://ec2-18-224-124-242.us-east-2.compute.amazonaws.com:8000/Legend/${window.encodeURIComponent(imgPath)}`});
                    }
                }
            }
        );
    }

    selectTopic(val){
        // console.log(val, this.props.funnelData[val])
        if(this.props.funnelData[val].length > 0){
            this.flip(val, this.props.funnelData[val].length);
        }
    }

    selectInquiry(str){
        // console.log(str)
        if(str.length > 0 && str.indexOf(',') > 0){
            let val = str.slice(0, str.indexOf(','));
            let url = str.slice(str.indexOf(',') + 1);
            // console.log(val, url);
            this.props.dispatch(editModalDisplay(true));
    
            return new Promise((resolve, reject) => {
                this.setState({ selectionType: val, selectionTypeURL: url });
                resolve();
            }).then(() => {
                this.getLegend();
            });
        }
    }
    
    injectOption(){
        return (
        <option>
            Select An Inquiry
        </option>
        );
    }

    render() {
        // let context = this;
        var funnelData = this.props.funnelData;
        let new_funnel_data = {};
        // console.log(funnelData);

        //splicing out things that aren't supposed to be in there. to double check
        
        for(let property in funnelData) {
            new_funnel_data[property] = [];
            if(property !== "Marketing and Recruiting" && property !== "Admissions" && property !== "Custom" && property !== "Financial Aid"){
                for(let i = 0; i < funnelData[property].length; i++){
                    new_funnel_data[property].push(funnelData[property][i]);
                }
            }
        }
        

        if(funnelData['Marketing and Recruiting']){
            for (let z = 0; z < funnelData['Marketing and Recruiting'].length; z++) {
                if (funnelData['Marketing and Recruiting'][z][0] !== "Prospects and High Schools (Geospatial)") {
                    new_funnel_data['Marketing and Recruiting'].push(funnelData['Marketing and Recruiting'][z]);
                }
            }
        }
        

        if(funnelData['Admissions']){
            for (let x = 0; x < funnelData['Admissions'].length; x++) {
                if (funnelData['Admissions'][x][0] === "Current Year RC with Prior Year Admits" ||
                funnelData['Admissions'][x][0] === "Global Admissions" || funnelData['Admissions'][x][0] === "Applicants" 
                || funnelData['Admissions'][x][0] === "Applicants by High School" || funnelData['Admissions'][x][0] === "Review Committee") {
                    new_funnel_data['Admissions'].push(funnelData['Admissions'][x]);
                }
            }
        }

        if(funnelData['Custom']){
            for (let j = 0; j < funnelData['Custom'].length; j++) {
                if (funnelData['Custom'][j][0] === "Applicants" || funnelData['Custom'][j][0] === "Pre-College" ||
                funnelData['Custom'][j][0] === "Adversity" || funnelData['Custom'][j][0] === "First Source" || 
                funnelData['Custom'][j][0] === "Retention Geo" || funnelData['Custom'][j][0] === "Retention Non Geo") {
                    new_funnel_data['Custom'].push(funnelData['Custom'][j]);
                }
            }
        }  
        // console.log(new_funnel_data)

        const styleForFirstViewSelect = {
            fontSize:"1.877vh", 
            padding: "7px",
            fontFamily: "ITCFranklinGothicStd-Med",
            letterSpacing: "0.5px",
        };

        const styleForSecViewSelect = {
            width: "100%", 
            margin: "0 auto", 
            height: "3.650vh",
            fontSize: "1.876vh", 
            fontFamily: "ITCFranklinGothicStd-Med",
            letterSpacing: "0.5px",
            whiteSpace: "nowrap", 
            overflow: "hidden", 
            textOverflow: "ellipsis",
        };

        const h3TableHeader = {
            fontSize: "2.086vh", 
            fontFamily: "ITCFranklinGothicStd-DmCd",
            margin: "0px",
            fontWeight: "300",
            textTransform: "uppercase",
            letterSpacing: "1px"
        };

        const h3TableHeader_2 = {
            color: (this.state.type !== "Funnel" ? "black" : "lightgrey"),
            fontSize: "2.086vh", 
            fontFamily: "ITCFranklinGothicStd-DmCd",
            margin: "0px",
            fontWeight: "300",
            textTransform: "uppercase",
            letterSpacing: "1px"
        };

        const dropDownSize = {
            minHeight: "5.4vh",
            minWidth: "17.2vw",
            outline: "none"
        };

        const cursor_no = {
            minHeight: "5.4vh",
            minWidth: "17.2vw",
            outline: "none",
            cursor: "not-allowed"
        };
        
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
            <div style={{height:"100%", overflowY: "hidden"}}>

                    {/* <ExpandTransition loading = { this.state.loading } open = { true } style = {{ overflow: "auto", height: "100%" }} > */}
                <Flexbox style={{/*height: "117vh", display: "inline-flex" */backgroundColor: "#e6e7e8", paddingLeft: "25px"}}>
                        <div>

                            <div style = {{ clear: "both", height:"100%"/* (this.state.type === "My Views" ? "" : *//* ) */ /* , display: "block" */ }} >
                               {/*  <div 
                                    className = "Card column-left" 
                                    style = {{ 
                                        display: (this.state.stepIndex === 0 ? "" : "none"),
                                        //marginTop: (this.state.type === "MarketingAndRecruiting" || this.state.type === "Admissions" || this.state.type === "FinancialAid" || this.state.type === "Custom" ? "-250px" : "0px")
                                    }} 
                                > */}
                                    <div
                                    id = "tabular-data"
                                    style = {{ 
                                        float: "left", 
                                        backgroundColor: "#e6e7e8",
                                        padding: "2.7vh 9.1vh 2.4vh 0px",
                                        }} > 
                                        <h3 style={h3TableHeader}>
                                            <img 
                                            style={{
                                                verticalAlign: "middle", 
                                                marginRight: "1.043vh", 
                                                height: "3.024vh"}} 
                                            alt="First Step"
                                            src="./Res/Img/1@2x.png"/>
                                            Choose A Topic
                                        </h3>
                                        <br/>
                                        <select 
                                            className="custom-select" 
                                            id="custom-select" 
                                            style={dropDownSize} 
                                            onChange={e => {this.selectTopic(e.target.value)}}
                                        >
                                            <option style={styleForFirstViewSelect}>Select A Topic</option>
            
                                            {Object.keys(this.props.funnelData).map((t,i) => 
                                                <option 
                                                className = {
                                                    `${(this.props.funnelData[t].length > 0 ? "noselect" : "cursorDefault noselect")} main-category-select light`}
                                                style={styleForFirstViewSelect}
                                                key={i} value={t}>{t}</option>)
                                            }
                                        </select>

                                    </div>

                                    {/*this is where the 2nd modal pops up after initial selection */}
                                    <div 
                                        id = "tabular-data-2"
                                        style = {{ 
                                            float: "right",
                                            backgroundColor: "#e6e7e8",
                                            padding: "2.7vh 0px 2.4vh 0px"
                                    }}>
                                        <h3 style={h3TableHeader_2}>
                                            <img style={{
                                                verticalAlign: "middle", 
                                                marginRight: "1.043vh",
                                                height: "3.024vh"}}
                                            alt="Second Step" 
                                            src="./Res/Img/2@2x.png"/> 
                                            Choose An Inquiry
                                        </h3>
                                        <br/>
                                        <Flexbox>
                                       
                                            {
                                                this.state.type !== "Funnel" ? 
                                                    (<select 
                                                        style={dropDownSize} 
                                                        onChange={(e => this.selectInquiry(e.target.value))}
                                                        > {this.injectOption()}
                                                            {new_funnel_data[this.state.type].map((t) => 
                                                                <option 
                                                                className = "noselect second-view-selection"
                                                                style = {styleForSecViewSelect} 
                                                                key={t} value={t}>{t[0]}</option>)}
                                                    </select>)
                                                   
                                            : <select disabled style={cursor_no}><option>—</option></select>
                                            }
                                        </Flexbox>
                                    </div>
                                
                            </div>
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
    storedViews: state.filterState.StoredViews,
    uid: state.filterState.uid
  }
}

export const setCurrentVizParams = (vizParams) => ({
   type: 'SET_VIZ_PARAMS',
   vizParams,
});

export const setLegendUrlArr = (arr) => ({
    type: 'SET_LEGEND_URL_ARR',
    arr,
 });


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(ViewsManager);
import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Divider from 'material-ui/Divider';
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
        allViews: false,
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

    //new version of onlaunch with desktop side
    socketRecentLaunch(view){
        // let ref = this.refs.viewsManager.getWrappedInstance();
        if(view.length > 4){
            this.onLaunch(view, this.proceedToViz);
        }
        else {
            let savedViewObj;
            this.props.storedViews.savedViews.forEach((savedView) => {
                //
                //this needs to stay == for typecasting purposes!!
                //
				if (view[3] == savedView.ID) {
					savedViewObj = savedView;
                }
            });

            if (savedViewObj) {
				this.onSavedViewSelect(savedViewObj, this.proceedToViz);
			}
        }
    }

    onSavedViewSelect = (savedVizObj,callback) => {
        console.log('+++++++++++++++saved called')
        // console.log(savedVizObj, callback, recentViewClick, 'seeing what is passed in');
        var originalVizName = savedVizObj.OriginalVizName; 
        var query = savedVizObj.QueryString; 
        var funnelData;
        var keys = Object.keys(this.props.funnelData);
        // console.log(this.props, '*******************************************KEYS')
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
		
        if(typeof callback === 'function') callback(query, sdtPath, context);
	
		// var tempPath = path.substring(index + 1) + "&&&"+currentDate.getTime()+","+originalVizName+","+savedVizObj.ID+","+savedVizObj.Name;
        // makeServerCall(window.encodeURI('frontEndFilterData/' + tempPath ),
        //     function (responseText) {
        //         var response = JSON.parse(responseText);
                
        //         // Post the new data to the state and hide the window load-mask
        //         context.props.dispatch(
        //             setCurrentVizParams(
        //                 {
        //                     tableName: response.tableName,
        //                     datasourceId: response.datasourceId ,
        //                     query: query,
        //                     originalVizName:originalVizName,
        //                     filterAllowedColumnList:  response.filterAllowedColumnList,
        //                     sdtPath: sdtPath,
        //                     savedViz: true,
        //                     vizID:savedVizObj.ID,
        //                     savedVizName: savedVizObj.Name,
        //                     frontEndFilterString: savedVizObj.frontEndFilterString,
        //                     initialX: response.initialX,
        //                     initialY: response.initialY,
        //                     initialZ: response.initialZ
        //                 }
        //             )
        //         );

        //         if(typeof callback === 'function'){
		// 			callback(true, query, sdtPath, context);
		// 			//context.props.history.push('/glyph-viewer');
		// 		}
				
        //     }
        // );

        // makeServerCall('frontEndFiltersEC2',
        //     function (responseText) {
        //         var response = JSON.parse(responseText);
        //         console.log(response, 'response from fetch front filters ec2 in views manager');
        //         // Post the new data to the state and hide the window load-mask
        //         // context.props.dispatch(
        //         //     setCurrentVizParams(
        //         //         {
        //         //             tableName: response.tableName,
        //         //             datasourceId: response.datasourceId ,
        //         //             query: query,
        //         //             originalVizName:originalVizName,
        //         //             filterAllowedColumnList:  response.filterAllowedColumnList,
        //         //             sdtPath: sdtPath,
        //         //             savedViz: true,
        //         //             vizID:savedVizObj.ID,
        //         //             savedVizName: savedVizObj.Name,
        //         //             frontEndFilterString: savedVizObj.frontEndFilterString,
        //         //             initialX: response.initialX,
        //         //             initialY: response.initialY,
        //         //             initialZ: response.initialZ
        //         //         }
        //         //     )
        //         // );

        //         // if(typeof callback === 'function'){
		// 		// 	callback(true);
		// 		// 	//context.props.history.push('/glyph-viewer');
		// 		// }
				
        //     }, {
        //         post: true,
        //         data: {
        //             key: tempPath
        //         }
        //     }
        // );
    }
	

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

        var data = frontEndFilters.slice();
        var outerIndex;
        let query = "SELECT * FROM " + tableName + " WHERE ";
        
        // 2D Array
        //[[],[],[]]
        for (outerIndex = 0; outerIndex < data.length; outerIndex++) {
            var dataItem = data[outerIndex].slice();
            var columnName = dataItem[0];

            // Removes the 1st element that is the name.
            dataItem.shift(); 

            var values = '("' + dataItem.toString() + '")';
            query = query + columnName + " IN " + values.replace(/,/g , '","');
            
            if (outerIndex != data.length-1) {
                query = query + " AND ";
            }

            else {
                query = query + ";";
            }
        }

        if (typeof callback === 'function') callback(query, sdtPath, context);
		
		 //If you dont want it to update on 
		// makeServerCall('checkFrontEndFilterQuery',
		// 	function(res){
        //         res = JSON.parse(res);
                
        //         console.log("-----------------------Stage 3 reached");

		// 		// Check if match flag is true means that at least one row was returned using the query.
		// 		if (res.match === true || res.match === "true") {
		// 			// Set the params to the store and then goto viz page.
		// 			context.props.dispatch(setCurrentVizParams({
		// 					originalVizName: originalVizName, 
		// 					tableName: tableName,
		// 					datasourceId: datasourceId ,
		// 					query: res.query, 
		// 					filterAllowedColumnList: filterAllowedColumnList, 
		// 					sdtPath: sdtPath, 
		// 					frontEndFilters: frontEndFilters
		// 				}));
        //             context.props.dispatch(editModalDisplay(false));
		// 			//
		// 		}
				
		// 		if (typeof callback === 'function') {
		// 			callback(res.match, res.query, sdtPath, context);
        //         }
        //     },
        //     {
        //         post: true, 
        //         data:  { 
		// 			tableName: tableName,
		// 			time: currentDate.getTime(),
		// 			date: currentDate.getTime(), 
		// 			frontEndFilters: frontEndFilters, 
		// 			originalVizName: originalVizName, 
		// 			datasourceId: datasourceId, 
		// 			filterAllowedColumnList: filterAllowedColumnList, 
		// 			sdtPath: sdtPath 
		// 		}
        //     }
        // );
    }

    proceedToViz(query, sdt, context){
        console.log('&&&&&&&&&&&&&&&&called')
        var index = sdt.replace(/\\/g, "/");;
        
        makeServerCall(window.encodeURI('getLegendURL/' + sdt),
            function (responseText) { 
                let response;
                if(typeof responseText === 'string') response = JSON.parse(responseText);
                if(response.body){
                    console.log(response.body.imgArr,'imgPath');

                    context.props.webSocket.send(JSON.stringify({
                        url_uid: context.props.uid,
                        sdt: `https://viz-group-notredame-source.s3.us-east-2.amazonaws.com/${index}`,
                        legendURLArr: response.body.imgArr,
                        query,
                        launch: true
                    }));
                
                }
            }
        );
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

    viewsMap(view) {
        if (typeof view[1] === 'string' && !isNaN(view[1])) {
            view[1] = parseInt(view[1]);
        }
            
        let viewTimeStamp = new Date(view[1]);

        let day;
        
        if(viewTimeStamp.getDate() < 10) day = '0' + viewTimeStamp.getDate();
        else day = viewTimeStamp.getDate();
        

        let viewDate =  (viewTimeStamp.getMonth() + 1) + "/" + day + "/" + viewTimeStamp.getFullYear();

        let viewTime;
        if (viewTimeStamp.getHours() > 12) {
            viewTime =  
                viewTimeStamp.getHours() /* - 12 */ + ":" + 
                    (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : 
                        viewTimeStamp.getMinutes()) /* + 'pm'; */
        }
        else if (viewTimeStamp.getHours() === 12) {
            viewTime =  
                12 + ":" + 
                    (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() : 
                        viewTimeStamp.getMinutes()) /* + 'pm'; */
        }
        else {
            viewTime = 
                (viewTimeStamp.getHours() < 10 ? '0' + viewTimeStamp.getHours() : viewTimeStamp.getHours()) 
                /* viewTimeStamp.getHours() */+ ":" + 
                    (viewTimeStamp.getMinutes() < 10 ? '0' + viewTimeStamp.getMinutes() :
                        viewTimeStamp.getMinutes()) /* + 'am'; */
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
                        borderRadius: "0"
                        // marginTop: (view === newList[0] ? "0px" : "5px"),
                    }} 
                    key = { view } 
                >
                    <CardText
                    /* className="hover-recent-select" */
                        style = {{ padding: "7px", width: "100%" }}
                        onClick = { () => {this.props.loadRecentView(view); this.socketRecentLaunch(view);} }
                    >

                        <Flexbox flexDirection = "row" minWidth = "100%" >
                            <Flexbox style = {{ 
                                width: "100%", 
                                whiteSpace: "nowrap", 
                                overflow: "hidden", 
                                fontSize: "16px", 
                                letterSpacing: "0.05em",
                                fontFamily: "ITCFranklinGothicStd-Med" 
                            }} > 
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
                                
                            <div style = {{ 
                                width: "100%", 
                                textAlign: "right", 
                                fontFamily: "ITCFranklinGothicStd-Med",
                                fontSize: "16px" }} >
                                { viewTime } &nbsp; &nbsp; { viewDate }
                            </div>
                        
                        </Flexbox>

                    </CardText>
                </Card>
                <div className = "inherit-hover hover-recent-select" style={{backgroundColor: "#e6e7e8"}}>
                    <div style={{
                        height: "1px", 
                        width: "95.73%", 
                        borderBottom: "1px solid #9ea3af", 
                        margin: "auto"}}>
                    </div>
                </div>
            </div>
        )
    }
    

    render() {
        let context = this;

        let newList = this.state.recents.slice(0, 5);

        let recentViews = newList.map(view => this.viewsMap(view));

        let allViews = this.state.recents.map(view => this.viewsMap(view));

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
                    <span 
                        onClick= {() => this.setState({allViews: true})}
                        style={{
                            color: "white", 
                            fontSize: "18px", 
                            float: "right", 
                            fontFamily: "ITCFranklinGothicStd-DmCd",
                            cursor: "pointer",
                            margin: "-2px 14px 0px 0px"}}
                    >
                        See All&ensp;>
                    </span>

                    <Dialog 
                        bodyStyle = {{overflowY: "scroll"}}
                        title = { <div style = {{ fontWeight: "bold" }} > All Recent Views <Divider /></div> }
                        modal = { true }
                        open = { this.state.allViews }
                        actions = {
                            [
                                <FlatButton
                                    label = "Close"
                                    primary = { true }
                                    onClick = { () => this.setState({ allViews: false }) }
                                    style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                                    labelStyle = {{ lineHeight: "40px" }}
                                />
                            ]
                        }
                    >
                        <Flexbox flexDirection = "row" className = "noselect" style = {{ marginTop: "30px" }} >
                            <Flexbox 
                                flexDirection = "column" 
                                style = {{ 
                                    width: "75vw",
                                    color: "#000000", 
                                    paddingRight: "20px"
                                }} 
                            >
                                {/* <div style = {{ fontWeight: "bold" }} > New Features: </div> */}
                                {/* <ul>
                                    {features}
                                </ul> */}
                                    <div>
                                    <div style = {{ borderRadius: "3px"}}>
                                        {allViews}
                                    </div>
                                </div>
                            </Flexbox>
                            {/* {this.props.announcement.content.bugfixes.length > 0 ?
                                <Flexbox flexDirection = "column" style = {{ width: "50%", color: "#000000", paddingRight: "20px" }} >
                                    <div style = {{ fontWeight: "bold" }} > Bug Fixes: </div>
                                    <ul>
                                        {bugfixes}
                                    </ul>
                                </Flexbox>
                                :
                                null
                            } */}
                        </Flexbox>
                    </Dialog>

                </div>

                {/* tab content */}
                <div className="tab" style={{width: "25vw"}}>
                    
                    <button className="tablinks" id="tab-defaultOpen" 
                    onClick= {(e) => this.openViewsTab(e,'Recent')}
                    style = {{padding: "10px 52.3320px 10px 52.3320px", width: "50%"}}>
                        Recent
                    </button>
                    
                    <button className="tablinks"
                    onClick= {(e) =>  this.openViewsTab(e, 'Saved')}
                    style = {{padding: "10px 52.3320px 10px 52.3320px", width: "50%"}}>
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
                    <div className = "customScroll" style = {{ 
                        height: "253px", 
                        overflow: "auto", 
                        display: (this.state.loadMask ? "none" : ""),
                        width: "25vw"
                    }} >
                        {this.state.recents.length === 0 ? 
                        <div style = {{ 
                            margin: "30px 0px 15px 0px", 
                            fontSize: "18px", 
                            textAlign: "center",
                            fontFamily: "ITCFranklinGothicStd-Med"
                        }}> No Recent Views </div> 
                        : recentViews}
                    </div>
                </div>
                {/* Saved Views Tab */}
                <div id="Saved" className="tabcontent customScroll" style={{ 
                    overflow: "auto", 
                    paddingTop: "7px", 
                    backgroundColor: "#e6e7e8",
                    width: "25vw"
                }}>
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

export const setCurrentVizParams = (vizParams) => ({
    type: 'SET_VIZ_PARAMS',
    vizParams,
 });


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
    uid: state.filterState.uid,
	webSocket: state.filterState.webSocket,
	legend_url_arr: state.filterState.legend_url_arr,
    VizParams: state.filterState.VizParams,
    storedViews: state.filterState.StoredViews,
    funnelData: state.filterState.FunnelData,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(ViewsTab));
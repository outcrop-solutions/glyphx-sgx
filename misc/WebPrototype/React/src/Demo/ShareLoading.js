import React from 'react';
import { connect } from 'react-redux';
import { hideSplashScreen, showLoadMask, hideLoadMask } from '../LoadMaskHelper.js';
import { makeServerCall } from '../ServerCallHelper.js';
import { withRouter } from 'react-router-dom';
import 'rc-tooltip/assets/bootstrap.css';
import '../css/General.css';
import '../css/Views.css';


class ShareLoading extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            loadMask: false,
            done: false,
            status: false
        }
    }
    
    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
        hideSplashScreen();

        if(this.props.vars){
            console.log(this.props.vars);
            return Promise.all([
                this.checkSharedHash(this.props.vars)])
            .then(() => {
                this.props.dispatch(setShareID(this.props.vars.params.id))
                return;
            })
            .then(() => {
                this.setState({done: true});
                return;
            })
            .catch(err => {
                console.log(err);
            });
        }
    }

    componentDidUpdate(){
        //when it comes back with a response, it is set in redux state where the user is pushed to a different path based on redux

        window.setTimeout(() => {
            if(this.props.sharedLinkStatus && this.state.done === true && this.state.status === true){
                let url = 'login?username=' + "Guest1" + "&password=" + "e755b8411d20f7fef458f43afe5eef07f7a44d32071a17df867028236d9e7a48";

                makeServerCall(url, (response) => {
        
                    if(typeof response === 'string'){
                        response = JSON.parse(response);
                    }

                    if (response) { 
                        // Save the details to store
                        if (response.userInformation) {
                            response.userInformation.loggedInTime = new Date();
                            response.userInformation.idleTime = 0;
                        }

                        this.saveUserInfo(response.userInformation);
                    }
                    else {
                        this.props.history.push("/maintenance");
                    }

                    hideLoadMask();
                }, {onServerCallError: this.props.history.push('/maintenance')} );

                // this.props.history.push(`/glyph-viewer/shared/${this.props.shareID}`);
            }
            else if(!this.props.shareLinkStatus && this.state.done === true && this.state.status === false){
                this.props.history.push('/login');
            }
        }, 10000);
    }

    componentWillMount(){
        this.props.dispatch(resetShareLinkData());
    }

    async checkSharedHash(vars) {
        let context = this;
        //server call to check if the hash path entered actually exists in the remote backend
            makeServerCall('authShareLink',
                function(res,b,c) {
                    if (typeof res === 'string') res = JSON.parse(res);
                    
                    console.log(res);
                    console.log(res.body.authShare);
    
                    if(res.body && res.body.authShare){
                        context.props.dispatch(sharedLink(true));
                        context.setState({status: true});
                        context.props.dispatch(setCurrentVizParams_2({
							originalVizName: res.body.originalVizName, 
							tableName: res.body.tableName,
							datasourceId: res.body.datasourceId,
							query: res.body.query, 
							filterAllowedColumnList: res.body.filterAllowedColumnList, 
							sdtPath: res.body.sdtPath, 
							frontEndFilters: res.body.frontEndFilters
						}));
					// context.props.dispatch(editModalDisplay(false));
                    }
                    else if(res.body && !res.body.authShare){
                        context.props.dispatch(sharedLink(false));
                    }
                
                },
                {
                    post: true, 
                    data:  { 
                        id: vars.params.id
                    }
                }
            );
      }

    saveUserInfo(userInfo){
        //console.log('Success');
        // this.setState({ openPassword: false });
        this.props.dispatch(saveUserInfo_2(userInfo));

        // Call function post login if provided.
        // if (typeof this.props.doAfterLogin === 'function') {
        //     this.props.doAfterLogin(userInfo);
        // }

        // Redirect to home page.
        this.props.history.push(`/glyph-viewer/${this.props.shareID}`);
        // this.props.history.push("/glyph-viewer");
    }    

    render() {
        
        return (
            <div style = {{
                height: "100%", 
                width: "100%", 
                fontSize: "35px", 
                backgroundColor: "white"}}
            >
                <h1 style={{
                    textAlign: "center", 
                    color:"black", 
                    margin: "0px", 
                    paddingTop: "255px"}}
                >LOADING...</h1>

            </div>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const sharedLink = (t_Or_F) => ({
    type: 'SHARE_LINK_REDIRECT',
    t_Or_F,
});

export const setShareID = (id) => ({
    type: 'SET_SHARE_ID',
    id,
});

export const resetShareLinkData = () => ({
    type: 'RESET_LINK_REDIRECT'
});

export const saveUserInfo_2 = (userInfo) => ({
    type: 'SAVE_USER_INFO_ONLY',
    userInfo,
});

export const editModalDisplay_2 = (allViewsModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    allViewsModal,
});

export const setCurrentVizParams_2 = (vizParams) => ({
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
    sharedLinkStatus: state.filterState.sharedLinkStatus,
    shareID: state.filterState.shareID
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null)(ShareLoading));
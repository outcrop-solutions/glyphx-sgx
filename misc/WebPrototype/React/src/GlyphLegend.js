import React from 'react';
import { connect } from 'react-redux';
import { makeServerCall } from './ServerCallHelper.js';
import FontIcon from 'material-ui/FontIcon';
import Rnd from 'react-rnd';
import './css/StatisticModal.css';


/**
 * Draggable glyph legend
 * @param {function(bool)} handleCorrection: hide/show an invisible div over iframe
 */
class GlyphLegend extends React.Component {

    state = {
        width: 300,
        height: 200,
        imgPath: ''
    }

    componentDidMount() {
        var context = this;

        // debugger;
        
        makeServerCall(window.encodeURI('getLegendURL/' + this.props.VizParams.sdtPath ),
            function (responseText) { 

                // debugger;
                //console.log("RESPONSE TEXT");
                //console.log(responseText);

                //var imgPath = "../" + responseText.split("/WebViewer/")[1];

                //console.log(imgPath);

                context.setState({ imgPath: responseText });
            }
        );
        
        
    }

    render() {

        //var imgsrc = window.SERVER_URL + "getLegendImg/" + this.props.VizParams.sdtPath;
        //var imgsrc = window.SERVER_URL + "getLegendImg/" + window.encodeURIComponent(this.props.VizParams.sdtPath);

        var imgsrc = '';

        if (this.state.imgPath != '') {
            imgsrc = window.SERVER_URL + "getLegendImg/" + window.encodeURIComponent(this.state.imgPath);
        }

        // debugger;

        return (
            <Rnd 
                default = {{ x: 60, y: 10, width: this.state.width, height: this.state.height }}
                minWidth = { 150 }
                minHeight = { 100 }
                z = { 1000 }
                enableResizing = {{ top: false, right: false, bottom: false, left: false, topRight: false, bottomRight: true, bottomLeft: true, topLeft: false }}
                style = {{
                    display: ( this.props.legendDisplay ? "block" : "none" )
                }}
                onDragStart = { () => this.props.handleCorrection(true) }
                onDragStop = { () => this.props.handleCorrection(false) }
                onResizeStart = { () => this.props.handleCorrection(true) }
                onResizeStop = { () => this.props.handleCorrection(false) }
                onResize = {(e, direction, ref, delta, position) => {
                    this.setState({
                        width: (ref.offsetHeight * 1.5),
                        height: ref.offsetHeight
                    });
                }}
                bounds = "parent"
            >
                <div className = "statisticsBox" style = {{ height: "inherit", borderRadius: "7px" }} >

                    <div style = {{ backgroundColor: this.props.settings.colors.statisticsDragColor.topBar, height: "26px", color: "#ffffff", borderTopRightRadius: "5px", borderTopLeftRadius: "5px" }} >
                        <label style = {{ margin: "6px 0px 0px 5px", float: "left" }} > LEGEND </label>
                        <FontIcon 
                            className = "fa fa-window-close cursorHand" 
                            style = {{ color: "#ffffff",  fontSize: "25px", margin: "1px 0px 0px 0px", float: "right", paddingRight: "2px" }} 
                            onClick = { () => this.props.dispatch(editModalDisplay(false)) } 
                        />
                        <FontIcon 
                            className = "fa fa-external-link cursorHand" 
                            style = {{ color: "#ffffff",  fontSize: "24px", margin: "2px 3px 0px 0px", float: "right", paddingRight: "2px" }} 
                            onClick = { () => window.open(window.SERVER_URL + "getLegendImg/" + window.encodeURIComponent(this.state.imgPath)) } 
                        />
                    </div>

                    <div style = {{ width: "100%", height: "100%", padding: "3px" }} >
                        
                        <img 
                            //src = { this.state.imgPath } 
                            src = { imgsrc } 
                            style = {{ width: '100%', height: "calc(100% - 26px)", borderRadius: "3px" }} 
                            alt = "Legend" 
                            className = "legendImage noselect" 
                            draggable = { false } 
                        />
                    </div>

                </div>
            </Rnd>
        );
    }
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (legendModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    legendModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    legendDisplay: state.filterState.ModalDisplay.legendModal,
    VizParams: state.filterState.VizParams,
    userInfo: state.filterState.UserInfo,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(GlyphLegend);
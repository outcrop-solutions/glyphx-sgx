import React from 'react';
import { connect } from 'react-redux';
import FontIcon from 'material-ui/FontIcon';
import Rnd from 'react-rnd';
import 'font-awesome/css/font-awesome.min.css';
import './statisticModal.css';

class GlyphLegend extends React.Component {

    render() {
        return (
            <Rnd
                default = {{
                    x: 10,
                    y: 10,
                    width: 324,
                    height: 236,
                }}
                enableResizing = {{ top: false, right: false, bottom: false, left: false, topRight: false, bottomRight: false, bottomLeft: false, topLeft: false }}
                style = {{
                    display: ( this.props.legendDisplay ? "block" : "none" )
                }}
                bounds = "parent"
            >
                <div className = "statisticsBox" style = {{ height: "inherit" }} >

                    <div style = {{ backgroundColor: this.props.settings.colors.statisticsDragColor.topBar, height: "26px", color: "#ffffff", borderTopRightRadius: "5px", borderTopLeftRadius: "5px" }} >
                        <label style = {{ margin: "6px 0px 0px 5px", float: "left" }} > LEGEND </label>
                        <FontIcon 
                            className = "fa fa-window-close cursorHand" 
                            style = {{ color: "#ffffff",  fontSize: "21px", margin: "2px 0px 0px 0px", float: "right", paddingRight: "2px" }} 
                            onClick = { () => this.props.dispatch(editModalDisplay(false)) } 
                        />
                    </div>
                    <table style = {{ verticalAlign: "middle", textAlign: "center" }} >
                        <tbody>
                            <tr>
                                <img src = "./Res/Img/SampleLegend.png" style = {{ width: '320px' }} alt = "Legend" className = "legendImage" />
                            </tr>
                        </tbody>
                    </table>
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
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(GlyphLegend);
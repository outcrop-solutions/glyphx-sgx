import React from 'react'
import { connect } from 'react-redux';
import { Card, CardText } from 'material-ui/Card';
import Flexbox from 'flexbox-react';
import FloatingActionButton from 'material-ui/FloatingActionButton';
import Toggle from 'material-ui/Toggle';
import Tooltip from 'rc-tooltip';
import Slider from 'rc-slider/lib/Slider'; 
import 'rc-slider/assets/index.css';
import 'rc-tooltip/assets/bootstrap.css';


/**
 * -ADCMT
 * @param topNavBarHeight: -ADCMT
 */
class FloatingToggleButtons extends React.Component {

	state = {
		fullScreenMode: false,
        axisVisible: true,
        smallObjectWindow: false,
        smallObjectToggle: true,
        smallObjectValue: 200
    };

    


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
        
	}
	

	/**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {
		// Removing listener so it doesnt linger across the site
		//document.onkeydown = null;
	}

    componentWillReceiveProps(nextProps) {
        if (nextProps.iframeLoaded) {
            var iframe = document.getElementById('GlyphViewer').contentWindow;
            iframe.setSuperImposed(this.state.smallObjectToggle, this.state.smallObjectValue);
        }
    }


    /*
    handleKeyDown(e) {
        if (e.keyCode === 27 || e.keyCode === 122) {
            debugger;
            this.setState({ fullScreenMode: false });
        }
    }
    */
    
	
	/**
	 * This function opens/closes the floating menu on the bottom left corner.
	 */
    openCloseFloatingMenu() {
        var menuItems = document.getElementsByClassName('toggleOptionsMenuItems');
        var floatingToggleButton = document.getElementById("floatingToggleButton");
        var buttonElement = document.getElementById("collapsibleFloatingButton");
        var len = menuItems.length;
        var translate = 50; // as the 1st menu button should be little more higher than the spacing between the buttons.

        if (floatingToggleButton.classList.contains('fa-caret-up')) {
            floatingToggleButton.classList.remove('fa-caret-up');
            floatingToggleButton.classList.add('fa-caret-down');
            floatingToggleButton.style.margin = "2px 0px 0px 0px";
            buttonElement.style.backgroundColor = this.props.settings.colors.overviewButtonsColor.background;
            this.setState({ smallObjectWindow: false });
        }
        else {
            floatingToggleButton.classList.remove('fa-caret-down');
            floatingToggleButton.classList.add('fa-caret-up');
            floatingToggleButton.style.margin = "-2px 0px 0px 0px";
            buttonElement.style.backgroundColor = this.props.settings.colors.overviewButtonsColor.altBackground;
        }

        if (this.state.menuOpen) {
            for (var i = 0; i < len; i++) {
                menuItems[i].style.transform = '';
            }

            this.setState({ menuOpen: false });
        }
        else {
            for (var j = 0; j < len; j++) {
                menuItems[j].style.transform = 'translate(0px,' + translate + 'px)';
                translate = translate + 50;
            }

            this.setState({ menuOpen: true });
        }
    }
    


	/**
	 * This function toggles the full screen mode of the glyphviewer(iframe).
	 * @param {object} e: event object.
	 */
	toggleFullScreenMode(e) {
		var filterNav = document.getElementById("filterNav");
        var gv = document.getElementById('GlyphViewerContainer');
		var iconDiv = e && e.currentTarget ? e.currentTarget.querySelector('.fa') : null;
        var sidebarButton = document.getElementById("showSideBar");

        //debugger;

        if ((document.fullScreenElement !== undefined && document.fullScreenElement === null) || 
            (document.msFullscreenElement !== undefined && document.msFullscreenElement === null) || 
            (document.mozFullScreen !== undefined && !document.mozFullScreen) || 
            (document.webkitIsFullScreen !== undefined && !document.webkitIsFullScreen) ) {

            // Hide the filterSideNav if visible
            filterNav.style.transform = "translate(460px, 0px)";
			
			// Update the glyphviewer
            gv.style.width = "100%";
			
            if (iconDiv) {
				iconDiv.classList.remove('fa-arrows-alt');
				iconDiv.classList.add('fa-compress');
			}

            sidebarButton.style.display = "";

            var elem = document.body;
            if (elem.requestFullscreen) {
                elem.requestFullscreen();
            } 
            else if (elem.msRequestFullscreen) {
                elem.msRequestFullscreen();
            } 
            else if (elem.mozRequestFullScreen) {
                elem.mozRequestFullScreen();
            } 
            else if (elem.webkitRequestFullscreen) {
                elem.webkitRequestFullscreen();
            }

            this.setState({ fullScreenMode: true });
        }

        else {
            // Update the glyphviewer
            if (!this.props.settings.sideBarOverlap) {
                gv.style.width = "calc(100% - 450px)";
            }

            else {
                gv.style.width = "100%";
            }

            filterNav.style.transform = "translate(0px, 0px)";

			if (iconDiv) {
				iconDiv.classList.add('fa-arrows-alt');
			    iconDiv.classList.remove('fa-compress');
			}

            sidebarButton.style.display = "none";

            if (document.exitFullscreen) {
                document.exitFullscreen();
            } 
            else if (document.webkitExitFullscreen) {
                document.webkitExitFullscreen();
            } 
            else if (document.mozCancelFullScreen) {
                document.mozCancelFullScreen();
            } 
            else if (document.msExitFullscreen) {
                document.msExitFullscreen();
            }

            this.setState({ fullScreenMode: false });
        }
    }
    

    /**
	 * Toggles visibility of the draggable glyph legend
	 */
    toggleLegend() {
		if (this.props.legendDisplay) {
            this.props.dispatch(editModalDisplay(false, null));
        }

        else {
            this.props.dispatch(editModalDisplay(true, null));
        }
    }

    onSmallObjectToggle() {
        var iframe = document.getElementById('GlyphViewer').contentWindow;
        iframe.setSuperImposed(!this.state.smallObjectToggle, this.state.smallObjectValue);

        this.setState({ smallObjectToggle: !this.state.smallObjectToggle });
    }

    onAfterSlide() {
        this.props.handleDraggableCorrection(false);
        var iframe = document.getElementById('GlyphViewer').contentWindow;
        iframe.setSuperImposed(this.state.smallObjectToggle, this.state.smallObjectValue);
    }


    render() {
        return (
            <div>
                {/* Main Floating Button */}

                <Card 
                    style = {{ 
                        display: (this.state.smallObjectWindow ? "" : "none"),
                        position: "absolute", 
                        backgroundColor: "#fff", 
                        width: "175px", 
                        height: "3.962vh", 
                        top: "26.799vh", 
                        left: "5.735vh", 
                        borderRadius: "5px" 
                    }} 
                    containerStyle = {{ padding: "0px", borderRadius: "5px" }}
                >
                    <CardText style = {{ padding: "0.313vh 0.834vh", borderRadius: "5px" }} >
                        <div>
                            <Flexbox flexDirection = "row" style = {{ width: "100%" }} >
                                <Flexbox style = {{ width: "40%", margin: "4px 0px 0px -9px" }} >
                                    <Toggle 
                                        toggled = { this.state.smallObjectToggle } 
                                        onToggle = { () => this.onSmallObjectToggle() }
                                        trackStyle = {{ height: "12px", marginTop: "2px", width: "84%" }}
                                        trackSwitchedStyle = {{ backgroundColor: this.props.settings.colors.rangeColor.toggleTrack }}
                                        thumbStyle = {{ height: "17px", width: "17px", top: "3px" }}
                                        thumbSwitchedStyle = {{ backgroundColor: this.props.settings.colors.rangeColor.toggleCircle, left: "40%" }}
                                    />
                                </Flexbox>

                                <Flexbox style = {{ width: "30%", margin: "5px 0px 0px" }} >
                                    <div style = {{ fontWeight: "bold", fontSize: "18px", textAlign: "center" }} > {this.state.smallObjectValue} </div>
                                </Flexbox>

                                <Flexbox style = {{ width: "60%", margin: "7px 7px 0px 0px" }} >
                                    <Slider
                                        min = { 25 }
                                        max = { 350 }
                                        step = { 1 }
                                        value = { this.state.smallObjectValue }
                                        onChange = { (e) => this.setState({ smallObjectValue: e }) }
                                        onBeforeChange = { () => this.props.handleDraggableCorrection(true) }
                                        onAfterChange = { () => this.onAfterSlide() }
                                        railStyle = {{ backgroundColor: "#d2d2d2" }}
                                        trackStyle = { [{ backgroundColor: this.props.settings.colors.rangeColor.sliderTrack }] }
                                        handleStyle = { [{ backgroundColor: this.props.settings.colors.rangeColor.sliderCircle, borderColor: this.props.settings.colors.rangeColor.sliderCircle }] }
                                    />
                                </Flexbox>
                            </Flexbox>
                        </div>
                    </CardText>
                </Card>

                <Tooltip
                    placement = 'right'
                    mouseEnterDelay = { 0.5 }
                    mouseLeaveDelay = { 0.15 }
                    destroyTooltipOnHide = { false }
                    trigger = { Object.keys( {hover: 1} ) }
                    overlay = { 
                        <div> 
                            Expand Toolbar Options
                        </div> 
                    }
                >
                    <FloatingActionButton 
                        id = "collapsibleFloatingButton"
                        backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                        style = {{
                            top: '8px',
                            left: '5px',
                            position: 'absolute',
                            zIndex: '10',
                        }} 
                        mini = { true }
                        onClick = { this.openCloseFloatingMenu.bind(this) }
                    >
                        <i 
                            id = "floatingToggleButton" 
                            className = "fa fa-caret-down" 
                            style = {{
                                fontSize: '1.8em',
                                color: this.props.settings.colors.collapsibleColor.mainIcon,
                                //transform: 'rotateZ(90deg)',
                                margin: "2px 0px 0px 0px"
                            }}
                        /> 
                    </FloatingActionButton>
                </Tooltip>

                {/* Mini Floating Buttons */}
                <Tooltip
                    placement = 'right'
                    mouseEnterDelay = { 0.5 }
                    mouseLeaveDelay = { 0.15 }
                    destroyTooltipOnHide = { false }
                    trigger = { Object.keys( {hover: 1} ) }
                    overlay = { 
                        <div> 
                            Fullscreen Mode
                        </div> 
                    }
                >
                    <FloatingActionButton 
                        backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                        style = { styles.floatingMiniStyles } 
                        className = "toggleOptionsMenuItems"
                        mini = { true }
                        onClick = { this.toggleFullScreenMode.bind(this) }
                    >
                        <i className = "fa fa-arrows-alt" style = {{ fontSize: "1.668vh", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                    </FloatingActionButton>
                </Tooltip>

                <Tooltip
                    placement = 'right'
                    mouseEnterDelay = { 0.5 }
                    mouseLeaveDelay = { 0.15 }
                    destroyTooltipOnHide = { false }
                    trigger = { Object.keys( {hover: 1} ) }
                    overlay = { 
                        <div> 
                            View Legend
                        </div> 
                    }
                >
                    <FloatingActionButton 
                        backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                        style = { styles.floatingMiniStyles } 
                        className = "toggleOptionsMenuItems"
                        mini = { true }
                        onClick = { this.toggleLegend.bind(this) }
                    >
                        <span className = "fa-stack fa-lg noselect" style = {{ margin: "2px 0px 0px -3px" }} >
                            <i className = "fa fa-cube fa-stack-2x" style = {{ fontSize: '1.2rem', margin: "11px 0px 0px 0px", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                            <i 
                                className = { this.props.legendDisplay ? "fa fa-search-minus fa-stack-1x" : "fa fa-search-plus fa-stack-1x" } 
                                style = {{ fontSize: "1.668vh", margin: "-10px 0px 0px 8px", color: this.props.settings.colors.collapsibleColor.mainIcon }} 
                            />
                        </span>
                    </FloatingActionButton>
                </Tooltip>

                <Tooltip
                    placement = 'right'
                    mouseEnterDelay = { 0.5 }
                    mouseLeaveDelay = { 0.15 }
                    destroyTooltipOnHide = { false }
                    trigger = { Object.keys( {hover: 1} ) }
                    overlay = { 
                        <div> 
                            Change Axes
                        </div> 
                    }
                >
                    <FloatingActionButton 
                        backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                        style = { styles.floatingMiniStyles } 
                        className = "toggleOptionsMenuItems"
                        mini = { true }
                        onClick = { () => this.props.dispatch(editModalDisplay(null, true)) }
                    >
                        <span className = "fa-stack fa-lg noselect" style = {{ margin: "2px 0px 0px -5px" }} >
                            <i className = "fa fa-arrows-h fa-stack-3x" style = {{ fontSize: '1.2rem', margin: "14px -7px 0px 0px", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                            <i className = "fa fa-arrows-v fa-stack-2x" style = {{ fontSize: '1.2rem', margin: "8px -3px 0px", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                            <i 
                                className = { "fa fa-map fa-stack-1x" } 
                                style = {{ fontSize: '0.7rem', margin: "-8px 0px 0px 8px", color: this.props.settings.colors.collapsibleColor.mainIcon }} 
                            />
                        </span>
                    </FloatingActionButton>
                </Tooltip>

                <Tooltip
                    placement = 'right'
                    mouseEnterDelay = { 0.5 }
                    mouseLeaveDelay = { 0.15 }
                    destroyTooltipOnHide = { false }
                    trigger = { Object.keys( {hover: 1} ) }
                    overlay = { 
                        <div> 
                            Small Object Culling
                        </div> 
                    }
                >
                    <FloatingActionButton 
                        backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                        style = { styles.floatingMiniStyles } 
                        className = "toggleOptionsMenuItems"
                        mini = { true }
                        onClick = { () => this.setState({ smallObjectWindow: !this.state.smallObjectWindow }) }
                    >
                        <span className = "fa-stack fa-lg noselect" style = {{ margin: "2px 0px 0px -3px" }} >
                            <i className = "fa fa-cubes fa-stack-2x" style = {{ fontSize: '1.668vh', margin: "9px 0px 0px -2px", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                            <i 
                                className = "fa fa-arrows-v fa-stack-1x"
                                style = {{ fontSize: "1.668vh", margin: "-3px 0px 0px 12px", color: this.props.settings.colors.collapsibleColor.mainIcon }} 
                            />
                        </span>
                    </FloatingActionButton>
                </Tooltip>

            </div>
        );
    }
}


/**
 * Local styling
 **/
const styles = {
	floatingMiniStyles: {
		top: '56px',
		left: "5px",
		position: 'absolute',
		zIndex: '5',
        transition: '0.5s'
	}
};


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (legendModal, XYZModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    legendModal,
    XYZModal
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
    userInfo: state.filterState.UserInfo,
    legendDisplay: state.filterState.ModalDisplay.legendModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(FloatingToggleButtons);
import React from 'react'
import { connect } from 'react-redux';
import FloatingActionButton from 'material-ui/FloatingActionButton';
import Tooltip from 'rc-tooltip';
import 'rc-tooltip/assets/bootstrap.css';


/**
 * -ADCMT
 * @param topNavBarHeight: -ADCMT
 */
class FloatingToggleButtons extends React.Component {

	state = {
		fullScreenMode: false,
        axisVisible: true
    };


    /**
	 * React built-in which is called when component mounts
	 */
	componentDidMount() {
        // Check if he axis is visible and set axisVisible appropriately in the state
        //var iframe = document.getElementById('GlyphViewer').contentWindow;
        //this.setState({ axisVisible: iframe.isAxisVisible() });

		// Mouseup listener used to handle click drag selection
		//document.onkeydown = this.handleKeyDown.bind(this);
	}
	

	/**
	 * React built-in which is called when component unmounts
	 */
	componentWillUnmount() {
		// Removing listener so it doesnt linger across the site
		//document.onkeydown = null;
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


    render() {
        return (
            <div>
                {/* Main Floating Button */}
                <Tooltip
                    placement = 'right'
                    mouseEnterDelay = { 0.5 }
                    mouseLeaveDelay = { 0.15 }
                    destroyTooltipOnHide = { false }
                    trigger = { Object.keys( {hover: 1} ) }
                    overlay = { 
                        <div> 
                            Expand toolbar options.
                        </div> 
                    }
                >
                    <FloatingActionButton 
                        id = "collapsibleFloatingButton"
                        backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                        style = {{
                            top: '56px',
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
                            Fullscreen mode.
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
                        <i className = "fa fa-arrows-alt" style = {{ fontSize: '1rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
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
                            View glyph legend.
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
                                style = {{ fontSize: '1rem', margin: "-10px 0px 0px 8px", color: this.props.settings.colors.collapsibleColor.mainIcon }} 
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
                            Change Axes.
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
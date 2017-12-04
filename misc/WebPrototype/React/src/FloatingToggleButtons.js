import React from 'react'
import { connect } from 'react-redux';
import FloatingActionButton from 'material-ui/FloatingActionButton';


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
        var len = menuItems.length;
        var translate = 70; // as the 1st menu button should be little more higher than the spacing between the buttons.

        if (floatingToggleButton.style.transform == '') {
            floatingToggleButton.style.transform = 'rotateZ(180deg)';
        }
        else {
            floatingToggleButton.style.transform = '';
        }

        if (this.state.menuOpen) {
            for (var i = 0; i < len; i++) {
                menuItems[i].style.transform = '';
            }

            this.setState({ menuOpen: false });
        }
        else {
            for (var j = 0; j < len; j++) {
                menuItems[j].style.transform = 'translate(0px,-' + translate + 'px)';
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
            this.props.dispatch(editModalDisplay(false));
        }

        else {
            this.props.dispatch(editModalDisplay(true));
        }
    }


    toggleAxis() {
        if (document.getElementById('GlyphViewer')) {
            document.getElementById('GlyphViewer').contentWindow.postMessage({action: 'toggleAxis'}, '*');
            this.state.axisVisible ? this.setState({ axisVisible: false }) : this.setState({ axisVisible: true });
        }
    }

	
    render() {
        return (
            <div>
                {/* Main Floating Button */}
                <FloatingActionButton 
                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                    style = {{
                        bottom: '25px',
                        left: '28px',
                        position: 'absolute',
                        zIndex: '10',
                        transition: '0.5s'
                    }} 
                    mini = { true }
                    onClick = { this.openCloseFloatingMenu.bind(this) }
                >
                    <i 
                        id = "floatingToggleButton" 
                        className = "fa fa-caret-up" 
                        style = {{
                            fontSize: '1.8em',
                            color: this.props.settings.colors.collapsibleColor.mainIcon,
                            transition: 'transform 500ms'
                        }}
                    /> 
                </FloatingActionButton>

                {/* Mini Floating Buttons */}
                <FloatingActionButton 
                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                    style = { styles.floatingMiniStyles } 
                    className = "toggleOptionsMenuItems"
                    mini = { true }
                    onClick = { this.toggleFullScreenMode.bind(this) }
                >
                    <i className = "fa fa-arrows-alt" style = {{ fontSize: '1rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                </FloatingActionButton>

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

                <FloatingActionButton 
                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                    style = { styles.floatingMiniStyles } 
                    className = "toggleOptionsMenuItems"
                    mini = { true }
                    onClick = { this.toggleAxis.bind(this) }
                >
                    <span className = "fa-stack fa-lg noselect" style = {{ margin: "2px 0px 0px -5px" }} >
                        <i className = "fa fa-arrows-h fa-stack-3x" style = {{ fontSize: '1.2rem', margin: "14px -7px 0px 0px", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                        <i className = "fa fa-arrows-v fa-stack-2x" style = {{ fontSize: '1.2rem', margin: "8px -3px 0px", color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                        <i 
                            className = { this.state.axisVisible ? "fa fa-eye fa-stack-1x" : "fa fa-eye-slash fa-stack-1x" } 
                            style = {{ fontSize: '0.8rem', margin: "-6px 0px 0px 7px", color: this.props.settings.colors.collapsibleColor.mainIcon }} 
                        />
                    </span>
                </FloatingActionButton>

            </div>
        );
    }
}


/**
 * Local styling
 **/
const styles = {
	floatingMiniStyles: {
		bottom: '25px',
		left: "28px",
		position: 'absolute',
		zIndex: '5',
		transition: '0.5s'
	}
};


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
    userInfo: state.filterState.UserInfo,
    legendDisplay: state.filterState.ModalDisplay.legendModal,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(FloatingToggleButtons);
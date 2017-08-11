import React from 'react'
import { connect } from 'react-redux';
import FloatingActionButton from 'material-ui/FloatingActionButton';
import 'font-awesome/css/font-awesome.min.css';

class FloatingToggleButtons extends React.Component {
    
	//initial state of the component.
	state = {
		fullScreenMode: false,
    };
    
	
	/**
	 * This function opens/closes the floating menu on the bottom left corner.
	 */
    openCloseFloatingMenu() {
        var menuItems = document.getElementsByClassName('toggleOptionsMenuItems');
        var len = menuItems.length;
        var translate = 70; // as the 1st menu button should be little more higher than the spacing between the buttons.

        if( this.state.menuOpen) {
            for (var i = 0; i < len; i++) {
                menuItems[i].style.transform = '';
            }
            this.setState({ menuOpen: false });
        }
        else {
            for (var j = 0; j < len; j++) {
                menuItems[j].style.transform = 'translate(0px,-'+translate+'px)';
                translate = translate + 50;
            }
            this.setState({ menuOpen: true });
        }
    }


	/**
	 * This function toggles the full screen mode of the glyphviewer(iframe).
	 * @param {object} event- event object.
	 */
	toggleFullScreenMode(e){
        var topNav = document.getElementById('TopNav');
		var filterNav = document.getElementById("filterNav");
        var gv = document.getElementById('GlyphViewerContainer');
		var iconDiv = e && e.currentTarget ? e.currentTarget.querySelector('.fa') : null;
		
		if (!this.state.fullScreenMode) {

			// Hide the filterSideNav if visible
            filterNav.style.transform = "translate(460px, 0px)";
			
			// Update the glyphviewer
            gv.style.width = "100%";
            topNav.style.height = '0px';
			topNav.style.overflow = 'hidden';
			
            if (iconDiv) {
				iconDiv.classList.remove('fa-arrows-alt');
				iconDiv.classList.add('fa-compress');
			}

			this.setState({fullScreenMode: true});
		}
		else {
			//Update the glyphviewer
            topNav.style.height = this.props.topNavBarHeight + "px";
			topNav.style.overflow = '';

			if (iconDiv) {
				iconDiv.classList.add('fa-arrows-alt');
			    iconDiv.classList.remove('fa-compress');
			}
			
			this.setState({fullScreenMode: false});
		}
    }

    toggleLegend(){
		if (this.props.legendDisplay) {
            this.props.dispatch(editModalDisplay(false));
        }
        else {
            this.props.dispatch(editModalDisplay(true));
        }
    }
	

    render() {
        

        return (
            <div>
                {/* Main Floating Button */}
                <FloatingActionButton 
                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                    style = {{
                        bottom: '20px',
                        left: '20px',
                        position: 'absolute',
                        zIndex: '10',
                        transition: '0.5s'
                    }} 
                    onClick = { this.openCloseFloatingMenu.bind(this) }
                >
                    <i className = "fa fa-pencil" style = {{ fontSize: '1.3rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
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
                {/*//fa-eye-slash(for the alternate toggle icon) */}
                    <i className = "fa fa-eye" style = {{ fontSize: '1rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                </FloatingActionButton>

                <FloatingActionButton 
                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                    style = { styles.floatingMiniStyles } 
                    className = "toggleOptionsMenuItems"
                    mini = { true }
                >
                    <i className = "fa fa-video-camera" style = {{ fontSize: '1rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
                </FloatingActionButton>

                <FloatingActionButton 
                    backgroundColor = { this.props.settings.colors.overviewButtonsColor.background }
                    style = { styles.floatingMiniStyles } 
                    className = "toggleOptionsMenuItems"
                    mini = { true }
                >
                    <i className = "fa fa-pencil" style = {{ fontSize: '1rem', color: this.props.settings.colors.collapsibleColor.mainIcon }} />
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
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(FloatingToggleButtons);


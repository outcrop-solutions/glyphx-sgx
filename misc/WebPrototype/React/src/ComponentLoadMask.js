import React from 'react';
import CircularProgress from 'material-ui/CircularProgress';
import Flexbox from 'flexbox-react';
import './General.css';


/**
 * Load mask which takes the full size of the parent component.
 */
class ComponentLoadMask extends React.Component {

	render() {
		return(

            <Flexbox flexDirection = "column" alignSelf = "center"  style = {{ height: "100%", width: "100%", backgroundColor: "white" }}>
                <Flexbox style = {{ width: "80px", margin: "0 auto", alignItems: "center", height: "100%" }} >
                    <CircularProgress size = { 80 } thickness = { 5 } color = { this.props.color } /> 
                </Flexbox>
            </Flexbox>

		);
	}
}



/**
 * Connects the redux store to get access to global states.
 **/
export default ComponentLoadMask;
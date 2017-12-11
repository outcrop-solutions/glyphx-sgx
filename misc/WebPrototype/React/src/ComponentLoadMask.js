import React from 'react';
import CircularProgress from 'material-ui/CircularProgress';
import Flexbox from 'flexbox-react';
import './General.css';


/**
 * Load mask which takes the full size of the parent component.
 * @param color: color of the spinner
 * @param bgColor: color of the background
 */
class ComponentLoadMask extends React.Component {

	render() {
		return(

            <Flexbox flexDirection = "column" alignSelf = "center"  style = {{ height: "100%", width: "100%", backgroundColor: this.props.bgColor ? this.props.bgColor : "white" }} >
                <Flexbox style = {{ width: this.props.imgLink ? "300px" : "80px", margin: "0 auto", alignItems: "center", height: "100%" }} >
                    <Flexbox flexDirection = "column" >
                        {this.props.imgLink ?
                            <img src = { this.props.imgLink } style = {{ width: '300px', margin: "0 auto" }} alt = "GlyphEd" className = "noselect" draggable = { false } />
                            :
                            null
                        }
                        <div style = {{ margin: "0 auto" }} >
                            <CircularProgress size = { 80 } thickness = { 5 } color = { this.props.color } /> 
                        </div>
                    </Flexbox>
                </Flexbox>
            </Flexbox>

		);
	}
}



/**
 * Connects the redux store to get access to global states.
 **/
export default ComponentLoadMask;
import React from 'react';
import CircularProgress from 'material-ui/CircularProgress';
import Flexbox from 'flexbox-react';
import './General.css';

var loadingMessages = [
    {text: "Hang on... We're beaming your data back down to earth...", textShadow: ""},
    {text: "Hold on its not our fault WebGL isn't as fast as the desktop...", textShadow: ""},
    {text: "Two Glyphs walk into a bar...", textShadow: ""},
    {text: "The bar chart was invented in 1781...", textShadow: ""},
    {text: "Say GlyphEd 3 times fast...", textShadow: ""},
    {text: "We’re just waiting on your computer...", textShadow: ""},
    {text: "Imagine doing this in Excel...", textShadow: ""},
    {text: "PUT ON YOUR 3D GOGGLES NOW...", textShadow: "-0.12em 0 red, 0.12em 0 cyan"},
    {text: "Thank you for deleting your data! Just kidding...", textShadow: ""},
    {text: "This isn't exactly easy...", textShadow: ""},
    {text: "Just a second we have to hand draw these...", textShadow: ""},
    {text: "Using Glyphs twice a day can make you fly... through data...", textShadow: ""},
    {text: "Knock knock... Who’s there... Glyphs hopefully...", textShadow: ""},
    {text: "We're stuck... Can you get out and push...", textShadow: ""},
    {text: "Buckle up... Its going to get bumpy...", textShadow: ""},
    {text: "Hey, I'm still doing this faster than you...", textShadow: ""},
    {text: "Our Glyph artist is on coffee break, please wait...", textShadow: ""},
    {text: "Dont refresh, I'm trying my best...", textShadow: ""},
    {text: "Hang on a sec, I know your data is here somewhere...", textShadow: ""},
    {text: "Its raining Glyphs! Hallelujah!", textShadow: ""}

]

/**
 * Load mask which takes the full size of the parent component.
 * @param color: color of the spinner
 * @param bgColor: color of the background
 */
class ComponentLoadMask extends React.Component {

    state = {
        position: 0
    }


    componentDidMount() {
        if ( this.props.imgLink ) {
            for (let i = loadingMessages.length - 1; i > 0; i--) {
                const j = Math.floor(Math.random() * (i + 1));
                [loadingMessages[i], loadingMessages[j]] = [loadingMessages[j], loadingMessages[i]];
            }

            var context = this;

            setInterval(function(){
                var currentPosition = context.state.position;
                if (currentPosition + 1 > loadingMessages.length - 1) {
                    currentPosition = -1;
                }
                context.setState({ position: ++currentPosition });
            }, 5000);
        }
    }

	render() {
		return(

            <Flexbox flexDirection = "column" alignSelf = "center"  style = {{ height: "100%", width: "100%", backgroundColor: this.props.bgColor ? this.props.bgColor : "white" }} >
                <Flexbox alignSelf = "center" style = {{ width: this.props.imgLink ? "1000px" : "80px", margin: "0 auto", alignItems: "center", height: "100%" }} >
                    <Flexbox flexDirection = "column" style = {{ width: "1000px" }} >
                        {this.props.imgLink ?
                            <div>
                                <div style = {{ width: "300px", margin: "0 auto" }} >
                                    <img src = { this.props.imgLink } style = {{ width: '300px', margin: "0 auto" }} alt = "GlyphEd" className = "noselect" draggable = { false } />
                                </div>
                                <div style = {{ width: "auto" }}>
                                    <div style = {{ textAlign: "center", textShadow: loadingMessages[this.state.position].textShadow, color: "#231f1f", fontWeight: "bold", fontSize: "22px", fontFamily: 'Verdana', marginBottom: "20px" }} > {loadingMessages[this.state.position].text} </div>
                                </div>
                            </div>
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
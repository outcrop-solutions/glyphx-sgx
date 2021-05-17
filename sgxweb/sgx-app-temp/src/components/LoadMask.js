import React, { useState, useEffect } from 'react';
import CircularProgress from '@material-ui/core/CircularProgress';
import Flexbox from 'flexbox-react';
import './General.css';

var loadingMessages = [
    {text: "Hang on... We're beaming your data back down to earth...", textShadow: ""},
    {text: "Two Glyphs walk into a bar...", textShadow: ""},
    {text: "The bar chart was invented in 1781...", textShadow: ""},
    {text: "Say GlyphEd 3 times fast...", textShadow: ""},
    {text: "We’re just waiting on your computer...", textShadow: ""},
    {text: "Imagine doing this in spreadsheets...", textShadow: ""},
    {text: "PUT ON YOUR 3D GOGGLES NOW...", textShadow: "-0.12em 0 red, 0.12em 0 cyan"},
    {text: "Thank you for deleting your data! Just kidding...", textShadow: ""},
    {text: "This isn't exactly easy...", textShadow: ""},
    {text: "Just a second... these are hand-drawn you know...", textShadow: ""},
    {text: "Using Glyphs twice a day can make you fly... through data...", textShadow: ""},
    {text: "A Glyph a day keeps the spreadsheets away...", textShadow: ""},
    {text: "Knock knock... Who’s there... Glyphs hopefully...", textShadow: ""},
    {text: "We're stuck... Can you get out and push...", textShadow: ""},
    {text: "Buckle up... It's going to get bumpy...", textShadow: ""},
    {text: "Hey, I'm still doing this faster than you...", textShadow: ""},
    {text: "Our Glyph artist is on coffee break. Please wait...", textShadow: ""},
    {text: "Don't refresh! I'm trying my best...", textShadow: ""},
    {text: "Hang on a sec, I know your data is here somewhere...", textShadow: ""},
    {text: "Its raining Glyphs! Hallelujah!", textShadow: ""}

]

/**
 * Load mask which takes the full size of the parent component.
 * @param color: color of the spinner
 * @param bgColor: color of the background
 */
export default function LoadMask(props) {

    const [position, setPosition] = React.useState(0);

    useEffect(() => {
        if (props.imgLink) {
            // console.log(this.props.imgLink, 'imgLink props');
            for (let i = loadingMessages.length - 1; i > 0; i--) {
                const j = Math.floor(Math.random() * (i + 1));
                [loadingMessages[i], loadingMessages[j]] = [loadingMessages[j], loadingMessages[i]];
            }
        }
    });


    return(
        <Flexbox flexDirection = "column" alignSelf = "center"  style = {{ height: "100%", width: "100%", backgroundColor: props.bgColor ? props.bgColor : "white" }} >
            <Flexbox alignSelf = "center" style = {{ width: props.imgLink ? "1000px" : "80px", margin: "0 auto", alignItems: "center", height: "100%" }} >
                <Flexbox flexDirection = "column" style = {{ width: "1000px" }} >
                    {props.imgLink ?
                        <div>
                            <div style = {{ width: "300px", margin: "0 auto" }} >
                                <img src = { props.imgLink } style = {{ width: '300px', margin: "0 auto" }} alt = "GlyphEd" className = "noselect" draggable = { false } />
                            </div>
                            <div style = {{ width: "auto" }}>
                                <div style = {{ textAlign: "center", textShadow: loadingMessages[position].textShadow, color: "#231f1f", fontWeight: "bold", fontSize: "22px", fontFamily: 'Verdana', marginBottom: "20px" }} > {loadingMessages[this.state.position].text} </div>
                            </div>
                        </div>
                        :
                        null
                    }
                    <div style = {{ margin: "0 auto" }} >
                        <CircularProgress size = { 80 } thickness = { 5 } /> 
                    </div>
                </Flexbox>
            </Flexbox>
        </Flexbox>
    );
}

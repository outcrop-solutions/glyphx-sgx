import React, { Component } from 'react';
import Dialog from 'material-ui/Dialog';
import {hideSplashScreen,showLoadMask,hideLoadMask} from './LoadMaskHelper.js';

class Maintenance extends Component {

    componentDidMount() {
        hideSplashScreen();
    }
    
    render() {
        return(

            <Dialog
                overlayStyle = {{ backgroundColor: 'white' }}
                contentStyle = {{ width:'40%', maxWidth: "none" }}
                modal = { true }
                bodyStyle = {{ boxShadow: 'rgb(99, 205, 255) 0px 0px 45px, rgb(24, 115, 208) 0px 1px 18px'}}
                open = { true }
                actions = {
                    [    ]
                }
            >   
            <center>
                <img src = "./Res/Img/SynGlyphX-inside.png" />
                <br/><br/>
                <label style = {{ height: '20px', fontSize: '18px' }} > 
                    This Site is under Maintenance.
                </label>
                <br/>
                <label  
                    style = {{ 
                        height: '20px',
                        fontSize: '18px'
                    }}
                > Please check again later.
                </label>
            </center>  
            </Dialog>
        )
    }
}

export default Maintenance;
import React from 'react';
import { hideSplashScreen } from './LoadMaskHelper.js';
import Dialog from 'material-ui/Dialog';


class Maintenance extends React.Component {

    componentDidMount() {
        hideSplashScreen();
    }
    
    render() {
        return(

            <Dialog
                overlayStyle = {{ backgroundColor: 'white' }}
                contentStyle = {{ width:'60%', maxWidth: "none" }}
                modal = { true }
                bodyStyle = {{ boxShadow: 'rgb(99, 205, 255) 0px 0px 45px, rgb(24, 115, 208) 0px 1px 18px'}}
                open = { true }
                actions = { [    ] }
            >   
                <div>
                    <div>
                        <img src = "./Res/Img/SynGlyphX-inside.png" alt = "SynGlyphX" style = {{ display: "block", margin: "0 auto" }} />
                    </div>

                    <br/><br/>

                    <div style = {{ height: '20px', fontSize: '18px', textAlign: "center" }} > 
                        This Site is under Maintenance.
                    </div>

                    <div style = {{ height: '20px', fontSize: '18px', textAlign: "center" }} > 
                        Please check again later.
                    </div>
                </div>  
            </Dialog>
        )
    }
}

export default Maintenance;
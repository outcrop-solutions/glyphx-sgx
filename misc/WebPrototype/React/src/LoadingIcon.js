import React, { Component } from 'react';
import CircularProgress from 'material-ui/CircularProgress';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';

class LoadingIcon extends Component {


    render() {
        return (
            <MuiThemeProvider>
                <div 
                    id="LoadingIconMask"  
                    style={{ 
                        position: 'absolute',    
                        width: '100%',
                        height: '100%',
                        zIndex: 1500,
                        backgroundColor: '#000000',
                        opacity: 0.5,
                        textAlign: 'center',
                        display: 'table'
                    }} >

                    <CircularProgress id="innerLoadingIconMask"
                        size={60} 
                        style={
                            {
                                    display: 'table-cell',
                                    verticalAlign: 'middle'
                            }
                        }
                        thickness={7} />

                </div>
            </MuiThemeProvider>
        )};
}


export default LoadingIcon;
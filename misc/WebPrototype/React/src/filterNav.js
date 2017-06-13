import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import Range from './range.js';
import 'font-awesome/css/font-awesome.css';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import './filterNav.css';

class FilterNav extends Component {
    render() {
        return (
            <MuiThemeProvider> 
                
                <div className="TopNav">
                    {/* TOP SECTION */}
                    HELLO I AM FILTER

                    {/* BOTTOM SECTION */}
                    <Range></Range>
                </div>

            </MuiThemeProvider>
        );
    }
}

export default FilterNav;
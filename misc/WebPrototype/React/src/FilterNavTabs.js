import React from 'react';
import {Tabs, Tab} from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import FontIcon from 'material-ui/FontIcon';
import FilterTable from './FilterTable.js';
import NumberRangeTable from './NumberRange.js';
import TextRangeTable from './TextRange.js';
import FilterViewForm from './filterView.js';
import { connect } from 'react-redux';

class FilterTabs extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            slideIndex: 0, //0 is Elastic, 1 is Range
        };
    };

    componentDidMount() {
        /*Events.scrollEvent.register('begin', function(to, element) {
        console.log("begin", arguments);
        });
    
        Events.scrollEvent.register('end', function(to, element) {
        console.log("end", arguments);
        });
        
        scrollSpy.update();
        */

    };

    handleChange = (value, context) => {
        context.setState({
            slideIndex: value
        });

    };


    render() {
        return (
            <div>
                <Tabs
                    onChange = { (value) => this.handleChange(value, this) }
                    value = { this.state.slideIndex }
                >
                    <Tab 
                        label = "Overview" 
                        value = { 0 }
                        icon = { <FontIcon className="fa fa-sitemap" ></FontIcon> }
                        buttonStyle = { {height: "55px", backgroundColor: this.props.settings.secondaryColor, color: this.props.settings.textColor} }
                    />
                    <Tab 
                        label = "Statistics" 
                        value = { 1 }
                        icon = { <FontIcon className="fa fa-bar-chart"></FontIcon> }
                        buttonStyle = { {height: "55px", backgroundColor: this.props.settings.secondaryColor, color: this.props.settings.textColor} }
                    />
                </Tabs>

                

                <SwipeableViews
                    index = { this.state.slideIndex }
                    onChangeIndex = { this.handleChange }

                    style = {{
                        overflowY: "hidden",
                    }}           
                >
                    <div
                        id = { this.state.tableID }
                        style = {{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                    
                    
                    </div>


                    <div
                        id = { this.state.rangeID }
                        style = {{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                       
                    
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

/**
 * maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    settings: state.filterState.Settings
  }
}


/**
 * connects the FilterTabs component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(FilterTabs);
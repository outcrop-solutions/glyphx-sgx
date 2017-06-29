import React from 'react';
import {Tabs, Tab} from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import FontIcon from 'material-ui/FontIcon';
import FilterTable from './FilterTable.js';
import RangeForm from './range.js';

class FilterTabs extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            slideIndex: 0,
            Range: null,
            tableData: props.data,
            tableID: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
            rangeID: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36)
        };
    };


    active = "RANGE";

    handleChange = (value,context) => {
        context.setState({
            slideIndex: value
        });

        if (this.active === "ELASTIC") {
            this.active = "RANGE";
        }
        else {
            this.active = "ELASTIC";
        }
        
        if (this.active === "ELASTIC") {
            if (document.getElementById(this.state.rangeID)) {
                document.getElementById(this.state.tableID).style.maxHeight = "0px";
                document.getElementById(this.state.rangeID).style.maxHeight = "440px";
            }
        }
        else {
            if (document.getElementById(this.state.tableID)) {
                document.getElementById(this.state.rangeID).style.maxHeight = "0px";
                document.getElementById(this.state.tableID).style.maxHeight = "440px";
            }
        }

    };


    render() {
        return (
            <div>
                <Tabs
                    onChange = { (value) => this.handleChange(value,this) }
                    value = { this.state.slideIndex }
                >
                    <Tab 
                        label = "Elastic" 
                        value = { 0 }
                        icon = { <FontIcon className="fa fa-list-ul"></FontIcon> }
                        buttonStyle = { {height: "55px", backgroundColor: "#0b7f8e"} }
                    />
                    <Tab 
                        label = "Range" 
                        value = { 1 }
                        icon = { <FontIcon className="fa fa-sliders"></FontIcon> }
                        buttonStyle = { {height: "55px", backgroundColor: "#0b7f8e"} }
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
                        <FilterTable internalColName={this.props.internalColName} displayName={this.props.displayName} tableData={this.state.tableData} id={this.props.id} selectedRows={this.state.FilterTableSelectedRows}></FilterTable>
                    </div>
                    <div
                        id = { this.state.rangeID }
                        style = {{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <RangeForm colName={this.props.id} data={this.state.tableData} minVal={-100} maxVal={100}></RangeForm>
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

export default FilterTabs;

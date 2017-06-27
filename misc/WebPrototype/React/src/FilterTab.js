import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import {Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn} from 'material-ui/Table';
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
            tableData: props.data
        };
    };

    handleChange = (value,context) => {
        context.setState({
            slideIndex: value
        });
    };

    render() {
        return (
            <div>
                <Tabs
                    onChange={(value) => this.handleChange(value,this)}
                    value={this.state.slideIndex}
                >
                    <Tab 
                        label="Elastic" 
                        value={0}
                        icon={<FontIcon className="fa fa-list-ul"></FontIcon>}
                    />
                    <Tab 
                        label="Range" 
                        value={1}
                        icon={<FontIcon className="fa fa-sliders"></FontIcon>}
                    />
                </Tabs>

                

                <SwipeableViews
                    index={this.state.slideIndex}
                    onChangeIndex={this.handleChange}

                    style={{
                        overflowY: "hidden",
                        height: "440px",
                    }}
                            
                >
                    <div
                        style={{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <FilterTable tableData={this.state.tableData} id={this.props.colName} selectedRows={this.state.FilterTableSelectedRows}></FilterTable>
                    </div>
                    <div
                        style={{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <RangeForm colName={this.props.colName} minVal={-100} maxVal={100}></RangeForm>
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

export default FilterTabs;

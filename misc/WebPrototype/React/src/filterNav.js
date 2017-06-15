import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import SelectField from 'material-ui/SelectField';
import MenuItem from 'material-ui/MenuItem';
import Range from './range.js';
import {Flex} from 'react-flex-material';
import Divider from 'material-ui/Divider';
import {Tabs, Tab} from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import FontIcon from 'material-ui/FontIcon';
import {Card, CardActions, CardHeader, CardText} from 'material-ui/Card';
import {Table, TableBody, TableFooter, TableHeader, TableHeaderColumn, TableRow, TableRowColumn} from 'material-ui/Table';
import TextField from 'material-ui/TextField';
import IconButton from 'material-ui/IconButton';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';

const items = [];
for (let i = 0; i < 50; i++ ) {
    items.push(<MenuItem value={i} key={i} primaryText={'View '+i+''} />);
}

class FilterNav extends Component {
	
    constructor(props) {
        super(props);

        this.state  = {
            menu:{
                open: false,
            },
            viewSelect:{
                value: 10,
            }
        };
  }

  
  /**
	* This method is called when the menu button is clicked. The menu button is in the top right corner of the filerbar.
	**/
    menuDropdownClick = (event) => {
        event.preventDefault();

        this.setState(
        {
            menu: {open: true,
            anchorEl: event.currentTarget,}
        });
    };
  
  /**
	* This method is called when there is loss of focus when the menu is open.
	**/
    handleRequestClose = () => {
        this.setState(
        {
        menu: {open: false}
        });
    };
  
  /**
	* This method is called when an item is selected in the "select view" dropdown that is present in the top left.
	**/
    onSelectViewChange = (event, index, value) => {
        this.setState({viewSelect:{
            value: value,
        }});
        console.log(value);
    };

  
	
    render() {
        return (
            <div className="TopNav" style={{height: '100%'}}>
                {/* TOP SECTION */}
                <Flex layout="column" style={{height: '100%'}}>
                    <Flex flex="30" id="TopView">
       
                        {/* Row 1 */}
                        <Flex layout="row" align="space-between center"    style={{height: '20%'}}>
                            <Flex flex="80">
                                <SelectField
                                    value={this.state.viewSelect.value}
                                    onChange={this.onSelectViewChange}
                                    style={{width:"100%",fontSize:'13px'}}
                                >
                                    {items}
                                </SelectField>
                            </Flex>
                            
                            <Flex divider />
                            <Flex flex="20">
                                <div>
                                    <RaisedButton
                                        onClick={this.menuDropdownClick}
                                        label="Menu"
                                        buttonStyle={{color: 'rgb(0, 188, 212)'}}
                                        labelStyle={{fontSize: '13px'}}
                                        primary={true}
                                    />
                                    <Popover
                                        open={this.state.menu.open}
                                        anchorEl={this.state.menu.anchorEl}
                                        onRequestClose={this.handleRequestClose}
                                        style={{fontSize: '13px'}}
                                    >
                                        <Menu>
                                            <MenuItem primaryText="New" style={{fontSize: '13px'}}/>
                                            <Divider />
                                            <MenuItem primaryText="Save" style={{fontSize: '13px'}}/>
                                            <MenuItem primaryText="Save As" style={{fontSize: '13px'}}/>
                                        </Menu>
                                    </Popover>
                                </div>
                            </Flex>
                        </Flex>
                                        
                        {/* Row 2 */}
                        <Flex layout="row" style={{height:'50%'}}>
                        </Flex>
                        
                        {/* Row 3 */}
                        <Flex layout="row" style={{height:'15%'}}>
                        </Flex>
                        
                        {/* Row 4 */}
                        <Flex layout="row" style={{height:'15%'}}>
                        </Flex>
                    </Flex>


                    {/* BOTTOM SECTION */}
                    <Flex flex="70">

                        <Card>
                            <CardHeader
                                title="Active"
                                titleColor="white"
                                actAsExpander={true}
                                showExpandableButton={true}
                                className="collapse-header"
                                iconStyle={{color: "white"}}
                            />
                                <CardText expandable={true}>
                                    HUMINA
                                </CardText>
                            
                        </Card>

                        <Card>
                            <CardHeader
                                title="Filters"
                                titleColor="white"
                                actAsExpander={true}
                                showExpandableButton={true}
                                className="collapse-header"
                                iconStyle={{color: "white"}}
                            />
                            <CardText expandable={true}>
                                
                                <Card>
                                    <CardHeader
                                        title="Highschool"
                                        titleColor="white"
                                        actAsExpander={true}
                                        showExpandableButton={true}
                                        className="collapse-header"
                                        iconStyle={{color: "white"}}
                                    />
                                    <CardText expandable={true}>
                                        <Card>
                                            <CardHeader
                                                title="Age"
                                                titleColor="white"
                                                actAsExpander={true}
                                                showExpandableButton={true}
                                                className="collapse-header"
                                                iconStyle={{color: "white"}}
                                            />
                                            <CardText expandable={true}>
                                                <FilterTabs></FilterTabs>
                                            </CardText>
                                        </Card>

                                        <Card>
                                            <CardHeader
                                                title="Year"
                                                titleColor="white"
                                                actAsExpander={true}
                                                showExpandableButton={true}
                                                className="collapse-header"
                                                iconStyle={{color: "white"}}
                                            />
                                            <CardText expandable={true}>
                                                <FilterTabs></FilterTabs>
                                            </CardText>
                                        </Card>

                                        <Card>
                                            <CardHeader
                                                title="GPA"
                                                titleColor="white"
                                                actAsExpander={true}
                                                showExpandableButton={true}
                                                className="collapse-header"
                                                iconStyle={{color: "white"}}
                                            />
                                            <CardText expandable={true}>
                                                <FilterTabs></FilterTabs>
                                            </CardText>
                                        </Card>
                                    </CardText>
                                </Card>

                                <Card>
                                    <CardHeader
                                        title="University"
                                        titleColor="white"
                                        actAsExpander={true}
                                        showExpandableButton={true}
                                        className="collapse-header"
                                        iconStyle={{color: "white"}}
                                    />
                                    <CardText expandable={true}>
                                        <Card>
                                            <CardHeader
                                                title="Age"
                                                titleColor="white"
                                                actAsExpander={true}
                                                showExpandableButton={true}
                                                className="collapse-header"
                                                iconStyle={{color: "white"}}
                                            />
                                            <CardText expandable={true}>
                                                <FilterTabs></FilterTabs>
                                            </CardText>
                                        </Card>

                                        <Card>
                                            <CardHeader
                                                title="Year"
                                                titleColor="white"
                                                actAsExpander={true}
                                                showExpandableButton={true}
                                                className="collapse-header"
                                                iconStyle={{color: "white"}}
                                            />
                                            <CardText expandable={true}>
                                                <FilterTabs></FilterTabs>
                                            </CardText>
                                        </Card>

                                        <Card>
                                            <CardHeader
                                                title="GPA"
                                                titleColor="white"
                                                actAsExpander={true}
                                                showExpandableButton={true}
                                                className="collapse-header"
                                                iconStyle={{color: "white"}}
                                            />
                                            <CardText expandable={true}>
                                                <FilterTabs></FilterTabs>
                                            </CardText>
                                        </Card>
                                    </CardText>
                                </Card>

                            </CardText>
                        </Card>



                                
                            

                    </Flex>
                </Flex>
            </div>
        );
    }
}

class FilterTabs extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            slideIndex: 0,
        };
    }

    handleChange = (value) => {
        this.setState({
            slideIndex: value,
        });
    };

    render() {
        return (
            <div>
                <Tabs
                    onChange={this.handleChange}
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
                        <FilterTable></FilterTable>
                    </div>
                    <div
                        style={{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <Range></Range>
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

const tableData = [
    {
        value: 2000,
        count: '20/200',
        percent: '10%',
    },
    {
        value: 2001,
        count: '40/200',
        percent: '20%',
    },
    {
        value: 2002,
        count: '2/200',
        percent: '1%',
    },
    {
        value: 2003,
        count: '8/200',
        percent: '4%',
    },
    {
        value: 2004,
        count: '10/200',
        percent: '5%',
    },
    {
        value: 2005,
        count: '60/200',
        percent: '30%',
    },
    {
        value: 2006,
        count: '20/200',
        percent: '10%',
    },
    {
        value: 2007,
        count: '20/200',
        percent: '10%',
    },
    {
        value: 2008,
        count: '20/200',
        percent: '10%',
    },
];


class FilterTable extends Component {
    state = {
        fixedHeader: true,
        fixedFooter: true,
        stripedRows: false,
        showRowHover: true,
        selectable: true,
        multiSelectable: true,
        enableSelectAll: true,
        deselectOnClickaway: true,
        showCheckboxes: true,
        height: '500px',
    };

    handleToggle = (event, toggled) => {
        this.setState({
        [event.target.name]: toggled,
        });
    };

    handleChange = (event) => {
        this.setState({height: event.target.value});
    };

    render() {
        return (
            <div>
                <Table
                    height={this.state.height}
                    fixedHeader={this.state.fixedHeader}
                    fixedFooter={this.state.fixedFooter}
                    selectable={this.state.selectable}
                    multiSelectable={this.state.multiSelectable}
                >
                <TableHeader
                    displaySelectAll={this.state.showCheckboxes}
                    adjustForCheckbox={this.state.showCheckboxes}
                    enableSelectAll={this.state.enableSelectAll}
                >
                    <TableRow>
                        <TableHeaderColumn >Value</TableHeaderColumn>
                        <TableHeaderColumn >Count</TableHeaderColumn>
                        <TableHeaderColumn >Percent</TableHeaderColumn>
                    </TableRow>

                </TableHeader>

                <TableBody
                    displayRowCheckbox={this.state.showCheckboxes}
                    deselectOnClickaway={this.state.deselectOnClickaway}
                    showRowHover={this.state.showRowHover}
                    stripedRows={this.state.stripedRows}
                >
                    {tableData.map( (row, index) => (
                        <TableRow key={index}>
                            <TableRowColumn>{row.value}</TableRowColumn>
                            <TableRowColumn>{row.count}</TableRowColumn>
                            <TableRowColumn>{row.percent}</TableRowColumn>
                        </TableRow>
                    ))}
                </TableBody>
                </Table>
            </div>
        );
    }
}

export default FilterNav;
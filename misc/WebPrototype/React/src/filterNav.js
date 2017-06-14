import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import SelectField from 'material-ui/SelectField';
import MenuItem from 'material-ui/MenuItem';
import Range from './range.js';
import {Flex} from 'react-flex-material';
import Divider from 'material-ui/Divider';
import {List, ListItem} from 'material-ui/List';
import IconButton from 'material-ui/IconButton';




class FilterNav extends Component {
	
     
    constructor(props) {
        
        super(props);
        
        var viewSelectItems= [];
        var tableSelectItems = [];
        var appliedFilters = [];

        for (let i = 0; i < 20; i++ ) {
            viewSelectItems.push(<MenuItem className="menuItemStyling" value={i} key={i} primaryText={'View '+i+''} />);
            tableSelectItems.push(<MenuItem className="menuItemStyling" value={i} key={i} insetChildren={true} checked={false} primaryText={'Table '+i+''} />)
            appliedFilters.push(<ListItem disabled={true} style={{fontSize: '13px',}} innerDivStyle={{padding: '11px 11px 11px 40px'}} primaryText="Filter 1" leftIcon={<i className="fa fa-times cancelIconStyle" onClick={this.onDeleteFilter}  aria-hidden="true" name="Filter1"></i>} />);
            appliedFilters.push(<Divider />);
        }

        this.state  = {
            menu:{
                open: false,
            },
            viewSelect:{
                value: null,
                items: viewSelectItems
            },
            tableSelect:{
                    values: [],
                    items: tableSelectItems
            },
            appliedFilters:{
                items: appliedFilters
            }
        };

        
        
    };

  
  /**
	* This method is called when the menu button is clicked. The menu button is in the top right corner of the filerbar.
	**/
    menuDropdownClick = (event) => {
        event.preventDefault();

        this.setState(
        {
            menu: {
                open: true,
                anchorEl: event.currentTarget,
            }
        });
    };
  
  /**
	* This method is called when there is loss of focus when the menu is open.
	**/
    handleRequestClose = () => {
        this.setState(
        {
            menu: {
                open: false
            }
        });
    };
  
  /**
	* This method is called when an item is selected in the "select view" dropdown that is present in the top left.
	**/
    onSelectViewChange = (event, index, value) => {
        this.setState(
        {
            viewSelect:{
                    value: value,
            }
        }
        );
        console.log(value);
    };

    /**
	* This method is called when an item is selected in the "select Table" dropdown that is present in the top left.
	**/
    onSelectTableChange = (event, index, value) => {
        this.setState(
        {
            tableSelect:{
                    value: value,
            }
        }
        );
        console.log(value);
    };

    /**
	* This method is called when the user clicks on one of the 'X' to delete a filter that is applied.
	**/
    onDeleteFilter = (event) => {
        console.log("1");
    };

    /**
	* This method is called when the user clicks on one of the 'arrow' to hide the top view of the filter
	**/
    toggleTopView = (event) => {
        var collapseTopViewButton = document.getElementById("collapseTopViewButton");
        var topView = document.getElementById("TopView");

        if(topView.hidden == true)
        {
            topView.hidden = false;
            collapseTopViewButton.classList.remove('fa-caret-down');
            collapseTopViewButton.classList.add('fa-caret-up');
        }
        else{
            topView.hidden = true;
            collapseTopViewButton.classList.remove('fa-caret-up');
            collapseTopViewButton.classList.add('fa-caret-down');
        }
            
    };
	
    render() {
        return (
            <div className="TopNav" style={{height: '100%'}}>
                {/* TOP SECTION */}
                <Flex layout="column" style={{height: '100%'}}>
                    <Flex flex="35" id="TopView">
       
                        {/* Row 1 */}
                        <Flex layout="row" align="space-between center"    style={{height: '20%'}}>
                            <Flex flex="80">
                                <SelectField
                                    value={this.state.viewSelect.value}
                                    onChange={this.onSelectViewChange}
                                    style={{width:"100%",fontSize:'13px'}}
                                    hintText="Select View"
                                >
                                    {this.state.viewSelect.items}
                                </SelectField>
                            </Flex>
                            
                            <Flex divider />
                            <Flex flex="20">
                                <div>
                                    <RaisedButton
                                        onClick={this.menuDropdownClick}
                                        label="Menu"
                                    buttonStyle={{height: '28px'}}
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
                                            <MenuItem primaryText="New" className="menuItemStyling"/>
                                            
                                            <MenuItem primaryText="Save" className="menuItemStyling"/>
                                            
                                            <MenuItem primaryText="Save As" className="menuItemStyling"/>
                                        </Menu>
                                    </Popover>
                                </div>
                            </Flex>
                        </Flex>
                                        
                        {/* Row 2 */}
                        <Flex layout="row" style={{height:'50%'}}>
                            <div style={{width:'100%',border:'1px',borderStyle: 'double',margin:'2px',overflow:'auto'}}>
                            <List id="FilterList">
                               {this.state.appliedFilters.items}
                            </List>
                            </div>
                        </Flex>
                        
                        {/* Row 3 */}
                        <Flex layout="row" style={{height:'12%'}}>
                            <Flex flex="25">
                               <RaisedButton 
                                    label="Clear All" 
                                    buttonStyle={{height: '28px',width:'100%',paddingTop: '5px'}}
                                    labelStyle={{fontSize: '13px',height: '28px',}} 
                                    style = {{height: '28px',width:'100%',}}
                                    primary={true} />
                            </Flex>
                            <Flex divider />
                            <Flex flex="25">
                                <RaisedButton 
                                    label="Disable" 
                                    buttonStyle={{height: '28px',paddingTop: '5px'}}
                                    labelStyle={{fontSize: '13px',height: '28px',}} 
                                    style = {{height: '28px',width:'100%',}}
                                    primary={true} />
                            </Flex>
                            
                        </Flex>
                        
                        {/* Row 4 */}
                        <Flex layout="row" style={{height:'18%'}}>
                            <SelectField
                                    value={this.state.tableSelect.value}
                                    onChange={this.onSelectTableChange}
                                    style={{width:"100%",fontSize:'13px',height:'44px'}}
                                    hintText="Select Table"
                                    multiple={true}
                                >
                                    {this.state.tableSelect.items}
                                </SelectField>
                        </Flex>
                    </Flex>

                    <RaisedButton 
                        fullWidth={true} 
                        primary={true} 
                        onClick={this.toggleTopView}
                        style = {{height: '20px'}}>
                        
                        <i id="collapseTopViewButton" className="fa fa-caret-up" style={{fontSize: '1.6em'}}> 
                        </i> 
                    
                    </RaisedButton>

                    {/* BOTTOM SECTION */}
                    <Flex flex="65">
                        <Range ></Range >
                    </Flex>
                </Flex>
            </div>
        );
    }
}

export default FilterNav;
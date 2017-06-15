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
import Dialog from 'material-ui/Dialog';
import TextField from 'material-ui/TextField';
import FlatButton from 'material-ui/FlatButton';
import Snackbar from 'material-ui/Snackbar';
import AlertContainer from 'react-alert'


class FilterNav extends Component {
	
     
    constructor(props) {
        
        super(props);
        
        var viewSelectItems= [];
        var tableSelectItems = [];
        var appliedFilters = [];

        for (let i = 0; i < 20; i++ ) {
            viewSelectItems.push(<MenuItem className="menuItemStyling" value={i} key={"View "+i} primaryText={'View '+i+''} />);
            tableSelectItems.push(<MenuItem className="menuItemStyling" value={i} key={"Table "+i} insetChildren={true} checked={false} primaryText={'Table '+i+''} />)
            appliedFilters.push(<ListItem disabled={true} value={i} key={"item "+i} style={{fontSize: '13px',}} innerDivStyle={{padding: '11px 11px 11px 40px'}} primaryText={'Filter '+ i} leftIcon={<i className="fa fa-times cancelIconStyle" onClick={this.onDeleteFilter}  aria-hidden="true" name="Filter1"></i>} />);
            appliedFilters.push(<Divider value={i} key={i+100} />);
        }

        //Store the states of all the elements inside this data structure.
        this.state  = {
            topViewVisible: true,
            hideShowButtonTextFlag: true,
            menu:{
                open: false,
            },
            viewSelect:{
                value: null,
            },
            tableSelect:{
                    value: [],
            },
            saveDailog:{
                open: false,
            },
            savedSnackBar: {
                 open: false,
            },
        };
        
        //Store data of all the elements inside this data structure.
        this.data = {
            viewSelectItems: viewSelectItems,
            tableSelectItems: tableSelectItems,
            appliedFiltersItems: appliedFilters
        }
        
    };

    alertOptions = {
    offset: 14,
    position: 'bottom left',
    theme: 'dark',
    time: 2000,
    transition: 'scale'
  }
 
  showAlert = () => {
    this.msg.show('Success The View has been saved!', {
      time: 3000,
      type: 'success',
      icon: <i className="fa fa-check-circle" style={{fontSize: '2.5em',color:'green'}}></i>
    })
  }

    /**
	* This method is called when the menu button is clicked. The menu button is in the top right corner of the filerbar.
	**/
    handleOpenSaveDailog = () => {
          this.setState({saveDailog:{
                open: true
            }});
    };

    /**
	* This method is called when the menu button is clicked
	**/
    handleCloseSaveDailog = () => {
        this.setState({saveDailog:{
                open: false
            }});
    };

    /**
	* This method is called when the save button is pressed in the save dailog.
	**/
    onSaveDailog = (context) => {
        var viewName = document.getElementById("tf_viewName").value;
        var nameAlreadyExists = false;
        var lbl_error = document.getElementById('lbl_saveError');
        lbl_error.hidden = true;

        if(viewName == null || viewName == "")
        {
            //error
        }    
        
        //check if same name view already exists
        context.data.viewSelectItems.forEach(function(element) {
            if(element.key == viewName )
                nameAlreadyExists = true;
        });

        if(nameAlreadyExists)
        {
            console.log("Error! A view with the same name already exists!");
            lbl_error.hidden = false;
        }
        else{
            
            //Save the view


            //Close the dialog
            context.handleCloseSaveDailog();
            console.log("View saved with name: " + viewName);

            //show the success message
            this.showAlert();
            
        }
        
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

        //Load Filters!
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

        //Load Table Columns
    };

    /**
	* This method is called when the user clicks on one of the 'X' to delete a filter that is applied.
	**/
    onDeleteFilter = (event) => {
        console.log("1");
        var but = document.getElementById("buttonHideShow");
        but.value
    };

    /**
	* This method is called when the user clicks on the 'arrow' to hide the top view of the filter
	**/
    toggleTopView = (event) => {
        var collapseTopViewButton = document.getElementById("collapseTopViewButton");
        var topView = document.getElementById("TopView");
        var filterWindow = document.getElementById("FilterWindowOuterContiner");
        var appBar = document.getElementById("AppBar");

        if(this.state.topViewVisible == true)
        {
            filterWindow.style.transform = "translate(0px,-"+topView.clientHeight+"px)"
            collapseTopViewButton.classList.remove('fa-caret-up');
            collapseTopViewButton.classList.add('fa-caret-down');
            this.state.topViewVisible = false;
        }
        else{
            filterWindow.style.transform = "translate(0px,0px)"
            collapseTopViewButton.classList.remove('fa-caret-down');
            collapseTopViewButton.classList.add('fa-caret-up');
            this.state.topViewVisible = true;
        }
            
    };

    /**
	* This method is called when the user clicks on the 'clear all' button.
	**/
    onClearAllFilters = (event) => {
        console.log("clear all");
    };

    /**
	* This method is called when the user clicks on the 'Hide/Show'.
	**/
    onHideFilteredData = (event) => {
        console.log("Hide");
        this.setState(
        {
            hideShowButtonTextFlag: !this.state.hideShowButtonTextFlag,
        });
        
        //if the flag true then button --> |HIDE| else button --> |SHOW|
        if(this.state.hideShowButtonTextFlag)
        {
            //hide the glyphs that don't match the filter critera.
        }
        else{
            //show all the glyphs!
        }
    };
	
    /**
	* This method is called when the user clicks on the 'New' inside the menu.
	**/
    onMenuNewClick  = (event) => {
        console.log("new");

        //close the menu
        this.handleRequestClose();
    };

     /**
	* This method is called when the user clicks on the 'Save' inside the menu.
	**/
    onMenuSaveClick = (event) => {
        console.log("Save");
        
        
        //close the menu
        this.handleRequestClose();
    };

    /**
	* This method is called when the user clicks on the 'Save As' inside the menu.
	**/
    onMenuSaveAsClick = (event) => {
        console.log("Save As");
        
        this.handleOpenSaveDailog();
        //close the menu
        this.handleRequestClose();
    };

    render() {
        
        return (
            <div className="TopNav" id="FilterWindowOuterContiner" style={{height: '100%',transition:'1s',paddingLeft:'1%',paddingRight: '1%'}}>
                <div>
                    <AlertContainer ref={a => this.msg = a} {...this.alertOptions} />
                </div>
                <Flex layout="column" style={{height: '100%'}}>
                     {/* TOP SECTION */}
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
                                    {this.data.viewSelectItems}
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
                                            <MenuItem primaryText="New" className="menuItemStyling" onClick={this.onMenuNewClick}/>
                                            
                                            <MenuItem primaryText="Save" className="menuItemStyling" onClick={this.onMenuSaveClick}/>

                                            <MenuItem primaryText="Save As" className="menuItemStyling" onClick={this.onMenuSaveAsClick}/>
                                        </Menu>
                                    </Popover>
                                    <Dialog
                                        title="Save View"
                                        actions={
                                            [<FlatButton
                                                label="Save"
                                                primary={true}
                                                onClick={() => this.onSaveDailog(this)}/>,
                                             <FlatButton
                                                label="Cancel"
                                                primary={true}
                                                onClick={this.handleCloseSaveDailog}/>]
                                        }
                                        modal={true}
                                        open={this.state.saveDailog.open}>
                                        Please enter a name for the view you are saving. <br />
                                        <TextField id="tf_viewName" floatingLabelText="View Name" /> <br />
                                        <label id="lbl_saveError" hidden style={{color:'red'}}> Error! A view with the same name already exists! Please provide a different name! </label>
                                    </Dialog>
                                </div>
                            </Flex>
                        </Flex>
                                        
                        {/* Row 2 */}
                        <Flex layout="row" style={{height:'50%'}}>
                            <div style={{width:'100%',border:'1px',borderStyle: 'double',margin:'2px',overflow:'auto'}}>
                            <List id="FilterList">
                               {this.data.appliedFiltersItems}
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
                                    onClick={this.onClearAllFilters}
                                    primary={true} />
                            </Flex>
                            <Flex divider />
                            <Flex flex="25">
                                <RaisedButton 
                                    label={this.state.hideShowButtonTextFlag ? "Hide" : "Show"}
                                    id="buttonHideShow"
                                    buttonStyle={{height: '28px',paddingTop: '5px'}}
                                    labelStyle={{fontSize: '13px',height: '28px',}} 
                                    style = {{height: '28px',width:'100%',}} 
                                    onClick={this.onHideFilteredData}
                                    primary={true} />
                                    {/*<Toggle
                                        label="Hide Filtered Data"
                                        style={{paddingTop:'3px'}}
                                        labelPosition="right"
                                        />
                                    */}
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
                                    {this.data.tableSelectItems}
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
import React, { Component } from 'react';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import SelectField from 'material-ui/SelectField';
import MenuItem from 'material-ui/MenuItem';
import RangeForm from './range.js';
import {Flex} from 'react-flex-material';
import FontIcon from 'material-ui/FontIcon';
import Divider from 'material-ui/Divider';
import TextField from 'material-ui/TextField';
import IconButton from 'material-ui/IconButton';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import Snackbar from 'material-ui/Snackbar';
import AlertContainer from 'react-alert';
import {List, ListItem} from 'material-ui/List';
import {Card, CardActions, CardHeader, CardText} from 'material-ui/Card';
import FilterTabs from './FilterTab.js';
import Global from './Global.js';
import Collapsible from 'react-collapsible';
import store from './index.js';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';

class FilterNav extends Component {
	
     
    constructor(props) {
        super(props);
        
        var viewSelectItems= [];
        var tableSelectItems = [];
        var appliedFiltersItems = [];

        //Load values into the view select dropdown.
        viewSelectItems = this.makeList(['view 0','view 1','view 4','view 5'],"viewSelectItems");

        //Load values into the table select dropdown.
        tableSelectItems = this.makeList(['table 0','table 1','table 4','table 5'],"tableSelectItems");

        //For now! dummy filters too!
        appliedFiltersItems = this.makeList(['Filter 0','Filter 1','Filter 4','Filter 5'],"appliedFiltersItems");

        //Store the states of all the elements inside this data structure.
        this.state  = {
            topViewVisible: true,
            GLOBAL: null,
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
            tableData: this.fetchData(),
            appliedFiltersItems: appliedFiltersItems,
            viewSelectItems: viewSelectItems,
            tableSelectItems: tableSelectItems,
            viewNameTextFieldError: ""
        };
        
    };

    fetchData = () => {
    var data = require('../src/Data/TempData.json');
    return data.Data;
    };
 
    /**
	* This method is called when
	*/
    showAlert = () => {
        this.msg.show('Success The View has been saved!', {
        time: 3000,
        type: 'success',
        icon: <i className="fa fa-check-circle" style={{fontSize: '2.5em',color:'green'}}></i>
        })
    };

    /**
	* This method is called when
	*/
    makeList = (arrValues,type) => {
        if(!Array.isArray(arrValues))
            return "PLEASE PROVIDE AN ARRAY";

        var len = arrValues.length;
        var index;
        var arrReturn =[];

        switch(type){
            case 'viewSelectItems':
                for(index=0;index<len;index++)
                {
                    arrReturn.push(<MenuItem className="menuItemStyling" value={arrValues[index]} key={arrValues[index]} primaryText={arrValues[index]} />);
                }
                break;
            case 'tableSelectItems':
                for(index=0;index<len;index++)
                {
                    arrReturn.push(<MenuItem className="menuItemStyling" value={arrValues[index]} key={arrValues[index]} insetChildren={true} checked={false} primaryText={arrValues[index]} />)
                }
                break;
            case 'appliedFiltersItems':
                 for(index=0;index<len;index++)
                {
                    arrReturn.push(<ListItem disabled={true} value={arrValues[index]} key={arrValues[index]} style={{fontSize: '13px'}} innerDivStyle={{padding: '11px 11px 11px 40px'}} primaryText={arrValues[index]} leftIcon={<i className="fa fa-times cancelIconStyle" onClick={() => this.onDeleteFilter(this,arrValues[index])}  aria-hidden="true" name={arrValues[index]}></i>} />);
                    arrReturn.push(<Divider value={arrValues[index]} key={arrValues[index]+'divider'} />);
                }
                break;
        }

        return arrReturn;
    };

    /**
	* This method is called when
	*/
    handleOpenSaveDailog = () => {
          this.setState({saveDailog:{
                open: true
            },
            viewNameTextFieldError: ""
        });
            
    };

    /**
	* This method is called when
	*/
    handleCloseSaveDailog = () => {
        this.setState({saveDailog:{
                open: false
            }});
    };

    /**
	* This method is called when the menu button is clicked. The menu button is in the top right corner of the filerbar.
	*/
    handleOpenOptionsMenu = (event) => {
        event.preventDefault();

        this.setState(
        {
            menu: {
                open: true,
                anchorEl: event.currentTarget
            }
        });
    };

    /**
	* This method is called to close the menu.
	*/
    handleCloseOptionsMenu = () => {
        this.setState(
        {
            menu: {
                open: false
            }
        });
    };

   /**
	* This method is called when the save button is pressed in the save dailog.
    * @param context: This is the instance of the current class.
	*/
    onSaveDailog = (context) => {
        var viewName = document.getElementById("tf_viewName").value;
        var nameAlreadyExists = false;
        var lbl_error = document.getElementById('lbl_saveError');
        lbl_error.hidden = true;

        if(viewName == null || viewName.trim() == "")
        {
            //error
            context.setState({viewNameTextFieldError: "This textfield is required!"});
        }
        else{
        
            //check if same name view already exists
            context.state.viewSelectItems.forEach(function(element) {
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
                context.saveView(context,viewName);
                
                //Actually Add it to the existing list of views!
                context.state.viewSelectItems.push(context.makeList([viewName],'viewSelectItems')[0]);
                context.setState({viewSelectItems: context.state.viewSelectItems,});

                //Make it selected in the list of views
                context.setState({viewSelect:{ value: viewName}});

                //Close the dialog
                context.handleCloseSaveDailog();
                console.log("View saved with name: " + viewName);

                //show the success message
                this.showAlert();
                
            }
        }
        
    };

    /**
     * 
     */
    saveView = (context,viewName) => {

    };
    
    /**
     * 
     */
    applyFilter = () => {
        console.log('Filter Applied');
    };
  
    /**
	* This method is called when an item is selected in the "select view" dropdown that is present in the top left.
	*/
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
	*/
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
	*/
    onDeleteFilter = (context,key) => {
        var index = null;
        
        //search for index of item
        context.state.appliedFiltersItems.forEach(function(element) {
            if(element.key == key)
                index = context.state.appliedFiltersItems.indexOf(element);
        });

        //change the state to delete the div!
        context.state.appliedFiltersItems.splice(index,2);
        context.setState({
            appliedFiltersItems: context.state.appliedFiltersItems,}
        )


    };

   

    /**
	* This method is called when the user clicks on the 'clear all' button.
	*/
    onClearAllFilters = (event) => {
        console.log("clear all");

    };

    /**
	* This method is called when the user clicks on the 'Hide/Show'.
	*/
    onHideFilteredData = (event) => {
        console.log("Hide");
        this.setState(
        {
            hideShowButtonTextFlag: !this.state.hideShowButtonTextFlag,
        });

        console.log(this.state.GLOBAL.getData()["Filter"]["Range"]);
        
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
	*/
    onMenuNewClick  = (event) => {
        console.log("new");

        //Maybe are you sure you don't want to save existing work? if filters applied.


        //Clear All Filters
        this.onClearAllFilters();

        //Clear the value of viewName
        this.setState ({viewSelect:{value:null}});

        //close the menu
        this.handleCloseOptionsMenu();
    };

     /**
	* This method is called when the user clicks on the 'Save' inside the menu.
	*/
    onMenuSaveClick = (event) => {
        console.log("Save");
        
        //Save
        this.saveView();
        
        //close the menu
        this.handleCloseOptionsMenu();
    };

    /**
	* This method is called when the user clicks on the 'Save As' inside the menu.
	*/
    onMenuSaveAsClick = (event) => {
        console.log("Save As");
        
        this.handleOpenSaveDailog();
        //close the menu
        this.handleCloseOptionsMenu();
    };

   /**
	* This method is called when the user clicks on the 'arrow' to hide/show the top view of the filter
	*/
    toggleTopView = (event) => {
        var collapseTopViewButton = document.getElementById("collapseTopViewButton");
        var topView = document.getElementById("TopView");
        var filterWindow = document.getElementById("FilterWindowOuterContiner");
        var appBar = document.getElementById("AppBar");

        if(this.state.topViewVisible == true)
        {
            filterWindow.style.transform = "translate(0px,-"+topView.clientHeight+"px)"
            //collapseTopViewButton.classList.remove('fa-caret-up');
            //collapseTopViewButton.classList.add('.Collapsible__trigger.is-open:after');
            collapseTopViewButton.style.transform = 'rotateZ(180deg)';
            this.state.topViewVisible = false;
        }
        else{
            filterWindow.style.transform = "translate(0px,0px)"
            //collapseTopViewButton.classList.remove('fa-caret-down');
            //collapseTopViewButton.classList.add('fa-caret-up');
            collapseTopViewButton.style.transform = 'none';
            this.state.topViewVisible = true;
        }
            
    };

    render() {
         var keys = Object.keys(this.state.tableData);
         var data = this.state.tableData;
         var context = this;

        var columns = keys.map(function(column) {
            return (<Collapsible 
                        transitionTime={200} 
                        key={column} 
                        trigger={
                                <div>
                                    <span onClick={() => function(){console.log('Hi');}}><i className="fa fa-thumb-tack" style={{fontSize: '1.3rem'}}></i></span>
                                    <span 
                                        style={{
                                        paddingLeft: '10px',
                                        fontSize: '1rem'
                                    }}>
                                        {column}
                                    </span>
                                </div>} 
                        triggerClassName='columnNameHeader'>
                            <FilterTabs colName={column} data={data[column]}></FilterTabs>
                    </Collapsible>
                    );
        });

        return (
            <div className="TopNav" id="FilterWindowOuterContiner" style={{height: '100%',transition:'1s',paddingLeft:'1%',paddingRight: '1%'}}>
                <Global ref={inst => this.state.GLOBAL = inst} />
                <div>
                    <AlertContainer ref={a => this.msg = a} {...this.alertOptions} />
                </div>

                <RangeForm minVal = {-100} maxVal = {100} colName = "StaffAssigned" />
                
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
                                    {this.state.viewSelectItems}
                                </SelectField>
                            </Flex>
                            
                            <Flex divider />
                            <Flex flex="20">
                                <div>
                                    <RaisedButton
                                        onClick={this.handleOpenOptionsMenu}
                                        label="Menu"
                                        buttonStyle={{height: '28px'}}
                                        labelStyle={{fontSize: '13px'}}
                                        primary={true}
                                    />
                                    <Popover
                                        open={this.state.menu.open}
                                        anchorEl={this.state.menu.anchorEl}
                                        onRequestClose={this.handleCloseOptionsMenu}
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
                                        <TextField id="tf_viewName" errorText={this.state.viewNameTextFieldError} floatingLabelText="View Name" /> <br />
                                        <label id="lbl_saveError" hidden style={{color:'red'}}> Error! A view with the same name already exists! Please provide a different name! </label>
                                    </Dialog>
                                </div>
                            </Flex>
                        </Flex>
                                        
                        {/* Row 2 */}
                        <Flex layout="row" style={{height:'50%'}}>
                            <div style={{width:'100%',border:'1px',borderColor: '#e0e0e0',borderRadius:'10px',borderStyle: 'double',margin:'2px',overflow:'auto'}} className="sidenavbar">
                                <List id="FilterList">
                                    {this.state.appliedFiltersItems}
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
                                    onClick={this.onHideFilteredData.bind(this)}
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
                                    {this.state.tableSelectItems}
                                </SelectField>
                        </Flex>
                    </Flex>

                    <RaisedButton 
                        fullWidth={true} 
                        primary={true} 
                        onClick={this.toggleTopView}
                        style = {{height: '20px'}}>
                        
                        <i 
                            id="collapseTopViewButton" 
                            className="fa fa-caret-up" 
                            style={{
                                fontSize: '1.6em',
                                color: 'white',
                                transition: 'transform 500ms'
                            }}
                        > 
                        </i> 
                    
                    </RaisedButton>

                    <Flex divider />
                    <Flex divider />

                    {/* BOTTOM SECTION */}

                    <Flex flex="65">

                        <RaisedButton primary={true} label="Apply Filters" onClick={this.applyFilter} fullWidth={true} />
                        <br />
                        <br />
                        <Collapsible 
                            transitionTime={200} 
                            trigger={
                                <div>
                                    <i className="fa fa-thumb-tack" style={{fontSize: '1.3rem'}}></i>
                                    <span 
                                        style={{
                                        paddingLeft: '10px',
                                        fontSize: '1rem'
                                    }}>
                                        Active
                                    </span>
                                </div>}>
                        </Collapsible>

                        <Collapsible 
                            transitionTime={200} 
                            trigger={
                                <div>
                                    <i className="fa fa-filter" style={{fontSize: '1.3rem'}}></i>
                                    <span 
                                        style={{
                                        paddingLeft: '10px',
                                        fontSize: '1rem'
                                    }}>
                                        Filter
                                    </span>
                                </div>}>
                                 {columns}
                        </Collapsible>

                    </Flex>
                </Flex>
            </div>
        );
    }
}

export default FilterNav;
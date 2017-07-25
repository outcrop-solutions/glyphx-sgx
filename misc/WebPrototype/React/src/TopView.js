import React, { Component } from 'react';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import SelectField from 'material-ui/SelectField';
import MenuItem from 'material-ui/MenuItem';
import {Flex} from 'react-flex-material';
import Divider from 'material-ui/Divider';
import TextField from 'material-ui/TextField';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import ListItem from 'material-ui/List';
import FilterViewForm from './filterView.js';
import FilterNavTabs from './FilterNavTabs.js';
import Select from 'react-select';
import { connect } from 'react-redux';
import './filterNav.css';
import 'react-select/dist/react-select.css';

/**
 * 
 */
class TopView extends Component {
	
	constructor(props) {
        super(props);
	
        //Load values into the view select dropdown.
        var viewSelectItems = this.makeList(props.initParams.viewSelectItems,"viewSelectItems");

        //Store the states of all the elements inside this data structure.
        this.state = {
            topViewVisible: true,
            hideShowButtonTextFlag: true,
            menu: { open: false },
            viewSelectValue: null,
            tableSelectValues: [],
            saveDailogOpen: false,
            viewSelectItems: viewSelectItems,
            viewNameTextFieldError: ""
        };
	}
	
	/**
     * This function holds the templates of certain UI elements. Call this and pass a type
     * to generate a JSX code that you can use to render that particular element.
     * @param {array} arrValues: Single dimension array of values.
     * @param {string} type: any one of [viewSelectItems,tableSelectItems,appliedFiltersItems,columns]
     * @param {object} extra: An object to pass extra params.
     */
    makeList(arrValues,type,extra){
        if (!Array.isArray(arrValues)) {
            return "PLEASE PROVIDE AN ARRAY";
        }

        var len = arrValues.length;
        var index;
        var arrReturn = [];
        var objReturn = null;

        switch (type) {
            case 'viewSelectItems':
                for (index = 0; index < len; index++) {
                    arrReturn.push(
                        {
                            label: arrValues[index], value: arrValues[index]
                        }
                    );
                }

                break;

            case 'tableSelectItems':
                for (index = 0; index < len; index++) {
                    arrReturn.push(
                        <MenuItem 
                            className = "menuItemStyling" 
                            value = { arrValues[index] } 
                            key = { arrValues[index] } 
                            insetChildren = { true } 
                            checked = { false } 
                            primaryText = { arrValues[index] } 
                        />
                    );
                }

                break;
                 
            default:
                return null;
        }
        return objReturn != null ? objReturn : arrReturn;
    };

	
	/**
     * 
     */
    handleOpenClose = (strName,open,evt) =>{
        switch (strName){
            case 'save':
                if (open) {
                    this.setState({
						saveDailogOpen: true,
                        viewNameTextFieldError: ""
                    }); 
                }
                else {
                    this.setState({ saveDailogOpen: false });
                }
                
                break;
            
            case 'menu':
                 if (open) {
                    evt.preventDefault();

                    this.setState({
                        menu: {
                            open: true,
                            anchorEl: evt.currentTarget
                        }
                    });
                }
                else {
                    this.setState({
                        menu: {
                            open: false
                        }
                    });
                }

                break;
            
            default:
                return null;
        }
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

        if( viewName == null || viewName.trim() == "" )
        {
            //error
            context.setState({ viewNameTextFieldError: "This textfield is required!" });
        }
        else {
        
            //check if same name view already exists
            context.state.viewSelectItems.forEach(function(element) {
                if (element.key == viewName ) {
                    nameAlreadyExists = true;
                }
            });

            if (nameAlreadyExists) {
                console.log("Error! A view with the same name already exists!");
                lbl_error.hidden = false;
            }
            else {
                
                //Save the view
                if (context.saveView(context,viewName)) {
					//Actually Add it to the existing list of views!
					context.state.viewSelectItems.push(context.makeList([viewName],'viewSelectItems')[0]);
					context.setState({ viewSelectItems: context.state.viewSelectItems });

					//Make it selected in the list of views
					context.setState({ viewSelectValue: viewName });

					//Close the dialog
					context.handleOpenClose('save',false);
					console.log("View saved with name: " + viewName);

					//show the success message
					context.props.showAlert('Success The View has been saved!');
				}
				else { //didn't save due to some network error.
					console.error("ERROR");
				}
            }
        }
    };


    /**
     * 
     */
    saveView = (context,viewName) => {
		var saveObj = {
			viewName: viewName,
			filterObj: {}
		};
		
		//make the object to be sent to the server to save.
		saveObj.filterObj = context.props.GLOBALSTORE.Filter;
		
		//make the ajax call to the server to save.
		
		
		return true;
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
    onSelectViewChange = (value) => {
        this.setState({
            viewSelectValue: value
        });
        console.log(value);

        //Load Filters!
    };


    /**
	* This method is called when an item is selected in the "select Table" dropdown that is present in the top left.
	*/
    onSelectTableChange = (value) => {
        this.setState({
            tableSelectValues: value
        });
        console.log(value);

        //Load Table Columns
    };


    /**
	* This method is called when the user clicks on the 'clear all' button.
	*/
    onClearAllFilters = (event) => {
        console.log("clear all");
        var filterSummaryView = this.refs.filterSummaryView.getWrappedInstance();
        var columnsFilterApplied = filterSummaryView.refs;
        //filterSummaryView.handleRowDel('StaffAssigned');
        
        for (var property in columnsFilterApplied) {
            columnsFilterApplied[property].onDelEvent();
        }
        
    };


    /**
	* This method is called when the user clicks on the 'Hide/Show'.
	*/
    onHideFilteredData = (event) => {
        console.log("Hide");
        this.setState({
            hideShowButtonTextFlag: !this.state.hideShowButtonTextFlag,
        });

        //console.log(this.state.GLOBAL.getData()["Filter"]["Range"]);
        
        //if the flag true then button --> |HIDE| else button --> |SHOW|
        if (this.state.hideShowButtonTextFlag) {
            //hide the glyphs that don't match the filter critera.
        }
        else {
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
        this.setState({ viewSelectValue:null });

        //close the menu
        this.handleOpenClose('menu',false,event);
    };

     /**
	* This method is called when the user clicks on the 'Save' inside the menu.
	*/
    onMenuSaveClick = (event) => {
        console.log("Save");
        
        //Save As call
        if(this.state.viewSelectValue == null || this.state.viewSelectValue == "")
        {
            this.onMenuSaveAsClick(event);
            return;
        }

        //Save
        this.saveView(this,this.state.viewSelectValue);
        
        //close the menu
        this.handleOpenClose('menu',false,event);
    };

    /**
	* This method is called when the user clicks on the 'Save As' inside the menu.
	*/
    onMenuSaveAsClick = (event) => {
        console.log("Save As");
        
        this.handleOpenClose('save',true);
        //close the menu
        this.handleOpenClose('menu',false,event);
    };

    shouldComponentUpdate(newProps,newState){
        if (this.state != newState) {
            return true;
        }
        else if (this.props.initParams != newProps.initParams) {
            return true;
        }
        else if (this.props.settings != newProps.settings) {
            return true;
        }

        return false;
    };
	
	render = () => {
		 //Load values into the table select dropdown.
        var tableSelectItems = [];
        var context = this;
		
		tableSelectItems = this.props.initParams.tableSelectItems.map(function(value){
			return(
                {
                    label: value, value: value
                }
            );
		});
		
		return(
			<Flex id = "TopView">
                {/* Row 1 */}
                <Flex layout = "row" align = "space-between center" style = {{ height: '15%',marginBottom: '5px' }}>
                    <Flex flex = "80">
                        <Select 
                            className = "selectViewName"
                            simpleValue
                            value = { this.state.viewSelectValue } 
                            placeholder = "Select a view" 
                            options = { this.state.viewSelectItems } 
                            onChange = { this.onSelectViewChange } 
                        />
                    </Flex>
                    
                    <Flex divider />
                    <Flex flex = "20">
                        <div>
                            <RaisedButton
                                onClick = { (evt) => this.handleOpenClose('menu', true,evt) }
                                label = "Menu"
                                buttonStyle = {{ height: '28px', backgroundColor: this.props.settings.overviewButtonsColor.background }}
                                labelStyle = {{ fontSize: '13px', color: this.props.settings.overviewButtonsColor.text }}
                                primary = { true }
                            />

                            <Popover
                                open = { this.state.menu.open }
                                anchorEl = { this.state.menu.anchorEl }
                                onRequestClose = { (evt) => this.handleOpenClose('menu', false,evt) }
                                style = {{ fontSize: '13px' }}
                            >
                                <Menu>
                                    <MenuItem primaryText = "New" className = "menuItemStyling" onClick = { this.onMenuNewClick }/>
                                    <MenuItem primaryText = "Save" className = "menuItemStyling" onClick = { this.onMenuSaveClick }/>
                                    <MenuItem primaryText = "Save As" className = "menuItemStyling" onClick = { this.onMenuSaveAsClick }/>
                                </Menu>
                            </Popover>

                            <Dialog
                                title = "Save View"
                                actions = {
                                    [
                                        <FlatButton
                                            label = "Save"
                                            primary = { true }
                                            onClick = { () => this.onSaveDailog(this) }
                                            style = {{ color: this.props.settings.saveModalColor.saveButton }}
                                        />,
                                        <FlatButton
                                            label = "Cancel"
                                            primary = { true }
                                            onClick = { () => this.handleOpenClose('save', false) }
                                            style = {{ color: this.props.settings.saveModalColor.cancelButton }}
                                        />
                                    ]
                                } 
                                modal = { true }
                                open = { this.state.saveDailogOpen }
                            >
                                Please enter a name for the view you are saving. <br />
                                <TextField 
                                    id = "tf_viewName" 
                                    errorText = { this.state.viewNameTextFieldError } 
                                    floatingLabelText = "View Name" 
                                    underlineFocusStyle = {{ borderColor: this.props.settings.saveModalColor.textFieldUnderline }}
                                    floatingLabelStyle = {{ color: this.props.settings.saveModalColor.textFieldUnderline }}
                                /> <br />
                                <label id = "lbl_saveError" hidden style = {{ color:'red' }}> Error! A view with the same name already exists! Please provide a different name! </label>
                            </Dialog>

                        </div>
                    </Flex>
                </Flex>
                                
                {/* Row 2 */}
                <Flex layout = "row" style = {{ height:'65%',marginBottom: '5px' }}>
                    {/* <FilterNavTabs scrollToElement = {this.props.initParams.scrollToElement} /> */}
                    <FilterViewForm ref = 'filterSummaryView' onScroll = { (element, elastic) => this.props.initParams.scrollToElement(element, elastic) }/> 
                </Flex>

                
                
                
                
                {/* Row 3 */}
                <Flex layout = "row" style = {{ height:'10%',marginBottom: '5px' }}>
                    <Flex flex = "25">
                        <RaisedButton 
                            label = "Clear All" 
                            buttonStyle = {{ height: '28px', width:'100%', paddingTop: '5px', backgroundColor: this.props.settings.overviewButtonsColor.background }}
                            labelStyle = {{ fontSize: '13px', height: '28px', color: this.props.settings.overviewButtonsColor.text }} 
                            style = {{ height: '28px', width:'100%' }}
                            onClick = { this.onClearAllFilters }
                            primary = {true } />
                    </Flex>

                    <Flex divider />

                    <Flex flex = "25">
                        <RaisedButton 
                            label = { this.state.hideShowButtonTextFlag ? "Hide" : "Show" }
                            id = "buttonHideShow"
                            buttonStyle = {{ height: '28px', paddingTop: '5px', backgroundColor: this.props.settings.overviewButtonsColor.background }}
                            labelStyle = {{ fontSize: '13px', height: '28px', color: this.props.settings.overviewButtonsColor.text }} 
                            style = {{ height: '28px', width:'100%' }} 
                            onClick = { this.onHideFilteredData.bind(this) }
                            primary = { true } />

                            {/*<Toggle
                                label="Hide Filtered Data"
                                style={{paddingTop:'3px'}}
                                labelPosition="right"
                                />
                            */}
                    </Flex>

                    <Flex divider />

                    <Flex flex = "35">
                        <RaisedButton 
                            primary = { true } 
                            label = "Apply Filters" 
                            onClick = { this.applyFilter.bind(this) } 
                            buttonStyle = {{ height: '28px', paddingTop: '5px', backgroundColor: this.props.settings.overviewButtonsColor.background }}
                            labelStyle = {{ fontSize: '13px', height: '28px', color: this.props.settings.overviewButtonsColor.text }} 
                            style = {{ height: '28px', width:'100%' }} 
                        />
                    </Flex>
                    
                </Flex>
                
                {/* Row 4 */}
                <Flex layout = "row" style = {{ height:'10%' }}>
                    {/*<SelectField
                        value = { this.state.tableSelectValues }
                        onChange = { this.onSelectTableChange }
                        style = {this.selectStyle.style}
                        hintText = "Select Table"
                        multiple = { true }
                        hintStyle = {this.selectStyle.hintStyle}
                        iconStyle = {this.selectStyle.iconStyle}
                        underlineStyle = {this.selectStyle.underlineStyle}
                        selectedMenuItemStyle = {this.selectStyle.selectedMenuItemStyle}
                    >
                        {tableSelectItems}
                    </SelectField>*/}

                    <Select 
                        multi 
                        className="selectTableName"
                        simpleValue
                        value={this.state.tableSelectValues} 
                        placeholder="Select your table(s)" 
                        options={tableSelectItems} 
                        onChange={this.onSelectTableChange.bind(this)} 
                    />

                </Flex>
                
                
            </Flex>
		);
	}

    selectStyle ={
        style:{
            width:"100%", fontSize:'13px', maxHeight:'30px', border:'1px solid #aaaaaa',borderRadius:'5px'
        },
        hintStyle : { 
            color: this.props.settings.tableSelectColor.text ,
            bottom: '3px'
        },
        iconStyle : { 
            fill: this.props.settings.tableSelectColor.text, height: '30px',padding: '0px'
        },
        underlineStyle : { 
            borderColor: this.props.settings.tableSelectColor.text, display:'none', 
        },
        selectedMenuItemStyle : { 
            backgroundColor: this.props.settings.tableSelectColor.selectedBackground, color: this.props.settings.tableSelectColor.selectedText
        }
    };
}


const FLAVOURS = [
	{ label: 'Chocolate', value: 'chocolate' },
	{ label: 'Vanilla', value: 'vanilla' },
	{ label: 'Strawberry', value: 'strawberry' },
	{ label: 'Caramel', value: 'caramel' },
	{ label: 'Cookies and Cream', value: 'cookiescream' },
	{ label: 'Peppermint', value: 'peppermint' },
];


const mapStateToProps = function(state){
  return {
    GLOBALSTORE: state.filterState,
    settings: state.filterState.Settings
  }
};

export default connect(mapStateToProps)(TopView);
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
import DualListBox from 'react-dual-listbox';
import Collapsible from 'react-collapsible';
import FilterViewForm from './filterView.js';
import { connect } from 'react-redux';
import 'font-awesome/css/font-awesome.css';
import 'react-dual-listbox/lib/react-dual-listbox.css';
import './filterNav.css';

const mapStateToProps = function(state){
  return {
    GLOBAL: state.filterState.Filter,
  }
};

export const init = (storeFilterStruc) => ({
  type: 'INIT',
  storeFilterStruc
});


class FilterNav extends Component {

    constructor(props) {
        super(props);
        
        var viewSelectItems= [];
        var tableSelectItems = [];
        var appliedFiltersItems = [];
        var objCols = {};
        var tableData = this.fetchData();

        //Load values into the view select dropdown.
        viewSelectItems = this.makeList(['view 0','view 1','view 4','view 5'],"viewSelectItems");

        //Load values into the table select dropdown.
        tableSelectItems = this.makeList(['table 0','table 1','table 4','table 5'],"tableSelectItems");

        //For now! dummy filters too!
        appliedFiltersItems = this.makeList(['Filter 0','Filter 1','Filter 4','Filter 5'],"appliedFiltersItems");

        //Make columns and global store structure
        this.makeFilterStructure(tableData);
        
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
            pinDailog: {
                open: false,
            },
            tableData: tableData,
            appliedFiltersItems: appliedFiltersItems,
            viewSelectItems: viewSelectItems,
            tableSelectItems: tableSelectItems,
            activeColumns: [],
            pinnedDialogOptionValues: [],
            pinnedDialogSelectedValues: [],
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
     * This function holds the templates of certain UI elements. Call this and pass a type
     * to generate a JSX code that you can use to render that particular element.
     * @param {array} arrValues: Single dimension array of values.
     * @param {string} type: any one of [viewSelectItems,tableSelectItems,appliedFiltersItems,columns]
     * @param {object} extra: An object to pass extra params.
     */
    makeList(arrValues,type,extra){
        if(!Array.isArray(arrValues))
            return "PLEASE PROVIDE AN ARRAY";

        var len = arrValues.length;
        var index;
        var arrReturn =[];
        var objReturn = null;

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
        return objReturn != null ? objReturn : arrReturn;
    };

    /**
     * 
     */
    makeFilterStructure = (Obj,Options) => {
        var rangeStructure = {};
        var elasticStructure = {};
        
        for(var property in Obj){
            var column = property;
            var context = this;
            var minMax;
            var displayName = "";
            
            var type = isNaN(Obj[property][0]) ? 'Text' : 'Number';

            minMax = type=='Number'? this.findMinMax(Obj[property]) : {min:0,max:0};

            rangeStructure[column] = {
                rangeList: [[minMax.min,minMax.max,( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),false]],
                highlightedValues:[],
                bounds:[minMax.min,minMax.max]
            };
            
            elasticStructure[column] = {
                selectedValues: [],
                highlightedValues:[],
                applied: false,
                pinned: false,
                type: type,
                displayName: this.generateDisplayName(column)
            };
        }
        this.props.dispatch(init({Ranges:rangeStructure,Elastic:elasticStructure}));
    };

    /**
     * This function will make columns and filterStructure
     * @param {Object} data - pass object of objects
     *  Eg: {
     *     colName: {
     *        data: []
     *      }      
     * }
     */
     makeColumns = (data,extra) => {
        var pinnedDialogValues = [];
        var pinnedColumns = [];
        var temp = null;
        var index;
        var columnsFilterStructure = this.props.GLOBAL;
        var arrColumnsReturn = [];
        var arrPinnedColumnsReturn = [];
        var arrPinDialogOptions = [];
        var arrPinDialogSelected = [];
        var objReturn = {};

        for(var property in data)
        {
            var columnName = property;
            var colElasticFilterStruc = columnsFilterStructure.Elastic ? columnsFilterStructure.Elastic[property] : {};
            var colRangeFilterStruc = columnsFilterStructure.Range ? columnsFilterStructure.Range[property] : {};
            var context = this;
            var displayName = colElasticFilterStruc.displayName;

            temp = <Collapsible 
                transitionTime={200} 
                key={columnName} 
                triggerOpenedClassName="columnNameHeader"
                contentOuterClassName="cursorNormal"
                trigger={
                        <div>
                                <IconButton 
                                id={"btn_"+columnName} 
                                onClick={context.onPinClick.bind(context)} 
                                iconClassName= {colElasticFilterStruc.pinned ? "fa fa-thumb-tack pinned " + columnName : "fa fa-thumb-tack unpinned " + columnName}
                                style={{padding:'0px',width:'inherit',height:'inherit'} } />
                            <span 
                                style={{
                                paddingLeft: '10px',
                                fontSize: '1rem'
                            }}>
                                {displayName}
                            </span>
                        </div>} 
                triggerClassName='columnNameHeader'
                >
                    <FilterTabs internalColName={colElasticFilterStruc.pinned ? columnName+"_pinned" : columnName} id={columnName} displayName={displayName} data={data[columnName]}></FilterTabs>
            </Collapsible>;

            arrColumnsReturn.push(temp);
            arrPinDialogOptions.push({value: columnName, label: displayName});
            if(colElasticFilterStruc.pinned) 
            {
                arrPinnedColumnsReturn.push(temp);
                arrPinDialogSelected.push({value: columnName, label: displayName});
            }
        }
        objReturn = {columns:arrColumnsReturn,pinnnedColumns:arrPinnedColumnsReturn,pinnedOptions:arrPinDialogOptions,pinSelected:arrPinDialogSelected};
        return objReturn;
    };

    /**
     * This function replaces "_" with "<space>" and replaces "<Capital letter>" with "<space><Capital letter>" to make the display name!
     * @param str: The column name as it is.
     * @return display name string.
     */
    generateDisplayName = (str) => {
        var newString = str.charAt(0);
        var len = str.length;
        for(var i=1;i<len;i++){
            var ch = str.charAt(i);
            if(isNaN(ch))
            {
                
                if(ch == "_")
                {
                    newString = newString + " ";
                }
                else if(ch == ch.toUpperCase())
                {
                    newString = newString + " " + ch;
                }
                else{
                    newString = newString + ch;
                }
            }
        }
        return newString;
    };

    /**
     * This function caculates the min and max of the value in an array.
     * @param arrValues: an array of column data.
     * @return Object: {min: <MinValue>, max:<MaxValue>}
     */
    findMinMax = (arrValues) => {
        var len = arrValues.length;
        
        var obj = {
            min: arrValues[0],
            max: arrValues[0]
        };

        for(var i=0;i<len;i++)
        {
            if(arrValues[i] > obj.max)
                obj.max = arrValues[i];

            if(arrValues[i] < obj.min)
                obj.min = arrValues[i];
        }

        return obj;
    };

    /**
     * 
     */
    handleOpenClose = (strName,open,evt) =>{
        switch(strName){
            case 'save':
            {
                if(open){
                    this.setState({saveDailog:{
                            open: true
                        },
                        viewNameTextFieldError: ""
                    }); 
                }
                else{
                    this.setState({saveDailog:{
                        open: false
                    }});
                }
                break;
            }
            case 'pin':
            {
                if(open){
                    this.setState({pinDailog:{
                            open: true
                        }
                    }); 
                }else{
                    this.setState({pinDailog:{
                        open: false
                    }});
                }
                break;
            }
            case 'menu':
            {
                 if(open){
                   evt.preventDefault();

                    this.setState(
                    {
                        menu: {
                            open: true,
                            anchorEl: evt.currentTarget
                        }
                    });
                }else{
                    this.setState(
                    {
                        menu: {
                            open: false
                        }
                    });
                }
                break;
            }
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
                context.handleOpenClose('save',false);
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

        //console.log(this.state.GLOBAL.getData()["Filter"]["Range"]);
        
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
        this.setState({viewSelect:{value:null}});

        //close the menu
        this.handleOpenClose('menu',false,event);
    };

     /**
	* This method is called when the user clicks on the 'Save' inside the menu.
	*/
    onMenuSaveClick = (event) => {
        console.log("Save");
        
        //Save
        this.saveView();
        
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

    /**
     * 
     */
    onPinnedOkDailog = (context) => {
        var pinnedValues = context.state.pinnedDialogSelectedValues;
        context.onPinClick(null,{pinnedValues: pinnedValues});
        this.handleOpenClose('pin',false);
    };

    /**
     * 
     */
    onPinClick = (event,extra) => {
        console.log("pinned to active!");
        var but, colName;
        var pinned;
        
        if(event)
        {
            event.stopPropagation();
            but = event.currentTarget;
            colName = but.id.split("btn_")[1];
            pinned = this.props.GLOBAL.Elastic[colName].pinned;
            this.props.dispatch({type:'Update_Pin', details:{colName: colName,pinned: !pinned}});
        }
        else{
            colName = extra.pinnedValues;
            var len = colName.length;
            //pin all together.
            for(var i=0;i<len;i++)
            {
                this.props.dispatch({type:'Update_Pin', details:{colName: colName[i],pinned: !pinned}});
            }
        }
    };

    componentDidMount = () => {
        //this.setState({ 'abc':'123'});
    }

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

    render = () => {
         var pinnedEmptyString = <div className="centerText cursorNormal"><h3> Nothing Pinned! </h3><label> Anything you pin shows up here, so <br/> you can keep track of filters you <br/> need to get back to. </label></div>;
         var columnsObj = this.makeColumns(this.state.tableData);

        return (
            <div className="TopNav" id="FilterWindowOuterContiner" style={{height: '100%',transition:'1s',paddingLeft:'1%',paddingRight: '1%'}}>
                <div>
                    <AlertContainer ref={a => this.msg = a} {...this.alertOptions} />
                </div>
                <Flex layout="column" style={{height: '100%'}}>
                     {/* TOP SECTION */}
                    <Flex flex="50" id="TopView">
       
                        {/* Row 1 */}
                        <Flex layout="row" align="space-between center"    style={{height: '15%'}}>
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
                                        onClick={(evt) => this.handleOpenClose('menu',true,evt)}
                                        label="Menu"
                                        buttonStyle={{height: '28px'}}
                                        labelStyle={{fontSize: '13px'}}
                                        primary={true}
                                    />
                                    <Popover
                                        open={this.state.menu.open}
                                        anchorEl={this.state.menu.anchorEl}
                                        onRequestClose={(evt) => this.handleOpenClose('menu',false,evt)}
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
                                                onClick={() => this.handleOpenClose('save',false)}/>]
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
                        <Flex layout="row" style={{height:'65%'}}>
                            {/* <div style={{width:'100%',border:'1px',borderColor: '#e0e0e0',borderRadius:'10px',borderStyle: 'double',margin:'2px',overflow:'auto'}} className="sidenavbar">
                                <List id="FilterList">
                                    {this.state.appliedFiltersItems}
                                </List>
                            </div> */}

                            <FilterViewForm />
                        </Flex>
                        
                        
                        {/* Row 3 */}
                        <Flex layout="row" style={{height:'10%'}}>
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
                            <Flex divider />
                            <Flex flex="35">
                            <RaisedButton 
                                primary={true} 
                                label="Apply Filters" 
                                onClick={this.applyFilter.bind(this)} 
                                buttonStyle={{height: '28px',paddingTop: '5px'}}
                                labelStyle={{fontSize: '13px',height: '28px',}} 
                                style = {{height: '28px',width:'100%',}} 
                                    />
                            </Flex>
                            
                        </Flex>
                        
                        {/* Row 4 */}
                        <Flex layout="row" style={{height:'10%'}}>
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


                    {/* BOTTOM SECTION */}

                    <Flex flex="50">

                        
                        <Collapsible 
                            transitionTime={200} 
                            contentOuterClassName="cursorNormal"
                            trigger={
                                <div>
                                    <i className="fa fa-thumb-tack" style={{fontSize: '1.3rem'}}></i>
                                    <span 
                                        style={{
                                        paddingLeft: '10px',
                                        fontSize: '1rem'
                                    }}>
                                        Pinned
                                    </span>
                                </div>}>

                            <Dialog
                                title="Pinning Views!"
                                actions={
                                    [<FlatButton
                                        label="Ok"
                                        primary={true}
                                        onClick={() => this.onPinnedOkDailog(this)}/>,
                                        <FlatButton
                                        label="Cancel"
                                        primary={true}
                                        onClick={() => this.handleOpenClose('pin',false)}/>]
                                }
                                modal={true}
                                open={this.state.pinDailog.open}>
                                    <DualListBox
                                        canFilter
                                        preserveSelectOrder 
                                        ref="pinnedDialog"
                                        options={columnsObj.pinnedOptions}
                                        selected={this.state.pinnedDialogSelectedValues}
                                        onChange={(selected) => {
                                            this.setState({pinnedDialogSelectedValues: selected});
                                            //this.onPinClick(null,{pinnedValues: []})
                                            debugger;
                                        }}
                                    />
                            </Dialog>

                                
                                <IconButton 
                                    onClick={() => this.handleOpenClose('pin',true)}
                                        iconClassName="fa fa-plus-square"
                                        iconStyle={{color: '#00bcd4'}} />

                                {columnsObj.pinnnedColumns.length > 0 ? columnsObj.pinnnedColumns : pinnedEmptyString}
                        </Collapsible>

                        <Collapsible 
                            transitionTime={200} 
                            contentOuterClassName="cursorNormal"
                            trigger={
                                <div>
                                    <i className="fa fa-filter" style={{fontSize: '1.3rem'}}></i>
                                    <span 
                                        style={{
                                        paddingLeft: '10px',
                                        fontSize: '1rem'
                                    }}>
                                        Filters
                                    </span>
                                </div>}>
                               {columnsObj.columns}
                        </Collapsible>

                    </Flex>
                </Flex>
            </div>
        );
    }
}


export default connect(mapStateToProps)(FilterNav);

//{this.columnsObj.columns}
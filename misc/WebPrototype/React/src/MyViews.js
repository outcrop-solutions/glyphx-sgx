import React from 'react';
import { connect } from 'react-redux';
import { Step, Stepper, StepLabel } from 'material-ui/Stepper';
import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import SearchBox from './SearchBox.js';
import RaisedButton from 'material-ui/RaisedButton';
import MenuItem from 'material-ui/MenuItem';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import DropDownMenu from 'material-ui/DropDownMenu';
import Collapsible from 'react-collapsible';
import ExpandTransition from 'material-ui/internal/ExpandTransition';
import FontIcon from 'material-ui/FontIcon';
import './MyViews.css';
import './General.css';


class MyViews extends React.Component {

    constructor(props){
        super(props);

        this.state = {
            sharedViewsData: [{sharedBy:'MSloan',name:'SubViz1',origin:'VizGlobalAdmissions',date:'7-7-2017'},
                              {sharedBy:'BLewis',name:'SubViz2',origin:'VizG',date:'7-7-2017'},
                              {sharedBy:'BHolster',name:'Viz3',origin:'VizGA',date:'7-7-2017'},
                              {sharedBy:'MSloan',name:'Viz1',origin:'VizGlobalAdmissions',date:'7-7-2017'},
                              {sharedBy:'BLewis',name:'Viz2',origin:'VizG',date:'7-7-2017'},
                              {sharedBy:'BHolster',name:'SubViz3',origin:'VizGA',date:'7-7-2017'},
                              {sharedBy:'MSloan2',name:'SubViz1',origin:'VizGlobalAdmissions',date:'7-7-2017'},
                              {sharedBy:'BLewis2',name:'SubViz2',origin:'VizG',date:'7-7-2017'},
                              {sharedBy:'BHolster2',name:'SubViz3',origin:'VizGA',date:'7-7-2017'},
                              {sharedBy:'MSloan2',name:'Viz1',origin:'VizGlobalAdmissions',date:'7-7-2017'},
                              {sharedBy:'BLewis2',name:'Viz2',origin:'VizG',date:'7-7-2017'},
                              {sharedBy:'BHolster2',name:'SubViz3',origin:'VizGA',date:'7-7-2017'},],
            savedViewsData: [{name:'SubViz1',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'SubViz2',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'SubViz3',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'SubViz4',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'SubViz5',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'SubViz6',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'Viz1',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'Viz2',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'Viz3',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'Viz4',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'Viz5',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'},
                             {name:'Viz6',origin:'VizGlobalAdmissions',lastModified:'7-7-2017'}]
        }
    }

    render(){
        return(
            <div style = {{ padding: "10px" }} >

                <div style = {{ marginBottom: "3px" }} >
                    <Collapsible
                        transitionTime = {200} 
                        trigger = "Saved Views"
                        className = "noselect"
                        openedClassName = "noselect"
                    >
                        
                        <SimpleTable  
                            id = "SavedViews"
                            settings = { this.props.settings }
                            data = { this.state.savedViewsData }
                        />
                    </Collapsible>
                </div>

                <div style = {{ marginBottom: "3px" }} >
                    <Collapsible
                        transitionTime = {200} 
                        trigger = "Shared Views"
                        className = "noselect"
                        openedClassName = "noselect"
                    >
                        <SimpleTable  
                            id = "SharedViews"
                            settings = { this.props.settings }
                            data = { this.state.sharedViewsData }
                        />
                    </Collapsible>
                </div>
                
            </div>       
        );
    }
};


/**
 * This represents the searchbox and the table that you see in the view manager views.
 * props:
 *      {
 *          id="SavedViews"
 *          settings = {this.props.settings}
 *          data = {this.state.savedViewsData} [{},{},{},...] Any column name is acceptable except "searchString" && "index".
 *      }
 * This class automatically creates the columns and maps the data to them.
 * It's SearchBox Also supports multi column search.
 */
class SimpleTable extends React.Component {
        
    state = {
        selected: [],
        flatData: this.props.data
    };

    columnCount = 0;
    
    isSelected = (index) => {
        return this.state.selected.indexOf(index) !== -1;
    };

      /**
     * This method searches on the data of table. Allows Multisearch using "," as a separator. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table("table +id") and the textfield("tf +id").
     */
	onKeyUpMultiSearch = (context, id) => {
		var input, filter, tr, tdList, td, i,searchString;
        input = document.getElementById("tf-" + id);
        filter = input.value.toUpperCase();
        tr = this.fetchTableRows(this.props.id);
		var shouldBeVisible;
        var filterValues = filter.split(',');
        
        for (i = 0; i < this.props.data.length; i++) {
            searchString = this.state.flatData[i].searchString;
            shouldBeVisible = false;
            for (var k = 0; k < filterValues.length; k++) {
                if (searchString.toUpperCase().indexOf(filterValues[k]) > -1) {
                    shouldBeVisible = true;
                    break;
                } 
            }
            
            !shouldBeVisible ? tr[i].style.display = "none" : tr[i].style.display = "";
        }
    }
    

    /**
     * Returns the rendered table rows(HTML).
     * @param id: tableId
     * @returns: array of table rows.
     */
    fetchTableRows = (id) => {
        var table = document.getElementsByClassName(id)[1]; // The table header and table data are 2 separate tables. So we fetch the 2nd index as we want the data table.
        var tr = table.getElementsByTagName("tr");
        return tr;
    };


    clearSearchBox = (evt, strSearchBoxId) => {
		var sb = document.getElementById("tf-" + strSearchBoxId);
		if (sb) {
			sb.value = '';
			this.onKeyUpMultiSearch(this, strSearchBoxId, this.state.indexColumnToSearch);
		}
    }
    
    /**
     * This is called when the header is clicked for sorting.
     * @param {Obj} evt: The actual event object.
     * @return {string} columnName: id of the columnHeader.
     */
    onSortClick = (evt, columnName, type) => {
        var columnObj = document.getElementById(this.props.id + columnName);
        var columns = document.getElementsByClassName(this.props.id + "sortableColumn");
        var otherColumn;
        var flatData = this.state.flatData.slice();

        //find the other column & remove the sort applied icon.
        for (var i = 0; i < columns.length; i++) {
            if (columns[i].id != this.props.id + columnName) {
                columns[i].classList.remove('fa-sort-amount-desc');
                columns[i].classList.remove('fa-sort-amount-asc');
                columns[i].classList.add('fa-sort');
            }
        }
        
        //update the sorting icon
        var sortDirection = this.updateSortIcon(columnObj);

        switch(type) {
            case "Text":
                if (sortDirection == "asc") {
                    flatData.sort(function(a, b) {
                        if (a[columnName] < b[columnName]) {
                            return -1;
                        }
                        if (a[columnName] > b[columnName]) {
                            return 1;
                        }
                        return 0;
                    });
                }
                else{
                    flatData.sort(function(a,b) {
                        if (a[columnName] < b[columnName]) {
                            return -1;
                        }
                        if (a[columnName] > b[columnName]) {
                            return 1;
                        }
                        return 0;
                    }).reverse();
                }
                    
                break;
            case "Number":
                if (sortDirection == "asc") {
                    flatData.sort(function(a, b) { return a[columnName] - b[columnName] });
                }
                else {
                    flatData.sort(function(a, b) { return a[columnName] - b[columnName] }).reverse();
                }
                break;
        }
        
        this.setState({flatData: flatData});
    }
    
    /**
     * This updates the sortIcon for the correspoding column header clicked.
     * @param {Obj} columnObj: The column header object whose sort icon has be updated.
     */
    updateSortIcon = (columnObj) => {
        var removeIconName = "";
        var addIconName = "";
        var sortDirection = "";
        var currentState = columnObj.classList.contains('fa-sort') ? 'init' : (columnObj.classList.contains('fa-sort-amount-asc') ? 'asc' : 'desc');
        
        switch(currentState){
            case "init":
                removeIconName = "fa-sort";
                addIconName = "fa-sort-amount-asc";
                sortDirection = "asc";
                break;
            case "asc":
                removeIconName = "fa-sort-amount-asc";
                addIconName = "fa-sort-amount-desc";
                sortDirection = "desc";
                break;
            case "desc":
                removeIconName = "fa-sort-amount-desc";
                addIconName = "fa-sort-amount-asc";
                sortDirection = "asc";
                break;
        }
        
        columnObj.classList.remove(removeIconName);
        columnObj.classList.add(addIconName);
        return sortDirection;
    }

    handleRowSelection = (selectedRows) => {
        this.setState({
            selected: selectedRows,
        });

        //open that viz.

    };
    
    render() {
        
        var colNames = [];
        var rows = [];
        var columnCount = 0;

        if (this.state.flatData != null && this.state.flatData.length > 0) {
            var keys = Object.keys(this.state.flatData[0]);
            columnCount = keys.length;

            if (keys.indexOf('searchString') > -1) {
                columnCount--;
            }
            
            if (keys.indexOf('index') > -1) {
                columnCount--;
            }

            for (var j = 0; j < this.state.flatData.length; j++) {
                var dataRowObject = this.state.flatData[j];
                var data = [];
                var searchString = '';
                
                for (var k = 0; k < columnCount; k++) {
                    let temp = keys[k];
                    if (j == 0) {
                        colNames.push(
                            <TableHeaderColumn key = { this.props.id + temp } style = {{ height: "36px", color: "#000000", paddingLeft: "10px", paddingRight: "0px" }} > 
                                <div onClick = { (evt) => this.onSortClick(evt, temp, 'Text') } >
                                    {temp} &nbsp;
                                    <i id = { this.props.id + temp } className = { "fa fa-sort " + this.props.id + "sortableColumn" } /> 
                                </div>
                            </TableHeaderColumn>
                        );
                    }
    
                    data.push(<TableRowColumn key = { dataRowObject[temp] } style = {{ paddingLeft: "10px", paddingRight: "0px" }} > {dataRowObject[temp]} </TableRowColumn>);
                    searchString = searchString + dataRowObject[temp] + ',';
                }
                
                // Creating a search string to help filter data.
                this.state.flatData[j].searchString = searchString;
                this.state.flatData[j].index = j;

                rows.push(
                    <TableRow key = {j} selected = { this.isSelected(j) } >
                        {data}
                    </TableRow>
                );
            }

        }

        return (
            <div>
                <div style = {{ margin: "1px 4px -9px", width: "98%" }} >
                    <SearchBox 
                        ref = "SearchBox"
                        hintText = "Search for value..." 
                        settings = {{
                            SearchBoxClearHover: this.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
                            searchBoxUnderline: this.props.settings.colors.pinFilterColor.searchBoxUnderline,
                            overviewButtonsColorBg: this.props.settings.colors.overviewButtonsColor.background,
                            overviewButtonsColorText: this.props.settings.colors.overviewButtonsColor.text,
                            tableSelectColor: this.props.settings.colors.tableSelectColor.background
                        }}
                        onTextFieldValueChange = { (evt) => this.onKeyUpMultiSearch(this, this.props.id) }
                        id = { "tf-" + this.props.id }
                        collapseButton = { false }
                    />
                </div>

                <br/>

                <Table 
                    className = { this.props.id } 
                    onRowSelection = { this.handleRowSelection } 
                    wrapperStyle = {{
                        maxHeight: "350px",
                        overflow: 'hidden',
                        borderRadius: "4px",
                    }}
                    bodyStyle = {{
                        maxHeight: "314px",
                        overflow: 'auto',
                        width: "100%",
                    }}
                >
                    <TableHeader
                        adjustForCheckbox = { false }
                        displaySelectAll = { false }
                        fixedHeader = { true }
                        fixedFooter = { true }

                    >
                    <TableRow style = {{ height: "36px", backgroundColor: "#dadada" }} >
                        {colNames}
                    </TableRow>
                    </TableHeader>
                    <TableBody displayRowCheckbox = { false } >
                        {rows}
                    </TableBody>
                </Table>
            </div>
        );
      }
}

/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
  }
}


/**
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(MyViews);
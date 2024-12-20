import React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import { makeServerCall } from './ServerCallHelper.js';
import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import SearchBox from './SearchBox.js';
import Collapsible from 'react-collapsible';
import Flexbox from 'flexbox-react';
import './General.css';


class MyViews extends React.Component {

    constructor(props){
        super(props);
		this.goToVizView = this.goToVizView.bind(this);
		this.onClickSaved = this.onClickSaved.bind(this);
        this.state = {
            savedViews: [],//this.props.storedViews.savedViews,
            sharedViews: []
        }
    }
	
	goToVizView(){
		this.props.history.push('/glyph-viewer');
	}
	
	onClickSaved(data){
		this.props.onSavedViewSelect(data,this.goToVizView);
	}

    render() {
        return (
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
                            data = { this.props.storedViews.savedViews }
                            onClickSaved = { this.onClickSaved }
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
                            data = { this.state.sharedViews }
                            onClickSaved = { this.onClickSaved }
                        />
                    </Collapsible>
                </div>
                
            </div>       
        );
    }
};


/**
 * Represents the searchbox and the table that you see in the view manager views.
 * Automatically creates the columns and maps the data to them.
 * Supports multi-column search.
 */
class SimpleTable extends React.Component {
    
	constructor(props){
		super(props);
		this.state = {
			selected: [],
			flatData: this.props.data
		};
	}

    columnCount = 0;

	componentWillReceiveProps(nextProps) {
		if(this.props.flatData !== nextProps.data)
			this.setState({flatData: nextProps.data});
	}

    /**
     * -ADCMT
     * @param index: -ADCMT
     */
    isSelected = (index) => {
        return this.state.selected.indexOf(index) !== -1;
    };


      /**
     * This method searches on the data of table. Allows Multisearch using "," as a separator. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table("table +id") and the textfield("tf +id").
     */
	onKeyUpMultiSearch = (context, id) => {
		var input, filter, tr, i,searchString;

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
        // The table header and table data are 2 separate tables. So we fetch the 2nd index as we want the data table.
        var table = document.getElementsByClassName(id)[1]; 
        var tr = table.getElementsByTagName("tr");
        return tr;
    };


    /**
     * -ADCMT
     * @param e: -ADCMT
     * @param strSearchBoxId: -ADCMT
     */
    clearSearchBox = (e, strSearchBoxId) => {
		var sb = document.getElementById("tf-" + strSearchBoxId);
		if (sb) {
			sb.value = '';
			this.onKeyUpMultiSearch(this, strSearchBoxId, this.state.indexColumnToSearch);
		}
    }

    
    /**
     * This is called when the header is clicked for sorting.
     * @param {Obj} e: The actual event object.
     * @param columnName: -ADCMT
     * @param type: -ADCMT
     * @return {string} columnName: id of the columnHeader.
     */
    onSortClick = (e, columnName, type) => {
        var columnObj = document.getElementById(this.props.id + columnName);
        var columns = document.getElementsByClassName(this.props.id + "sortableColumn");
        var flatData = this.state.flatData.slice();

        // Find the other column & remove the sort applied icon.
        for (var i = 0; i < columns.length; i++) {
            if (columns[i].id != this.props.id + columnName) {
                columns[i].classList.remove('fa-sort-amount-desc');
                columns[i].classList.remove('fa-sort-amount-asc');
                columns[i].classList.add('fa-sort');
            }
        }
        
        // Update the sorting icon
        var sortDirection = this.updateSortIcon(columnObj);

        switch (type) {
            case "Text":
                if (sortDirection == "asc") {
                    flatData.sort(
                        function(a, b) {
                            if (a[columnName] < b[columnName]) {
                                return -1;
                            }
                            if (a[columnName] > b[columnName]) {
                                return 1;
                            }
                            return 0;
                        }
                    );
                }

                else {
                    flatData.sort(
                        function(a,b) {
                            if (a[columnName] < b[columnName]) {
                                return -1;
                            }
                            if (a[columnName] > b[columnName]) {
                                return 1;
                            }
                            return 0;
                        }
                    ).reverse();
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

            default:
                return null;
        }
        
        this.setState({ flatData: flatData });
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
        
        switch (currentState) {
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

            default:
                return null;
        }
        
        columnObj.classList.remove(removeIconName);
        columnObj.classList.add(addIconName);

        return sortDirection;
    }

    handleRowSelection = (selectedRows) => {
        this.props.onClickSaved(this.state.flatData[selectedRows]);
    };
    
    render() {
        var colNames = [];
        var rows = [];
        var columnCount = 0;

        /*
        debugger;

        if (this.state.flatData && this.state.flatData.length > 0) {
            var keys = Object.keys(this.state.flatData[0]);

            for (var i = keys.length - 1; i > -1 ; i--) {
                if (keys[i] === "ID" || keys[i] === "UserID" || keys[i] === "QueryString" || keys[i] === "InstitutionId" || keys[i] === "Shareable") {
                    keys.splice(i, 1);
                }
            }

            var columnList = keys.map( function(col) {
                return (
                    <TableHeaderColumn key = { col } style = {{ height: "36px", color: "#000000", paddingLeft: "10px", paddingRight: "0px" }}>
                        {col === "OriginalVizName" ?
                            "Origin"
                            :
                            (col === "CreationDate" ?
                                "Created"
                                :
                                (col === "LastModifiedDate" ?
                                    "Modified"
                                    :
                                    col
                                )
                            )
                        }
                    </TableHeaderColumn>
                )
            });
            

            //debugger;

            var rows = [];

            var flatData = this.state.flatData;

            debugger;

            for (var i = 0; i < keys.length; i++) {
                let temp = keys[i];
            }

            for (var i = 0; i < flatData.length; i++) {
                rows.push(
                    <TableRow key = { flatData[i].ID } >
                        {keys.map( function(key) {
                            <TableRowColumn style = {{ paddingLeft: "10px", paddingRight: "0px" }} > 1 </TableRowColumn>
                        })}
                    </TableRow>
                );
            }

            debugger;
            keys;
            flatData[0];
            flatData[0][keys[3]];
            rows;
            columnList;
        }

*/






        if (this.state.flatData != null && this.state.flatData.length > 0) {
            var keys = Object.keys(this.state.flatData[0]);
            var flatData = this.state.flatData;

            for (var i = keys.length - 1; i > -1 ; i--) {
                if (keys[i] === "ID" || keys[i] === "UserID" || keys[i] === "QueryString" || keys[i] === "InstitutionId" || keys[i] === "Shareable" || keys[i] === "frontEndFilterString") {
                    keys.splice(i, 1);
                }
            }

            for (var i = 0; i < flatData.length; i++) {
                var cDate = new Date(flatData[i].CreationDate);
                var mDate = new Date(flatData[i].LastModifiedDate);

                cDate = (cDate.getMonth() + 1) + '-' + cDate.getDate() + '-' + cDate.getFullYear();
                mDate = (mDate.getMonth() + 1) + '-' + mDate.getDate() + '-' + mDate.getFullYear();

                flatData[i].CreationDate = cDate;
                flatData[i].LastModifiedDate = mDate;
            }

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
                    var displayTemp = (
                        (temp === "OriginalVizName" ?
                            "Origin"
                            :
                            (temp === "CreationDate" ?
                                "Created"
                                :
                                (temp === "LastModifiedDate" ?
                                    "Modified"
                                    :
                                    temp
                                )
                            )
                        )
                    );

                    if (j === 0) {
                        colNames.push(
                            <TableHeaderColumn key = { this.props.id + temp } > 
                                <div onClick = { (evt) => this.onSortClick(evt, temp, 'Text') } >
                                    {displayTemp} &nbsp;
                                    <i id = { this.props.id + temp } className = { "fa fa-sort " + this.props.id + "sortableColumn" } /> 
                                </div>
                            </TableHeaderColumn>
                        );
                    }
    
                    data.push(<TableRowColumn key = { dataRowObject[temp] } style = {{ paddingLeft: "10px", paddingRight: "0px" }} > {dataRowObject[temp]} </TableRowColumn>);
                    searchString = searchString + dataRowObject[temp] + ',';
                }
                
                // Creating a search string to help filter data.
                var fData = this.state.flatData[j];

                fData.searchString = searchString;
                fData.index = j;

                //this.setState({ flatData: fData });

                rows.push(
                    <TableRow key = {j} selected = { this.isSelected(j) } >
                        {data}
                    </TableRow>
                );
            }
        }

        

        return (
            <div>
                <div style = {{ margin: "1px 4px -9px", padding: "0px 8px" }} >
                    <SearchBox 
                        ref = "SearchBox"
                        id = { "tf-" + this.props.id }
                        collapseButton = { false }
                        hintText = "Search for value..." 
                        settings = {{
                            SearchBoxClearHover: this.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
                            searchBoxUnderline: this.props.settings.colors.pinFilterColor.searchBoxUnderline,
                            overviewButtonsColorBg: this.props.settings.colors.overviewButtonsColor.background,
                            overviewButtonsColorText: this.props.settings.colors.overviewButtonsColor.text,
                            tableSelectColor: this.props.settings.colors.tableSelectColor.background
                        }}
                        onTextFieldValueChange = { (evt) => this.onKeyUpMultiSearch(this, this.props.id) }
                    />
                </div>

                <br/>
                <Flexbox style = {{ padding: "0px 12px" }} >
                    <Table 
                        className = { this.props.id }
                        fixedHeader = { true }
                        fixedFooter = { true }
                        onRowSelection = { (row) => this.handleRowSelection(row) } 
                        height = "350px"
                        wrapperStyle = {{ borderRadius: "4px" }}
                        //onRowSelection = { () => console.log("row selected") }
                    >
                        <TableHeader
                            adjustForCheckbox = { false }
                            displaySelectAll = { false }
                            style = {{ backgroundColor: "#dadada" }}
                        >
                            <TableRow style = {{ height: "36px" }} >
                                {colNames}
                            </TableRow>
                        </TableHeader>

                        <TableBody displayRowCheckbox = { false } >
                            {rows}
                        </TableBody>
                    </Table>
                </Flexbox>
            </div>
        );
    }
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
 return {
   settings: state.filterState.Settings,
   storedViews: state.filterState.StoredViews,
   funnelData: state.filterState.FunnelData
 }
}


export const setCurrentVizParams = (vizParams) => ({
   type: 'SET_VIZ_PARAMS',
   vizParams,
});


/**
 * Connects the redux store to get access to global states.
 **/
export default withRouter(connect(mapStateToProps,null,null,{withRef:true})(MyViews));
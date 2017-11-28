import React from 'react';
import { connect } from 'react-redux';
import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import Checkbox from 'material-ui/Checkbox';
import ScrollIntoView from 'scroll-into-view-if-needed';
import SearchBox from './SearchBox.js';
import './General.css'


/**
 * Elastic Tables
 * @param id: id you want to give to the table.
 * @param internalColName: name of the column used for internal mapping purposes
 * @param displayName: the name of the column as showed on the UI.
 * @param tableData: {colName:{values:{value:'',count:'',recId:''},totalCount:,flatValues:[]} , ...}
 * @param selectedRows: -ADCMT
 * @param tableID: -ADCMT
 */

class FilterTable extends React.Component {

    refresh=true;

    constructor(props) {
        super(props);

        this.state = {
            selectAll: false,
            prevSelectedIndex: null,
            checkboxClicked:false,
            tableData: props.tableData,
            flatData: [],
            indexColumnToSearch: (props.columnToSearch ? props.columnToSearch : 1),
        };
    }

    componentWillReceiveProps(nextProps) {
        if(this.props.tableData != nextProps.tableData){
            this.setState({tableData: nextProps.tableData});
            this.refresh=true;
        }
    }

    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        return (this.props.tableState[this.props.id].selectedValues != nextProps.tableState[this.props.id].selectedValues || 
                this.props.settings != nextProps.settings || 
                this.state.flatData != nextState.flatData ||
                this.props.tableData != nextProps.tableData
                );

        /*
        if (this.props.tableState[this.props.id].selectedValues != nextProps.tableState[this.props.id].selectedValues) {
            return true;
        }
        else if (this.props.settings != nextProps.settings) {
            return true;
        }
        else if (this.state.flatData != nextState.flatData) {
            return true;
        }
        else {
            return false;
        }
        */
    }


    /**
	 * React built-in which is called when component updates
	 */
    componentDidUpdate() {
        if (this.props.tableState[this.props.id].selectedValues.length === this.state.flatData.length) {
            this.setState({ selectAll: true });
        }
        else {
            this.setState({ selectAll: false });
        }
    }


    /**
	 * React built-in which is called when component mounts
	 */
    componentDidMount() {
        this.componentDidUpdate();
        
        // Force update on mount for pinned columns to show the same selections before change
        this.forceUpdate();
    }


    /**
     * This processes the data and counts the number of ocurrences and removes duplicates.
     * @param tableData 
     * @return Object : {'data':<object of data in form of "dataValue : count",'totalCount': totalCount}
     */
    processData(tableData) {
        var temp = {};
        var totalCount = tableData.length;

        //ADCMT (This should be a normal loop and not map since map expects to be returning something)
        tableData.map((row) => {
            if (temp.hasOwnProperty(row)) {
                ++temp[row]['count'];
            }
            else {
                temp[row] = {value: row, count: 1};
            }
        });

        return {'data': temp, 'totalCount': totalCount};
    };


    /**
     * This method is called on selection of the row in the table. 
     * @param context: This is the actual instance of the FilterTable. Mainly used to access the state and store.
     * @param rowSelection: This is an array that has index(according to the table rows) of all the rows selected.
     * @param all: boolean value stating whether or not select all is clicked.
     * @param evt: -ADCMT
     * @param checked: -ADCMT
     */
    onRowSelect = (context, rowSelection, all, evt, checked) => {
            var index, len = context.state.flatData.length;
            var selectedValues;

            if (this.state.prevSelectedIndex == null) {
                this.setState({ prevSelectedIndex : rowSelection });
            }

            if (evt && evt.shiftKey) {
                // Implementation for shift key selection!
            }

            if (all) {
                this.setState({ selectAll: true });
                selectedValues = [];

                for (index = 0; index < len; index++) {
                    selectedValues.push(context.state.flatData[index].value);
                }
                checked = true;
            }

            else if (false === all) {
                this.setState({ selectAll: false });
                selectedValues = [];
                checked = false;
            }

            else {
                selectedValues = context.props.tableState[context.props.id].selectedValues.slice();
                if (checked) {
                    selectedValues.push(context.state.flatData[rowSelection].value);
                    if (selectedValues.length === len) {
                        this.setState({ selectAll: true });
                    }
                }

                else {
                    if (this.state.selectAll === true) {
                        this.setState({ selectAll: false });
                    }
                    selectedValues.splice(selectedValues.indexOf(context.state.flatData[rowSelection].value), 1);
                }
            }

            var filterStructure = {
                colName : context.props.id,
                selectedValues: selectedValues,
                value: (context.state.flatData[rowSelection] ? context.state.flatData[rowSelection].value : null),
                checked: checked,
                type: context.props.tableState[context.props.id].type,
                data: context.props.tableData.flatValues
            }

            context.props.dispatch(addRemoveElastic(filterStructure));        
    };
    

    /**
     * This method is called when the keyup event is fired in the search textfield on top of the table. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table("table +id") and the textfield("tf +id").
     * @param indexColumnToSearch: The search will work on the column whose number is passed here.(Rem the checkbox is column number 0)
     */
    onKeyUp = (context, id, indexColumnToSearch) => {
        var input, filter, tr, td, i;
        input = document.getElementById("tf-" + id);
        filter = input.value.toUpperCase();
        tr = this.fetchTableRows(this.getInternalTableName());

        //typeof input.value == "string" ? input.value.toUpperCase() :  input.value;

        // Loop through all table rows, and hide those who don't match the search query
        for (i = 0; i < tr.length; i++) {
            td = tr[i].getElementsByTagName("td")[indexColumnToSearch];
            if (td) {
                if (td.innerHTML.toUpperCase().indexOf(filter) > -1) {
                    tr[i].style.display = "";
                } 
                else {
                    tr[i].style.display = "none";
                }
            } 
        }
    };

    
     /**
     * This method searches on the data of table. Allows Multisearch using "," as a separator. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table("table +id") and the textfield("tf +id").
     * @param indexColumnToSearch: The search will work on the column whose number is passed here.(Rem the checkbox is column number 0)
     */
	onKeyUpMultiSearch = (context, id, indexColumnToSearch) => {
		var input, filter, tr, td, i;
        input = document.getElementById("tf-"+id);
        filter = input.value.toUpperCase();
        tr = this.fetchTableRows(this.getInternalTableName());
		var shouldBeVisible;
		var filterValues = filter.split(',');
		
        // Loop through all table rows, and hide those who don't match the search query
        for (i = 0; i < tr.length; i++) {
            td = tr[i].getElementsByTagName("td")[indexColumnToSearch];
			shouldBeVisible = false;
            if (td) {
				for(var j = 0; j < filterValues.length; j++)
				{
					if (td.innerHTML.toUpperCase().indexOf(filterValues[j]) > -1) {
						shouldBeVisible = true;
					} 
				}
				
				!shouldBeVisible ? tr[i].style.display = "none" : tr[i].style.display = "" ;
            } 
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
     * This method returns the internalTableName.
     * @return string: internal table name.
     */
    getInternalTableName = () => {
        return "table-" + this.props.internalColName;
    }

	
	/**
	 * This method clears the value of the serachbox and calls onKeyUp for 
     * @param {obj} evt: the event object.
     * @param {string} strSearchBoxId: the id of the searchbox.
	 */
	clearSearchBox = (evt, strSearchBoxId) => {
		var sb = document.getElementById("tf-" + strSearchBoxId);
		if (sb) {
			sb.value = '';
			this.onKeyUp(this, strSearchBoxId, this.state.indexColumnToSearch);
		}
	}


    /**
     * Hide the main scrollbar when hovering inside the range table while the range table has a scrollbar itself
     * Only hides if the setting is enabled
     */
    mouseIn = (evt) => {
        if (this.props.settings.hideScrollHover) {
            var elements = document.getElementsByClassName(this.props.internalColName + "-E");
            if (elements[0].scrollHeight >= 300) {
                var scrollDiv = document.getElementsByClassName("sidenavbar");
                scrollDiv[0].setAttribute("class", "sidenavbar disableScroll");
                this.scroll(this.props.tableID);
            }
        }
    }


    /**
     * Show the main scrollbar when exiting from the hover
     */
    mouseOut = () => {
        if (this.props.settings.hideScrollHover) {
            var scrollDiv = document.getElementsByClassName("sidenavbar");
            scrollDiv[0].setAttribute("class", "sidenavbar enableScroll");
        }
    }


    /**
     * This is called when the header is clicked for sorting.
     * @param {Obj} evt: The actual event object.
     * @param columnName: -ADCMT
     * @return {string} columnName: id of the columnHeader.
     */
    onSortClick = (evt, columnName) => {
        var columnObj = document.getElementById(columnName);
        var columnType = this.props.tableState[this.props.id].type;
        var columnClicked = columnName.indexOf('value') > -1 ? 'value' : 'count';
        //var data = this.state.tableData.values;
        //var keys = Object.keys(data);
        var otherColumn;
        var context = this;
        var flatData = context.state.flatData.slice();

        // Find the other column & remove the sort applied icon.
        if (columnClicked === 'value') {
            otherColumn = document.getElementById(columnName.replace('value','count'));
        }

        else {
            otherColumn = document.getElementById(columnName.replace('count','value'));
        }

        if (otherColumn) {
            otherColumn.classList.remove('fa-sort-amount-desc');
            otherColumn.classList.remove('fa-sort-amount-asc');
            otherColumn.classList.add('fa-sort');
        }
        
        // Update the sorting icon
        var sortDirection = this.updateSortIcon(columnObj);

        switch (columnType) {
            case "Text":
                if (sortDirection === "asc") {
                    flatData.sort(
                        function(a, b) {
                            if (a[columnClicked] < b[columnClicked]) {
                                return -1;
                            }
                            if (a[columnClicked] > b[columnClicked]) {
                                return 1;
                            }
                            return 0;
                        }
                    );
                }

                else {
                    flatData.sort(
                        function(a, b) {
                            if (a[columnClicked] < b[columnClicked]) {
                                return -1;
                            }
                            if (a[columnClicked] > b[columnClicked]) {
                                return 1;
                            }
                            return 0;
                        }
                    ).reverse();
                } 
                break;

            case "Number":
                if (sortDirection === "asc") {
                    flatData.sort(function(a, b) { return a[columnClicked] - b[columnClicked] } );
                }

                else {
                    flatData.sort(function(a, b) { return a[columnClicked] - b[columnClicked] } ).reverse();
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


    /**
     * This creates the rows based on the sort property.
     * Initially the sortedvalues property is [] so fetches from data.
     */
    createRows = () => {
        
        var id = this.props.id;
        var data = this.state.tableData.values;
        var totalCount = this.state.tableData.totalCount;
        var selectedValues = this.props.tableState[id].selectedValues;
        var rows = [];
        var count; 
        var index = 0;
        var percentStr;
        var sortedValues = this.state.flatData;

        /**
         * structure of data is:
         * {
         *      abc/num: {
         *          value: 'abc' or num,
         *          count: num
         *          }
         * }
         */
        if (sortedValues.length > 0 && !this.refresh) {
            for (var i = 0; i < sortedValues.length; i++) {
                count = sortedValues[i].count;
                percentStr = count + " (" + ((count/totalCount) * 100).toFixed(2) + "%)";
                rows.push(this.generateRowHTML(sortedValues[i].value, sortedValues[i].value, count, percentStr, (selectedValues.indexOf(sortedValues[i].value) !== -1), index));
                index++;
            }
        }

        else {
            this.refresh=false;
            for (var property in data) {
                count = data[property].count;
                percentStr = count + " (" + ((count/totalCount)*100).toFixed(2) + "%)";
                rows.push(this.generateRowHTML(property,data[property].value, count, percentStr, (selectedValues.indexOf(data[property]) !== -1), index, true));
                index++;
            }   
        }

        return rows;
    }


    /**
     * This generates the HTML markup.
     * @param {string} property: this is the "key" of the object
     * @param {string} value: this is the value of the object.
     * @param {int} count: this is the count of that value.
     * @param {string} percentStr: this is the percentage of the count.
     * @param {bool} checked: selected or not.
     * @param {int} index: reference index for the flatdata that is updated.
     * @param {bool} initialRowCreate: true if the table is loaded the 1st time.
     */
    generateRowHTML = (property, value, count, percentStr, checked, index, initialRowCreate) =>{
        var row;
        var context = this;
        
        row = (
            <FilterRow 
                onRowSelect = { (evt,rowSelection,checked) => this.onRowSelect(this, rowSelection, null, evt, checked) } 
                key = { property } 
                index = { index } 
                checked = { checked } 
                value = { value } 
                percentStr = { percentStr }
                settings = { context.props.settings }
            />
        );

        if (initialRowCreate) {
            this.state.flatData.push({ value: value, count: count, index: index });
        }
            
        return row;
    }


    /**
     * This function will bring the scroll to that element.
     * @param {String/Object} element: this is the name of the column
     */
    scroll = (element) => {
        if (typeof element == 'string') {
            element = document.getElementById(element);
        }

        if (element) {
            ScrollIntoView(element, false, { duration: 150 });
        }
        
        else {
            return false;
        }
    }

    render() {
        var id = this.props.id;
        var internalColName = this.props.internalColName;
        var tableBodyHeight = (300 - 32) + "px";
        var rows = this.createRows();

        return (

            <div style = {{ height:"inherit" }} >
                <br/>

                <div style = {{ margin: "-2px 15px -5px 15px" }} >
                    <SearchBox 
                        ref = "pinnedCollapisbleSearchBox"
                        id = { "tf-" + internalColName }
                        hintText = "Search for value..." 
                        pinned = { false }
                        collapseButton = { false }
                        settings = {{
                            SearchBoxClearHover: this.props.settings.colors.pinFilterColor.SearchBoxClearHover, 
                            searchBoxUnderline: this.props.settings.colors.pinFilterColor.searchBoxUnderline,
                            overviewButtonsColorBg: this.props.settings.colors.overviewButtonsColor.background,
                            overviewButtonsColorText: this.props.settings.colors.overviewButtonsColor.text,
                            tableSelectColor: this.props.settings.colors.tableSelectColor.background
                        }}
                        onTextFieldValueChange = { (evt) => this.onKeyUpMultiSearch(this, internalColName, this.state.indexColumnToSearch) }
                    />
                </div>

				<br/>
				
                <div style = {{ padding: "0px 15px" }} >
                    <div onMouseEnter = { this.mouseIn } onMouseLeave = { this.mouseOut } >
                        <Table
                            className = { "table-" + internalColName }
                            fixedHeader = { true }
                            fixedFooter = { true }
                            selectable = { false }
                            wrapperStyle = {{ maxHeight: "300px", overflow: 'hidden', borderRadius: "4px" }}
                            bodyStyle = {{ maxHeight: tableBodyHeight, overflow: 'auto', width: "100%" }}
                            style = {{ backgroundColor: this.props.settings.colors.tableSelectColor.background }}
                            multiSelectable = { true }
                            onRowSelection = { (rowSelection) => this.onRowSelect(this,rowSelection) }
                        >
                            <TableHeader
                                displaySelectAll = { false }
                                adjustForCheckbox = { false }
                                enableSelectAll = { false }
                                style = {{ backgroundColor: "#b2b2b2" }}
                            >
                                <TableRow style = {{ height:'30px' }} >
                                    
                                    {/*Header Checkbox*/}
                                    <TableHeaderColumn style = {{ height:'inherit', width:'25px' }} >
                                        <Checkbox 
                                            id = { "cb-" + internalColName }
                                            iconStyle = {{ fill: this.props.settings.colors.elasticColor.checkAllBox }}
                                            checked = { this.props.tableState[id].selectedValues.length === this.state.flatData.length } 
                                            onCheck = { (evt) =>  this.onRowSelect(this, [], !this.state.selectAll) }
                                        />
                                    </TableHeaderColumn>
                                    
                                    {/*Value Column header*/}
                                    <TableHeaderColumn 
                                        style = {{ 
                                            paddingLeft: '7px', 
                                            paddingRight: '0px', 
                                            height:'inherit',
                                            userSelect: 'none',
                                            color: "#000000"
                                        }}
                                    >
                                        <div onClick = { (evt) => this.onSortClick(evt,"valueColumnHeader" + internalColName) } >
                                            Value &nbsp;
                                            <i id = { "valueColumnHeader" + internalColName } className = "fa fa-sort" /> 
                                        </div> 
                                    </TableHeaderColumn>
                                    
                                    {/*Count Column Header*/}
                                    <TableHeaderColumn style = {{ height:'inherit', userSelect: 'none', color: "#000000" }} >
                                        <div  onClick = { (evt) => this.onSortClick(evt,"countColumnHeader" + internalColName) } >
                                            Count(%) &nbsp;
                                            <i id = { "countColumnHeader" + internalColName } className = "fa fa-sort" /> 
                                        </div>
                                    </TableHeaderColumn>

                                </TableRow>

                            </TableHeader>

                            <TableBody
                                displayRowCheckbox = { false }
                                deselectOnClickaway = { false }
                                showRowHover = { true }
                                stripedRows = { false }
                                className = {internalColName + "-E"}
                            >
                                {rows}
                            </TableBody>
                        </Table>
                    </div>
                </div>
            </div>
        );
    }
}


/**
 * This class represents each row inside the filter table.
 * @param props: {
 *      onRowSelect = { function() }
 *      index = { int: row index } 
 *      checked = { boolean value } 
 *      value = { the value to be displayed in the 1st column. } 
 *      percentStr = { the value to be displayed in the 2nd column. percent }
 *      settings = { global theme settings object }
 * }
 */
class FilterRow extends React.Component {


    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        return (this.props.checked !== nextProps.checked || this.props.settings != nextProps.settings);
    }


    /**
     * React built-in which tells react if it should re-render the component
     * @param evt: -ADCMT
     **/
    onClickRow = (evt) => {
        this.props.onRowSelect(evt, this.props.rowNumber, !this.props.checked);
    }


    render() {
        return(
            <TableRow key = { this.props.index } style = {{ height:'30px' }} selected = { this.props.checked } onClick = { this.onClickRow } >
                <TableRowColumn style = {{ height:'inherit', width:'25px' }}> {/*REMOVED FOR PERFORMANCE <Checkbox checked = { this.props.checked } iconStyle = {{ fill: this.props.settings.colors.elasticColor.checkBox }}/> */} </TableRowColumn>
                <TableRowColumn style = {{ paddingLeft:'7px', paddingRight: '0px', height:'inherit' }}>{this.props.value}</TableRowColumn>
                <TableRowColumn style = {{ height:'inherit' }}>{this.props.percentStr}</TableRowColumn>
            </TableRow>
        );
    };
}


/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const addRemoveElastic = (filter) => ({
  type: 'ADD_REMOVE_ELASTIC',
  filter
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    tableState: state.filterState.Filter,
    settings: state.filterState.Settings
  }
};


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(FilterTable);

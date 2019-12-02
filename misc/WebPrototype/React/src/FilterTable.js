import React from 'react';
import { connect } from 'react-redux';
import { Column, Table } from 'react-virtualized';
//import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
import { TableRow, TableRowColumn } from 'material-ui/Table';
/* import { guidGenerator } from './GeneralFunctions.js'; */
import { makeServerCall } from './ServerCallHelper.js';
import Checkbox from 'material-ui/Checkbox';
import Promise from 'bluebird';
import ScrollIntoView from 'scroll-into-view-if-needed';
/* import SearchBox from './SearchBox.js'; */
import Flexbox from 'flexbox-react';
import FontIcon from 'material-ui/FontIcon';
import TextField from 'material-ui/TextField';
import RaisedButton from 'material-ui/RaisedButton';
import './css/General.css';
import 'react-virtualized/styles.css';



/**
 * Default row renderer for Table.
 */
function CustomRowRendererComponent({
    className,
    columns,
    index,
    key,
    onRowClick,
    onRowDoubleClick,
    onRowMouseOut,
    onRowMouseOver,
    onRowRightClick,
    rowData,
    style,
    selectedData,
    searchTerm
}) {
    const a11yProps = {};

    if ( onRowClick || onRowDoubleClick || onRowMouseOut || onRowMouseOver || onRowRightClick ) {
        a11yProps['aria-label'] = 'row';
        a11yProps.tabIndex = 0;

        if (onRowClick) {
            a11yProps.onClick = event => onRowClick({event, index, rowData});
        }
        if (onRowDoubleClick) {
            a11yProps.onDoubleClick = event =>
            onRowDoubleClick({event, index, rowData});
        }
        if (onRowMouseOut) {
            a11yProps.onMouseOut = event => onRowMouseOut({event, index, rowData});
        }
        if (onRowMouseOver) {
            a11yProps.onMouseOver = event => onRowMouseOver({event, index, rowData});
        }
        if (onRowRightClick) {
            a11yProps.onContextMenu = event =>
            onRowRightClick({event, index, rowData});
        }
    }

    if (selectedData.indexOf(rowData.value) !== -1) {
        style.backgroundColor = "#e0e0e0";
    }

    return (
        <div
            {...a11yProps}
            className = { className }
            key = { key }
            role = "row"
            style = { style }
            >
            {columns}
        </div>
    );
}



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

    refresh = true;

    constructor(props) {
        super(props);

        this.state = {
            selectAll: false,
            checkboxClicked:false,
            tableData: props.tableData,
            flatData: [],
            indexColumnToSearch: (props.columnToSearch ? props.columnToSearch : 1),
            searchTerm: "",
            height: document.documentElement.clientHeight * .361197
        };
    }

    componentWillReceiveProps(nextProps) {
        if (this.props.tableData !== nextProps.tableData) {
            this.setState({ tableData: nextProps.tableData, flatData: [] });
            this.refresh = true;
        }
    }

    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {

        return (this.props.tableState[this.props.id].selectedValues !== nextProps.tableState[this.props.id].selectedValues || 
                this.props.settings !== nextProps.settings ||
                this.state.flatData !== nextState.flatData ||
                this.state.searchTerm !== nextState.searchTerm ||
                this.props.tableData !== nextProps.tableData ||
                this.props.ShowAllTables !== nextProps.ShowAllTables ||
                this.state.height !== nextState.height
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

        // var undoRedoHistory = {
        //     history: [{filterList: this.props.tableState, tableData: this.props.fullTableData}],
        //     position: 0
        // }

        //this.props.dispatch(editUndoRedoHistory(undoRedoHistory));
        
        // Force update on mount for pinned columns to show the same selections before change
        //this.forceUpdate();
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
     * - ADCMT
     */
    applyFilter = () => {
        //console.log('Filter Applied');
        var context = this;
        var iframe;
        if(!context.props.uid){
            iframe = document.getElementById('GlyphViewer').contentWindow;
            console.log(iframe);
        }

        makeServerCall('applyFilters',
            function(result, b) {
                var resultJson = JSON.parse(result);
                console.log(resultJson);
                // debugger;
                var data = resultJson.data;
                var tempRowIds = [];

                if(context.props.uid){
                    context.props.webSocket.send(JSON.stringify({
                        url_uid: context.props.uid,
                        filters: data
                    }));
                }
                if(!context.props.uid){
                    if (data && Array.isArray(data)) {
                        if (data.length > 0) {							
                            for (var index = 0; index < data.length; index++) {
                                tempRowIds.push(parseInt(Object.values(data[index]).toString(), 10));
                            }
                        }
                        else {
                            // No data was matched.
                            console.log('NO MATCH');
                        }
                    }
                    console.log(tempRowIds);
                    context.props.setFilterIDs(tempRowIds);
                    iframe.filterGlyphs(tempRowIds);
                }

                // debugger;
                
                context.props.setFilterBusy(false);


                context.props.dispatch( setTimer(new Date().getTime()) );
            },
            {post: true, 
                data: { tableName: this.props.VizParams.tableName, filterObj: this.props.tableState } 
            }
        );

        // makeServerCall('applyFiltersEC2',
        //     function(responseText) {
        //         console.log(JSON.parse(responseText), 'response from apply filts on filt table');


        //     },
        //     {
        //         post: true,
        //         data: { tableName: this.props.VizParams.tableName, filterObj: this.props.tableState}
        //     });
    };


    /**
     * This method is called on selection of the row in the table. 
     * @param context: This is the actual instance of the FilterTable. Mainly used to access the state and store.
     * @param rowSelection: This is an array that has index(according to the table rows) of all the rows selected.
     * @param all: boolean value stating whether or not select all is clicked.
     * @param evt: -ADCMT
     * @param checked: -ADCM
     */
    onRowSelect = (e) => {
        var selectedValues = this.props.tableState[this.props.id].selectedValues.slice();

        

        var checked = false;

        for (var i = 0; i < selectedValues.length; i++) {
            if (selectedValues[i] === e.rowData.value) {
                checked = true;
            }
        }


        // debugger;
        

        if (!checked) {
            selectedValues.push(e.rowData.value);
            if (selectedValues.length === this.state.flatData.length) {
                this.setState({ selectAll: true });
            }
        }

        else {
            if (this.state.selectAll === true) {
                this.setState({ selectAll: false });
            }
            selectedValues.splice(selectedValues.indexOf(e.rowData.value), 1);
        }
        
        

        var filterStructure = {
            colName : this.props.id,
            selectedValues: selectedValues,
            //value: (this.state.flatData[rowSelection] ? this.state.flatData[rowSelection].value : null),
            value: e.rowData.value,
            checked: checked,
            type: this.props.tableState[this.props.id].type,
            data: this.props.tableData.flatValues
        }

        this.props.setFilterBusy(true);
        
        var context = this;

        let pom = new Promise(function (resolve, reject) {
                context.props.dispatch(addRemoveElastic(filterStructure));
                resolve('done');
        });

        pom.then(() => context.props.refreshTableDataOnRowSelection()).then(() => context.addToHistory()).then(() => context.applyFilter());
        

    };


    addToHistory() {
        var undoRedoHistory = {
            history: this.props.UndoRedoHistory.history.slice(0),
            position: this.props.UndoRedoHistory.position
        }

        // debugger;

        if (undoRedoHistory.position !== undoRedoHistory.history.length - 1) {
            undoRedoHistory.history = undoRedoHistory.history.slice(0, undoRedoHistory.position + 1);
        }

        undoRedoHistory.history.push({filterList: this.props.tableState, tableData: this.props.fullTableData});
        undoRedoHistory.position = undoRedoHistory.position + 1;

        this.props.dispatch(editUndoRedoHistory(undoRedoHistory));
    }
    

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
     * This creates data for the table.
     */
    createRows = () => {
        var rows = [];

        var sTerm = this.state.searchTerm.trim();

        for (var i in this.state.tableData.values) {

            if (sTerm !== "") {
                if (this.state.tableData.values[i].value.toString().toLowerCase().indexOf(sTerm.toLowerCase()) !== -1) {
                    rows.push({ 
                        value: this.state.tableData.values[i].value, 
                        count: this.state.tableData.values[i].count + " (" + ((this.state.tableData.values[i].count/this.state.tableData.totalCount) * 100).toFixed(2) + "%)" });
                }
            }
            else {
                rows.push({ 
                    value: this.state.tableData.values[i].value, 
                    count: this.state.tableData.values[i].count + " (" + ((this.state.tableData.values[i].count/this.state.tableData.totalCount) * 100).toFixed(2) + "%)" });
            }
        }

        return rows;
    }



    /**
     * This function will bring the scroll to that element.
     * @param {String/Object} element: this is the name of the column
     */
    scroll = (element) => {
        if (typeof element === 'string') {
            element = document.getElementById(element);
        }

        if (element) {
            ScrollIntoView(element, false, { duration: 150 });
        }
        
        else {
            return false;
        }
    }


    onSearchChange(e) {
        this.setState({ searchTerm: e.target.value });
    }


    getRowRenderer = (props) => {
        return <CustomRowRendererComponent searchTerm = { this.state.searchTerm } selectedData = { this.props.tableState[this.props.id].selectedValues } {...props } />;
    }

    updateCheck(col) {
        var list = this.props.ShowAllTables.slice(0);

        var index = list.indexOf(col);

        if (index === -1) {
            list.push(col);
            var tableData = this.props.fullTableData;
            tableData[col] = this.props.UndoRedoHistory.history[0].tableData[col];
            this.props.setTableData(tableData);
        }
        else {
            list.splice(index, 1);
            this.props.refreshTableDataOnRowSelection();
        }

        this.props.dispatch(setShowAllTables(list));
    }


    render() {
/*         var id = this.props.id;
        var internalColName = this.props.internalColName;
        var tableBodyHeight = (300 - 32) + "px"; */
        //creating the rows
        var rows = this.createRows();
        // console.log(rows);
        console.log(document.documentElement.clientHeight, document.documentElement.clientHeight * .361197)
        return (

            <div style = {{ height: "inherit" }} >
                <br/>

                <div style = {{ margin: "-1.157vh 1.577vh -1.262vh" }} >

                    <Flexbox flexDirection = "row" style = {{ width: "100%" }} >
                        <Flexbox style = {{ width: "100%", borderRadius: "5px", backgroundColor: this.props.settings.colors.tableSelectColor.background }} > 

                            <TextField
                                type = "search"
                                ref = "SearchTextField"
                                value = { this.state.searchTerm }
                                style = {{
                                    borderColor: "#d9d9d9 #ccc #b3b3b3",
                                    borderRadius: "4px",
                                    border: "1px solid #ccc",
                                    width: "100%",
                                    height: "3.155vh",
                                    lineHeight: "2.524vh",
                                    fontSize: "1.682vh"
                                }}
                                inputStyle = {{
                                    paddingLeft:"0.526vh",
                                    paddingRight:"0.526vh"
                                }}
                                hintStyle = {{
                                    paddingLeft:"0.736vh",
                                    bottom: "-0.105vh"
                                }}
                                underlineStyle = {{
                                    margin: "0px 0px -0.841vh"
                                }}
                                onChange = { (e) => this.onSearchChange(e) } 
                                hintText = {
                                    <span style = {{ fontSize: 'inherit', color: 'rgba(0, 0, 0, 0.6)' }} >
                                        <FontIcon
                                            className = "fa fa-search" 
                                            style = {{
                                                padding: '0px', 
                                                width: '2.524vh',
                                                height: '2.524vh',
                                                fontSize: 'inherit',
                                                color: 'inherit'
                                            }}
                                        />
                                        Search for value...
                                    </span>
                                }
                                underlineFocusStyle = {{ borderColor: this.props.settings.colors.pinFilterColor.searchBoxUnderline, margin: "0px 0px -8px 0px" }}
                            /> 
                        </Flexbox>

                        <Flexbox style = {{ width: "35%" }} >
                            <div 
                                style = {{ 
                                    backgroundColor: this.props.settings.colors.tableSelectColor.background, 
                                    margin: "0px 0px 0px 1.262vh", 
                                    //borderColor: "#d9d9d9 #ccc #b3b3b3",
                                    borderRadius: "4px",
                                    //border: "1px solid #ccc", 
                                }} 
                            >
                                <Checkbox
                                    label = "Show All"
                                    checked = { this.props.ShowAllTables.indexOf(this.props.internalColName.replace("_pinned", "")) !== -1 ? true : false }
                                    onCheck = { () => this.updateCheck(this.props.internalColName.replace("_pinned", "")) }
                                    iconStyle = {{ 
                                        width: "2.524vh",
                                        height: "2.524vh",
                                        fill: 'rgba(0, 0, 0, 0.6)', 
                                        margin: "0.210vh 0.210vh 0px 0px" }}
                                    labelStyle = {{ 
                                        width: "100%", 
                                        lineHeight: "2.524vh",
                                        fontSize: "1.342vh",
                                        margin: "0.516vh 0.516vh 0px 0px", 
                                        color: 'rgba(0, 0, 0, 0.6)' }}
                                />
                            </div>
                           
                        </Flexbox>

                    </Flexbox>
                </div>

				<br/>
				
                <div style = {{ padding: "0px 1.577vh" }} >
                    <div 
                        style = {{ 
                            height: "38.486vh",
                            width: "19vw",
                            overflow: "hidden" ,
                            height: (document.documentElement.clientHeight * .02477 * (rows.length + 1) > this.state.height ? 
                                this.state.height + (document.documentElement.clientHeight * .016512) : document.documentElement.clientHeight * .02477 * (rows.length + 1))
                        }} 
                        onMouseEnter = { this.mouseIn } 
                        onMouseLeave = { this.mouseOut } >
                        

                        <Table
                            width = { document.documentElement.clientHeight * .37668 }
                            height = { document.documentElement.clientHeight * .02477 * (rows.length + 1) > this.state.height ? 
                                    this.state.height : document.documentElement.clientHeight * .02477 * (rows.length + 1) }
                            headerHeight = { document.documentElement.clientHeight * .02477 }
                            rowHeight = { document.documentElement.clientHeight * .02477 }
                            gridStyle = {{ 
                                backgroundColor: "white", 
                                color: "black", 
                                marginBottom: 24 * (Object.keys(this.state.tableData.values).length + 1) > this.state.height ? 
                                    "0" : "-1.051vh" }}
                            headerStyle = {{ color: "black" }}
                            rowStyle = {{ borderBottom: "solid 1px #d3d3d3" }}
                            rowCount = { rows.length }
                            rowGetter = { ({ index }) => rows[index] }
                            rowRenderer = { this.getRowRenderer }
                            onRowClick = { (e) => this.onRowSelect(e) }
                        >
                            <Column
                                label = 'Value'
                                dataKey ='value'
                                width = { document.documentElement.clientHeight * .257997 }
                            />
                            <Column
                                width = { document.documentElement.clientHeight * .103199 }
                                label = 'Count'
                                dataKey = 'count'
                            />
                        </Table>

                        <RaisedButton 
                            primary = { true } 
                            onClick = { () => this.setState({ height: this.state.height === (document.documentElement.clientHeight * .361197) ? 
                                    (document.documentElement.clientHeight * .619195) : (document.documentElement.clientHeight * .361197) }) }
                            buttonStyle = {{ backgroundColor: "#b6b6b5", width: "19vw" }}
                            style = {{ height: '1.682vh', display: (24 * (rows.length + 1) > this.state.height ? "" : "none") }}
                        >
                            <i 
                                className = { this.state.height === (document.documentElement.clientHeight * .361197) ? "fa fa-caret-down" : "fa fa-caret-up" }
                                style = {{
                                    fontSize: '2.208vh',
                                    margin: "-0.210vh 0px 0px 0px",
                                    color: "#000",
                                }}
                            /> 
                        </RaisedButton>

                        {/*
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
                                    
                                    <TableHeaderColumn style = {{ height:'inherit', width:'25px' }} >
                                        <Checkbox 
                                            id = { "cb-" + internalColName }
                                            iconStyle = {{ fill: this.props.settings.colors.elasticColor.checkAllBox }}
                                            checked = { this.props.tableState[id].selectedValues.length === this.state.flatData.length } 
                                            onCheck = { (evt) =>  this.onRowSelect(this, [], !this.state.selectAll) }
                                        />
                                    </TableHeaderColumn>
                                    
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

                        */}



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
        return (this.props.checked !== nextProps.checked || 
            this.props.settings !== nextProps.settings || 
            this.props.value !== nextProps.value || 
            this.props.percentStr !== nextProps.percentStr);
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
            <TableRow key = { this.props.index } style = {{ height:'24px' }} selected = { this.props.checked } onClick = { this.onClickRow } >
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

export const editUndoRedoHistory = (undoRedoHistory) => ({
  type: 'UPDATE_HISTORY',
  undoRedoHistory
});

export const setTimer = (timeoutTimer) => ({
    type: 'SET_TIMEOUT_TIMER',
    timeoutTimer,
});

export const setShowAllTables = (showAllTables) => ({
    type: 'SET_SHOW_ALL_TABLES',
    showAllTables,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    tableState: state.filterState.Filter,
    settings: state.filterState.Settings,
    VizParams: state.filterState.VizParams,
    UndoRedoHistory: state.filterState.UndoRedoHistory,
    ShowAllTables: state.filterState.ShowAllTables,
    webSocket: state.filterState.webSocket,
    uid: state.filterState.uid
  }
};


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(FilterTable);

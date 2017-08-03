import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import {Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn} from 'material-ui/Table';
import Checkbox from 'material-ui/Checkbox';
import SearchBox from './SearchBox.js';
import IconButton from 'material-ui/IconButton';
import { connect } from 'react-redux';
import './General.css'


/**
 * This is the class that represents the Tables.
 * @param id: id you want to give to the table.
 * @param columnToSearch: the index of the column you want the search funcationality to work on.
 * @param internalColName: name of the column used for internal mapping purposes
 * @param displayName: the name of the column as showed on the UI.
 * @param tableData: [array of strings]  eg: tableData: ['a','b','c','a']
 */

class FilterTable extends Component {

    flatData = [];
    
    constructor(props) {
        super(props);
        
        var processedData =  this.processData(props.tableData ? props.tableData : []);
        var tableData = processedData;

        this.state = {
        fixedHeader: true,
        fixedFooter: true,
        stripedRows: false,
        showRowHover: true,
        selectable: true,
        multiSelectable: true,
        enableSelectAll: false,
        deselectOnClickaway: false,
        showCheckboxes: false,
        selectAll: false,
        prevSelectedIndex: null,
        checkboxClicked:false,
        tableData: tableData,
        indexColumnToSearch: (props.columnToSearch ? props.columnToSearch : 1),
        };
    }

    /**
     * This processes the data and counts the number of ocurrences and removes duplicates.
     * @param tableData 
     * @return Object : {'data':<object of data in form of "dataValue : count",'totalCount': totalCount}
     */
    processData(tableData){
        var temp = {};
        var totalCount = tableData.length;
        tableData.map((row) => {
            if(temp.hasOwnProperty(row)) {
                ++temp[row]['count'];
            }
            else {
                temp[row] = {value:row, count:1};
            }
        });
        return {'data':temp,'totalCount':totalCount};
    };
    

    static COUNT = 0;

    /**
     * This method is called on selection of the row in the table. 
     * @param context: This is the actual instance of the FilterTable. Mainly used to access the state and store.
     * @param rowSelection: This is an array that has index(according to the table rows) of all the rows selected.
     * @param all: boolean value stating whether or not select all is clicked.
     */
    onRowSelect = (context,rowSelection,all,evt,checked) => {
            var index,len = context.flatData.length;
            var selectedValues;

            if (this.state.prevSelectedIndex == null) {
                this.setState({prevSelectedIndex : rowSelection});
            }

            if (evt && evt.shiftKey) {
                //Implementation for shift key selection!
            }

            if (all)
            {
                this.setState({selectAll:true});
                selectedValues = [];

                for (index = 0; index < len; index++) {
                        selectedValues.push(context.flatData[index].value);
                }
                checked = true;
            }
            else if (false === all) {
                this.setState({selectAll:false});
                selectedValues = [];
                checked = false;
            }
            else {
                selectedValues = context.props.tableState[context.props.id].selectedValues.slice();

                if (checked) {
                    selectedValues.push(context.flatData[rowSelection].value);
                    
                    if (selectedValues.length === len) {
                        this.setState({selectAll:true});
                    }
                }
                else {
                    if (this.state.selectAll === true) {
                        this.setState({selectAll:false});
                    }
                    
                        selectedValues.splice(selectedValues.indexOf(context.flatData[rowSelection].value),1);
                }
            }

            var filterStructure = {
                    colName : context.props.id,
                    selectedValues: selectedValues,
                    value: (context.flatData[rowSelection] ? context.flatData[rowSelection].value : null),
                    checked: checked,
                    type: context.props.tableState[context.props.id].type,
                    data: context.props.tableData
            }

            context.props.dispatch(addRemoveElastic(filterStructure));
        
    };

    /**
     * This method is called when the keyup event is fired in the search textfield on top of the table. 
     * @param context: the instance of this react element.
     * @param id: This is the id used to identify the table("table +id") and the textfield("tf +id").
     * @param indexColumnToSearch: The search will work on the column whose number is passed here.(Rem the checkbox is column number 0)
     */
    onKeyUp = (context,id,indexColumnToSearch) => {
        var input, filter, tr, td, i;
        input = document.getElementById("tf-"+id);
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
	onKeyUpMultiSearch = (context,id,indexColumnToSearch) => {
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
				for(var j=0; j < filterValues.length; j++)
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
        var table = document.getElementsByClassName(id)[1]; // The table header and table data are 2 separate tables. So we fetch the 2nd index as we want the data table.
        var tr = table.getElementsByTagName("tr");
        return tr;
    };

    /**
     * This method returns the internalTableName.
     * @return string - internal table name.
     */
    getInternalTableName = () => {
        return "table-" + this.props.internalColName;
    }
	
	/**
	 * This method clears the value of the serachbox and calls onKeyUp for 
	 */
	clearSearchBox = (evt,strSearchBoxId) => {
		var sb = document.getElementById("tf-" + strSearchBoxId);
		if(sb){
			sb.value = '';
			this.onKeyUp(this, strSearchBoxId, this.state.indexColumnToSearch);
		}
	}
	
    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.tableState[this.props.id].selectedValues != nextProps.tableState[this.props.id].selectedValues) {
            return true;
        }
        if (this.props.settings != nextProps.settings) {
            return true;
        }
        else {
            return false;
        }
    }

    componentDidUpdate() {
        if(this.props.tableState[this.props.id].selectedValues.length == this.flatData.length)
            this.setState({ selectAll: true });
        else
            this.setState({ selectAll: false });
    }

    componentDidMount() {
        this.componentDidUpdate();
    }

    mouseIn = () => {
        var elements = document.getElementsByClassName(this.props.internalColName + "-E");
        if (elements[0].scrollHeight > 300) {
            var scrollDiv = document.getElementsByClassName("sidenavbar");
            scrollDiv[0].setAttribute("class", "sidenavbar disableScroll");
        }
        
    }

    mouseOut = () => {
        var elements = document.getElementsByClassName(this.props.internalColName + "-E");
        if (elements[0].scrollHeight > 300) {
            var scrollDiv = document.getElementsByClassName("sidenavbar");
            scrollDiv[0].setAttribute("class", "sidenavbar enableScroll");
        }
    }


    render() {
        var id = this.props.id;
        var internalColName = this.props.internalColName;
        var data = this.state.tableData.data;
        var totalCount = this.state.tableData.totalCount;
        var selectedValues = this.props.tableState[id].selectedValues;
        var rows = []; var index = 0;
        this.flatData = [];

        /**
         * structure of data is:
         * {
         *      abc/num: {
         *          value: 'abc' or num,
         *          count: num
         *          }
         * }
         */
        for(var property in data) {

            var prop = data[property].value;
            var percentStr =  data[property].count + " (" + ((data[property].count/totalCount)*100).toFixed(2) + "%" + ")";
            var context = this;

            rows.push(<FilterRow 
                            onRowSelect = { (evt,rowSelection,checked) => this.onRowSelect(this,rowSelection,null,evt,checked) } 
                            key = { prop } 
                            index = { index } 
                            checked = { selectedValues.indexOf(prop) !== -1 } 
                            value = { prop } 
                            percentStr = { percentStr }
                            settings = { context.props.settings }
                        /> );

            this.flatData.push({
                    value: prop,
                    count: data[property].count,
                    index: index
            });
        

            index ++;
        }
		
        return (

            <div>
                <br/>
                <div
                    style={{
                        marginLeft: '5%'
                    }}
                >
                    <SearchBox 
                        ref="pinnedCollapisbleSearchBox"
                        hintText= "Search for value.." 
                        settings={{
                            SearchBoxClearHover: this.props.settings.pinFilterColor.SearchBoxClearHover, 
                            searchBoxUnderline: this.props.settings.pinFilterColor.searchBoxUnderline,
                            overviewButtonsColorBg: this.props.settings.overviewButtonsColor.background,
                            overviewButtonsColorText: this.props.settings.overviewButtonsColor.text
                        }}
                        onTextFieldValueChange= {(evt) => this.onKeyUpMultiSearch(this,internalColName,this.state.indexColumnToSearch)}
                        id={ "tf-" + internalColName }
                        pinned={false}
                        collapseButton={false}
                    />
            </div>

				<br/>
				
                <Table
                    className = { "table-" + internalColName }
                    fixedHeader = { true }
                    fixedFooter = { true }
                    selectable = { this.state.selectable }
                    wrapperStyle={{
                        maxHeight: '300px',
                        //overflow: 'auto'
                    }}
                    multiSelectable = { this.state.multiSelectable }
                    onRowSelection = { (rowSelection) => this.onRowSelect(this,rowSelection) }
                >
                <TableHeader
                    displaySelectAll = { false } // just need to implement our own select all.
                    adjustForCheckbox = { this.state.showCheckboxes }
                    enableSelectAll = { this.state.enableSelectAll }
                >
                    <TableRow style = {{ height:'30px' }}>
                        <TableHeaderColumn style = {{ height:'inherit', width:'25px' }}>
                            <Checkbox 
                                id = { "cb-" + internalColName } 
                                checked = { this.props.tableState[id].selectedValues.length == this.flatData.length } 
                                onCheck = { (evt) =>  this.onRowSelect(this, [], !this.state.selectAll) }
                                iconStyle = {{ fill: this.props.settings.elasticColor.checkAllBox }}
                            />
                        </TableHeaderColumn>
                        <TableHeaderColumn style = {{ paddingLeft:'0px', paddingRight: '0px', height:'inherit' }} >Value</TableHeaderColumn>
                        <TableHeaderColumn style = {{ height:'inherit' }}>Count(Percent)</TableHeaderColumn>
                    </TableRow>

                </TableHeader>

                <TableBody
                    displayRowCheckbox = { this.state.showCheckboxes }
                    deselectOnClickaway = { this.state.deselectOnClickaway }
                    showRowHover = { this.state.showRowHover }
                    stripedRows = { false }
                    style={{
                        overflow: 'auto'
                    }}
                    className = {internalColName + "-E"}
                    onMouseEnter = { this.mouseIn }
                    onMouseLeave = { this.mouseOut }
                >
                    {rows}
                </TableBody>
                </Table>
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
class FilterRow extends Component {

    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.checked != nextProps.checked || this.props.settings != nextProps.settings) {
            return true;
        }
        else if (this.props.value != nextProps.value) {
            return true; 
        }
        else {
            return false;
        }
    }

    onClickRow = (evt,) => {
        this.props.onRowSelect(evt, this.props.rowNumber, !this.props.checked);
    }

    render(){
        return(
            <TableRow 
                key = { this.props.index }
                style = {{ height:'30px' }}
                //className = {this.props.highlighted ? 'highlightedRows' : '' }
                selected = { this.props.checked }
                onClick = { this.onClickRow }
            >
            <TableRowColumn style = {{ height:'inherit', width:'25px' }}><Checkbox checked = { this.props.checked } iconStyle = {{ fill: this.props.settings.elasticColor.checkBox }}/></TableRowColumn>
            <TableRowColumn style = {{ paddingLeft:'0px', paddingRight: '0px', height:'inherit' }}>{this.props.value}</TableRowColumn>
            <TableRowColumn style = {{ height:'inherit' }}>{this.props.percentStr}</TableRowColumn>
            </TableRow>
        );
    };
}


export const addRemoveElastic = (filter) => ({
  type: 'ADD_REMOVE_ELASTIC',
  filter
});


const mapStateToProps = function(state){
  return {
    tableState: state.filterState.Filter,
    settings: state.filterState.Settings
  }
};

export default connect(mapStateToProps)(FilterTable);

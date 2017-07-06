import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import {Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn} from 'material-ui/Table';
import Checkbox from 'material-ui/Checkbox';
import { connect } from 'react-redux';


/**
 * This is the class that represents the Tables.
 * @param id: id you want to give to the table.
 * @param columnToSearch: the index of the column you want the search funcationality to work on.
 * @param internalColName: name of the column used for internal mapping purposes
 * @param displayName: the name of the column as showed on the UI.
 * @param  tableData: {data:[array of strings],totalCount: integer}
 *          eg:  tableData: {data:['a','b','c','a'],totalCount: 4}
 */

class FilterTable extends Component {

    flatData = [];
    
    constructor(props) {
        super(props);
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
        checkboxClicked:false,
        tableData: this.processData(props.tableData ? props.tableData : []),
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
        if(temp.hasOwnProperty(row))
            ++temp[row];
        else
            temp[row] = 1;
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
    onRowSelect = (context,rowSelection,all) => {
            var index,len = all ? context.flatData.length : rowSelection.length;
            var selectedValues = [];
            var highlightedValues = [];

            if(all || rowSelection.length == context.flatData.length)
            {
                this.setState({selectAll:true});
                for(index=0;index<len;index++)
                    selectedValues.push(context.flatData[index].value);
            }
            else{
                this.setState({selectAll:false});
                for(index=0;index<len;index++)
                    selectedValues.push(context.flatData[rowSelection[index]].value);
            }
            

            var filterStructure = {
                    colName : context.props.id,
                    selectedValues: selectedValues,
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
        var input, filter, table, tr, td, i;
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
            } else {
                tr[i].style.display = "none";
            }
            } 
        }
    };

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

    shouldComponentUpdate(nextProps, nextState) {
        if(this.props.tableState.Elastic[this.props.id].selectedValues != nextProps.tableState.Elastic[this.props.id].selectedValues)
            return true;
        else if(this.props.tableState.Ranges[this.props.id].highlightedValues != nextProps.tableState.Ranges[this.props.id].highlightedValues)
            return true;
        else
            return false;
    }


    render() {
        var id = this.props.id;
        var internalColName = this.props.internalColName;
        var data = this.state.tableData.data;
        var totalCount = this.state.tableData.totalCount;
        var rows = []; var index=0;
        this.flatData = [];

        for(var property in data) {
            rows.push(<TableRow striped={true}
                key={index}
                style={{height:'30px'}}
                selected={this.props.tableState.Elastic[id].selectedValues.indexOf(property) !== -1}
                //striped={this.props.tableState.Ranges[id].highlightedValues.indexOf(property) !== -1}
                
                >
                    <TableRowColumn style={{height:'inherit', width:'25px'}}><Checkbox checked={this.props.tableState.Elastic[id].selectedValues.indexOf(property) !== -1} /></TableRowColumn>
                    <TableRowColumn style={{paddingLeft:'0px',paddingRight: '0px',height:'inherit'}}>{property}</TableRowColumn>
                    <TableRowColumn style={{height:'inherit'}}>{data[property] + " (" + ((data[property]/totalCount)*100).toFixed(2) + "%" + ")"}</TableRowColumn>
            </TableRow>);

            this.flatData.push({
                value: property,
                count: data[property],
                index: index
            });

            index ++;
        };

        return (
            <div>   
                <TextField
                    type="text" 
                    id={"tf-"+internalColName}
                    className={"tf-"+internalColName} 
                    onKeyUp={() => this.onKeyUp(this,internalColName,this.state.indexColumnToSearch)} 
                    hintText="Search for value.." /> 
                <br/>
                <Table
                    className={"table-"+internalColName}
                    height='300px'
                    fixedHeader={this.state.fixedHeader}
                    fixedFooter={this.state.fixedFooter}
                    selectable={this.state.selectable}
                    multiSelectable={this.state.multiSelectable}
                    onRowSelection={(rowSelection) => this.onRowSelect(this,rowSelection)}
                >
                <TableHeader
                    displaySelectAll={false} // just need to implement our own select all.
                    adjustForCheckbox={this.state.showCheckboxes}
                    enableSelectAll={this.state.enableSelectAll}
                >
                    <TableRow  style={{height:'30px'}}>
                        <TableHeaderColumn style={{height:'inherit', width:'25px'}}><Checkbox checked={this.props.tableState.Elastic[id].selectedValues.length == this.flatData.length} onCheck={(evt) =>  this.onRowSelect(this,[],!this.state.selectAll)}/></TableHeaderColumn>
                        <TableHeaderColumn style={{paddingLeft:'0px',paddingRight: '0px',height:'inherit'}} >Value</TableHeaderColumn>
                        <TableHeaderColumn style={{height:'inherit'}}>Count(Percent)</TableHeaderColumn>
                    </TableRow>

                </TableHeader>

                <TableBody
                    displayRowCheckbox={this.state.showCheckboxes}
                    deselectOnClickaway={this.state.deselectOnClickaway}
                    showRowHover={this.state.showRowHover}
                    stripedRows={this.state.stripedRows}
                >
                    {rows}
                </TableBody>
                </Table>
            </div>
        );
    }
}



export const addRemoveElastic = (filter) => ({
  type: 'ADD_REMOVE_ELASTIC',
  filter
});


const mapStateToProps = function(state){
  return {
    tableState: state.filterState.Filter,
  }
};

export default connect(mapStateToProps)(FilterTable);

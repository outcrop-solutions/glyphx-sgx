import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import {Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn} from 'material-ui/Table';
import {Tabs, Tab} from 'material-ui/Tabs';
import SwipeableViews from 'react-swipeable-views';
import FontIcon from 'material-ui/FontIcon';
import Global from './Global.js';
import RangeForm from './range.js';

class FilterTabs extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
            slideIndex: 0,
            FilterTable: null,
            FilterTableSelectedRows: [],
            Range: null,
            tableData: props.data
        };
    };

    handleChange = (value,context) => {
        context.setState({
            slideIndex: value,
            FilterTableSelectedRows: context.state.FilterTable.state.selectedRows
        });

    };

    /**
     * This method is called after the table is created inside the tab. We store the instance of the filter table for future use.
     * @param instance: this is the instance of the filter table created
     */
    onCreateTable = (instance) => {
        this.setState({
            FilterTable: instance
        });
    }

    render() {
        return (
            <div>
                <Global ref={(inst) => function(inst){this.setState({GLOBAL: inst.getGlobalData()})}} /> 
                    
                <Tabs
                    onChange={(value) => this.handleChange(value,this)}
                    value={this.state.slideIndex}
                >
                    <Tab 
                        label="Elastic" 
                        value={0}
                        icon={<FontIcon className="fa fa-list-ul"></FontIcon>}
                    />
                    <Tab 
                        label="Range" 
                        value={1}
                        icon={<FontIcon className="fa fa-sliders"></FontIcon>}
                    />
                </Tabs>

                

                <SwipeableViews
                    index={this.state.slideIndex}
                    onChangeIndex={this.handleChange}

                    style={{
                        overflowY: "hidden",
                        height: "440px",
                    }}
                            
                >
                    <div
                        style={{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <FilterTable tableData={this.state.tableData} id={this.props.colName} selectedRows={this.state.FilterTableSelectedRows} ref={this.onCreateTable}></FilterTable>
                    </div>
                    <div
                        style={{
                            maxHeight: "440px",
                            overflowX: "hidden"
                        }}
                    >
                        <RangeForm colName={this.props.colName} minVal={-100} maxVal={100}></RangeForm>
                    </div>
                </SwipeableViews>
            </div>
        );
    }
}

/**
 * This is the class that represents the Tables.
 * @param id: id you want to give to the table.
 * @param columnToSearch: the index of the column you want the search funcationality to work on.
 * @param selectedRows: array of row positions selected [0,5,...]
 * @param  tableData: {data:[array of strings],totalCount: integer}
 *          eg:  tableData: {data:['a','b','c','a'],totalCount: 4}
 */

class FilterTable extends Component {
    
    constructor(props) {
        super(props);
        
        this.state = {
        id: props.id,
        fixedHeader: true,
        fixedFooter: true,
        stripedRows: false,
        showRowHover: true,
        selectable: true,
        multiSelectable: true,
        enableSelectAll: false,
        deselectOnClickaway: false,
        showCheckboxes: true,
        tableData: this.processData(props.tableData ? props.tableData : []),
        indexColumnToSearch: (props.columnToSearch ? props.columnToSearch : 1),
        selectedRows: (props.selectedRows ? props.selectedRows : [])
        };
    }

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
     */
    onRowSelect = (context,rowSelection) => {
        
        //var index,len = rowSelection.length;
        
        //for(index=0;index<len;index++)
         //   console.log(JSON.stringify(context.state.tableData[rowSelection[index]].value));

        this.setState({selectedRows: rowSelection});
    };

    handleToggle = (event, toggled) => {
        this.setState({
        [event.target.name]: toggled,
        });
    };

    /**
     * This method is called when the keyup event is fired in the search textfield on top of the table. 
     * @param id: This is the id used to identify the table("table +id") and the textfield("tf +id").
     * @param indexColumnToSearch: The search will work on the column whose number is passed here.(Rem the checkbox is column number 0)
     */
    onKeyUp = (id,indexColumnToSearch) => {
        var input, filter, table, tr, td, i;
        input = document.getElementById("tf-"+id);
        filter = input.value.toUpperCase();
        table = document.getElementsByClassName("table-"+id)[1]; // The table header and table data are 2 separate tables. So we fetch the 2nd index as we want the data table.
        tr = table.getElementsByTagName("tr");

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

    handleChange = (event) => {
        this.setState({height: event.target.value});
    };

    render() {
        var id = this.state.id;
        var data = this.state.tableData.data;
        var totalCount = this.state.tableData.totalCount;
        var rows = []; var index=0;

        for(var property in data) {
            rows.push(<TableRow 
                key={index}
                selected={this.state.selectedRows.indexOf(index) !== -1}>
                    <TableRowColumn style={{paddingLeft:'0px'}}>{property}</TableRowColumn>
                    <TableRowColumn>{data[property] + " (" + ((data[property]/totalCount)*100).toFixed(2) + "%" + ")"}</TableRowColumn>
            </TableRow>);
            index ++;
        };

        return (
            <div>   
                <TextField
                    type="text" 
                    id={"tf-"+id}
                    className={"tf-"+id} 
                    onKeyUp={() => this.onKeyUp(id,this.state.indexColumnToSearch)} 
                    hintText="Search for value.." /> 
                <br/>
                <Table
                    className={"table-"+id}
                    
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
                    <TableRow>
                        <TableHeaderColumn style={{paddingLeft:'0px'}} >Value</TableHeaderColumn>
                        <TableHeaderColumn >Count(Percent)</TableHeaderColumn>
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


export default FilterTabs;

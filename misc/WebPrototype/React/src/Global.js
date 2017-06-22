import { Component } from 'react';

/**
 * This is the global class.
 * Use this by adding the <Global> tag to your component structure, use ref to provide a callback and then call the getGlobalInst function to get the instance of Global
 * Example usage:
 * 
 * import Global from 'global.js'
 * 
 * class Example extends React.Component
 * {
 *      ....
 *      ....
 *      onGlobalLoad = (instance) => {
 *      this.setState({GLOBAL:instance.getGlobalInst()});
 *      }
 * 
 *      render(){
 *          <div>
 *           <Global ref={this.onGlobalLoad()} />
 * 
 *                      or
 *            No Need for the extra function.
 *           <Global ref={(inst) => function(inst){this.setState({GLOBAL: inst.getGlobalInst()})}} />    
 *           ....
 *           ....
 *          </div>
 *      }
 * 
 * }
 * 
 */
class Global extends Component {
    static Global_Data = {
        Filter:{
            Range:[{
                fieldName: 'age',
                ranges: [[10, 20, 'id'], [30, 40, 'id'], [30, 40, 'id']]
            }],
            Elastic:[{
                fieldName: 'age',
                values: [[10, 'id'], [20, 'id'], [30, 'id'], [40, 'id']],
            }]
        }
    };

    render() {
        return null;
    }

    getData(){
        return Global.Global_Data;
    }


    /**
     * Adds range to the global range tracker
     * @param colName: name of the column to add the filter to (example: Age)
     * @param valueArray: [val1, val2, id]: min-max range values to filter on
     **/
    addRange(colName, valueArray) {
        var found = false;
        var push = true;
        for (var i = 0; i < Global.Global_Data.Filter.Range.length; i++) {
            if (Global.Global_Data.Filter.Range[i]["fieldName"] == colName) {
                if (Global.Global_Data.Filter.Range[i]["ranges"].length == 0) {
                    Global.Global_Data.Filter.Range[i]["ranges"].push(valueArray);
                    found = true;
                }
                else {
                    for (var j = 0; j < Global.Global_Data.Filter.Range[i]["ranges"].length; j++) {
                        if (Global.Global_Data.Filter.Range[i]["ranges"][j][2] == valueArray[2]) {
                            push = false;
                            found = true;
                        }
                    }
                    if (push) {
                        Global.Global_Data.Filter.Range[i]["ranges"].push(valueArray);
                        found = true;
                    }
                }
            }
        }
        if(!found) {
            Global.Global_Data.Filter.Range.push( {fieldName: colName, ranges: [valueArray]} );
        }
    }


    /**
     * Removes range from the global range tracker
     * @param colName: name of the column to remove the filter from (example: Age)
     * @param id: id used to find what to remove
     **/
    removeRange(colName, id) {
        for (var i = 0; i < Global.Global_Data.Filter.Range.length; i++) {
            if (Global.Global_Data.Filter.Range[i]["fieldName"] === colName) {
                for (var j = 0; j < Global.Global_Data.Filter.Range[i]["ranges"].length; j++) {
                    if (Global.Global_Data.Filter.Range[i]["ranges"][j][2] == id) {
                        console.log("Removed: [" + Global.Global_Data.Filter.Range[i]["ranges"][j][0] + ", " + Global.Global_Data.Filter.Range[i]["ranges"][j][1] + ", " + Global.Global_Data.Filter.Range[i]["ranges"][j][2] + "]");
                        Global.Global_Data.Filter.Range[i]["ranges"].splice(j, 1);
                    }
                }
            }
        }
    }


    /**
     * Adds elastic filter to the global elastic filter tracker
     * @param colName: name of the column to add the filter to (example: Age)
     * @param value: elastic value to filter on
     **/
    addFilter(colName, value) {
        var found = false;
        for (var elastic in Global.Global_Data.Filter.Elastic) {
            if (elastic.fieldName === colName) {
                elastic.values.push(value);
                found = true;
            }
        }
        if(!found) {
            Global.Global_Data.Filter.Elastic.push( {fieldName: colName, values: [value]} );
        }
    }


    /**
     * Removes elastic filter from the global elastic filter tracker
     * @param colName: name of the column to remove the filter from (example: Age)
     * @param value: elastic value to remove
     **/
    removeFilter(colName, value) {
        for (var elastic in Global.Global_Data.Filter.Elastic) {
            if (elastic.fieldName === colName) {
                elastic.values.splice(elastic.values.indexOf(value), 1);
            }
        }
    }
}

export default Global;
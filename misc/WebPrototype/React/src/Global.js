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
                ranges: [[10,20],[30,40],[30,40]],
            }],
            Elastic:[{
                fieldName: 'age',
                values: [10,20,30,40],
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
     * @param valueArray: [val1, val2]: min-max range values to filter on
     **/
    addRange(colName, valueArray) {
        var found = false;
        for (var i = 0; i < Global.Global_Data.Filter.Range.length; i++) {
            if (Global.Global_Data.Filter.Range[i]["fieldName"] === colName) {
                Global.Global_Data.Filter.Range[i]["ranges"].push(valueArray);
                found = true;
            }
        }
        if(!found) {
            Global.Global_Data.Filter.Range.push( {fieldName: colName, ranges: [valueArray]} );
        }
    }


    /**
     * Removes range from the global range tracker
     * @param colName: name of the column to remove the filter from (example: Age)
     * @param valueArray: [val1, val2]: min-max range values to remove
     **/
    removeRange(colName, valueArray) {
        console.log(Global.Global_Data.Filter.Range[0]["ranges"]);
        for (var i = 0; i < Global.Global_Data.Filter.Range.length; i++) {
            if (Global.Global_Data.Filter.Range[i]["fieldName"] === colName) {
                Global.Global_Data.Filter.Range[i]["ranges"].splice(Global.Global_Data.Filter.Range[i]["ranges"].indexOf(valueArray), 1);
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
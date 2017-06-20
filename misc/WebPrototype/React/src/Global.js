import React, { Component } from 'react';


/**
 * This is the global class.
 * Use this by adding the <Global> tag to your component structure, use ref to provide a callback and then call the getGlobalData function to get the instance of the Global_Data
 * Example usage:
 * 
 * import Global from 'global.js'
 * 
 * class Example extends React.Component
 * {
 *      ....
 *      ....
 *      onGlobalLoad = (instance) => {
 *      this.setState({GLOBAL:instance.getGlobalData()});
 *      }
 * 
 *      render(){
 *          <div>
 *           <Global ref={this.onGlobalLoad()} />
 * 
 *                      or
 *            No Need for the extra function.
 *           <Global ref={(inst) => function(inst){this.setState({GLOBAL: inst.getGlobalData()})}} />    
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
                range: [[10,20],[30,40]],
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

    getGlobalData(){
        return Global.Global_Data;
    }

}

export default Global;
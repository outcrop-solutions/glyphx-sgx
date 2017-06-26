import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import App from './App.js';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import registerServiceWorker from './registerServiceWorker';
import './index.css';


const initialFilterState = {
        Filter: {
            Ranges: {
                'StaffAssigned': {
                    //ranges: [['min','max','id', 'applied']],
                    rangeList: [[-10, 50, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true], [-10, 50, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true]],
                },
                'Academic_Rating': {
                    //ranges: [['min','max','id', 'applied']],
                    rangeList: [[-20, 60, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), false] ],
                },
                'Last_Decision_Cluster': {
                    //ranges: [['min','max','id', 'applied']],
                    rangeList: [[-30, 70, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true] ],
                },
                'Year': {
                    //ranges: [['min','max','id', 'applied']],
                    rangeList: [[-30, 70, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true] ],
                }
            },
            Elastic: {
                'colName1': {
                    selectedValues: [],
                    highlightedValues:[]
                },
                'colName2': {
                    selectedValues: [],
                    highlightedValues:[]
                },
                'colName3': {
                    selectedValues: [],
                    highlightedValues:[]
                }
            }
        }
    };

const filterReducer = function(state = initialFilterState, action) {
    switch (action.type) {
        
        case 'ADD_RANGE':
            console.log('ADD-RANGE');
            
            var stateVal = state.Filter.Ranges[action.colName].rangeList.slice();
            stateVal.push([action.min, action.max, action.id, action.applied]);
            
            //state.Filter.Ranges[action.colName].rangeList = stateval;
            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    Ranges : {
                        ...state.Filter.Ranges,
                        [action.colName] : {
                            ...state.Filter.Ranges[action.colName],
                            rangeList : stateVal
                        }
                    }
                }
            }

            console.log(newState);
            return newState;

        case 'REMOVE_RANGE':
            console.log('REMOVE-RANGE');
            var newState = state;

            var rangeList = newState.Filter.Ranges[action.colName].rangeList;
            for (var i = 0; i < rangeList.length; i++) {
                if (rangeList[i][2] == action.id) {
                    debugger;
                    rangeList.splice(i, 1);
                }
            }
            
            newState.Filter.Ranges[action.colName] = { rangeList: rangeList };

            console.log(newState);
            return newState;

        case 'UPDATE_RANGE':
            console.log('UPDATE-RANGE');
            var newState = state;

            var rangeList = newState.Filter.Ranges[action.colName].rangeList;
            for (var i = 0; i < rangeList.length; i++) {
                if (rangeList[i][2] == action.id) {
                    rangeList[i][0] = action.min;
                    rangeList[i][1] = action.max;
                    rangeList[i][3] = action.applied;
                }
            }
            
            newState.Filter.Ranges[action.colName] = { rangeList: rangeList };

            console.log(newState);
            return newState;

        case 'ADD_ELASTIC':
        {
            console.log('ADD Elastic');
              
            var newState = state;
            var col = action.filter.colName;
            newState.Filter.Elastic[col] = { selectedValues: action.filter.selectedValues, highlightedValues:action.filter.highlightedValues };
            
            //newState = Object.assign({}, state, elasticFilter);
            console.log(newState);
            return newState;
        }

        case 'REMOVE_ELASTIC':
            console.log('ADD');
            var newState = Object.assign({}, state,{ stateText : action.text} );
            console.log(newState);
            return newState;          

        default:
            return state;
    }
};

const elasticReducer = function(state = initialFilterState, action) {
  return state;
}

const reducers = combineReducers({
  filterState: filterReducer,
  elasticState: elasticReducer
});

let store = createStore(reducers);



ReactDOM.render(<Provider store={store}><TopNav /></Provider>, document.getElementById('root'));

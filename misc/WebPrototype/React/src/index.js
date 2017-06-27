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
            'StaffAssigned': {
                Range: {
                    // [minVal, maxVal, id generator, true]
                    rangeList: [[-100, 100, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true] ],
                    type: 'number',
                    applied: false
                },
                Elastic: {
                    selectedValues: [],
                    highlightedValues:[],
                    applied: false
                },
                Data: []
            },

            'Academic_Rating': {
                Ranges: {
                    // [minVal, maxVal, id generator, true]
                    rangeList: [[-100, 100, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true] ],
                    type: 'number',
                    applied: false
                },
                Elastic: {
                    selectedValues: [],
                    highlightedValues:[],
                    applied: false
                },
                Data: []
            },

            'Last_Decision_Cluster': {
                Ranges: {
                    // [minVal, maxVal, id generator, true]
                    rangeList: [[-100, 100, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true] ],
                    type: 'number',
                    applied: false
                },
                Elastic: {
                    selectedValues: [],
                    highlightedValues:[],
                    applied: false
                },
                Data: []
            },

            'Year': {
                Ranges: {
                    // [minVal, maxVal, id generator, true]
                    rangeList: [[-100, 100, ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36), true] ],
                    type: 'number',
                    applied: false
                },
                Elastic: {
                    selectedValues: [],
                    highlightedValues:[],
                    applied: false
                },
                Data: []
            }
        },
    };

const filterReducer = function(state = initialFilterState, action) {
    switch (action.type) {
        
        case 'ADD_RANGE':
            console.log('ADD-RANGE');
            
            var stateVal = state.Filter[action.colName].Range.rangeList.slice();
            stateVal.push([action.min, action.max, action.id, action.applied]);
            
            //state.Filter[action.colName].Range.rangeList = stateval;
            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        Range : {
                            ...state.Filter[action.colName].Range,
                            rangeList : stateVal
                        }
                    }
                }
            }

            console.log(newState);
            return newState;

        case 'REMOVE_RANGE':
            console.log('REMOVE-RANGE');

            var stateVal = state.Filter[action.colName].Ranges.rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    stateVal.splice(i, 1);
                }
            }

            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        Range : {
                            ...state.Filter[action.colName].Range,
                            rangeList : stateVal
                        }
                    }
                }
            }

            console.log(newState);
            return newState;

        case 'UPDATE_RANGE':
            console.log('UPDATE-RANGE');

            var stateVal = state.Filter[action.colName].Ranges.rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    if (action.min != null) {
                        stateVal[i][0] = action.min;
                    }
                    if (action.max != null) {
                        stateVal[i][1] = action.max;
                    }
                    if (action.applied != null) {
                        stateVal[i][3] = action.applied;
                    }
                }
            }

            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        Range : {
                            ...state.Filter[action.colName].Range,
                            rangeList : stateVal
                        }
                    }
                }
            }

            console.log(newState);
            return newState;

        case 'ADD_REMOVE_ELASTIC': 
        {
            console.log('ADD_REMOVE_ELASTIC');
            var col = action.filter.colName;

            var newState;

            /*if(action.filter.selectedValues.length < 1 && action.filter.highlightedValues < 1 && newState.Filter.Elastic.hasOwnProperty(col))
            {
                delete newState.Filter.Elastic[col];
            }
            else*/
            {
                newState  = {
                    ...state,
                    Filter : {
                        ...state.Filter,
                        Elastic : {
                            ...state.Filter.Elastic,
                            [col] : {
                                selectedValues: action.filter.selectedValues, 
                                highlightedValues: action.filter.highlightedValues
                            }
                        }
                    }
                };
            }
            
            //newState = Object.assign({}, state, elasticFilter);

            console.log(newState);
            return newState;
        }


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

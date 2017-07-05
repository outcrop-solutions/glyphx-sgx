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
        }
    };

const filterReducer = function(state = initialFilterState, action) {
    switch (action.type) {
        case 'INIT':
        {
            var newState;
            newState  = {
                    ...state,
                    Filter : action.storeFilterStruc
            }
            console.log(newState);
            return newState;
        }
        case 'ADD_RANGE':
            console.log('ADD-RANGE');
            
            var stateVal = state.Filter.Ranges[action.colName].rangeList.slice();
            stateVal.push(["", "", action.id, action.applied]);
            
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

            var stateVal = state.Filter.Ranges[action.colName].rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    stateVal.splice(i, 1);
                }
            }

            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    Ranges : {
                        ...state.Filter.Ranges,
                        [action.colName] : {
                            ...state.Filter.Ranges[action.colName],
                            rangeList : stateVal,
                            highlightedValues: action.highlighted,
                        }
                    }
                }
            }

            console.log(newState);
            return newState;

        case 'UPDATE_RANGE':
            console.log('UPDATE-RANGE');

            console.log("Start new state", performance.now());

            var stateVal = state.Filter.Ranges[action.colName].rangeList.slice();

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

            console.log("Changed values", performance.now());


            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    Ranges : {
                        ...state.Filter.Ranges,
                        [action.colName] : {
                            ...state.Filter.Ranges[action.colName],
                            rangeList : stateVal,
                            highlightedValues: action.highlighted,
                        }
                    }
                }
            }

            console.log("Finished new state", performance.now());
            

            console.log(newState);
            return newState;

        

         case 'REMOVE_FILTER_VIEW': 
            console.log('REMOVE_FILTER_VIEW');

            var stateVal = state.Filter.Ranges[action.colName].rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][3]) {
                    stateVal[i][3] = false;
                }
            }


            var newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    Ranges : {
                        ...state.Filter.Ranges,
                        [action.colName] : {
                            ...state.Filter.Ranges[action.colName],
                            rangeList : stateVal,
                        }
                    },
                    Elastic : {
                            ...state.Filter.Elastic,
                            [action.colName] : {
                                ...state.Filter.Elastic[action.colName],
                                selectedValues: [],
                                applied: false,
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
            //{
                newState  = {
                    ...state,
                    Filter : {
                        ...state.Filter,
                        Elastic : {
                            ...state.Filter.Elastic,
                            [col] : {
                                ...state.Filter.Elastic[col],
                                selectedValues: action.filter.selectedValues,
                                applied: action.filter.selectedValues.length > 0 ? true : (state.Filter.Range[col].highlightedValues.length > 0 ? true : false),
                            }
                        }
                    }
                };
            //}
            
            //newState = Object.assign({}, state, elasticFilter);

            console.log(newState);
            return newState;
        }

        case 'Update_Pin':
        {
            var col = action.details.colName;
            var newState;
            newState  = {
                ...state,
                Filter : {
                    ...state.Filter,
                    Elastic : {
                        ...state.Filter.Elastic,
                        [col] : {
                            ...state.Filter.Elastic[col],
                            pinned: action.details.pinned
                        }
                    }
                }
            };
            
            console.log(newState);
            return newState;
        }

        case 'UPDATE_PINS':
        {
            var cols = action.details.colNames;
            var newState;
            var elist = {};

            for(var i=0;i<cols.len;i++)
            {
                elist[cols[i]] = {
                    ...state.Filter.Elastic[col],
                    pinned: action.details.pinned
                }
            }

            debugger;

            newState  = {
                ...state,
                Filter : {
                    ...state.Filter,
                    Elastic : {
                        ...state.Filter.Elastic,
                        [col] : {
                            ...state.Filter.Elastic[col],
                            pinned: action.details.pinned
                        }
                    }
                }
            };
            
            console.log(newState);
            return newState;
        }

        default:
            return state;
    }
};

const dummyReducer = function(state = initialFilterState, action) {
  return state;
}

const reducers = combineReducers({
  filterState: filterReducer,
  dummyState: dummyReducer
});

let store = createStore(reducers);



ReactDOM.render(<Provider store={store}><TopNav /></Provider>, document.getElementById('root'));

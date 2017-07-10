import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import './index.css';


const initialFilterState = {
        Filter: {
        }
    };

const filterReducer = function(state = initialFilterState, action) {
    var newState;
    var stateVal;
    
    switch (action.type) {
        case 'INIT':
        {
            newState  = {
                    ...state,
                    Filter : action.storeFilterStruc
            }
            console.log(newState);
            return newState;
        }
        case 'ADD_RANGE':
            console.log('ADD-RANGE');
            
            stateVal = state.Filter[action.colName].rangeList.slice();
            stateVal.push(["", "", action.id, action.applied]);
            
            //state.Filter.Ranges[action.colName].rangeList = stateval;
            newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        rangeList : stateVal
                    }
                }
            }

            console.log(newState);
            return newState;

        case 'REMOVE_RANGE':
            console.log('REMOVE-RANGE');

            stateVal = state.Filter.Ranges[action.colName].rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    stateVal.splice(i, 1);
                }
            }

            var highlighted = calcHighlighted(stateVal, action.rangeType, action.data);

            newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        rangeList : stateVal,
                        highlightedValues: highlighted,
                    }
            }
            }

            console.log(newState);
            return newState;

        case 'UPDATE_RANGE':
            console.log('UPDATE-RANGE');

            console.log("Start new state", performance.now());

            stateVal = state.Filter[action.colName].rangeList.slice();


            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    var min = stateVal[i][0];
                    var max = stateVal[i][1];
                    var applied = stateVal[i][3];


                    if (action.min != null) {
                        min = action.min;
                    }
                    if (action.max != null) {
                        max = action.max;
                    }
                    if (action.applied != null) {
                        applied = action.applied;
                    }

                    stateVal[i] = [min, max, action.id, applied];
                }
            }

            var highlighted = calcHighlighted(stateVal, action.rangeType, action.data);


            console.log("Changed values", performance.now());


            newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        rangeList : stateVal,
                        highlightedValues: highlighted,
                    }
                }
            }

            console.log("Finished new state", performance.now());
            

            console.log(newState);
            return newState;

        

         case 'REMOVE_FILTER_VIEW': 
            console.log('REMOVE_FILTER_VIEW');

            stateVal = state.Filter[action.colName].rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][3]) {
                    stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false]
                }
            }


            newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                        [action.colName] : {
                            ...state.Filter[action.colName],
                            rangeList : stateVal,
                            highlightedValues: [],
                            selectedValues: [],
                            applied: false,
                        }
                }
            }
            
            
            console.log(newState);
            return newState;

        case 'ADD_REMOVE_ELASTIC':
        {
            console.log('ADD_REMOVE_ELASTIC');
            var col = action.filter.colName;

            newState  = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [col] : {
                        ...state.Filter[col],
                        selectedValues: action.filter.selectedValues,
                        applied: action.filter.selectedValues.length > 0 ? true : (state.Filter[col].highlightedValues.length > 0 ? true : false),
                    }
                }
            };

            console.log(newState);
            return newState;
        }

        case 'Update_Pin':
        {
            var colName = action.details.colName;
            newState  = { 
                ...state,
                Filter : {
                    ...state.Filter,
                    [colName] : {
                        ...state.Filter[colName],
                        pinned: action.details.pinned
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

function calcHighlighted(rList, rangeType, data) {
    console.log("Start highlighted", performance.now());
        var highlighted = [];

        if (rangeType == "Number") {
            for (var i = 0; i < rList.length; i++) {
                if (rList[i][3] == true) {
                    for (var j = 0; j < data.length; j++) {
                        if (highlighted.indexOf(data[j]) == -1) {
                            var curNum = parseInt(data[j], 10)
                            if (curNum >= rList[i][0] && curNum  <= rList[i][1]) {
                                highlighted.push(data[j]);
                            }
                        }
                    }
                }
            }
        }
        console.log("Finish highlighted", performance.now());
        console.log(highlighted);
        return highlighted;
}

const dummyReducer = function(state = initialFilterState, action) {
  return state;
}

const reducers = combineReducers({
  filterState: filterReducer,
  dummyState: dummyReducer
});

let store = createStore(reducers);



ReactDOM.render(<Provider store={store}><TopNav /></Provider>, document.getElementById('root'));

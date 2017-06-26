import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import App from './App.js';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import registerServiceWorker from './registerServiceWorker';
import './index.css';


const initialFilterState = {
        Filter:{
            Ranges: [
                {
                    id: ( + new Date() + Math.floor( Math.random() * 999999 ) ).toString(36),
                    min: 0,
                    max: 100,
                    applied: true,
                }
            ],
            Elastic: {
                'StaffAssigned': {
                    selectedValues: ["Alyssa ORourke"],
                    highlightedValues:[]
                }
            }
        }
    };

const filterReducer = function(state = initialFilterState, action) {
    switch (action.type) {
        
        case 'ADD_RANGE':
            console.log('ADD');
            var newState = Object.assign({}, state,{ stateText : action.text} );
            console.log(newState);
            return newState;

        case 'REMOVE_RANGE':
            console.log('Remove');
            var newState = Object.assign({}, state, { foo: 123 });
            return newState;

        case 'ADD_REMOVE_ELASTIC': 
        {
            console.log('ADD_REMOVE_ELASTIC');
              
            var newState = state;
            var col = action.filter.colName;

            if(action.filter.selectedValues.length < 1 && action.filter.highlightedValues < 1 && newState.Filter.Elastic.hasOwnProperty(col))
            {
                delete newState.Filter.Elastic[col];
            }
            else
                newState.Filter.Elastic[col] = { selectedValues: action.filter.selectedValues, highlightedValues:action.filter.highlightedValues };
            
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

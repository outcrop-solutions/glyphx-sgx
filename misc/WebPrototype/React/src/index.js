import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import './index.css';


const initialFilterState = {
    Filter: {
    },
    Settings: {
        primaryColor: "#2d3091",
        primaryColorLight: shadeHexColors("#2d3091", 0.2),
        primaryColorDark: shadeHexColors("#2d3091", -0.4),
        secondaryColor: "#575d5e",
        secondaryColorLight: shadeHexColors("#575d5e", 0.4),
        secondaryColorDark: shadeHexColors("#575d5e", -0.4),
        textColor: "#ffffff"
    }
};

const themeSettingColors = [["#2d3091", "#575d5e", "#ffffff"], ["#575d5e", "#2d3091", "#ffffff"], ["#bb8c00", "#012c70", "#ffffff"], ["#012c70", "#bb8c00", "#ffffff"]];

// Will darken or lighten a color by a percent (-1.0 to 1.0) input
function shadeHexColors(color, percent) {   
    var f=parseInt(color.slice(1),16),t=percent<0?0:255,p=percent<0?percent*-1:percent,R=f>>16,G=f>>8&0x00FF,B=f&0x0000FF;
    return "#"+(0x1000000+(Math.round((t-R)*p)+R)*0x10000+(Math.round((t-G)*p)+G)*0x100+(Math.round((t-B)*p)+B)).toString(16).slice(1);
}


const filterReducer = function(state = initialFilterState, action) {
    var newState;
    var stateVal;
    
    switch (action.type) {
        case 'INIT':
            newState  = {
                    ...state,
                    Filter : action.storeFilterStruc,
            }
            console.log(newState);
            return newState;

        case 'ADD_RANGE':
            console.log('ADD-RANGE');
            
            stateVal = state.Filter[action.colName].rangeList.slice();
            stateVal.push( ["", "", action.id, action.applied] );

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

            stateVal = state.Filter[action.colName].rangeList.slice();

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    stateVal.splice(i, 1);
                }
            }

            var highlighted = calcSelected(stateVal, action.rangeType, action.data);

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

            stateVal = state.Filter[action.colName].rangeList.slice();

            if (action.rangeType == "Number") {
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
            }

            else if (action.rangeType == "Text") {
                for (var i = 0; i < stateVal.length; i++) {
                    if (stateVal[i][2] == action.id) {
                        var min = stateVal[i][0];
                        var max = stateVal[i][1];
                        var applied = stateVal[i][3];
                        var selectType = stateVal[i][4];
                        var text = stateVal[i][5];

                        if (action.min != null) {
                            min = action.min;
                        }
                        if (action.max != null) {
                            max = action.max;
                        }
                        if (action.applied != null) {
                            applied = action.applied;
                        }
                        if (action.selectType != null) {
                            selectType = action.selectType;
                        }
                        if (action.text != null) {
                            text = action.text;
                        }

                        stateVal[i] = [min, max, action.id, applied, selectType, text];
                    }
                }
            }


            var selected = calcSelected(stateVal, action.rangeType, action.data, action.previousRange, state.Filter[action.colName].selectedValues.slice());
            
            newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        rangeList : stateVal,
                        selectedValues: selected,
                    }
                }
            }


            console.log(newState.Filter[action.colName].selectedValues);
            

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

            stateVal = state.Filter[col].rangeList.slice();

            if (!action.filter.checked) {
                if(action.filter.selectedValues.length > 0)
                {    
                    for (var i = 0; i < stateVal.length; i++) {
                        if (stateVal[i][3]) {
                            if (parseFloat(action.filter.value) >= stateVal[i][0] && parseFloat(action.filter.value) <= stateVal[i][1]) {
                                stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false ];
                            }
                        }
                    }
                }
                else {
                    for (var j = 0; j < stateVal.length; j++) {
                        stateVal[j] = [stateVal[j][0], stateVal[j][1], stateVal[j][2], false ];
                    }
                }
            }

            newState  = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [col] : {
                        ...state.Filter[col],
                        selectedValues: action.filter.selectedValues,
                        rangeList: stateVal,
                        applied: action.filter.selectedValues.length > 0 ? true : (state.Filter[col].highlightedValues.length > 0 ? true : false),
                    }
                }
            };

            console.log(newState);
            return newState;
        }

        case 'Update_Pin':
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

        case 'EDIT_THEME':
            newState  = { 
                ...state,
                Settings : {
                    primaryColor: themeSettingColors[action.theme][0],
                    primaryColorLight: shadeHexColors(themeSettingColors[action.theme][0], 0.2),
                    primaryColorDark: shadeHexColors(themeSettingColors[action.theme][0], -0.4),
                    secondaryColor: themeSettingColors[action.theme][1],
                    secondaryColorLight: shadeHexColors(themeSettingColors[action.theme][1], 0.4),
                    secondaryColorDark: shadeHexColors(themeSettingColors[action.theme][1], -0.4),
                    textColor: themeSettingColors[action.theme][2]
                }
            };
            
            console.log(newState);
            return newState;
        


        

        default:
            return state;
    }
};

function calcSelected(rList, rangeType, data, previousRange, selectedValues) {
    if (rangeType == "Number") {
        for (var i = 0; i < rList.length; i++) {
            if (rList[i][3] == true) {

                if (previousRange && rList[i][2] == previousRange[2] && previousRange[3] == true && (rList[i][0] != previousRange[0] || rList[i][1] != previousRange[1]) ) {
                    var previousList = [];
                    var newList = [];

                    for (var j = 0; j < data.length; j++) {
                        var curNum = parseFloat(data[j], 10)
                        if (curNum >= rList[i][0] && curNum  <= rList[i][1]) {
                            newList.push(data[j]);
                        }

                        if (curNum >= previousRange[0] && curNum  <= previousRange[1]) {
                            previousList.push(data[j]);
                        }
                    }

                    for (j = 0; j < newList.length; j++) {
                        if (selectedValues.indexOf(newList[j]) == -1) {
                            selectedValues.push(newList[j]);
                        }
                    }

                    for (j = 0; j < previousList.length; j++) {
                        if (newList.indexOf(previousList[j]) == -1) {
                            var index = selectedValues.indexOf(previousList[j]);
                            if (index != -1) {
                                selectedValues.splice(index, 1);
                            }
                        }
                    }
                }

                else {

                    for (var j = 0; j < data.length; j++) {
                        if (selectedValues.indexOf(data[j]) == -1) {
                            var curNum = parseFloat(data[j], 10)
                            if (curNum >= rList[i][0] && curNum  <= rList[i][1]) {
                                selectedValues.push(data[j]);
                            }
                        }
                    }
                }
            }
        }
    }

    console.log("selectedValues: ");
    console.log(selectedValues);
    return selectedValues;
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

import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import themeSettingColors from './ColorThemes.js';
import './index.css';

// Will darken or lighten a color by a percent (-1.0 to 1.0) input
function shadeHexColors(color, percent) {   
    var f = parseInt(color.slice(1),16), t = percent < 0 ? 0 : 255, p = percent < 0 ? percent * -1 : percent, R = f >> 16, G = f >> 8 & 0x00FF, B = f & 0x0000FF;
    return "#" + (0x1000000 + (Math.round((t - R) * p) + R) * 0x10000 + (Math.round((t - G) * p) + G) * 0x100 + (Math.round((t - B) * p) + B)).toString(16).slice(1);
}

const alphabet = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];

const initialFilterState = {
    Filter: {
    },
    Settings: {
        rangeColor: {
            sliderCircle: "#2d3091",
            sliderTrack: shadeHexColors("#2d3091", 0.4),
            textFieldUnderline: "#2d3091",
            toggleCircle: "#2d3091",
            toggleTrack: shadeHexColors("#2d3091", 0.4),
            deleteHover: "#b81616",
            addHover: "#339cee"
        },

        elasticColor: {
            checkBox: "#2d3091",
            checkAllBox: shadeHexColors("#575d5e", -0.4),
            searchBoxUnderline: "#2d3091"
        },

        filterTabColor: {
            titleText: "#ffffff",
            tabBackground: shadeHexColors("#2d3091", -0.4),
        },

        collapsibleColor: {
            mainBackground: "#2d3091",
            mainCollapsed: shadeHexColors("#2d3091", -0.4),
            mainHover: shadeHexColors("#2d3091", 0.2),
            mainText: "#ffffff",
            mainIcon: "#ffffff",

            subBackground: "#575d5e",
            subCollapsed: "#575d5e",
            subHover: shadeHexColors("#2d3091", 0.2),
            subText: "#ffffff",
            pinned: "#ffffff",
            unpinned: shadeHexColors("#2d3091", -0.4)
        },

        pinFilterColor: {
            addPinBackground: shadeHexColors("#2d3091", -0.4),
            okButton: "#2d3091",
            cancelButton: "#575d5e"
        },

        hideTopViewButtonColor: {
            background:  shadeHexColors("#575d5e", -0.4),
            icon: "#ffffff"
        },

        settingsModalColor: {
            saveButton: "#2d3091",
            cancelButton: "#575d5e"
        },

        saveModalColor: {
            saveButton: "#2d3091",
            cancelButton: "#575d5e",
            textFieldUnderline: "#2d3091",
        },

        viewSelectColor: {
            text: "black",
            selectedBackground: "#2d3091",
            selectedText: "#ffffff"
        },

        tableSelectColor: {
            text: "black",
            selectedBackground: "#2d3091",
            selectedText: "#ffffff"
        },

        topNavbar: {
            barBackground: "#2d3091"
        },

        overviewButtonsColor: {
            background: "#2d3091",
            text: "white"
        },

        filterOverviewColor: {
            badgeBackground: shadeHexColors("#2d3091", -0.4),
            badgeText: "white",
            deleteHover: "#b81616",
            elasticHover: "#339cee",
            rangeHover: "#339cee"
        },
    }
};


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

        case 'ADD_TEXT_RANGE':
            console.log('ADD_TEXT_RANGE');
            
            stateVal = state.Filter[action.colName].rangeList.slice();
            // [min, max, id, applied, selectType, text]
            stateVal.push( ["", "", action.id, action.applied, action.selectType, ""] );

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
            var previousRange, selected;

            for (var i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] == action.id) {
                    previousRange = stateVal[i];
                    stateVal.splice(i, 1);
                }
            }


            if (previousRange[3]) {
                if (action.rangeType == "Number") {
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else if (action.rangeType == "Text") {
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else {

                }
                
            }

            newState = {
                ...state,
                Filter : {
                    ...state.Filter,
                    [action.colName] : {
                        ...state.Filter[action.colName],
                        rangeList : stateVal,
                        selectedValues : selected
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
                var previousRange;
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
                        previousRange = stateVal[i];

                        stateVal[i] = [min, max, action.id, applied, selectType, text];
                    }
                }
            }

            var selected = state.Filter[action.colName].selectedValues.slice()

            if (action.selectType != null) {
                selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
            }
            else if (action.applied == false) {
                if (action.rangeType == "Number") {
                    //ADD NUMBER UNTOGGLE HERE
                    console.log("Num untoggle");
                    console.log(stateVal);
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else if (action.rangeType == "Text") {
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else {

                }

                //stateVal = checkUntoggle(stateVal, selected, action.data);
            }
            else {
                selected = calcSelected(stateVal, action.rangeType, action.data, previousRange, state.Filter[action.colName].selectedValues.slice());
            }

            
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
                            if (stateVal[i].length == 6) {
                                var selected = calcTextSelected(stateVal[i], action.filter.data);

                                if (selected.indexOf(action.filter.value) != -1) {
                                    stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false, stateVal[i][4], stateVal[i][5] ];
                                }
                            }
                            else {  
                                if (parseFloat(action.filter.value) >= stateVal[i][0] && parseFloat(action.filter.value) <= stateVal[i][1]) {
                                    stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false ];
                                }
                            }
                        }
                    }
                }
                else {
                    for (var j = 0; j < stateVal.length; j++) {
                        
                        if (stateVal[i].length == 6) {
                            stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false, stateVal[i][4], stateVal[i][5] ];
                        }
                        else {  
                            stateVal[j] = [stateVal[j][0], stateVal[j][1], stateVal[j][2], false ];
                        }
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
                        //applied: action.filter.selectedValues.length > 0 ? true : (state.Filter[col].highlightedValues.length > 0 ? true : false),
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
                Settings : themeSettingColors[action.theme]
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

    // [min, max, id, applied, selectType, text]
    // (rList, rangeType, data, previousRange, selectedValues)
    else if (rangeType == "Text") {
        for (var i = 0; i < rList.length; i++) {
            if (rList[i][3] == true) {

                if (previousRange && rList[i][2] == previousRange[2] && previousRange[3] == true && (rList[i][0] != previousRange[0] || rList[i][1] != previousRange[1] || rList[i][5] != previousRange[5]) ) {
                    var previousList = calcTextSelected(previousRange, data);
                    var newList = calcTextSelected(rList[i], data);

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

                    var newList = calcTextSelected(rList[i], data);

                    for (j = 0; j < newList.length; j++) {
                        if (selectedValues.indexOf(newList[j]) == -1) {
                            selectedValues.push(newList[j]);
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

function calcTextSelected(range, data) {
    var selectedValues = [];

    if (range[4] == 1) {
        for (var j = 0; j < data.length; j++) {
            if (selectedValues.indexOf(data[j]) == -1) {
                if ( data[j].toUpperCase().includes(range[5].toUpperCase()) ) {
                    selectedValues.push(data[j]);
                }
            }
        }

    }
    else if (range[4] == 2) {

        for (var j = 0; j < data.length; j++) {
            if (selectedValues.indexOf(data[j]) == -1) {
                if ( !data[j].toUpperCase().includes(range[5].toUpperCase()) ) {
                    selectedValues.push(data[j]);
                }
            }
        }
        
    }
    else if (range[4] == 3) {
        var regx = new RegExp("^" + range[5].toUpperCase() );

        for (var j = 0; j < data.length; j++) {
            if (selectedValues.indexOf(data[j]) == -1) {
                if ( regx.test(data[j].toUpperCase()) ) {
                    selectedValues.push(data[j]);
                }
            }
        }
    }
    else if (range[4] == 4) {
        var regx = new RegExp(range[5].toUpperCase() + "$");

        for (var j = 0; j < data.length; j++) {
            if (selectedValues.indexOf(data[j]) == -1) {
                if ( regx.test(data[j].toUpperCase()) ) {
                    selectedValues.push(data[j]);
                }
            }
        }
    }
    else if (range[4] == 5) {
        console.log("5");

        for (var j = parseInt(range[0], 10); j <= parseInt(range[1], 10); j++) {
            var regx = new RegExp("^" + alphabet[j] );

            for (var d = 0; d < data.length; d++) {
                if (selectedValues.indexOf(data[d]) == -1) {
                    if ( regx.test(data[d].toUpperCase()) ) {
                        selectedValues.push(data[d]);
                    }
                }
            }
        }

    }
    else if (range[4] == 6) {
        console.log("6");

        for (var j = parseInt(range[0], 10); j <= parseInt(range[1], 10); j++) {
            var regx = new RegExp(alphabet[j] + "$");

            for (var d = 0; d < data.length; d++) {
                if (selectedValues.indexOf(data[d]) == -1) {
                    if ( regx.test(data[d].toUpperCase()) ) {
                        selectedValues.push(data[d]);
                    }
                }
            }
        }

    }

    return selectedValues;
}

function calcSelectedNoPrevious(rList, rangeType, data) {
    var selectedValues = [];

    if (rangeType == "Number") {
        //debugger;
        for (var i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                for (var j = 0; j < data.length; j++) {
                    if (parseFloat(data[j], 10) >= rList[i][0] && parseFloat(data[j], 10) <= rList[i][1]) {
                        selectedValues.push(data[j]);
                    }
                }
            }
        }
    }

    else if (rangeType == "Text") {
        for (var i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                var newList = calcTextSelected(rList[i], data);
                for (var j = 0; j < newList.length; j++) {
                    if (selectedValues.indexOf(newList[j]) == -1) {
                        selectedValues.push(newList[j]);
                    }
                }
            }
        }
    }
    else {

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

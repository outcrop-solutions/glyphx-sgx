import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import themeSettingColors from './ColorThemes.js';
import './index.css';


/**
 * The initial state of the store (prior to loading table data)
 **/
const initialFilterState = {
    Filter: {},
    Settings: themeSettingColors[0]
};


/**
 * Redux Reducer which handles all changes to the store
 * @param state: The current state of the store
 * @param action: The action that called for a change on the store
 **/
const filterReducer = function(state = initialFilterState, action) {
    var stateVal, previousRange, selected, i;
    
    switch (action.type) {
        /**
         * Initializes the Filter structure
         * @param action.storeFilterStruc: What to set the initial Filter sub-structure to
         **/
        case 'INIT':
            return {
                ...state,
                Filter: action.storeFilterStruc,
            }


        /**
         * Adds a range to the rangeList of the Filter structure
         * @param action.colName: The column which represents the range
         * @param action.applied: Whether or not the range is applied
         * @param action.id: Generated ID for the new range row
         * @param action.rangeType: Type of the range (Number, Text, Date)
         * @param action.selectType: Only gets a value for Text-Ranges (1 = Contains, 2 = Does Not Contain, 3 = Begins With, 4 = Ends With, 5 = Begins With [Range], 6 = Ends With [Range])
         **/
        case 'ADD_RANGE':
            stateVal = state.Filter[action.colName].rangeList.slice();

            if (action.rangeType === "Number") {
                stateVal.push( ["", "", action.id, action.applied] );
            }
            else if (action.rangeType === "Text") {
                stateVal.push( ["", "", action.id, action.applied, action.selectType, ""] );
            }
            else {
                // Date range
            }

            return {
                ...state,
                Filter: {
                    ...state.Filter,
                    [action.colName]: {
                        ...state.Filter[action.colName],
                        rangeList: stateVal
                    }
                }
            }


        /**
         * Removes a range from the rangeList of the Filter structure
         * @param action.colName: The column which represents the range
         * @param action.data: The data which corresponds to the column
         * @param action.rangeType: Type of the range (Number, Text, Date)
         * @param action.id: ID of the row to be removed
         **/
        case 'REMOVE_RANGE':
            stateVal = state.Filter[action.colName].rangeList.slice();

            // Remove the range and keep track of the node that was removed
            for (i = 0; i < stateVal.length; i++) {
                if (stateVal[i][2] === action.id) {
                    previousRange = stateVal[i];
                    stateVal.splice(i, 1);
                }
            }

            // If what was removed was applied, then recaculate what should be selected in the elastic list now
            if (previousRange[3]) {
                if (action.rangeType === "Number") {
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else if (action.rangeType === "Text") {
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else {
                    // TODO: Add Date range here
                }

                return {
                    ...state,
                    Filter: {
                        ...state.Filter,
                        [action.colName]: {
                            ...state.Filter[action.colName],
                            rangeList: stateVal,
                            selectedValues: selected
                        }
                    }
                }
            }

            // Removed node was not applied so no need to recalculate selected elastic items
            return {
                ...state,
                Filter: {
                    ...state.Filter,
                    [action.colName]: {
                        ...state.Filter[action.colName],
                        rangeList: stateVal
                    }
                }
            }


        /**
         * Updates the values in an existing range
         * @param action.colName: The column which the represents the range
         * @param action.data: The data which corresponds to the column
         * @param action.rangeType: Type of the range (Number, Text, Date)
         * @param action.id: ID of the row to be updated
         * @param action.min: Min value of the slider
         * @param action.max: Max value of the slider
         * @param action.applied: Applied status of the range row
         * @param action.selectType: Only gets a value for Text-Ranges (1 = Contains, 2 = Does Not Contain, 3 = Begins With, 4 = Ends With, 5 = Begins With [Range], 6 = Ends With [Range])
         * @param action.text: Only gets a value for Text-Ranges (input for Contains, Does Not Contain, Begins With, Ends With)
         **/
        case 'UPDATE_RANGE':
            stateVal = state.Filter[action.colName].rangeList.slice();
            var min, max, applied, selectType, text;

            if (action.rangeType === "Number") {
                // Update the values of the target range row, but also save the old ones
                for (i = 0; i < stateVal.length; i++) {
                    if (stateVal[i][2] === action.id) {
                        min = stateVal[i][0];
                        max = stateVal[i][1];
                        applied = stateVal[i][3];

                        if (action.min != null) {
                            min = action.min;
                        }
                        if (action.max != null) {
                            max = action.max;
                        }
                        if (action.applied != null) {
                            applied = action.applied;
                        }

                        previousRange = stateVal[i];
                        stateVal[i] = [min, max, action.id, applied];
                        break;
                    }
                }
            }

            else if (action.rangeType === "Text") {
                // Update the values of the target range row, but also save the old ones
                for (i = 0; i < stateVal.length; i++) {
                    if (stateVal[i][2] === action.id) {
                        min = stateVal[i][0];
                        max = stateVal[i][1];
                        applied = stateVal[i][3];
                        selectType = stateVal[i][4];
                        text = stateVal[i][5];

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
                        break;
                    }
                }
            }

            else {
                // TODO: Add Date range here
            }

            // Determines if selected values should be recalculated
            if (previousRange[3] || action.applied) {
                if (action.selectType != null) {
                    selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                }
                else if (action.applied === false) {
                    if (action.rangeType === "Number") {
                        selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                    }
                    else if (action.rangeType === "Text") {
                        selected = calcSelectedNoPrevious(stateVal, action.rangeType, action.data);
                    }
                    else {
                        // TODO: Add Date range here
                    }
                }
                else {
                    selected = calcSelected(stateVal, action.rangeType, action.data, previousRange, state.Filter[action.colName].selectedValues.slice());
                }

                return {
                    ...state,
                    Filter: {
                        ...state.Filter,
                        [action.colName]: {
                            ...state.Filter[action.colName],
                            rangeList: stateVal,
                            selectedValues: selected
                        }
                    }
                }
            }
         
            return {
                ...state,
                Filter: {
                    ...state.Filter,
                    [action.colName]: {
                        ...state.Filter[action.colName],
                        rangeList: stateVal,
                    }
                }
            }


        /**
         * Removes all elastic selections and unapplies all ranges from a target column
         * @param action.colName: The column which the represents the range
         **/
         case 'REMOVE_FILTER_VIEW': 
            stateVal = state.Filter[action.colName].rangeList.slice();
            for (i = 0; i < stateVal.length; i++) {
                if (stateVal[i][3]) {

                    //debugger;
                    if (stateVal[i].length === 6) {
                        stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false, stateVal[i][4], stateVal[i][5]];
                    }
                    else {
                        stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false];
                    }
                }
            }

            return {
                ...state,
                Filter: {
                    ...state.Filter,
                    [action.colName]: {
                        ...state.Filter[action.colName],
                        rangeList: stateVal,
                        selectedValues: [],
                    }
                }
            }

        
        /**
         * Checks or unchecks an elastic table row, unapplies ranges if they include the target value
         * @param action.filter.colName: The column which the represents the elastic table
         * @param action.filter.checked: Boolean informing if checkbox was checked or unchecked
         * @param action.filter.selectedValues: Array of currently selected values (after change)
         * @param action.filter.data: The data which corresponds to the column
         * @param action.filter.value: The value of the row which had its checked status changed
         **/
        case 'ADD_REMOVE_ELASTIC':
            var col = action.filter.colName;
            var shouldUpdateRange = false;
            stateVal = state.Filter[action.filter.colName].rangeList.slice();

            if (!action.filter.checked) {
                if (action.filter.selectedValues.length > 0) {    
                    for (i = 0; i < stateVal.length; i++) {
                        if (stateVal[i][3]) {
                            if (stateVal[i].length === 6) {
                                selected = calcTextSelected(stateVal[i], action.filter.data);
                                if (selected.indexOf(action.filter.value) !== -1) {
                                    stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false, stateVal[i][4], stateVal[i][5] ];
                                    shouldUpdateRange = true;
                                }
                            }
                            else {  
                                if (parseFloat(action.filter.value) >= stateVal[i][0] && parseFloat(action.filter.value) <= stateVal[i][1]) {
                                    stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false ];
                                    shouldUpdateRange = true;
                                }
                            }
                        }
                    }
                }

                else {
                    for (i = 0; i < stateVal.length; i++) {
                        if (stateVal[i].length === 6) {
                            stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false, stateVal[i][4], stateVal[i][5] ];
                            shouldUpdateRange = true;
                        }
                        else {  
                            stateVal[i] = [stateVal[i][0], stateVal[i][1], stateVal[i][2], false ];
                            shouldUpdateRange = true;
                        }
                    }
                }
            }

            if (shouldUpdateRange) {
                return {
                    ...state,
                    Filter: {
                        ...state.Filter,
                        [col]: {
                            ...state.Filter[col],
                            selectedValues: action.filter.selectedValues,
                            rangeList: stateVal,
                        }
                    }
                };
            }

            return {
                ...state,
                Filter: {
                    ...state.Filter,
                    [col]: {
                        ...state.Filter[col],
                        selectedValues: action.filter.selectedValues,
                    }
                }
            };


        /**
         * Toggles the pinned status of a column
         * @param action.details.colName: Column name to be pinned or unpinned
         * @param action.details.pinned: Boolean indicating whether or not the column is pinned
         **/
        case 'Update_Pin':
            return { 
                ...state,
                Filter: {
                    ...state.Filter,
                    [action.details.colName]: {
                        ...state.Filter[action.details.colName],
                        pinned: action.details.pinned
                    }
                }
            };


        /**
         * Changes the color theme
         * @param action.theme: theme to change to
         **/
        case 'EDIT_THEME':
            return { 
                ...state,
                Settings: themeSettingColors[action.theme]
            };


        /**
         * Shouldn't reach here unless theres a typo in the action
         **/
        default:
            console.log("TYPO IN ACTION: " + action.type);
            //debugger;
            return state;
    }
};


/**
 * Calculates selected values given the updated range List as well as the range prior to its edit
 * @param rList: List of all ranges for one column
 * @param rangeType: Type of the column (Number, Text, Date)
 * @param data: Data corresponding to the column
 * @param previousRange: range that was edited prior to edit
 * @param selectedValues: list of currently selected values
 **/
function calcSelected(rList, rangeType, data, previousRange, selectedValues) {
    var i, j, curNum, previousList, newList, index;
    if (rangeType === "Number") {
        for (i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                if (previousRange && rList[i][2] === previousRange[2] && previousRange[3] && (rList[i][0] !== previousRange[0] || rList[i][1] !== previousRange[1]) ) {
                    previousList = [];
                    newList = [];

                    for (j = 0; j < data.length; j++) {
                        curNum = parseFloat(data[j], 10);
                        if (curNum >= rList[i][0] && curNum  <= rList[i][1]) {
                            newList.push(data[j]);
                        }

                        if (curNum >= previousRange[0] && curNum  <= previousRange[1]) {
                            previousList.push(data[j]);
                        }
                    }

                    for (j = 0; j < newList.length; j++) {
                        if (selectedValues.indexOf(newList[j]) === -1) {
                            selectedValues.push(newList[j]);
                        }
                    }

                    for (j = 0; j < previousList.length; j++) {
                        if (newList.indexOf(previousList[j]) === -1) {
                            index = selectedValues.indexOf(previousList[j]);
                            if (index !== -1) {
                                selectedValues.splice(index, 1);
                            }
                        }
                    }
                }

                else {
                    for (j = 0; j < data.length; j++) {
                        if (selectedValues.indexOf(data[j]) === -1) {
                            curNum = parseFloat(data[j], 10)
                            if (curNum >= rList[i][0] && curNum  <= rList[i][1]) {
                                selectedValues.push(data[j]);
                            }
                        }
                    }
                }
            }
        }
    }

    else if (rangeType === "Text") {
        for (i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                if (previousRange && rList[i][2] === previousRange[2] && previousRange[3] && (rList[i][0] !== previousRange[0] || rList[i][1] !== previousRange[1] || rList[i][5] !== previousRange[5]) ) {
                    previousList = calcTextSelected(previousRange, data);
                    newList = calcTextSelected(rList[i], data);

                    for (j = 0; j < newList.length; j++) {
                        if (selectedValues.indexOf(newList[j]) === -1) {
                            selectedValues.push(newList[j]);
                        }
                    }

                    for (j = 0; j < previousList.length; j++) {
                        if (newList.indexOf(previousList[j]) === -1) {
                            index = selectedValues.indexOf(previousList[j]);
                            if (index !== -1) {
                                selectedValues.splice(index, 1);
                            }
                        }
                    }
                }

                else {
                    newList = calcTextSelected(rList[i], data);
                    for (j = 0; j < newList.length; j++) {
                        if (selectedValues.indexOf(newList[j]) === -1) {
                            selectedValues.push(newList[j]);
                        }
                    }
                }
            }
        }
    }

    else {
        // TODO: Add Date Range here
    }

    console.log("selectedValues: ");
    console.log(selectedValues);
    return selectedValues;
}


/**
 * Only for text ranges, calculates selected values for one range based on its drop-down menu choice
 * @param range: Single range [min, max, id, applied, selectType, text]
 * @param data: Data corresponding to the column
 **/
function calcTextSelected(range, data) {
    var selectedValues = [];
    var i, j, regx;

    if (range[4] === 1) {
        for (i = 0; i < data.length; i++) {
            if (selectedValues.indexOf(data[i]) === -1) {
                if ( data[i].toUpperCase().includes(range[5].toUpperCase()) ) {
                    selectedValues.push(data[i]);
                }
            }
        }
    }

    else if (range[4] === 2) {

        for (i = 0; i < data.length; i++) {
            if (selectedValues.indexOf(data[i]) === -1) {
                if ( !data[i].toUpperCase().includes(range[5].toUpperCase()) ) {
                    selectedValues.push(data[i]);
                }
            }
        }
    }

    else if (range[4] === 3) {
        regx = new RegExp("^" + range[5].toUpperCase() );

        for (i = 0; i < data.length; i++) {
            if (selectedValues.indexOf(data[i]) === -1) {
                if ( regx.test(data[i].toUpperCase()) ) {
                    selectedValues.push(data[i]);
                }
            }
        }
    }

    else if (range[4] === 4) {
        regx = new RegExp(range[5].toUpperCase() + "$");

        for (i = 0; i < data.length; i++) {
            if (selectedValues.indexOf(data[i]) === -1) {
                if ( regx.test(data[i].toUpperCase()) ) {
                    selectedValues.push(data[i]);
                }
            }
        }
    }

    else if (range[4] === 5) {
        for (i = parseInt(range[0], 10); i <= parseInt(range[1], 10); i++) {
            regx = new RegExp("^" + alphabet[i] );

            for (j = 0; j < data.length; j++) {
                if (selectedValues.indexOf(data[j]) === -1) {
                    if ( regx.test(data[j].toUpperCase()) ) {
                        selectedValues.push(data[j]);
                    }
                }
            }
        }
    }

    else if (range[4] === 6) {
        for (i = parseInt(range[0], 10); i <= parseInt(range[1], 10); i++) {
            regx = new RegExp(alphabet[i] + "$");

            for (j = 0; j < data.length; j++) {
                if (selectedValues.indexOf(data[j]) === -1) {
                    if ( regx.test(data[j].toUpperCase()) ) {
                        selectedValues.push(data[j]);
                    }
                }
            }
        }
    }
    return selectedValues;
}


/**
 * Calculate the selected values not worrying about the previous range
 * @param rList: List of all ranges for one column
 * @param rangeType: Type of the range (Number, Text, Date)
 * @param data: Data corresponding to the column
 **/
function calcSelectedNoPrevious(rList, rangeType, data) {
    var selectedValues = [];
    var i, j;

    if (rangeType === "Number") {
        for (i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                for (j = 0; j < data.length; j++) {
                    if (parseFloat(data[j], 10) >= rList[i][0] && parseFloat(data[j], 10) <= rList[i][1]) {
                        selectedValues.push(data[j]);
                    }
                }
            }
        }
    }

    else if (rangeType === "Text") {
        for (i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                var newList = calcTextSelected(rList[i], data);
                for (j = 0; j < newList.length; j++) {
                    if (selectedValues.indexOf(newList[j]) === -1) {
                        selectedValues.push(newList[j]);
                    }
                }
            }
        }
    }

    else {
        // TODO: Add Date Range here
    }
    
    console.log("selectedValues: ");
    console.log(selectedValues);
    return selectedValues;
}

/**
 * Used to translate slider values to letters
 **/
const alphabet = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];


const dummyReducer = function(state = initialFilterState, action) {
  return state;
}

const reducers = combineReducers({
  filterState: filterReducer,
  dummyState: dummyReducer
});

let store = createStore(reducers);



ReactDOM.render(<Provider store={store}><TopNav /></Provider>, document.getElementById('root'));

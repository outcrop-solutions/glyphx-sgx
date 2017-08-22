import React from 'react';
import ReactDOM from 'react-dom';
import VisualizationView from './VisualizationView';
import { Provider } from 'react-redux';
import { createStore, combineReducers } from 'redux';
import themeSettingColors from './ColorThemes.js';
import {calcTextSelected,calcSelectedRemoved,calcSelected} from './CalculateSelectedHelper.js';
import RedirectRouter from './Router.js';
import './General.css';


/**
 * The initial state of the store (prior to loading table data)
 **/
const initialFilterState = {
    Filter: {},
    Settings: { 
        colors: themeSettingColors[0],
        sideBarOverlap: false,
    },
    ModalDisplay: {
        settingsModal: false,
        statisticsModal: false,
        announcementsModal: false,
        legendModal: true,
        allViewsModal: false,
        helpModal: false,
    },
    UserInfo: {
    },
    isUserLoggedIn: false,
    Statistics: { 
        colList: "",
        statList: ""
    },
};


/**
 * Redux Reducer which handles all changes to the store
 * @param state: The current state of the store
 * @param action: The action that called for a change on the store
 **/
const filterReducer = function(state = initialFilterState, action) {
    var stateVal, previousRange, selected, display, i;
    
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
                stateVal.push( [action.min, action.max, action.id, action.applied] );
            }
            else if (action.rangeType === "Text") {
                stateVal.push( [action.min, action.max, action.id, action.applied, action.selectType, ""] );
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
                
                selected = calcSelectedRemoved(stateVal, action.rangeType, action.data, previousRange, state.Filter[action.colName].selectedValues.slice());

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
                if (action.selectType !== undefined) {
                    selected = calcSelectedRemoved(stateVal, action.rangeType, action.data, previousRange, state.Filter[action.colName].selectedValues.slice());
                }
                else if (action.applied === false) {
                    selected = calcSelectedRemoved(stateVal, action.rangeType, action.data, previousRange, state.Filter[action.colName].selectedValues.slice());
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
        case 'EDIT_SETTINGS':

            display = state.ModalDisplay.settingsModal;
            var overlap = state.Settings.sideBarOverlap;
            var colors = state.Settings.colors;

            if (action.display != null) {
                display = action.display
            }
            if (action.overlap != null) {
                overlap = action.overlap
            }
            if (action.theme != null) {
                colors = themeSettingColors[action.theme]
            }

            return { 
                ...state,
                Settings: {
                    ...state.Settings,
                    colors: colors,
                    sideBarOverlap: overlap
                },
                ModalDisplay: {
                    ...state.ModalDisplay,
                    settingsModal: display
                }
            };

        case 'SAVE_USER_INFO':
            return {
                ...state,
                UserInfo: action.info,
                isUserLoggedIn: true
            }

        case 'UPDATE_STATISTICS':

            display = state.ModalDisplay.statisticsModal;
            var colList = state.Statistics.colList.slice();
            var statList = state.Statistics.statList.slice();

            if (action.display != null) {
                display = action.display
            }
            if (action.colList != null) {
                colList = action.colList
            }
            if (action.statList != null) {
                statList = action.statList
            }
            
            return { 
                ...state,
                Statistics: {
                    ...state.Statistics,
                    colList: colList,
                    statList: statList,
                },
                ModalDisplay: {
                    ...state.ModalDisplay,
                    statisticsModal: display
                }
            };

        case 'EDIT_MODAL_DISPLAY':

            var settingsDisplay = state.ModalDisplay.settingsModal;
            var statisticsDisplay = state.ModalDisplay.statisticsModal;
            var announcementsDisplay = state.ModalDisplay.announcementsModal;
            var legendDisplay = state.ModalDisplay.legendModal;
            var allViewsDisplay = state.ModalDisplay.allViewsModal;
            var helpDisplay = state.ModalDisplay.helpModal;
            
            if (action.settingsModal === true || action.settingsModal === false) {
                settingsDisplay = action.settingsModal;
            }

            if (action.statisticsModal === true || action.statisticsModal === false) {
                statisticsDisplay = action.statisticsModal;
            }

            if (action.announcementsModal === true || action.announcementsModal === false) {
                announcementsDisplay = action.announcementsModal;
            }

            if (action.legendModal === true || action.legendModal === false) {
                legendDisplay = action.legendModal;
            }

            if (action.allViewsModal === true || action.allViewsModal === false) {
                allViewsDisplay = action.allViewsModal;
            }

            if (action.helpModal === true || action.helpModal === false) {
                helpDisplay = action.helpModal;
            }


            return { 
                    ...state,
                    ModalDisplay: {
                        ...state.ModalDisplay,
                        settingsModal: settingsDisplay,
                        statisticsModal: statisticsDisplay,
                        announcementsModal: announcementsDisplay,
                        legendModal: legendDisplay,
                        allViewsModal: allViewsDisplay,
                        helpModal: helpDisplay
                    }
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


const dummyReducer = function(state = initialFilterState, action) {
  return state;
}

const reducers = combineReducers({
  filterState: filterReducer,
  dummyState: dummyReducer
});

let store = createStore(reducers);


ReactDOM.render(<Provider store={store}><RedirectRouter /></Provider>, document.getElementById('root'));

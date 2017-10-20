/**
 * Used to translate slider values to letters
 **/
const alphabet = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];


/**
 * Calculates selected values given the updated range List as well as the range prior to its edit
 * @param rList: List of all ranges for one column
 * @param rangeType: Type of the column (Number, Text, Date)
 * @param data: Data corresponding to the column
 * @param previousRange: range that was edited prior to edit
 * @param selectedValues: list of currently selected values
 **/
export function calcSelected(rList, rangeType, data, previousRange, selectedValues) {
    var i, j, curNum, previousList, newList, index;
    if (rangeType === "Number") {
        console.log("selectedValues1: ");
        for (i = 0; i < rList.length; i++) {
            if (rList[i][3]) {
                if (rList[i][2] === previousRange[2] && previousRange[3] && (rList[i][0] !== previousRange[0] || rList[i][1] !== previousRange[1]) ) {
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
        console.log("selectedValues2: ");
        var foundRange = false;
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
        console.log("selectedValues3: ");
    }

    console.log("selectedValues1000: ");
    console.log(selectedValues);
    return selectedValues;
}


/**
 * Only for text ranges, calculates selected values for one range based on its drop-down menu choice
 * @param range: Single range [min, max, id, applied, selectType, text]
 * @param data: Data corresponding to the column
 **/
export function calcTextSelected(range, data) {
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
        console.log("selectedValues21: ");
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
        console.log("selectedValues22: ");
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
        console.log("selectedValues23: ");
    }
    
    console.log("selectedValues2000: ");
    console.log(selectedValues);
    return selectedValues;
}


/**
 * 
 * @param rList: List of all ranges for one column
 * @param rangeType: Type of the column (Number, Text, Date)
 * @param data: Data corresponding to the column
 * @param previousRange: range that was edited prior to edit
 * @param selectedValues: list of currently selected values
 **/
export function calcSelectedRemoved(rList, rangeType, data, previousRange, selectedValues) {
    var i, j, curNum, previousList, newList, index;
    if (rangeType === "Number") {
        console.log("selectedValues31: ");

        newList = calcSelectedNoPrevious(rList, rangeType, data);
        previousList = [];

        for (i = 0; i < data.length; i++) {
            curNum = parseFloat(data[i], 10)
            if (curNum >= previousRange[0] && curNum  <= previousRange[1]) {
                previousList.push(data[i]);
            }
        }

        for (i = 0; i < previousList.length; i++) {
            index = selectedValues.indexOf(previousList[i]);
            if (newList.indexOf(previousList[i]) === -1 && index !== -1) {
                selectedValues.splice(index, 1);
            }
        }
    }

    else if (rangeType === "Text") {
        console.log("selectedValues32: ");
        newList = calcSelectedNoPrevious(rList, rangeType, data);
        previousList = calcTextSelected(previousRange, data);

        for (i = 0; i < previousList.length; i++) {
            index = selectedValues.indexOf(previousList[i]);
            if (newList.indexOf(previousList[i]) === -1 && index !== -1) {
                selectedValues.splice(index, 1);
            }
        }
    }

    else {
        // TODO: Add Date Range here
        console.log("selectedValues33: ");
    }

    console.log("selectedValues3000: ");
    console.log(selectedValues);
    return selectedValues;
}

import React from 'react';
import { connect } from 'react-redux';


/**
 * A "listener" on the filter section of the redux store
 * Logs valid changes to enable undo-redo
 **/
class UndoRedoChangeListener extends React.Component {

    state = {
        history: [],
    }


    /**
     * React built-in which tells react if it should re-render the component
     * @param nextProps: The props the component would have after the change
     * @param nextState: The state the component would have after the change
     * @returns: true if it should render and false if it shouldn't
     **/
    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.filterList != nextProps.filterList && nextProps.lastUpdate != "snapshot") {
            // Change to be tracked has been detected, save it
            this.handleHistoryUpdate(this.props.filterList, nextProps.filterList);
            return true;
        }

        else {
            return false;
        }
    }


    /**
     * Updates the history local storage variable
     * @param oldList: Filter list before the change
     * @param filterList: Filter list after the change
     **/
    handleHistoryUpdate(oldList, filterList) {
        var history = this.state.history;
        var newElement = {filterList: filterList, tableData: this.props.tableData};
        var jString = JSON.stringify(newElement);

        debugger;

        if (history[history.length - 1] != jString && jString !== "{}") {

            var position = parseInt(window.localStorage.getItem('position'), 10);

            if (position !== history.length - 1) {
                history = history.slice(0, position + 1);
                history.push(jString);
            }

            else {
                history.push(jString);
            }

            var formatHistory = "";

            if (history.length === 1) {
                formatHistory = history[0];
            }

            for (var i = 0; i < history.length; i++) {
                if (i === 0) {
                    formatHistory = history[0];
                }

                else {
                    formatHistory += ("!!" + history[i]);
                }
            }

            window.localStorage.setItem('history', formatHistory);

            // History element was added to increment position "pointer"
            var newPosition = ( parseInt(window.localStorage.getItem('position'), 10) + 1 );
            window.localStorage.setItem('position', newPosition);
        }
    }


    render() {
        return (
            <div style = {{ display: "none" }} />
        );
    }
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    lastUpdate: state.filterState.LatestFilterChange
  }
};


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(UndoRedoChangeListener);
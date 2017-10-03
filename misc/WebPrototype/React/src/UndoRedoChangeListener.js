import React from 'react';
import { connect } from 'react-redux';


class UndoRedoChangeListener extends React.Component {

    state = {
        history: []
    }

    componentDidMount() {
        var history = this.state.history;
        history.push(JSON.stringify(this.props.filterList));
    }


    shouldComponentUpdate(nextProps, nextState) {
        if (this.props.filterList != nextProps.filterList && nextProps.lastUpdate != "snapshot") {
            this.handleHistoryUpdate(nextProps.filterList);
            return true;
        }
        else {
            return false;
        }
    }

    handleHistoryUpdate(filterList) {
        var history = this.state.history;
        var jString = JSON.stringify(filterList);

        if (history[history.length - 1] != jString) {

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

const mapStateToProps = function(state){
  return {
    filterList: state.filterState.Filter,
    lastUpdate: state.filterState.LatestFilterChange
  }
};

export default connect(mapStateToProps)(UndoRedoChangeListener);
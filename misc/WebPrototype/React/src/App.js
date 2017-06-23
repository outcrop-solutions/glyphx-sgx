import React from 'react';
import { connect } from 'react-redux';

const mapStateToProps = function(state){
  return {
    stateText: state.filterState.stateText,
  }
}


export const addRange = (text) => ({
  type: 'ADD_RANGE',
  text
});



class App extends React.Component {

    hideLoadMask = () => {

        var lm = document.getElementById("ipl-progress-indicator");
        if(lm) {
            setTimeout(() => {
                document.getElementById("ipl-progress-indicator").classList.add('available');
                setTimeout(() => {
                document.getElementById("ipl-progress-indicator").outerHTML = '';
                }, 2000)
            }, 1000)
        }

    };

    componentDidMount() {
        this.hideLoadMask();
        //console.log(this.props.something);
    };

    clickDiv = (evt,context) => {
            debugger;
            console.log("before: " + this.props.stateText);
            this.props.dispatch(addRange("Hi2"));
            console.log("after: " +this.props.stateText);
    };

    render(){
        return(
            <div >
                <input type="text" value={this.props.stateText} />
                <button onClick={this.clickDiv.bind(this)} />
            </div>
        );

    }
};

export default connect(mapStateToProps)(App);
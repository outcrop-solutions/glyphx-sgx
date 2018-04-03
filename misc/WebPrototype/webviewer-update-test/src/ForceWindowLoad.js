import React, { Component } from 'react';
import {withRouter} from 'react-router';

class ForceWindowLoad extends Component {

  componentDidUpdate(prevProps) { 
    if (this.props.location !== prevProps.location) { // detect page change
      window.scrollTo(0, 0); // for example // or document.querySelector ... or any js
    }
  }

  render() {
   return this.props.children
  }

}

export default withRouter(ForceWindowLoad)
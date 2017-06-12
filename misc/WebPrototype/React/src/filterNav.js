import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'materialize-css/bin/materialize.js';
import 'materialize-css/dist/css/materialize.css';
import {Navbar, NavItem} from 'react-materialize';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';

class FilterNav extends Component {
  render() {
    return (
      <div className="TopNav">
          {/* TOP SECTION */}
          HELLO I AM FILTER

          {/* BOTTOM SECTION */}
      </div>
    );
  }
}

export default FilterNav;
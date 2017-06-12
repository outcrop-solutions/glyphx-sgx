import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'materialize-css/bin/materialize.js';
import 'materialize-css/dist/css/materialize.css';
import {Navbar, NavItem} from 'react-materialize';
import 'font-awesome/css/font-awesome.css';
import FilterNav from './filterNav.js';
import './topNav.css';

class TopNav extends Component {
  render() {
    return (
      <div className="TopNav">

        <Navbar brand='LOGO' right>
          <NavItem href=''><i className="fa fa-bars fa-2x"></i></NavItem>
          <NavItem href=''><i className="fa fa-floppy-o fa-2x"></i></NavItem>
          <NavItem href=''><i className="fa fa-cog fa-2x"></i></NavItem>
          <NavItem onClick={toggleNav}><i className="fa fa-filter fa-2x"></i></NavItem>
        </Navbar>

        <div id="filterNav" className="sidenav">
          <FilterNav></FilterNav>
        </div>

      </div>
    );
  }
}

// Hides the filter side nav by translating it off the screen so it doesnt resize and 
// wont have to be reloaded after it is "closed"
function toggleNav() {
    if (document.getElementById("filterNav").style.transform == "translate(460px, 0px)") {
        document.getElementById("filterNav").style.transform = "translate(0px, 0px)";
    }
    else {
        document.getElementById("filterNav").style.transform = "translate(460px, 0px)";
    }
}

export default TopNav;
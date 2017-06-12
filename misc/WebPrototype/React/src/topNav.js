import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'materialize-css/bin/materialize.js';
import 'materialize-css/dist/css/materialize.css';
import {Navbar, NavItem} from 'react-materialize';
import 'font-awesome/css/font-awesome.css';
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
          HELLO
        </div>

      </div>
    );
  }
}

function toggleNav() {
  transform: 
    if (document.getElementById("filterNav").style.transform == "translate(460px, 0px)") {
        document.getElementById("filterNav").style.transform = "translate(0px, 0px)";
    }
    else {
        document.getElementById("filterNav").style.transform = "translate(460px, 0px)";
    }
}

export default TopNav;


//transition: transform 450ms cubic-bezier(0.23, 1, 0.32, 1) 0ms; 
//transform: translate(460px, 0px);

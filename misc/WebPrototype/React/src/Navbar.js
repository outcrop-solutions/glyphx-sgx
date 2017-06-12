import React, { Component } from 'react';
import 'materialize-css/bin/materialize.js';
import 'materialize-css/dist/css/materialize.css';
import 'font-awesome/css/font-awesome.css';
import './Navbar.css';

class Navbar extends Component {
  render() {
    return (
      <div className="Navbar">

          <nav>
            <div className="nav-wrapper">
              <a className="brand-logo">Some Useful Toolbar</a>
              <ul className="right hide-on-med-and-down">
                <li><a><i className="fa fa-bars fa-2x"></i></a></li>
                <li><a><i className="fa fa-floppy-o fa-2x"></i></a></li>
                <li><a><i className="fa fa-cog fa-2x"></i></a></li>
                <li onClick={toggleNav}><a><i className="fa fa-filter fa-2x"></i></a></li>
              </ul>
            </div>
          </nav>

          <div id="filterNav" className="sidenav">
            HELLO
          </div>

      </div>
    );
  }
}

function toggleNav() {
    if (document.getElementById("filterNav").style.width == "450px") {
        document.getElementById("filterNav").style.width = "0";
    }
    else {
        document.getElementById("filterNav").style.width = "450px";
    }
}

export default Navbar;

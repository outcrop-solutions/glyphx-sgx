import React, { Component } from 'react';
import 'jquery';
import $ from 'jquery';
import 'materialize-css/bin/materialize.js';
import 'materialize-css/dist/css/materialize.css';
import 'material-icons';
import 'font-awesome/css/font-awesome.css';
import './App.css';

class App extends Component {
  render() {
    return (
      <div className="App">

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

          <div id="mySidenav" className="sidenav">
            HELLO
          </div>

      </div>
    );
  }
}

function toggleNav() {
    if (document.getElementById("mySidenav").style.width == "450px") {
        document.getElementById("mySidenav").style.width = "0";
    }
    else {
        document.getElementById("mySidenav").style.width = "450px";
    }
}

export default App;

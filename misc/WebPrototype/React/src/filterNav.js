import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'materialize-css/bin/materialize.js';
import 'materialize-css/dist/css/materialize.css';
import {Navbar, NavItem} from 'react-materialize';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';
import SelectionField from './SelectionField.js';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import MenuItem from 'material-ui/MenuItem';

class FilterNav extends Component {
	
	 constructor(props) {
    super(props);

    this.state  = {
      menu:{
	  open: false,
	  anchorOrigin: {
        horizontal: 'left',
        vertical: 'bottom',
      },
      targetOrigin: {
        horizontal: 'left',
        vertical: 'top',
      },
	  }
    };
  }
  
  handleTouchTap1 = (event) => {
    // This prevents ghost click.
    event.preventDefault();

    this.setState(
	{
		menu: {open: true,
		anchorEl: event.currentTarget,}
    });
  };
  handleRequestClose = () => {
    this.setState(
	{
      menu: {open: false}
    });
  };
  
	
  render() {
    return (
      <div className="TopNav" style={{height: '100%'}}>
          {/* TOP SECTION */}
		  <div id="TopView" className="container" style={{width:'93%',height: '30%'}}>
		  
		  {/* Row 1 */}
		  <div className="row" style={{'marginBottom': '0px',height:'15%'}}>
		  <div className="input-field col s9 filter-wrap filter-font" style={{marginTop: '0px'}}>
			<SelectionField />
		  </div>
			 <div className="input-field col s3 filter-wrap filter-font marginTop">
				<div>
					<RaisedButton
					  onClick={this.handleTouchTap1}
					  label="Menu"
					/>
					<Popover
					  open={this.state.menu.open}
					  anchorEl={this.state.menu.anchorEl}
					  onRequestClose={this.handleRequestClose}
					>
					  <Menu>
						<MenuItem primaryText="Refresh" />
						<MenuItem primaryText="Help &amp; feedback" />
						<MenuItem primaryText="Settings" />
						<MenuItem primaryText="Sign out" />
					  </Menu>
					</Popover>
				</div>
			 </div>
		  </div>
		  
		  {/* Row 2 */}
		  <div className="row" style={{'marginBottom': '0px',height:'50%'}}>
		  </div>
		  
		  {/* Row 3 */}
		  <div className="row" style={{'marginBottom': '0px',height:'10%'}}>
		  </div>
		  
		  {/* Row 4 */}
		  <div className="row" style={{'marginBottom': '0px',height:'25%'}}>
		  </div>
		  
		  </div>
          {/* BOTTOM SECTION */}
      </div>
    );
  }
}

export default FilterNav;
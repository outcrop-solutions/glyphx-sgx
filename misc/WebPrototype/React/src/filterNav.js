import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';
import SelectionField from './SelectionField.js';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import MenuItem from 'material-ui/MenuItem';
import Range from './range.js';
import {Flex} from 'react-flex-material';

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
			<Flex layout="column" style={{height: '100%'}}>
				<Flex flex="30" id="TopView">
							  
							  {/* Row 1 */}
							  <Flex layout="row" align="space-between center">
							  
									<Flex flex="80">
									  <div className="input-field col s9 filter-wrap filter-font" style={{marginTop: '0px'}}>
										<SelectionField />
									  </div>
									</Flex>
									<Flex divider />
										<Flex flex="20">
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
										 </Flex>
								</Flex>
								
							  
							  {/* Row 2 */}
							  <Flex layout="row" style={{'marginBottom': '0px',height:'50%'}}>
							  </Flex>
							  
							  {/* Row 3 */}
							  <Flex layout="row" style={{'marginBottom': '0px',height:'10%'}}>
							  </Flex>
							  
							  {/* Row 4 */}
							  <Flex layout="row" style={{'marginBottom': '0px',height:'25%'}}>
							  </Flex>
				</Flex>				  
							  {/* BOTTOM SECTION */}
				<Flex flex="75">
					<Range ></Range >
				</Flex>
			</Flex>
      </div>
    );
  }
}

export default FilterNav;
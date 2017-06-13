import React, { Component } from 'react';
import injectTapEventPlugin from 'react-tap-event-plugin';
import 'font-awesome/css/font-awesome.css';
import './filterNav.css';
import SelectionField from './SelectionField.js';
import RaisedButton from 'material-ui/RaisedButton';
import Popover from 'material-ui/Popover';
import Menu from 'material-ui/Menu';
import SelectField from 'material-ui/SelectField';
import MenuItem from 'material-ui/MenuItem';
import Range from './range.js';
import {Flex} from 'react-flex-material';
import Divider from 'material-ui/Divider';

const items = [];
for (let i = 0; i < 50; i++ ) {
  items.push(<MenuItem value={i} key={i} primaryText={'View '+i+''} />);
}

class FilterNav extends Component {
	
	 constructor(props) {
    super(props);

    this.state  = {
      menu:{
	  open: false,
	  },
	  viewSelect:{
		  value: 10,
	  }
    };
  }
  
  /**
	* This method is called when the menu button is clicked. The menu button is in the top right corner of the filerbar.
	**/
  menuDropdownClick = (event) => {
    event.preventDefault();

    this.setState(
	{
		menu: {open: true,
		anchorEl: event.currentTarget,}
    });
  };
  
  /**
	* This method is called when there is loss of focus when the menu is open.
	**/
  handleRequestClose = () => {
    this.setState(
	{
      menu: {open: false}
    });
  };
  
  /**
	* This method is called when an item is selected in the "select view" dropdown that is present in the top left.
	**/
   onSelectViewChange = (event, index, value) => {
    this.setState({viewSelect:{
		  value: value,
	  }});
	console.log(value);
  };

  
	
  render() {
    return (
      <div className="TopNav" style={{height: '100%'}}>
          {/* TOP SECTION */}
			<Flex layout="column" style={{height: '100%'}}>
				<Flex flex="30" id="TopView">
							  
							  {/* Row 1 */}
								<Flex layout="row" align="space-between center"    style={{height: '20%'}}>
							  
									<Flex flex="80">
									  <SelectField
										value={this.state.viewSelect.value}
										onChange={this.onSelectViewChange}
										style={{width:"100%",fontSize:'13px'}}
									  >
										{items}
									  </SelectField>
								</Flex>
									
									
									<Flex divider />
										<Flex flex="20">
										 	<div>
												<RaisedButton
												  onClick={this.menuDropdownClick}
												  label="Menu"
												  buttonStyle={{color: 'rgb(0, 188, 212)'}}
												  labelStyle={{fontSize: '13px'}}
												  primary={true}
												/>
												<Popover
												  open={this.state.menu.open}
												  anchorEl={this.state.menu.anchorEl}
												  onRequestClose={this.handleRequestClose}
												  style={{fontSize: '13px'}}
												>
												  <Menu>
													<MenuItem primaryText="New" style={{fontSize: '13px'}}/>
													<Divider />
													<MenuItem primaryText="Save" style={{fontSize: '13px'}}/>
													<MenuItem primaryText="Save As" style={{fontSize: '13px'}}/>
												  </Menu>
												</Popover>
											</div>
										 </Flex>
								</Flex>
								
							  
							  {/* Row 2 */}
							  <Flex layout="row" style={{height:'50%'}}>
							  
							  </Flex>
							  
							  {/* Row 3 */}
							  <Flex layout="row" style={{height:'15%'}}>
							  </Flex>
							  
							  {/* Row 4 */}
							  <Flex layout="row" style={{height:'15%'}}>
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
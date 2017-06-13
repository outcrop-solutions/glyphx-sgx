import React, {Component} from 'react';
import AutoComplete from 'material-ui/AutoComplete';
import MenuItem from 'material-ui/MenuItem';
import injectTapEventPlugin from 'react-tap-event-plugin';
injectTapEventPlugin();

export default class SelectionField extends Component {
  
  dataSource2 = ['View 1', 'View 2', 'View 3'];
  
  onSelect(value,position) {
	console.log("Selected " + value + " " + position);
  }
  
  render() {
    return (
      <div>
        <AutoComplete
	  style={{fontSize: '13px',width:'100%',height: '49px'}}
	  fullWidth={true}
	  textFieldStyle = {{fontSize: '13px'}}
      hintText={"Select to open a view"}
      filter={AutoComplete.caseInsensitiveFilter}
      openOnFocus={true}
      dataSource={this.dataSource2}
	  onNewRequest={this.onSelect}
    />
      </div>
    );
  }
}
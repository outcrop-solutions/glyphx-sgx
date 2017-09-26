import React from 'react';
import { connect } from 'react-redux';
import Flexbox from 'flexbox-react';
import { RadioButton, RadioButtonGroup } from 'material-ui/RadioButton';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import PieChart from './PieChart.js';
import RaisedButton from 'material-ui/RaisedButton';



class VotingModal extends React.Component {

    state = {
        voted: false,
        value: null
    }
	
	render() {
		return(
			<Dialog
				title = "Voting is still open"
				actions = {
					[
						<FlatButton
							label = "Close"
							primary = { true }
							onClick = { this.props.toggleDisplay }
							style = {{ color: this.props.settings.colors.settingsModalColor.saveButton }}
                            labelStyle = {{ lineHeight: "40px" }}
						/>
					]
				}
				modal = { true }
				open = { this.props.display }
			>
                <div style = {{ display: (this.state.voted ? "none" : "") }} >
                    <RadioButtonGroup 
                        name = "votingGroup"
                        style = {{
                            fontWeight: "normal"
                        }}
                        onChange = { (event, value) => this.setState({ value : value }) }

                    >
                        <RadioButton
                            value = {0}
                            label = "Have Aditya stop playing loud music"
                            style = {{
                                marginBottom: "10px",
                                marginTop: "14px"
                            }}
                            iconStyle = {{
                                fill: this.props.settings.colors.elasticColor.checkBox
                            }}
                        />
                        <RadioButton
                            value = {1}
                            label = "Have Brad stop messing with peoples desks"
                            style = {{
                                marginBottom: "10px"
                            }}
                            iconStyle = {{
                                fill: this.props.settings.colors.elasticColor.checkBox
                            }}
                        />
                        <RadioButton
                            value = {2}
                            label = "Have lunch on Mark"
                            style = {{
                                marginBottom: "10px"
                            }}
                            iconStyle = {{
                                fill: this.props.settings.colors.elasticColor.checkBox
                            }}
                        />
                        <RadioButton
                            value = {3}
                            label = "Have the day off"
                            style = {{
                                marginBottom: "10px"
                            }}
                            iconStyle = {{
                                fill: this.props.settings.colors.elasticColor.checkBox
                            }}
                        />
                    </RadioButtonGroup>

                    <RaisedButton 
                        label = "Submit"
                        style = {{
                            width: "100%",
                            margin: "10px auto",
                        }}
                        buttonStyle = {{
                            height: '35px',
                            lineHeight: '35px',
                            backgroundColor: this.props.settings.colors.buttons.general  
                        }} 
                        labelStyle = {{
                            fontSize: '14px',
                            color: this.props.settings.colors.overviewButtonsColor.text,
                            fontFamily: "Arial Black, Gadget, sans-serif",
                            margin: "0px 0px 0px -3px",
                            paddingLeft: "0px",
                            paddingRight: "0px"
                        }}
                        overlayStyle = {{
                            height: '35px',
                            lineHeight: '35px',
                        }}
                        onClick = { () => this.setState({ voted: true }) }
                        primary = {true } 
                    />

                </div>


                <Flexbox flexDirection="row" style = {{ marginLeft: "105px", display: (this.state.voted ? "" : "none") }} >
                    <PieChart
                        data={ [21, 45, 10, 15] }
                        radius={ 150 }
                        hole={ 50 }
                        colors={ ['#43A19E', '#7B43A1', '#F2317A', '#FF9824'] }
                        labels={ true }
                        percent={ true }
                        strokeWidth={ 3 }
                        stroke={ '#fff' }
                    />
    
                    <dl
                        style = {{ 
                            width: "200px",
                            background: "#fff",
                            padding: "5px 15px",
                            marginLeft: "10px"
                        }}
                    >
                        <dt style = {{ borderRadius: "3px", display: "inline", backgroundColor: "#7B43A1" }} > &nbsp; &nbsp;  </dt>
                        <dd style = {{ marginLeft: "15px", display: "inline" }} > Aditya Music </dd>

                        <br />

                        <dt style = {{ borderRadius: "3px", display: "inline", backgroundColor: "#43A19E" }} > &nbsp; &nbsp;  </dt>
                        <dd style = {{ marginLeft: "15px", display: "inline" }} > Brad Desk </dd>

                        <br />

                        <dt style = {{ borderRadius: "3px", display: "inline", backgroundColor: "#FF9824" }} > &nbsp; &nbsp;  </dt>
                        <dd style = {{ marginLeft: "15px", display: "inline" }} > Lunch Mark </dd>

                        <br />

                        <dt style = {{ borderRadius: "3px", display: "inline", backgroundColor: "#F2317A" }} > &nbsp; &nbsp;  </dt>
                        <dd style = {{ marginLeft: "15px", display: "inline" }} > Day Off </dd>
                    </dl>
              
                </Flexbox>
			</Dialog>
		);
	}
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
  }
}


/**
 * Connects the Announcements Dialog component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(VotingModal);
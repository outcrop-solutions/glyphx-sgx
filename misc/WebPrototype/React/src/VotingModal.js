import React from 'react';
import { connect } from 'react-redux';
import { RadioButton, RadioButtonGroup } from 'material-ui/RadioButton';
import Flexbox from 'flexbox-react';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import PieChart from './PieChart.js';
import RaisedButton from 'material-ui/RaisedButton';
import Snackbar from 'material-ui/Snackbar';


/**
 * -ADCMT
 */
class VotingModal extends React.Component {

    state = {
        snackbarVisible: false,
        voted: false,
        value: null,
        voteCount: [21, 45, 10, 15]
    }


    /**
	 * React built-in which is called when component mounts.
	 */
    componentDidMount() {
        if (!this.props.votingClosed) {
            // Check if user voted and set state accordingly

        }

        // Get vote count and set state accordingly
    }


    componentWillReceiveProps(nextProps) {
        if (nextProps.votingClosed) {
            this.setState({ voted: true })
        }
    }


    /**
	 * -ADCMT
	 */
    onSubmit() {
        if (this.state.value === null) {
            this.setState({ snackbarVisible: true });
        }

        else {
            // Send this.state.value to the backend
            // Increment vote of voteCount[this.state.value] in the backend and frontend

            // Update voted status of the user through the backend

            var voteCount = this.state.voteCount;
            voteCount[this.state.value] += 1;

            this.setState({ voted: true, voteCount: voteCount });

        }
    }

	
	render() {
        var context = this;

        var options = this.props.poll.options.map( 
            function(option) {
                return (
                    <RadioButton
                        key = { option }
                        value = { context.props.poll.options.indexOf(option) }
                        label = { option }
                        style = {{ marginBottom: "10px", marginTop: "14px" }}
                        iconStyle = {{ fill: context.props.settings.colors.elasticColor.checkBox }}
                    />
                )
            }
        );

        var shortOptions = this.props.poll.shortOptions.map( 
            function(option) {
                return (
                    <div key = { option } >
                        <dt style = {{ borderRadius: "3px", display: "inline", backgroundColor: context.props.poll.colors[context.props.poll.shortOptions.indexOf(option)] }} > &nbsp; &nbsp;  </dt>
                        <dd style = {{ marginLeft: "15px", display: "inline" }} > {option} </dd>
                    </div>
                )
            }
        );


		return(
			<Dialog
				title = { this.props.votingClosed ? "Voting is now closed" : "Voting is still open" }
                modal = { true }
				open = { this.props.display }
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
			>
                <div style = {{ display: (this.state.voted ? "none" : "") }} >
                    <RadioButtonGroup name = "votingGroup" onChange = { (event, value) => this.setState({ value : value }) } >
                        {options}
                    </RadioButtonGroup>


                    <RaisedButton 
                        label = "Submit"
                        style = {{ width: "100%", margin: "10px auto" }}
                        buttonStyle = {{
                            height: '35px',
                            lineHeight: '35px',
                            backgroundColor: this.props.settings.colors.buttons.general  
                        }} 
                        labelStyle = {{
                            fontSize: '14px',
                            color: this.props.settings.colors.overviewButtonsColor.text,
                            margin: "0px 0px 0px -3px",
                            paddingLeft: "0px",
                            paddingRight: "0px"
                        }}
                        overlayStyle = {{ height: '35px', lineHeight: '35px' }}
                        onClick = { () => this.onSubmit() }
                        primary = { true } 
                    />

                </div>


                {this.state.voted ? 
                    <Flexbox flexDirection = "row" style = {{ marginLeft: "105px" }} >
                        <PieChart
                            data = { this.state.voteCount }
                            radius = { 150 }
                            hole = { 50 }
                            colors = { this.props.poll.colors }
                            labels = { true }
                            percent = { true }
                            strokeWidth = { 3 }
                            stroke = { '#fff' }
                        />
        
                        <dl style = {{ width: "200px", background: "#fff", padding: "5px 15px", marginLeft: "10px" }} >
                            {shortOptions}
                        </dl>
                    </Flexbox>
                    :
                    null
                }

                <Snackbar
                    open = { this.state.snackbarVisible }
                    message = "Please select an option to continue."
                    autoHideDuration = { 2000 }
                    onRequestClose = { () => this.setState({ snackbarVisible: false }) }
                />
			</Dialog>
		);
	}
}


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state) {
  return {
    settings: state.filterState.Settings,
  }
}


/**
 * Connects the redux store to get access to global states.
 **/
export default connect(mapStateToProps)(VotingModal);
import React from 'react';
import { connect } from 'react-redux';
import { Step, Stepper, StepLabel } from 'material-ui/Stepper';
import { Card, CardText } from 'material-ui/Card';
import RaisedButton from 'material-ui/RaisedButton';
import MenuItem from 'material-ui/MenuItem';
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import DropDownMenu from 'material-ui/DropDownMenu';
import Collapsible from 'react-collapsible';
import ExpandTransition from 'material-ui/internal/ExpandTransition';
import FontIcon from 'material-ui/FontIcon';
import './MyViews.css';
import './General.css';


class MyViews extends React.Component {

    render(){
        return(
            <div style = {{ padding: "10px" }} >

                <div style = {{ marginBottom: "3px" }} >
                    <Collapsible
                        transitionTime = {200} 
                        trigger = "Shared Views"
                    >
                        <Card containerStyle = {{ padding: "0px", backgroundColor: "#e6e6e6" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                            >
                                HI<br />Shared<br />Views
                            </CardText>
                        </Card>
                    </Collapsible>
                </div>

                <div style = {{ marginBottom: "3px" }} >
                    <Collapsible
                        transitionTime = {200} 
                        trigger = "Custom Views"
                    >
                        <Card containerStyle = {{ padding: "0px", backgroundColor: "#e6e6e6" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                            >
                                HI<br />Custom<br />Views
                            </CardText>
                        </Card>
                    </Collapsible>
                </div>

                <div style = {{ marginBottom: "3px" }} >
                    <Collapsible
                        transitionTime = {200} 
                        trigger = "Saved Views"
                    >
                        <Card containerStyle = {{ padding: "0px", backgroundColor: "#e6e6e6" }} >
                            <CardText
                                style = {{
                                    padding: "7px",
                                }}
                            >
                                HI<br />Saved<br />Views
                            </CardText>
                        </Card>
                    </Collapsible>
                </div>
            </div>       
        );
    }
};


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
 * Connects the TopNav component to the redux store
 **/
export default connect(mapStateToProps)(MyViews);
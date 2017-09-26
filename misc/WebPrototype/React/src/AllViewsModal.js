import React from 'react';
import { connect } from 'react-redux';
import Dialog from 'material-ui/Dialog';
import Flexbox from 'flexbox-react';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';
import { httpGetRequest } from './ServerCallHelper.js';
import './General.css';



class allViewsModal extends React.Component {

	state = {
		selectionList: [],
		dragState: 0,
		data: []
	}
	
	componentDidMount() {
		window.addEventListener('mouseup', this.handleMouseUp.bind(this));

		var context = this;

		httpGetRequest("http://ec2-35-162-196-131.us-west-2.compute.amazonaws.com:5000/frontEndFilterData/WGSData", 
			function(responseText) { 
				var preData = JSON.parse(responseText); 
				var data = [];
				var keyArray = Object.keys(preData);
				for (var i = 0; i < keyArray.length; i++) {
					var dataCol = [keyArray[i]];
					for (var j = 0; j < preData[keyArray[i]].length; j++) {
						dataCol.push(preData[keyArray[i]][j][keyArray[i]]);
					}
					data.push(dataCol);
				}
				context.setState({ data: data });
			},
		"options");
	}

	componentWillUnmount() {
		window.removeEventListener('mouseup', this.handleMouseUp.bind(this));
	}

	handleMouseUp() {
		if (this.props.allViewsDisplay) {
			this.setState({ dragState: 0 });
		}
	}

	handleBackClick = () => {
		this.props.dispatch(editModalDisplay(false));
	}

	selectDeselectCol(col, action) {
		var sList = this.state.selectionList.slice();

		for (var i = 1; i < col.length; i++) {
			var index = this.checkSelected([col[0], col[i]], sList);
			if (action === "select") {
				if (index === false) {
					sList.push([col[0], col[i]]);
				}
			}
			else {
				if (index !== false) {
					sList.splice(index, 1);
				}
			}
		}
		this.setState({ selectionList: sList });
	}

	selectDesectAll(data, action) {
		if (action === "deselect") {
			this.setState({ selectionList: [] });
		}
		else {
			var sList = [];
			for (var i = 0; i < data.length; i++) {
				for (var j = 1; j < data[i].length; j++) {
					sList.push([data[i][0], data[i][j]]);
				}
			}
			this.setState({ selectionList: sList });
		}
	}

	toggleSelection(selection) {
		var sList = this.state.selectionList.slice();
		var index = this.checkSelected(selection, sList);
		if (index !== false) {
			if (this.state.dragState === 0) {
				this.setState({ dragState: false });
				sList.splice(index, 1);
			}
		}
		else {
			if (this.state.dragState === 0) {
				this.setState({ dragState: true });
				sList.push(selection);
			}
		}
		this.setState({ selectionList: sList });
	}

	toggleDragSelection(selection) {
		var sList = this.state.selectionList.slice();
		var index = this.checkSelected(selection, sList);
		if (index !== false) {
			if (this.state.dragState === false) {
				sList.splice(index, 1);
			}
		}
		else {
			if (this.state.dragState === true) {
				sList.push(selection);
			}
		}

		this.setState({ selectionList: sList });
	}

	checkSelected(array, sList) {
		for (var i = 0; i < sList.length; i++) {
			if (sList[i][0] === array[0] && sList[i][1] === array[1]) {
				return i;
			}
		}
		return false;
	}
	
	render() {


		var data = this.state.data;
		var context = this;

		var displayData = this.state.data.map( function(col) {
			return (
				<Flexbox 
					flexDirection = "column" 
					style = {{ 
							width: "100%", 
							border: "1px solid", 
							borderLeft: (col === data[0] ? "1px solid" : "none"),
							borderBottomLeftRadius: (col === data[0] ? "3px" : ""),
							borderTopLeftRadius: (col === data[0] ? "3px" : ""),
							borderBottomRightRadius: (col === data[data.length - 1] ? "3px" : ""),
							borderTopRightRadius: (col === data[data.length - 1] ? "3px" : "")
					}} 
					key = { col } 
				>
					<div 
						className = "noselect" 
						style = {{ 
							backgroundColor: "#42459c", 
							color: "#ffffff", 
							height: "30px",
    						fontSize: "21px",
							textAlign: "center"
						}} 
					> 
						<div style = {{ textAlign: "left", marginTop: "2px", marginLeft: "5px", fontSize: "18px" }} > 
							<i className = "fa fa-check" style = {{ marginRight: "3px" }} onClick = { () => context.selectDeselectCol(col, "select") } /> 
							<i className = "fa fa-times" onClick = { () => context.selectDeselectCol(col, "deselect") } /> 
						</div> 
						<div style = {{ marginTop: "-16px", paddingBottom: "4px" }} > {col[0]} </div>
					</div>

					<div style = {{ overflow: "auto" }} >
						{col.map( function(elem) {
							return (
								(elem !== col[0] ? 
									<div
										className = "noselect lightHover"
										style = {{ 
											backgroundColor: ( context.checkSelected([col[0], elem], context.state.selectionList) !== false ? "#7c78a0" : "white" ), 
											textAlign: "center",
											padding: "2px",
										}} 
										key = {elem}
										onMouseDown = { () => context.toggleSelection([col[0], elem]) }
										onMouseEnter = { (e) => (e.buttons > 0 ? context.toggleDragSelection([col[0], elem]) : null ) }
									> 
										{elem} 
									</div> 
								: "")
							)
						})}
					</div>
				</Flexbox>
			)
		});

		return(
			<Dialog
				title = { this.props.type }
				contentStyle = {{ width: "90%", maxWidth: "none", backgroundColor: "#c5c5f7" }}
				bodyStyle = {{ backgroundColor: "#c5c5f7" }}
				actionsContainerStyle = {{ backgroundColor: "#c5c5f7" }}
				titleStyle = {{ backgroundColor: this.props.settings.colors.collapsibleColor.mainCollapsed, color: "#ffffff", fontSize: "36px", fontWeight: "bold", lineHeight: "16px" }}
				actions = {
					[
						<FlatButton
							label = "Back"
							primary = { true }
							onClick = { this.handleBackClick }
							style = {{ color: this.props.settings.colors.settingsModalColor.cancelButton }}
						/>,
						<RaisedButton 
							label = { "Launch" }
							style = {{
								width: "112px",
								margin: "0px 10px 9px 0px"
							}}
							buttonStyle = {{
								height: '35px',
								lineHeight: '35px',
								backgroundColor: (this.state.selectionList.length === 0 ? "grey" : this.props.settings.colors.homePageColors.myViewsButton) 
							}} 
							labelStyle = {{
								fontSize: '12px',
								textAlign: "center",
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
							disabled = { (this.state.selectionList.length === 0 ? true : false)  }
							onClick = { () => console.log("goon") }
							primary = {true } 
						/>
					]
				}
				modal = { true }
				open = { this.props.allViewsDisplay }
			>
				<div style = {{ height: "60vh", paddingBottom: "30px" }} >
					<RaisedButton 
						label = { <span> <i className = "fa fa-check" style = {{ fontSize: "22px", margin: "1px 0px 0px" }} /> Select All </span> }
						style = {{
							width: "121px",
							margin: "8px 10px 9px 0px"
						}}
						buttonStyle = {{
							height: '35px',
							lineHeight: '35px',
							backgroundColor: this.props.settings.colors.homePageColors.myViewsButton
						}} 
						labelStyle = {{
							fontSize: '12px',
							textAlign: "center",
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
						onClick = { () => this.selectDesectAll(data, "select") }
						primary = {true } 
					/>

					<RaisedButton 
						label = { <span> <i className = "fa fa-times" style = {{ fontSize: "22px", margin: "1px 0px 0px" }} /> Deselect All </span> }
						style = {{
							width: "135px"
						}}
						buttonStyle = {{
							height: '35px',
							lineHeight: '35px',
							backgroundColor: this.props.settings.colors.homePageColors.myViewsButton
						}} 
						labelStyle = {{
							fontSize: '12px',
							textAlign: "center",
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
						onClick = { () => this.selectDesectAll(data, "deselect") }
						primary = {true } 
					/>

					<Flexbox flexDirection = "row" style = {{ backgroundColor: "#ffffff", height: "100%" }} >
						{displayData}
					</Flexbox>
				</div>
			</Dialog>
		);
	}
}

/**
 * Constants defined to make dispatching for the redux store consistent
 **/
export const editModalDisplay = (allViewsModal) => ({
    type: 'EDIT_MODAL_DISPLAY',
    allViewsModal,
});


/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
const mapStateToProps = function(state){
  return {
    settings: state.filterState.Settings,
	allViewsDisplay: state.filterState.ModalDisplay.allViewsModal,
  }
}


/**
 * Connects the Announcements Dialog component to the redux store
 **/
export default connect(mapStateToProps,null,null,{withRef:true})(allViewsModal);
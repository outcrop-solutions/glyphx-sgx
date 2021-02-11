import React, { useState, useEffect } from 'react';
//import { useHistory } from "react-router-dom";
import { makeStyles, MuiThemeProvider } from '@material-ui/core/styles';
//import Grid from '@material-ui/core/Grid';
import FilterSideBar from '../components/FilterSideBar.js';
//import FilterPane from '../images/filterpane.png';
//import VizPane from '../images/vizpane.png';
//import "./Visualization.css";
  
const useStyles = makeStyles((theme) => ({
  root: {
    backgroundColor: theme.palette.background.paper,
    width: '100%',
    height: window.innerHeight-60,
  },
}));

const testData = [
  {'name': 'ndID', 'data': [{'value': 2017, 'count': 200}, {'value': 2018, 'count': 250}, {'value': 2019, 'count': 300}]},
  {'name': 'Gender', 'data': [{'value': 'M', 'count': 350}, {'value': 'F', 'count': 400}]},
  {'name': 'OneTest', 'data': [{'value': 1300, 'count': 150}, {'value': 1400, 'count': 200}, {'value': 1500, 'count': 300}, {'value': 1600, 'count': 100}]}
];

export default function Visualization() {
    //const history = useHistory();
    const classes = useStyles();
     
    return (
      <div style = {{ width: '100%', height: '64px' }}>
        <div id  = "filterNav" className = "sidenav" style = {{ height: "64px", overflow: "hidden" }} >
            <FilterSideBar data={testData} />
        </div>
      </div>
    );
}
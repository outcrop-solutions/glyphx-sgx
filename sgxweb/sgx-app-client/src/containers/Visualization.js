import React from "react";
//import { useHistory } from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import Grid from '@material-ui/core/Grid';
import FilterPane from '../images/filterpane.png';
import VizPane from '../images/vizpane.png';
//import "./Visualization.css";
  
  const useStyles = makeStyles((theme) => ({
    root: {
      backgroundColor: theme.palette.background.paper,
      width: '100%',
      height: window.innerHeight-60,
    },
    filter: {
        maxWidth: '100%',
        height: window.innerHeight-60,
    },
    viz: {
        maxWidth: '100%',
        height: window.innerHeight-60,
    },
    gridroot: {
        height: window.innerHeight-60,
        backgroundColor: 'black',
    },
    gridrootviz: {
        height: window.innerHeight-60,
        textAlign: 'center',
        backgroundColor: 'black',
    }
  }));

export default function Visualization() {
    //const history = useHistory();
    const classes = useStyles();
    
  return (
    <div className={classes.root}>
        <Grid container spacing={0} classes={{root: classes.gridroot}}>
            <Grid item xs={3} className={classes.gridroot}>
                <img src={FilterPane} alt="FilterPane" className={classes.filter}/>
            </Grid>
            <Grid item xs={9} className={classes.gridrootviz}>
                <img src={VizPane} alt="VizPane" className={classes.viz}/>
            </Grid>
        </Grid>
    </div>
  );
}
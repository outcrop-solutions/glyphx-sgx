import React, { /*useState, useEffect*/ } from "react";
//import { useHistory } from "react-router-dom";
//import { Storage } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
//import { Auth, API } from "aws-amplify";
import Grid from '@material-ui/core/Grid';
import Divider from '@material-ui/core/Divider';
import IconButton from '@material-ui/core/IconButton';
import AppsOutlinedIcon from '@material-ui/icons/AppsOutlined';
import Paper from '@material-ui/core/Paper';

const useStyles = makeStyles((theme) => ({
    divider: {
        backgroundColor: 'rgb(57,63,79)',
    },
    container: {
        width: '95%',
        margin: 'auto',
        marginTop: 50,
    },
}));

export default function SharedFrame() {
    const classes = useStyles();
    //const history = useHistory();
  
    return (
        <div className={classes.container}>
            <Grid container spacing={0}>
                <Grid item xs={6}>
                    <h1 style={{color: 'white', textAlign: 'left', fontWeight: 'normal'}}>SHARED WITH ME</h1>
                </Grid>
                <Grid item xs={6} style={{textAlign: 'right'}}>
                    <IconButton>
                        <AppsOutlinedIcon style={{color: 'white', width: '1.4em', height: '1.4em', marginTop: 15 }}/>
                    </IconButton>
                </Grid>
            </Grid>
            <Divider className={classes.divider}/>
            <Paper elevation={0} style={{ height: window.innerHeight-212, backgroundColor: 'rgb(13, 19, 33)', overflowX: 'hidden', overflowY:'auto'}}>
               
            </Paper>
        </div>
    );
}

import React from "react";
//import { useHistory } from "react-router-dom";
import { Storage } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
import { Auth } from "aws-amplify";
import Paper from '@material-ui/core/Paper';
import IconButton from '@material-ui/core/IconButton';
import Grid from '@material-ui/core/Grid';
import Table from '@material-ui/core/Table';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableRow from '@material-ui/core/TableRow';
import TableHead from '@material-ui/core/TableHead';
import AddIcon from '@material-ui/icons/Add';
import RecentViews from "../components/RecentViews";
import LoadData from "../components/LoadData";
import Announcements from "../components/Announcements";
import Education from "../components/Education";
import "./Home.css";

const useStyles = makeStyles((theme) => ({
    root: {
      flexGrow: 1,
    },
    paper: {
      textAlign: 'center',
      color: theme.palette.text.secondary,
      height: theme.spacing(50),
    },
    paper_row_two: {
      textAlign: 'center',
      color: theme.palette.text.secondary,
      //height: theme.spacing(3),
      height: window.innerHeight*0.37,
    },
    circle_button: {
        fontSize: '0.875rem', 
        position: 'absolute', 
        bottom: 12, 
        right: 12, 
        backgroundColor: 'rgb(63,81,181)',
        color: 'white',
        '&:hover': {
            backgroundColor: 'rgb(45, 48, 145)',
         },
    },
}));

export default function Home() {
    const classes = useStyles();
    
    async function handleFileChange(event) {
        var name = document.getElementById('icon-button-file'); 
        var file = name.files.item(0);

        if(file.name != null){
            const stored = await Storage.vault.put(file.name, file, {
                contentType: file.type,
            });
            console.log(stored.key);
        }
    }

    async function getDataSources() {
        try {
            let cred = await Auth.currentCredentials();
            return cred.identityId;
        }
        catch(e){
            return 0;
        }
    }

  return (
    <div className="Home">
        <div className={classes.root}>
            <Grid container spacing={3}>
                <Grid item xs={6}>
                    <LoadData data={getDataSources()}/>
                    <div style={{ position: 'relative' }}>
                        <input accept=".csv" id="icon-button-file" type="file" onChange={handleFileChange} hidden />
                        <label htmlFor="icon-button-file">
                            <IconButton color="default" className={classes.circle_button} component="span">
                                <AddIcon />
                            </IconButton>
                        </label>
                    </div>
                </Grid>
                <Grid item xs={6}>
                <RecentViews />
                <div style={{ position: 'relative' }}>
                    <IconButton onClick={() => { }} color="default" className={classes.circle_button}>
                        <AddIcon/>
                    </IconButton>
                </div>
                </Grid>
                <Grid item xs={6}>
                <Paper className={classes.paper_row_two} elevation={3}>
                    <TableContainer component={Paper} elevation={0}>
                        <Table className={classes.table} aria-label="custom pagination table">
                            <TableHead>
                            <TableRow>
                                <TableCell align="center" colSpan={4} style={{ fontSize: '1.25rem', padding: 10}}>
                                ANNOUNCEMENTS
                                </TableCell>
                            </TableRow>
                            </TableHead>
                        </Table>
                    </TableContainer>
                    <Announcements />
                </Paper>
                </Grid>
                <Grid item xs={6}>
                <Paper className={classes.paper_row_two} elevation={3}>
                    <Education />
                </Paper>
                </Grid>
            </Grid>
        </div>
    </div>
  );
}

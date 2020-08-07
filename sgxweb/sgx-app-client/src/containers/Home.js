import React from "react";
import { useHistory } from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
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
import "./Home.css";

const useStyles = makeStyles((theme) => ({
    root: {
      flexGrow: 1,
    },
    paper: {
      textAlign: 'center',
      color: theme.palette.text.secondary,
      height: theme.spacing(42),
    },
    paper_row_two: {
      textAlign: 'center',
      color: theme.palette.text.secondary,
      height: theme.spacing(30),
    },
    circle_button: {
        fontSize: '1.25rem', 
        position: 'absolute', 
        bottom: 30, 
        right: 30, 
        backgroundColor: 'rgb(63,81,181)',
        color: 'white',
        '&:hover': {
            backgroundColor: 'rgb(45, 48, 145)',
         },
    },
}));

export default function Home() {
    const classes = useStyles();
    const history = useHistory();

  return (
    <div className="Home">
        <div className={classes.root}>
            <Grid container spacing={3}>
                <Grid item xs={6}>
                    <LoadData />
                    <div style={{ position: 'relative' }}>
                        <IconButton onClick={() => { history.push("/mapper"); }} color="default" className={classes.circle_button}>
                            <AddIcon/>
                        </IconButton>
                    </div>
                </Grid>
                <Grid item xs={6}>
                <RecentViews />
                <div style={{ position: 'relative' }}>
                    <IconButton onClick={() => { history.push("/mapper"); }} color="default" className={classes.circle_button}>
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
                                <TableCell align="center" colSpan={4} style={{ fontSize: '2rem'}}>
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
                    <TableContainer component={Paper} elevation={0}>
                        <Table className={classes.table} aria-label="custom pagination table">
                            <TableHead>
                            <TableRow>
                                <TableCell align="center" colSpan={4} style={{ fontSize: '2rem'}}>
                                EDUCATION
                                </TableCell>
                            </TableRow>
                            </TableHead>
                        </Table>
                    </TableContainer>
                </Paper>
                </Grid>
            </Grid>
        </div>
    </div>
  );
}

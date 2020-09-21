import React from "react";
import { useHistory } from "react-router-dom";
import { Storage } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
import { Auth, API } from "aws-amplify";
import Paper from '@material-ui/core/Paper';
import IconButton from '@material-ui/core/IconButton';
import Grid from '@material-ui/core/Grid';
import Table from '@material-ui/core/Table';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableRow from '@material-ui/core/TableRow';
import TableHead from '@material-ui/core/TableHead';
import AddIcon from '@material-ui/icons/Add';
import Modal from '@material-ui/core/Modal';
import RecentViews from "../components/RecentViews";
import LoadData from "../components/LoadData";
import Announcements from "../components/Announcements";
import Education from "../components/Education";
//import CircularStatic from "../components/CircularStatic";
import CircularProgress from '@material-ui/core/CircularProgress';
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
    modal: {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
    },
    modal_paper: {
        backgroundColor: theme.palette.background.paper,
        border: '2px solid #000',
        boxShadow: theme.shadows[5],
        padding: theme.spacing(2, 4, 3),
    },
}));

export default function Home() {
    const classes = useStyles();
    const history = useHistory();
    const [open, setOpen] = React.useState(false);
    
    async function handleFileChange(event) {
        var name = document.getElementById('icon-button-file'); 
        var file = name.files.item(0);
        var directory = file.name.split(".csv")[0];
        var filename = directory+ "/" + file.name;

        if(file.name != null){
            setOpen(true);
            const stored = await Storage.vault.put(filename, file, {
                contentType: file.type,
            });
            console.log(stored.key);
            
            let identity = await getIdentityId();
            //let crawlerId = identity+"_"+directory;
            /*let cgc_output = await createGlueCrawler(identity, directory);
            console.log("createGlueCrawler", cgc_output);
            let sgc_output = await startGlueCrawler(crawlerId);
            console.log("startGlueCrawler", sgc_output);*/
            let cta_output = await addCSVtoAthena(identity, directory);
            let queryId = cta_output.body.replaceAll('"','');
            console.log("addCSVtoAthena", queryId);
            let timer = setInterval(async function(){
                /*let ctl_output = await getCrawlerTimeLeft(crawlerId);
                let body = JSON.parse(ctl_output["body"]);
                let tablesCreated = body.split(":")[2].split("}")[0];
                //console.log("getCrawlerTimerLeft", tablesCreated);
                if(tablesCreated == 1){
                    clearInterval(timer);
                    let cmt_output = await createMetadataTable(identity, directory);
                    console.log("createMetadataTable", cmt_output);
                    setOpen(false);
                    history.push({pathname:"/mapper", data: file.name});
                }*/
                let gqs_output = await getQueryStatus(queryId);
                let status = gqs_output.body.replaceAll('"','');
                console.log(status);
                if(status == "SUCCEEDED"){
                    clearInterval(timer);
                    let cmt_output = await createMetadataTable(identity, directory);
                    console.log("createMetadataTable", cmt_output);
                    setOpen(false);
                    history.push({pathname:"/mapper", data: file.name});
                }
            }, 2500);
        }
    }
/*
    function createGlueCrawler(identity, directory) {
        return API.post("sgx", "/create-glue-crawler", {
          body: "{\"identity\":\""+identity+"\", \"directory\":\""+directory+"\"}"
        });
    }

    function startGlueCrawler(identity) {
        return API.post("sgx", "/start-glue-crawler", {
          body: "{\"identity\":\""+identity+"\"}"
        });
    }

    function getCrawlerTimeLeft(identity) {
        return API.post("sgx", "/get-crawler-time-left", {
          body: "{\"identity\":\""+identity+"\"}"
        });
    }
*/
    function createMetadataTable(identity, directory) {
        return API.post("sgx", "/create-metadata-table", {
          body: "{\"identity\":\""+identity+"\", \"directory\":\""+directory+"\"}"
        });
    }

    function addCSVtoAthena(identity, directory) {
        return API.post("sgx", "/csv-to-athena", {
          body: "{\"identity\":\""+identity+"\", \"directory\":\""+directory+"\"}"
        });
    }

    function getQueryStatus(queryId) {
        return API.post("sgx", "/get-query-status", {
          body: "{\"queryId\":\""+queryId+"\"}"
        });
    }

    async function getIdentityId() {
        try {
            let cred = await Auth.currentCredentials();
            return cred.identityId;
        }
        catch(e){
            return 0;
        }
    }

    const body = (
        <div className={classes.modal_paper}>
            <h2 id="simple-modal-title">Uploading your data</h2>
            <div style={{textAlign:"center", padding: 20}}>
                <CircularProgress />
            </div>
        </div>
    );

  return (
    <div className="Home">
        <div className={classes.root}>
            <Grid container spacing={3}>
                <Grid item xs={6}>
                    <LoadData data={getIdentityId()}/>
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
        <Modal
            open={open}
            aria-labelledby="simple-modal-title"
            aria-describedby="simple-modal-description"
            className={classes.modal}
        >
            {body}
        </Modal>
    </div>
  );
}

import React, { useEffect } from "react";
import { useHistory } from "react-router-dom";
import { Storage } from "aws-amplify";
import PropTypes from 'prop-types';
import SwipeableViews from 'react-swipeable-views';
import { makeStyles, useTheme } from '@material-ui/core/styles';
import { useFormFields } from "../libs/hooksLib";
import AppBar from '@material-ui/core/AppBar';
import Tabs from '@material-ui/core/Tabs';
import Tab from '@material-ui/core/Tab';
import Box from '@material-ui/core/Box';
import Button from '@material-ui/core/Button';
import Modal from '@material-ui/core/Modal';
import Grid from '@material-ui/core/Grid';
import SaveIcon from '@material-ui/icons/Save';
import SaveOutlinedIcon from '@material-ui/icons/SaveOutlined';
import FolderOpenIcon from '@material-ui/icons/FolderOpen';
import CreateNewFolderIcon from '@material-ui/icons/CreateNewFolder';
import EditIcon from '@material-ui/icons/Edit';
import PlayArrowIcon from '@material-ui/icons/PlayArrow';
import TextField from '@material-ui/core/TextField';
import MapperProperties from './MapperProperties';
import MapperData from './MapperData';
import "./Mapper.css";

function TabPanel(props) {
    const { children, value, index, ...other } = props;
  
    return (
      <div
        role="tabpanel"
        hidden={value !== index}
        id={`full-width-tabpanel-${index}`}
        aria-labelledby={`full-width-tab-${index}`}
        {...other}
      >
        {value === index && (
          <Box p={index === 0 ? 0 : 3} height='100%'>
            {children}
          </Box>
        )}
      </div>
    );
  }
  
  TabPanel.propTypes = {
    children: PropTypes.node,
    index: PropTypes.any.isRequired,
    value: PropTypes.any.isRequired,
  };
  
  function a11yProps(index) {
    return {
      id: `full-width-tab-${index}`,
      'aria-controls': `full-width-tabpanel-${index}`,
    };
  }

  function createData(fieldname, type, min, max, countnn, countd) {
    return { fieldname, type, min, max, countnn, countd };
  }

  const useStyles = makeStyles((theme) => ({
    root: {
      backgroundColor: theme.palette.background.paper,
      width: '100%',
      height: window.innerHeight-60,
    },
    tab: {
      minWidth: '8.33%',
    },
    tabPanel: {
      //paddingTop: '8px',

    },
    button: {
        margin: theme.spacing(1),
        color: '#3f51b5',
    },
    view: {
        height: window.innerHeight-115,
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
    submit: {
      margin: theme.spacing(3, 0, 2),
    },
  }));

export default function Mapper() {
    const history = useHistory();
    const classes = useStyles();
    const theme = useTheme();
    const [value, setValue] = React.useState(0);
    const [rows, setRows] = React.useState([])
    const [open, setOpen] = React.useState(false);
    const [saveOpen, setSaveOpen] = React.useState(false);
    const [title, setTitle] = React.useState("");
    //const [subset, setSubset] = React.useState(null);
    const [id, setId] = React.useState(null);
    const [tablename, setTablename] = React.useState(null);
    const [identity, setIdentity] = React.useState(null);
    const [fields, setFields] = React.useState(null);
    const [formfields, handleFieldChange] = useFormFields({
      filename: ""
    });

    useEffect(() => {

      if(typeof history.location.data !== "undefined"){

        console.log(history.location.data);
        setId(history.location.data.timestamp);
        let prefix = null;
        if(history.location.data.name == null){
          prefix = history.location.data.contents.name;
        }
        else{
          prefix = history.location.data.name.split(".parquet")[0];
        }

        var filename = "metadata/" + prefix + ".json";
        console.log(filename);
        Storage.vault.get(filename, { download: true })
          .then(result => {
            result.Body.text().then(contents => {
              let fields = JSON.parse(contents);
              let temp_rows = [];
              for(let x in fields){
                let row = fields[x];
                //if(row["type"] !== "string"){
                  temp_rows.push(createData(row["fieldname"], row["type"], row["min"], row["max"], row["count"], row["distinct"]));
                //}
              }
              setRows(temp_rows);
              //console.log(temp_rows);
            });
          })
          .catch(err => {
              console.log('error axios');
              console.log(err)
          });

          setTablename(prefix.toLowerCase().split(" ").join("_"));
          setIdentity(history.location.data.identity);
          //let tablename = prefix.toLowerCase().split(" ").join("_");
          //let identity = history.location.data.identity;
          //let query = "SELECT * FROM "+tablename+" LIMIT 500";
          //let query = "SELECT * FROM "+tablename+" ORDER BY RAND() LIMIT 100";
          //fetchSubset(identity, query);

      }else{
        history.push("/");
      }

    }, []);
/*
    async function fetchSubset(identity, query){
      await getQueryResults(identity, query).then(result => {
        setSubset(result['body']);
      });
    }
*/
    const handleChange = (event, newValue) => {
        setValue(newValue);
    };

    const handleChangeIndex = (index) => {
        setValue(index);
    };

    const handleOpen = () => {
      setTitle("Open");
      setOpen(true);
    };

    const handleNew = () => {
      setTitle("New");
      setOpen(true);
    };

    const handleSave = () => {
      setTitle("Save");
      setSaveOpen(true);
      //saveState();
    };

    const handleSaveAs = () => {
      setTitle("Save As");
      setSaveOpen(true);
    };

    const handleEdit = () => {
      setTitle("Edit");
      setOpen(true);
    };
  
    const handleClose = () => {
      setOpen(false);
    };

    const handleSaveClose = () => {
      setSaveOpen(false);
    };

    async function handleRun() {
      history.push("/visualization");
    }
/*
    function getQueryResults(identityId, query) {
      return API.post("sgx", "/get-query-results", {
        body: "{\"identity\":\""+identityId+"\", \"query\":\""+query+"\"}"
      });
    }
*/
    const saveState = (name) => {
      let data = {
        name: history.location.data.name.split(".csv")[0],
        properties: {
          xaxis: {
            fieldname: fields['xAxis'],
            props: {}
          },
          yaxis: {
            fieldname: fields['yAxis'],
            props: {}
          },
          zaxis: {
            fieldname: fields['zAxis'],
            props: {}
          },
          gcolor: {
            fieldname: fields['gColor'],
            props: {}
          },
          gsize: {
            fieldname: fields['gSize'],
            props: {}
          },
          gtype: {
            fieldname: fields['gType'],
            props: {}
          }
        }
      };
      console.log(  
        JSON.stringify(data, null, 2)
      );

      let filename = "Saved/"+name+".json";

      const stored = Storage.vault.put(filename, JSON.stringify(data, null, 2), {
        contentType: "json",
      });
      //console.log("key:", stored.key);

    };

    const getData = (val) => {
      setFields(val);
    };

    async function handleSubmit(event) {
      event.preventDefault();
      //setIsLoading(true);
    
      saveState(formfields.filename);
      //console.log(formfields.filename);
      setSaveOpen(false);
    }

    const body = (
      <div className={classes.modal_paper}>
          <h2 id="simple-modal-title">{title}</h2>
      </div>
    );

    const saveas = (
      <div className={classes.modal_paper}>
        <h2 id="simple-modal-title">{title}</h2>
        <form className={classes.form} onSubmit={handleSubmit} noValidate>
          <TextField 
            required 
            id="filename" 
            label="File name" 
            variant="outlined" 
            value={formfields.filename} 
            onChange={handleFieldChange}
          />
          <Grid container>
            <Grid item xs>
              <Button
                type="submit"
                variant="contained"
                color="primary"
                className={classes.submit}
              >
                Save As
              </Button>
            </Grid>
            <Grid item>
              <Button
                variant="contained"
                color="default"
                className={classes.submit}
                onClick={() => {setSaveOpen(false)}}
              >
                Cancel
              </Button>
            </Grid>
          </Grid>
        </form>
      </div>
    );
  
  return (
    <div className="Mapper">
        <div className={classes.root}>
            <AppBar position="static" color="default" style={{flexDirection: 'row'}}>
                <Tabs
                value={value}
                onChange={handleChange}
                indicatorColor="primary"
                textColor="primary"
                variant="fullWidth"
                aria-label="full width tabs example"
                style={{width: '25%'}}
                >
                <Tab className={classes.tab} label="Properties" {...a11yProps(0)} />
                <Tab className={classes.tab} label="Data" {...a11yProps(1)} />
                <Tab className={classes.tab} label="Advanced" {...a11yProps(2)} />
                </Tabs>
                <div style={{margin: 'auto'}}>
                    <Button color="default" className={classes.button} startIcon={<FolderOpenIcon />} onClick={handleOpen}>
                        Open
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<CreateNewFolderIcon />} onClick={handleNew}>
                        New
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<SaveOutlinedIcon />} onClick={handleSave}>
                        Save
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<SaveIcon />} onClick={handleSaveAs}>
                        Save As
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<EditIcon />} onClick={handleEdit}>
                        Edit
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<PlayArrowIcon />} onClick={handleRun}>
                        Run
                    </Button>
                </div>
            </AppBar>
            <SwipeableViews
                className={classes.view}
                axis={theme.direction === 'rtl' ? 'x-reverse' : 'x'}
                index={value}
                onChangeIndex={handleChangeIndex}
            >
                <TabPanel className={classes.tabPanel} value={value} index={0} dir={theme.direction}>
                  <MapperProperties data={rows} tablename={tablename} identity={identity} id={id} sendData={getData} hist={history.location.data}/>
                </TabPanel>
                <TabPanel value={value} index={1} dir={theme.direction}>
                  <MapperData data={rows}/>
                </TabPanel>
                <TabPanel value={value} index={2} dir={theme.direction}>
                  Advanced Pane
                </TabPanel>
            </SwipeableViews>
        </div>
        <Modal
            open={open}
            aria-labelledby="simple-modal-title"
            aria-describedby="simple-modal-description"
            className={classes.modal}
            onClose={handleClose}
        >
            {body}
        </Modal>
        <Modal
            open={saveOpen}
            aria-labelledby="simple-modal-title"
            aria-describedby="simple-modal-description"
            className={classes.modal}
            onClose={handleSaveClose}
        >
            {saveas}
        </Modal>
    </div>
  );
}
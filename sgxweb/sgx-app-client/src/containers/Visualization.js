import React, { useState, useEffect }  from 'react';
import { useHistory } from "react-router-dom";
import { API } from "aws-amplify";
import clsx from 'clsx';
import { makeStyles, useTheme } from '@material-ui/core/styles';
import Drawer from '@material-ui/core/Drawer';
import CssBaseline from '@material-ui/core/CssBaseline';
import Divider from '@material-ui/core/Divider';
import IconButton from '@material-ui/core/IconButton';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
import ChevronRightIcon from '@material-ui/icons/ChevronRight';

import Paper from '@material-ui/core/Paper';
import Accordion from '@material-ui/core/Accordion';
import AccordionSummary from '@material-ui/core/AccordionSummary';
import AccordionDetails from '@material-ui/core/AccordionDetails';
import Typography from '@material-ui/core/Typography';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';

import VisualizationWindow from '../containers/VisualizationWindow.js';
import VirtualizedTable from '../components/VirtualizedTable.js';
import CircularProgress from '@material-ui/core/CircularProgress';

const drawerWidth = 400;

const useStyles = makeStyles((theme) => ({
  root: {
    display: 'flex',
    height: '64px',
  },
  appBarShift: {
    width: `calc(100% - ${drawerWidth}px)`,
    transition: theme.transitions.create(['margin', 'width'], {
      easing: theme.transitions.easing.easeOut,
      duration: theme.transitions.duration.enteringScreen,
    }),
    marginRight: drawerWidth,
  },
  title: {
    flexGrow: 1,
  },
  hide: {
    display: 'none',
  },
  openButton: {
    position: 'absolute',
    top: 68,
    right: 8,
    height: 48,
    width: 48,
    fontSize: '0.875rem', 
    backgroundColor: 'rgb(63,81,181)',
    color: 'white',
    '&:hover': {
        backgroundColor: 'rgb(45, 48, 145)',
    },
  },
  drawer: {
    width: drawerWidth,
    flexShrink: 0,
  },
  drawerPaper: {
    width: drawerWidth,
    height: window.innerHeight-60,
    marginTop: 60,
  },
  drawerHeader: {
    display: 'flex',
    alignItems: 'center',
    padding: theme.spacing(0, 1),
    // necessary for content to be below app bar
    ...theme.mixins.toolbar,
    justifyContent: 'flex-start',
  },
  content: {
    flexGrow: 1,
    height: window.innerHeight-60,
    transition: theme.transitions.create('margin', {
      easing: theme.transitions.easing.sharp,
      duration: theme.transitions.duration.leavingScreen,
    }),
    marginRight: -drawerWidth,
  },
  contentShift: {
    transition: theme.transitions.create('margin', {
      easing: theme.transitions.easing.easeOut,
      duration: theme.transitions.duration.enteringScreen,
    }),
    marginRight: 0,
  },
  heading: {
    fontSize: theme.typography.pxToRem(15),
    fontWeight: theme.typography.fontWeightRegular,
  },
}));

export default function Visualization(props) {
  const history = useHistory();
  const classes = useStyles();
  const theme = useTheme();
  const [open, setOpen] = React.useState(true);
  const [filterData, setFilterData] = React.useState([]);
  const [hasRun, setHasRun] = useState(false);
  const [hasData, setHasData] = useState(false);
  const [name, setName] = useState(null);
  const [data, setData] = useState(null);
  const [metadata, setMetadata] = useState(null);
  const [id, setId] = useState(0);

  useEffect(() => {

    if(typeof history.location.data !== "undefined"){
      let data = history.location.data;
      let query = "SELECT * FROM " + data.tablename;
      if(!hasRun){
        initData(data.identity, query);
        setName(data.saveFile);
        setMetadata(data.rows);
        setId(data.id);
        setHasRun(true);
      }
    }
    else{
      history.push("/");
    }

  });

  const handleDrawerOpen = () => {
    setOpen(true);
  };

  const handleDrawerClose = () => {
    setOpen(false);
  };

  const getDistinct = (data) => {
    var a = data.reduce(function (acc, curr) {
      if (typeof acc[curr] == 'undefined') {
        acc[curr] = 1;
      } else {
        acc[curr] += 1;
      }
      return acc;
    }, {});
    
    return sort_object(a);
  };
  
  function sort_object(obj) {
    let items = Object.keys(obj).map(function(key) {
        return [key, obj[key]];
    });
    items.sort(function(first, second) {
        return second[1] - first[1];
    });
    let a = [];
    items.forEach(function(v) {
        a.push({'value': v[0], 'count': v[1]});
    })
    return a;
  } 
  
  async function allQueryResults(identity, query) {
    return API.post("sgx", "/all-query-results", {
      body: "{\"identity\":\""+identity+"\", \"query\":\""+query+"\"}"
    });
  }

  const initData = (identity, query) => {
    allQueryResults(identity, query).then(function(results) {
      let contents = JSON.parse(results.body.toString('utf-8'));
      setData(new Promise((resolve) => {resolve(contents);}));
      console.log(contents);
      let keys = Object.keys(contents);
      console.log(keys);
      let fd = [];
      for(let key in keys){
        fd.push({'name': keys[key], 'data': getDistinct(contents[keys[key]])});
      }
      setFilterData(fd);
      setHasData(true);
    });
  };

  return (
    <div className={classes.root}>
        <IconButton aria-label="delete" className={classes.openButton} onClick={handleDrawerOpen}>
            <ChevronLeftIcon />
        </IconButton>
      <CssBaseline />
      <main
        className={clsx(classes.content, {
          [classes.contentShift]: open,
        })}
      >
        <div>
          {!hasData
          ?
          <div style={{textAlign: 'center', position: 'absolute', left: '40%', top: '49%'}}>
              <CircularProgress />
            </div>
          :
          <VisualizationWindow id={id} data={data} name={name} metadata={metadata} />
          }
        </div>
        
      </main>
      <Drawer
        className={classes.drawer}
        variant="persistent"
        anchor="right"
        open={open}
        classes={{
          paper: classes.drawerPaper,
        }}
      >
        <div className={classes.drawerHeader}>
          <IconButton onClick={handleDrawerClose}>
            {theme.direction === 'rtl' ? <ChevronLeftIcon /> : <ChevronRightIcon />}
          </IconButton>
        </div>
        <Divider />
        <Paper style={{width: '100%', height: 150}}>
            <div style={{textAlign: 'center', paddingTop: 20}}>
                <h2>Options</h2>
            </div>
        </Paper>
        <Divider />
        {!hasData
        ?
          <div style={{textAlign: 'center', marginTop: 100}}>
            <CircularProgress />
          </div>
        :
        <div>
          {(filterData).map((row, index) => (
              <Accordion key={"filter"+index} style={{margin: 2}}>
                  <AccordionSummary
                  expandIcon={<ExpandMoreIcon style={{color: 'white'}} />}
                  aria-controls="panel1a-content"
                  id="panel1a-header"
                  style={{backgroundColor: '#575d5e', color: "white"}}
                  >
                      <Typography className={classes.heading}><b>{row.name}</b></Typography>
                  </AccordionSummary>
                  <AccordionDetails>
                      <VirtualizedTable values={row.data} />
                  </AccordionDetails>
              </Accordion>
          ))}
        </div>
        }
      </Drawer>
    </div>
  );
}

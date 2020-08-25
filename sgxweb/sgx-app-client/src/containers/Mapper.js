import React from "react";
import { useHistory } from "react-router-dom";
import PropTypes from 'prop-types';
import SwipeableViews from 'react-swipeable-views';
import { makeStyles, useTheme } from '@material-ui/core/styles';
import AppBar from '@material-ui/core/AppBar';
import Tabs from '@material-ui/core/Tabs';
import Tab from '@material-ui/core/Tab';
import Box from '@material-ui/core/Box';
import Button from '@material-ui/core/Button';
import SaveIcon from '@material-ui/icons/Save';
import SaveOutlinedIcon from '@material-ui/icons/SaveOutlined';
import FolderOpenIcon from '@material-ui/icons/FolderOpen';
import CreateNewFolderIcon from '@material-ui/icons/CreateNewFolder';
import EditIcon from '@material-ui/icons/Edit';
import PlayArrowIcon from '@material-ui/icons/PlayArrow';
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
          <Box p={index === 0 ? 1 : 3} height='100%'>
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

  const rows = [
    createData('Month1', 'real', 1, 5, 18816, 5),
    createData('Month2', 'real', 1, 5, 18816, 5),
    createData('Month3', 'real', 1, 5, 18816, 5),
    createData('Month4', 'real', 1, 5, 18816, 5),
    createData('Month5', 'real', 1, 5, 18816, 5),
    createData('Month6', 'real', 1, 5, 18816, 5),
    createData('Month7', 'real', 1, 5, 18816, 5),
    createData('Month8', 'real', 1, 5, 18816, 5),
    createData('Month9', 'real', 1, 5, 18816, 5),
    createData('Month10', 'real', 1, 5, 18816, 5),
    createData('Month11', 'real', 1, 5, 18816, 5),
    createData('Month12', 'real', 1, 5, 18816, 5),
    createData('Month13', 'real', 1, 5, 18816, 5),
    createData('Month14', 'real', 1, 5, 18816, 5),
    createData('Month15', 'real', 1, 5, 18816, 5),
    createData('Month16', 'real', 1, 5, 18816, 5),
  ];
  
  const useStyles = makeStyles((theme) => ({
    root: {
      backgroundColor: theme.palette.background.paper,
      width: '100%',
      height: window.innerHeight-60,
    },
    tab: {
        minWidth: '8.33%',
    },
    button: {
        margin: theme.spacing(1),
        color: '#3f51b5',
    },
    view: {
        height: window.innerHeight-115,
    },
  }));

export default function Mapper() {
    const history = useHistory();
    const classes = useStyles();
    const theme = useTheme();
    const [value, setValue] = React.useState(0);

    const handleChange = (event, newValue) => {
        setValue(newValue);
    };

    const handleChangeIndex = (index) => {
        setValue(index);
    };

    async function handleRun() {
      history.push("/visualization");
    }

  //console.log(history.location.data);
  
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
                    <Button color="default" className={classes.button} startIcon={<FolderOpenIcon />}>
                        Open
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<CreateNewFolderIcon />}>
                        New
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<SaveOutlinedIcon />}>
                        Save
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<SaveIcon />}>
                        Save As
                    </Button>
                    <Button color="default" className={classes.button} startIcon={<EditIcon />}>
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
                <TabPanel value={value} index={0} dir={theme.direction}>
                <MapperProperties data={rows}/>
                </TabPanel>
                <TabPanel value={value} index={1} dir={theme.direction}>
                <MapperData data={rows}/>
                </TabPanel>
                <TabPanel value={value} index={2} dir={theme.direction}>
                Advanced Pane
                </TabPanel>
            </SwipeableViews>
        </div>
    </div>
  );
}
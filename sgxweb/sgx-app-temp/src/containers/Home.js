import React, { useState, useEffect } from "react";
//import { useHistory } from "react-router-dom";
//import { Storage } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
import { Auth } from "aws-amplify";
import Grid from '@material-ui/core/Grid';
import List from '@material-ui/core/List';
import Divider from '@material-ui/core/Divider';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import AddIcon from '@material-ui/icons/Add';
import Button from '@material-ui/core/Button';
import AccountCircle from '@material-ui/icons/AccountCircleOutlined';
import HomeOutlinedIcon from '@material-ui/icons/HomeOutlined';
import NoteOutlinedIcon from '@material-ui/icons/NoteOutlined';
import FolderOpenIcon from '@material-ui/icons/FolderOpen';
import StarOutlineIcon from '@material-ui/icons/StarBorder';
import AccessTimeIcon from '@material-ui/icons/AccessTime';
import DeleteOutlineIcon from '@material-ui/icons/DeleteOutline';
import FolderSharedOutlinedIcon from '@material-ui/icons/FolderSharedOutlined';
import HomeFrame from './Frames/HomeFrame';
import DraftsFrame from './Frames/DraftsFrame';
import ModelsFrame from './Frames/ModelsFrame';
import TemplatesFrame from './Frames/TemplatesFrame';
import ModifiedFrame from './Frames/ModifiedFrame';
import DeletedFrame from './Frames/DeletedFrame';
import SharedFrame from './Frames/SharedFrame';
import UserFrame from './Frames/UserFrame';
import "./Home.css";

const useStyles = makeStyles((theme) => ({
    root: {
      flexGrow: 1,
      backgroundColor: 'rgb(13, 19, 33)',
      borderTop: '1.5px solid rgb(57,63,79)',
      minHeight: window.innerHeight-80,
    },
    paper: {
        minHeight: window.innerHeight-80,
        borderRight: '1.5px solid rgb(57,63,79)',
        color: 'white',
        overflow: 'hidden'
    },
    icon: {
        width: '1.2em',
        height: '1.2em',
    },
    listItemIcon: {
        color: 'white',
        marginLeft: 12,
    },
    listItemText: {
        fontSize: '1.2rem',
        lineHeight: 1.75,
    },
    divider: {
        backgroundColor: 'rgb(57,63,79)',
    },
    newWorkspaceButton: {
        backgroundColor: 'rgb(57,63,79)',
        color: 'white',
        padding: '10px 16px',
        borderRadius: 20,
        position: 'absolute',
        left: (window.innerWidth*(2/12)-242)/2,
        bottom: 25,
        textTransform: 'none',
        fontSize: '1rem'
    },
}));

export default function Home(props) {
    const classes = useStyles();
    //const history = useHistory();
    const [userName, setUserName] = useState("");
    const [currentFrame, setCurrentFrame] = useState(null);
    const [institution, setInstitution] = useState(null);

    useEffect(() => {
        onLoad();
    }, []);

    useEffect(() => {
        if(institution)
            changeFrame('Home');
    }, [institution]);

    useEffect(() => {
        if(props.client && props.server)
            changeFrame('Home');
    }, [props.client, props.server]);

    async function onLoad() {
        let info = await Auth.currentUserInfo();
        let attr = info.attributes;
        setInstitution(attr.email.split('@')[1]);
        setUserName(attr.given_name + " " + attr.family_name);
    }

    const getIcon = (index) => {

        switch(index) {
            case 0:
                return(<FolderOpenIcon className={classes.icon} />);
            case 1:
                return(<StarOutlineIcon className={classes.icon} />);
            case 2:
                return(<AccessTimeIcon className={classes.icon} />);
            case 3:
                return(<DeleteOutlineIcon className={classes.icon} />);
            default:
                break;
        }
    };

    const changeFrame = (text) => {

        switch(text) {
            case 'User':
                setCurrentFrame(<UserFrame />);
                break;
            case 'Home':
                setCurrentFrame(<HomeFrame client={props.client} server={props.server} sendMessage={props.sendMessage} institution={institution}/>);
                break;
            case 'Drafts':
                setCurrentFrame(<DraftsFrame />);
                break;
            case 'All Models':
                setCurrentFrame(<ModelsFrame />);
                break;
            case 'Templates':
                setCurrentFrame(<TemplatesFrame />);
                break;     
            case 'Recently Modified':
                setCurrentFrame(<ModifiedFrame />);
                break;
            case 'Recently Deleted':
                setCurrentFrame(<DeletedFrame />);
                break;
            case 'Shared':
                setCurrentFrame(<SharedFrame />);
                break;
            default:
                break;
        }
    };
    
    return (
        <div className={classes.root}>
            <Grid container spacing={0}>
                <Grid item xs={2} className={classes.paper}>
                    <List>
                        <ListItem button key={'User'} onClick={() => { changeFrame('User') }}>
                            <ListItemIcon className={classes.listItemIcon}>{<AccountCircle className={classes.icon} />}</ListItemIcon>
                            <ListItemText primary={userName} disableTypography={true} className={classes.listItemText} />
                        </ListItem>
                    </List>
                    <Divider className={classes.divider}/>
                    <List>
                        {['Home', 'Drafts'].map((text, index) => (
                        <ListItem button key={text} onClick={() => { changeFrame(text) }}>
                            <ListItemIcon className={classes.listItemIcon}>{index % 2 === 0 ? <HomeOutlinedIcon className={classes.icon} /> : <NoteOutlinedIcon className={classes.icon} />}</ListItemIcon>
                            <ListItemText primary={text} disableTypography={true} className={classes.listItemText} />
                        </ListItem>
                        ))}
                    </List>
                    <Divider className={classes.divider}/>
                    <List>
                        {['All Models', 'Templates', 'Recently Modified', 'Recently Deleted'].map((text, index) => (
                        <ListItem button key={text} onClick={() => { changeFrame(text) }}>
                            <ListItemIcon className={classes.listItemIcon}>{getIcon(index)}</ListItemIcon>
                            <ListItemText primary={text} disableTypography={true} className={classes.listItemText} />
                        </ListItem>
                        ))}
                    </List>
                    <Divider className={classes.divider}/>
                    <List>
                        <ListItem button key={'Shared'} onClick={() => { changeFrame('Shared') }}>
                            <ListItemIcon className={classes.listItemIcon}>{<FolderSharedOutlinedIcon className={classes.icon} />}</ListItemIcon>
                            <ListItemText primary={'Shared With Me'} disableTypography={true} className={classes.listItemText} />
                        </ListItem>
                    </List>
                    <Button
                        variant="contained"
                        color="default"
                        className={classes.newWorkspaceButton}
                        startIcon={<AddIcon />}
                    >
                        Create New Workspace
                    </Button>
                </Grid>
                <Grid item xs={10} style={{textAlign:'center'}}>
                    {currentFrame}
                </Grid>
            </Grid>
        </div>
    );
}

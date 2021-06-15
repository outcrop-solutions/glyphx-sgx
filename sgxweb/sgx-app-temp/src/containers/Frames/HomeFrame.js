import React, { useState, useEffect } from "react";
//import { useHistory } from "react-router-dom";
//import { Storage } from "aws-amplify";
import { makeStyles, fade } from '@material-ui/core/styles';
import { Auth, API } from "aws-amplify";
import Grid from '@material-ui/core/Grid';
import Divider from '@material-ui/core/Divider';
import AddIcon from '@material-ui/icons/Add';
import IconButton from '@material-ui/core/IconButton';
import AppsOutlinedIcon from '@material-ui/icons/AppsOutlined';
import Paper from '@material-ui/core/Paper';
import RecentCard from "../../components/RecentCard";

const useStyles = makeStyles((theme) => ({
    divider: {
        backgroundColor: 'rgb(57,63,79)',
    },
    container: {
        width: '95%',
        margin: 'auto',
        marginTop: 50,
    },
    recentRow: {
        display: 'flex',
        flexWrap: 'nowrap',
        height: 350,
        overflowX: 'auto'
    },
    circle_button: {
        backgroundColor: 'white',
        width: '1em',
        height: '1em',
        marginTop: 115,
        '&:hover': {
            backgroundColor: fade(theme.palette.common.white, 0.25),
         },
    },
}));

export default function HomeFrame(props) {
    const classes = useStyles();
    //const history = useHistory();
    //const [institution, setInstitution] = useState(null);
    const [recentViews, setRecentViews] = useState(null);
    const [loadView, setLoadView] = useState(false);

    useEffect(() => {
        //console.log(props.institution);
        if(props.institution)
            getRecent(props.institution);
    }, [props.institution]);

    useEffect(() => {
        if(recentViews)
            setLoadView(true);
    }, [recentViews]);

    useEffect(() => {
        setInterval(function ping() {
            props.sendMessage('{"action" : "OnMessage" , "connectionId": "'+props.server+'", "message" : "ping"}');
            console.log("ping");
        }, 60000);
    }, [])


    /*const names = ['Purchasing Raw Materials China', 'Shipping Spend UPS', 'Manufacturing Production Asia', 'Purchasing Raw Materials Brazil', 'Shipping Spend Fedex', 'Warehouse Draper UT'];

    const data = () => {
        return {
            'title': names[Math.floor(Math.random() * 5)], 
            'lastViewed': '1618349877', 
            'whoViewed': ['name1', 'name2', 'name3'],
            'thumbnail': Math.floor(Math.random() * 3),
            'type': Math.floor(Math.random() * 4)
        };
    };*/

    function getRecent(institution) {
        getRecentList(institution).then((result) => {
            let data = JSON.parse(result.body);
            let views = [];
            for(let key in data){
                views.push(data[key]);
            }
            setRecentViews(views);
        }).catch((error) => {
            console.error(error);
        });;
    }

    function getRecentList(institution) {
        return API.post("sgx", "/get-recent-list", {
          body: "{\"institution\":\""+institution+"\"}"
        });
    }
    
    return (
        <div className={classes.container}>
            <Grid container spacing={0}>
                <Grid item xs={6}>
                    <h1 style={{color: 'white', textAlign: 'left', fontWeight: 'normal'}}>HOME</h1>
                </Grid>
                <Grid item xs={6} style={{textAlign: 'right'}}>
                    <IconButton>
                        <AppsOutlinedIcon style={{color: 'white', width: '1.4em', height: '1.4em', marginTop: 15 }}/>
                    </IconButton>
                </Grid>
            </Grid>
            <Divider className={classes.divider}/>
            <Paper elevation={0} style={{ height: window.innerHeight-212, backgroundColor: 'rgb(13, 19, 33)', overflowX: 'hidden', overflowY:'auto'}}>
                <div style={{marginTop: 50}}>
                    <h2 style={{color: 'white', textAlign: 'left', fontWeight: 'normal'}}>RECENTLY USED MODELS</h2>
                    <div className={classes.recentRow}>
                        <div style={{minWidth: 350, height: 325, border: '1.5px solid white', borderRadius: 15, float: 'left', marginRight: 50}}>
                            <IconButton color="default" className={classes.circle_button} component="span">
                                <AddIcon style={{color: 'rgb(13, 19, 33)', width: '1.4em', height: '1.4em'}}/>
                            </IconButton>
                            <div style={{color: 'white', marginTop: 20}}>New Model</div>
                        </div>
                        {loadView
                        ?
                        recentViews.map((view, index) => (
                            <RecentCard data={view} key={index} client={props.client} server={props.server} sendMessage={props.sendMessage}/>
                        ))
                        : <></>
                        }
                    </div>
                </div>
                <div style={{marginTop: 50}}>
                    <h2 style={{color: 'white', textAlign: 'left', fontWeight: 'normal'}}>RECENTLY VIEWED MODELS</h2>
                    <div className={classes.recentRow}>
                        {loadView
                        ?
                        recentViews.map((view, index) => (
                            <RecentCard data={view} key={index} client={props.client} server={props.server} sendMessage={props.sendMessage}/>
                        ))
                        : <></>
                        }
                    </div>
                </div>
            </Paper>
        </div>
    );
}

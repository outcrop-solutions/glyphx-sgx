import React, { useState, useEffect } from "react";
import { makeStyles } from '@material-ui/core/styles';
import Paper from '@material-ui/core/Paper';
import Grid from '@material-ui/core/Grid';
import AccountCircle from '@material-ui/icons/AccountCircleOutlined';
import IconOne from '../images/icon_1.png';
import IconTwo from '../images/icon_2.png';
import IconThree from '../images/icon_3.png';
import IconFour from '../images/icon_4.png';
import IconFive from '../images/icon_5.png';
import ThumbnailOne from '../images/image_1.png';
import ThumbnailTwo from '../images/image_2.png';
import ThumbnailThree from '../images/image_3.png';
import ThumbnailFour from '../images/image_4.png';

const useStyles = makeStyles((theme) => ({
  root: {
    minWidth: 350, 
    height: 325, 
    border: '1.5px solid white', 
    backgroundColor: 'rgb(13, 19, 33)',
    borderRadius: 15, 
    marginRight: 50,
  },
  head: {
      backgroundColor: 'white',
      height: 215,
      borderRadius: '15px 15px 0px 0px',
      padding: 5,
      backgroundSize: 'cover'
  },
  foot: {
      backgroundColor: 'rgb(13, 19, 33)',
      height: 100,
      borderRadius: '0px 0px 15px 15px',
      color: 'white'
  },
  title: {
    textAlign: 'left',
    paddingLeft: 15,
    paddingTop: 15
  },
  icon: {
      height: '1.4em',
      width: '1.4em',
      verticalAlign: 'middle'
  },
  typeIndicator: {
    width: 35, 
    height: 35, 
    borderRadius: 7.5, 
    float: 'right',
    backgroundSize: 'cover'
  }
}));

export default function RecentCard(props) {
    const classes = useStyles();
    const [lastViewed, setLastViewed] = useState(null);

    useEffect(() => {
        let now = Math.floor(Date.now() / 1000);
        let mins = (now - props.data.lastViewed)/60;
        let hours = mins/60; 
        setLastViewed(Math.round(hours/24));
    }, []);

    const icons = [
        {'icon': IconOne, 'color': 'rgb(0, 198, 177)'},
        {'icon': IconTwo, 'color': 'rgb(0, 88, 216)'},
        {'icon': IconThree, 'color': 'rgb(255, 197, 0)'},
        {'icon': IconFour, 'color': 'rgb(121, 217, 0)'},
        {'icon': IconFive, 'color': 'rgb(0, 88, 216)'}
    ];

    const modelClicked = (title) => {
        if(props.server)
            props.sendMessage('{"action" : "OnMessage" , "connectionId": "'+props.server+'", "message" : "'+title+'"}');
    };

    const thumbnails = [ThumbnailOne, ThumbnailTwo, ThumbnailThree, ThumbnailFour];

    return (
        <Paper className={classes.root} onClick={() => modelClicked(props.data.title)}>
            <div className={classes.head} style={{backgroundImage: 'url('+thumbnails[props.data.thumbnail]+')'}}>
                <div className={classes.typeIndicator} style={{backgroundColor: icons[props.data.type].color, backgroundImage: 'url('+icons[props.data.type].icon+')'}}></div>
            </div>
            <div className={classes.foot}>
                <div className={classes.title}>{props.data.title}</div>
                <Grid container spacing={0} style={{padding: 15}}>
                    <Grid item xs={6} style={{textAlign: 'left'}}>
                        <AccountCircle className={classes.icon} />
                        <AccountCircle className={classes.icon} />
                        +{props.data.whoViewed.length} more
                    </Grid>
                    <Grid item xs={6} style={{textAlign: 'right', paddingTop: 7.5}}>
                        {lastViewed}d ago
                    </Grid>
                </Grid>
            </div>
        </Paper>
    );
}

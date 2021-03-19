import React, { useEffect } from "react";
import { makeStyles } from '@material-ui/core/styles';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import IconButton from '@material-ui/core/IconButton';
import CommentIcon from '@material-ui/icons/Comment';

const useStyles = makeStyles((theme) => ({
  root: {
    width: '90%',
    margin: 'auto',
    backgroundColor: theme.palette.background.paper,
    paddingTop: 5,
  },
  text: {
    fontSize: '1rem',
  },
}));

function createData(text, value, url) {
  return { text, value, url };
}

export default function CheckboxList() {
  const classes = useStyles();
  const [announcements, setAnnouncements] = React.useState([]);

  useEffect(() => {
    fetchData();
  }, []);

  const handleToggle = (link) => () => {
    //console.log(link);
    window.open(link, "_blank");
  };

  const fetchData = async () => { 

    fetch('https://api.rss2json.com/v1/api.json?rss_url=https://www.synglyphx.com/blog?format=rss')
    .then(response => response.json())
    .then((jsonData) => {
      // jsonData is parsed json object received from url
      //console.log(jsonData.items);
      let list = [];
      for(let i = 0; i < 6; i++) {
        list.push(createData(jsonData.items[i].title, i+1, jsonData.items[i].link));
      }
      setAnnouncements(list);
    })
    .catch((error) => {
      // handle your errors here
      console.error(error);
    })
  
  };

  return (
    <List className={classes.root}>
      {announcements.map((row) => {
        const labelId = `checkbox-list-label-${row.value}`;

        return (
          <ListItem key={row.value} role={undefined} dense button onClick={handleToggle(row.url)}>
            <ListItemText classes={{primary: classes.text}} id={labelId} primary={row.text} />
            <ListItemSecondaryAction>
              <IconButton edge="end" aria-label="comments">
                <CommentIcon />
              </IconButton>
            </ListItemSecondaryAction>
          </ListItem>
        );
      })}
    </List>
  );
}

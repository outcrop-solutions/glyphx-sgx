import React from 'react';
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

const announcements = [
    {'text': 'Announcement numero uno', 'value': 1},
    {'text': 'Announcement numero dos', 'value': 2},
    {'text': 'Announcement numero tres', 'value': 3},
    {'text': 'Announcement numero quatro', 'value': 4},
]

export default function CheckboxList() {
  const classes = useStyles();
  const [checked, setChecked] = React.useState([0]);

  const handleToggle = (value) => () => {
    const currentIndex = checked.indexOf(value);
    const newChecked = [...checked];

    if (currentIndex === -1) {
      newChecked.push(value);
    } else {
      newChecked.splice(currentIndex, 1);
    }

    setChecked(newChecked);
  };

  return (
    <List className={classes.root}>
      {announcements.map((row) => {
        const labelId = `checkbox-list-label-${row.value}`;

        return (
          <ListItem key={row.value} role={undefined} dense button onClick={handleToggle(row.value)}>
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

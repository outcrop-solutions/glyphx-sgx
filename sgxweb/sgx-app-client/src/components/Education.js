import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Card from '@material-ui/core/Card';
import CardActionArea from '@material-ui/core/CardActionArea';
import CardActions from '@material-ui/core/CardActions';
import CardContent from '@material-ui/core/CardContent';
import CardMedia from '@material-ui/core/CardMedia';
import Button from '@material-ui/core/Button';
import Typography from '@material-ui/core/Typography';
import Lizard from "../images/lizard.jpg";

const useStyles = makeStyles({
  root: {
    maxWidth: '100%',
    float: 'left',
  },
  content: {
    float:'left', 
    width: '45%', 
    height: '150px', 
    paddingTop: window.innerHeight*0.035 , 
    paddingLeft: '2.5%',
    paddingRight: '2.5%',
    paddingBottom: 12
  },
});

export default function ImgMediaCard() {
  const classes = useStyles();

  return (
    <Card className={classes.root}>
      <CardActionArea style={{float:'left', width: '50%'}}>
        <CardMedia
          component="img"
          alt="Glyph"
          image={Lizard}
          title="Glyph"
          style={{width: '100%', height: window.innerHeight*0.37}}
        />
      </CardActionArea>
      <CardContent className={classes.content} >
          <Typography gutterBottom variant="h5" component="h2">
            Education Pane
          </Typography>
          <Typography variant="body2" color="textSecondary" component="p">
            Lizards are a widespread group of squamate reptiles, with over 6,000 species, ranging
            across all continents except Antarctica
          </Typography>
        </CardContent>
      <CardActions style={{display:'block', padding: 0}}>
        <Button size="small" color="primary">
          Share
        </Button>
        <Button size="small" color="primary">
          Learn More
        </Button>
      </CardActions>
    </Card>
  );
}

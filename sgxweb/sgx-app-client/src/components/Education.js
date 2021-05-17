import React, { useEffect } from "react";
import { makeStyles } from '@material-ui/core/styles';
import Card from '@material-ui/core/Card';
import CardActionArea from '@material-ui/core/CardActionArea';
import CardActions from '@material-ui/core/CardActions';
import CardContent from '@material-ui/core/CardContent';
import CardMedia from '@material-ui/core/CardMedia';
import Button from '@material-ui/core/Button';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import BackIcon from '@material-ui/icons/ArrowBack';
import ForwardIcon from '@material-ui/icons/ArrowForward';

const useStyles = makeStyles({
  root: {
    width: '100%',
    float: 'left',
    height: '100%',
    position: 'relative',
    borderBottomWidth: 0,
  },
  content: {
    float:'left', 
    width: '45%', 
    height: '150px', 
    paddingTop: window.innerHeight*0.025 , 
    paddingLeft: '2.5%',
    paddingRight: '2.5%',
    paddingBottom: 12
  },
  left_button: {
    position: 'absolute',
    left: 10,
    top: window.innerHeight*0.15,
    backgroundColor: 'rgb(63,81,181)',
    color: 'white',
    '&:hover': {
      backgroundColor: 'rgb(45, 48, 145)',
    },
    zIndex: 10,
    opacity: 0.5,
  },
  right_button: {
    position: 'absolute',
    right: 10,
    top: window.innerHeight*0.15,
    backgroundColor: 'rgb(63,81,181)',
    color: 'white',
    '&:hover': {
      backgroundColor: 'rgb(45, 48, 145)',
    },
    zIndex: 10,
    opacity: 0.5,
  },
});

function createData(title, value, date, url, img) {
  return { title, value, date, url, img };
}

export default function ImgMediaCard() {
  const classes = useStyles();
  const [caseStudies, setCaseStudies] = React.useState([]);
  const [currentTitle, setCurrentTitle] = React.useState("");
  const [currentPubDate, setCurrentPubDate] = React.useState("");
  const [currentURL, setCurrentURL] = React.useState("");
  const [currentImg, setCurrentImg] = React.useState("");
  const [currentIndex, setCurrentIndex] = React.useState(0);
  const [timer, setTimer] = React.useState(null);

  useEffect(() => {

    fetchData();

  }, []);

  useEffect(() => {

    if(timer)
      clearTimeout(timer);
    setTimer(setTimeout(shiftIndex(1), 7500));

  }, [caseStudies, currentIndex]);

  const fetchData = async () => { 

    fetch('https://api.rss2json.com/v1/api.json?rss_url=https://www.synglyphx.com/case-studies-1?format=rss')
    .then(response => response.json())
    .then((jsonData) => {
      // jsonData is parsed json object received from url
      //console.log(jsonData.items);
      let list = [];
      for(let i = 0; i < jsonData.items.length; i++) {
        list.push(createData(jsonData.items[i].title, i+1, new Date(jsonData.items[i].pubDate).toDateString(), jsonData.items[i].link, jsonData.items[i].enclosure.link));
      }
      setCaseStudies(list);
      setCurrentTitle(jsonData.items[0].title);
      setCurrentPubDate(new Date(jsonData.items[0].pubDate).toDateString());
      setCurrentURL(jsonData.items[0].link);
      setCurrentImg(jsonData.items[0].enclosure.link);
      console.log(list);

    })
    .catch((error) => {
      // handle your errors here
      console.error(error);
    })
  
  };

  const setCurrentFields = (index) => {

    setCurrentIndex(index);
    setCurrentTitle(caseStudies[index].title);
    setCurrentPubDate(caseStudies[index].date);
    setCurrentURL(caseStudies[index].url);
    setCurrentImg(caseStudies[index].img);

  };

  const handleToggle = (link) => () => {
    //console.log(link);
    window.open(link, "_blank");
  };

  const shiftIndex = (shift) => () => {
    
    if(caseStudies.length !== 0){
      if(shift === -1 && currentIndex === 0) {
        setCurrentFields(caseStudies.length-1);
      }
      else if( shift === 1 && currentIndex === caseStudies.length-1){
        setCurrentFields(0);
      }
      else {
        setCurrentFields(currentIndex+shift);
      }
    }
  };

  return (
    <Card className={classes.root}>
      <IconButton color="default" className={classes.left_button} component="span" onClick={shiftIndex(-1)}>
          <BackIcon />
      </IconButton>
      <CardActionArea style={{float:'left', width: '50%'}}>
        <CardMedia 
          component="img"
          alt="Glyph"
          image={currentImg}
          title="Glyph"
          style={{padding: 20, maxHeight: (window.innerHeight*0.37)-35}}
        />
      </CardActionArea>
      <CardContent className={classes.content} >
          <Typography gutterBottom variant="h5" component="h2">
            Case Studies
          </Typography>
          <Typography variant="body2" color="textSecondary" component="p">
            {currentPubDate}
          </Typography>
          <Typography variant="body2" component="h6" style={{marginTop: 30}}>
            {currentTitle}
          </Typography>
        </CardContent>
      <CardActions style={{display:'block', padding: 0}}>
        <Button size="small" color="primary" onClick={handleToggle(currentURL)}>
          Learn More
        </Button>
      </CardActions>
      <IconButton color="default" className={classes.right_button} component="span" onClick={shiftIndex(1)}>
          <ForwardIcon />
      </IconButton>
    </Card>
  );
}

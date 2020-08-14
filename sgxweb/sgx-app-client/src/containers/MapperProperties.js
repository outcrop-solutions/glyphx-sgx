import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Paper from '@material-ui/core/Paper';
import Grid from '@material-ui/core/Grid';
import TextField from '@material-ui/core/TextField';
import Autocomplete from '@material-ui/lab/Autocomplete';
import EmptyGlyph from "../images/empty-glyph-pane.png";
import XAxisImg from '../images/x-axis.png';
import YAxisImg from '../images/y-axis.png';
import ZAxisImg from '../images/z-axis.png';
import ColorImg from '../images/color.png';
import SizeImg from '../images/size.png';
import TypeImg from '../images/type.png';

const useStyles = makeStyles((theme) => ({
  root: {
    flexGrow: 1,
  },
  paper: {
    height: window.innerHeight-230,
    textAlign: 'center',
    color: theme.palette.text.secondary,
    paddingTop: window.innerHeight*0.07,
    paddingBottom: theme.spacing(4),
  },
  field: {
    float: 'left',
    width: '65%', 
    marginBottom: window.innerHeight*0.025,
  },
  images: {
    float: 'left', 
    width: 40, 
    height: 40, 
    marginLeft: theme.spacing(4), 
    marginRight: theme.spacing(2)
  }
}));

const labels = [
    {label: 'X Axis', index: 1, image: XAxisImg},
    {label: 'Y Axis', index: 2, image: YAxisImg},
    {label: 'Z Axis', index: 3, image: ZAxisImg},
    {label: 'Glyph Color', index: 4, image: ColorImg},
    {label: 'Glyph Size', index: 5, image: SizeImg},
    {label: 'Glyph Type', index: 6, image: TypeImg}
];

export default function MapperProperties(props) {
  const classes = useStyles();

  return (
    <div className={classes.root}>
      <Grid container spacing={0}>
        <Grid item xs={3}>
          <Paper className={classes.paper}>
            {labels.map((row) => {
                return (
                    <div key={"input"+row.index}>
                        <Grid item xs={3}>
                            <img alt={"mapimg"+row.index} src={row.image} className={classes.images}/>
                        </Grid>
                        <Grid item xs={9}>
                            <Autocomplete
                                className={classes.field}
                                id={"combo-box-demo"+row.index}
                                options={props.data}
                                getOptionLabel={(option) => option.fieldname}
                                renderInput={(params) => <TextField {...params} label={row.label} variant="outlined" />}
                            />
                        </Grid>
                    </div>
            );})}
          </Paper>
        </Grid>
        <Grid item xs={9} style={{textAlign:'center'}}>
            <div style={{textAlign: 'center'}}>
                <img src={EmptyGlyph} alt="EmptyGlyph" style={{maxWidth: 225, marginTop: '15%'}}/>
                <div style={{color: 'gray', maxWidth: 250, margin: 'auto', fontSize: '1rem'}}>
                    Select data headers from dropdowns to start exploring your data.
                </div>
            </div>
        </Grid>
      </Grid>
    </div>
  );
}
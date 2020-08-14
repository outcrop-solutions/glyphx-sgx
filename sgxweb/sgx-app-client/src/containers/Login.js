import React from "react";
import { Link, useHistory } from "react-router-dom";
import { Auth } from "aws-amplify";
import { useAppContext } from "../libs/contextLib";
import { useFormFields } from "../libs/hooksLib";
import { onError } from "../libs/errorLib";
import logo from "../images/synglyphx_logo_large.png";
import Button from '@material-ui/core/Button';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Checkbox from '@material-ui/core/Checkbox';
import TextField from '@material-ui/core/TextField';
import Grid from '@material-ui/core/Grid';
import { makeStyles } from '@material-ui/core/styles';
import Container from '@material-ui/core/Container';

const useStyles = makeStyles((theme) => ({
  paper: {
    marginTop: theme.spacing(8),
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
  },
  avatar: {
    margin: theme.spacing(1),
    backgroundColor: theme.palette.secondary.main,
  },
  form: {
    width: '100%', // Fix IE 11 issue.
    marginTop: theme.spacing(1),
  },
  submit: {
    margin: theme.spacing(3, 0, 2),
  },
}));

export default function Login() {
    const history = useHistory();
    const classes = useStyles();
    const { userHasAuthenticated } = useAppContext();
    //const [isLoading, setIsLoading] = useState(false);
    const [fields, handleFieldChange] = useFormFields({
      email: "",
      password: ""
    });

    function validateForm() {
        return fields.email.length > 0 && fields.password.length > 0;
    }

  async function handleSubmit(event) {
    event.preventDefault();
    //setIsLoading(true);
  
    try {
        await Auth.signIn(fields.email, fields.password);
        userHasAuthenticated(true);
        history.push("/");
    } catch (e) {
        onError(e);
        //setIsLoading(false);
    }
  }

  return (

    <Container component="main" maxWidth="xs">
      <div className={classes.paper}>
        <div className="Logo">
            <img src={logo} alt="Logo" />
        </div>
        <form className={classes.form} onSubmit={handleSubmit} noValidate>
          <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            id="email"
            label="Email Address"
            name="email"
            autoComplete="email"
            autoFocus
            value={fields.email}
            onChange={handleFieldChange}
          />
          <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            name="password"
            label="Password"
            type="password"
            id="password"
            autoComplete="current-password"
            value={fields.password}
            onChange={handleFieldChange}
          />
          <FormControlLabel
            control={<Checkbox value="remember" color="primary" />}
            label="Remember me"
          />
          <Button
            type="submit"
            fullWidth
            variant="contained"
            color="primary"
            className={classes.submit}
            disabled={!validateForm()}
          >
            Sign In
          </Button>
          <Grid container>
            <Grid item xs>
              <Link to="/login/reset" variant="body2">
                Forgot password?
              </Link>
            </Grid>
            <Grid item>
              <Link to="/signup" variant="body2">
                Don't have an account? Sign Up
              </Link>
            </Grid>
          </Grid>
        </form>
      </div>
    </Container>
  );
}
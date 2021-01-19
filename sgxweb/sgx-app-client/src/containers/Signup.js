import React, { useState } from "react";
import { Link, useHistory } from "react-router-dom";
import { Auth, API } from "aws-amplify";
//import LoaderButton from "../components/LoaderButton";
import { useAppContext } from "../libs/contextLib";
import { useFormFields } from "../libs/hooksLib";
import { onError } from "../libs/errorLib";
import logo from "../images/synglyphx_logo_large.png";
import Button from '@material-ui/core/Button';
import TextField from '@material-ui/core/TextField';
import Grid from '@material-ui/core/Grid';
import { makeStyles } from '@material-ui/core/styles';
import Container from '@material-ui/core/Container';
import "./Signup.css";

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

export default function Signup() {
  const [fields, handleFieldChange] = useFormFields({
    email: "",
    password: "",
    confirmPassword: "",
    confirmationCode: "",
  });
  const history = useHistory();
  const classes = useStyles();
  const [newUser, setNewUser] = useState(null);
  const { userHasAuthenticated } = useAppContext();
  //const [isLoading, setIsLoading] = useState(false);

  function validateForm() {
    return (
      fields.email.length > 0 &&
      fields.password.length > 0 &&
      fields.password === fields.confirmPassword
    );
  }

  function validateConfirmationForm() {
    return fields.confirmationCode.length > 0;
  }

  async function handleSubmit(event) {
    event.preventDefault();
  
    //setIsLoading(true);
  
    try {
      const newUser = await Auth.signUp({
        username: fields.email,
        password: fields.password,
      });
      //setIsLoading(false);
      setNewUser(newUser);
    } catch (e) {
      onError(e);
      //setIsLoading(false);
    }
  }

  async function handleConfirmationSubmit(event) {
    event.preventDefault();
  
    //setIsLoading(true);
  
    try {
      await Auth.confirmSignUp(fields.email, fields.confirmationCode);
      await Auth.signIn(fields.email, fields.password);
      let cred = await Auth.currentCredentials();
      createUserStorage(cred);
      createGlueDatabase(cred);

      userHasAuthenticated(true);
      history.push("/");
    } catch (e) {
      onError(e);
      //setIsLoading(false);
    }
  }

  function createUserStorage(identity) {
    return API.post("sgx", "/create-user-storage", {
      body: "{\"identity\":\""+identity.identityId+"\"}"
    });
  }

  function createGlueDatabase(identity) {
    return API.post("sgx", "/create-glue-database", {
      body: "{\"identity\":\""+identity.identityId+"\"}"
    });
  }

  function renderConfirmationForm() {
    return (
      <form className={classes.form} onSubmit={handleConfirmationSubmit} noValidate>
          <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            id="confirmationCode"
            label="Confirmation Code"
            name="confirmationCode"
            autoComplete="one-time-code"
            autoFocus
            value={fields.confirmationCode}
            onChange={handleFieldChange}
          />
          <Button
            type="submit"
            fullWidth
            variant="contained"
            color="primary"
            className={classes.submit}
            disabled={!validateConfirmationForm()}
          >
            Verify
          </Button>
        </form>
    );
  }

  function renderForm() {
    return (
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
          <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            name="confirmPassword"
            label="Confirm Password"
            type="password"
            id="confirmPassword"
            autoComplete="current-password"
            value={fields.confirmPassword}
            onChange={handleFieldChange}
          />
          <Button
            type="submit"
            fullWidth
            variant="contained"
            color="primary"
            className={classes.submit}
            disabled={!validateForm()}
          >
            Sign Up
          </Button>
          <Grid container>
            <Grid item xs>
              
            </Grid>
            <Grid item>
              <Link to="/login" variant="body2">
                Already have an account? Sign in
              </Link>
            </Grid>
          </Grid>
        </form>
    );
  }

  return (
    <Container component="main" maxWidth="xs">
      <div className={classes.paper}>
        <div className="Logo">
            <img src={logo} alt="Logo" />
        </div>
      {newUser === null ? renderForm() : renderConfirmationForm()}
      </div>
    </Container>
  );
}
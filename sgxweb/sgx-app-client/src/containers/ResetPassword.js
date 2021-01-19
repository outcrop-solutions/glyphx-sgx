import React, { useState } from "react";
import { Auth } from "aws-amplify";
import { Link } from "react-router-dom";
//import LoaderButton from "../components/LoaderButton";
import { useFormFields } from "../libs/hooksLib";
import { onError } from "../libs/errorLib";
import logo from "../images/synglyphx_logo_large.png";
import Button from '@material-ui/core/Button';
import TextField from '@material-ui/core/TextField';
import { makeStyles } from '@material-ui/core/styles';
import Container from '@material-ui/core/Container';
import CheckCircleOutlinedIcon from '@material-ui/icons/CheckCircleOutlined';
import "./ResetPassword.css";

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
  help: {
    marginTop: 0,
    fontSize: '0.85rem',
    color: 'gray',
  },
}));

export default function ResetPassword() {
  const [fields, handleFieldChange] = useFormFields({
    code: "",
    email: "",
    password: "",
    confirmPassword: "",
  });
  const classes = useStyles();
  const [codeSent, setCodeSent] = useState(false);
  const [confirmed, setConfirmed] = useState(false);
  //const [isConfirming, setIsConfirming] = useState(false);
  //const [isSendingCode, setIsSendingCode] = useState(false);

  function validateCodeForm() {
    return fields.email.length > 0;
  }

  function validateResetForm() {
    return (
      fields.code.length > 0 &&
      fields.password.length > 0 &&
      fields.password === fields.confirmPassword
    );
  }

  async function handleSendCodeClick(event) {
    event.preventDefault();

    //setIsSendingCode(true);

    try {
      await Auth.forgotPassword(fields.email);
      setCodeSent(true);
    } catch (error) {
      onError(error);
      //setIsSendingCode(false);
    }
  }

  async function handleConfirmClick(event) {
    event.preventDefault();

    //setIsConfirming(true);

    try {
      await Auth.forgotPasswordSubmit(
        fields.email,
        fields.code,
        fields.password
      );
      setConfirmed(true);
    } catch (error) {
      onError(error);
      //setIsConfirming(false);
    }
  }

  function renderRequestCodeForm() {
    return (
      <form className={classes.form} onSubmit={handleSendCodeClick} noValidate>
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
        <Button
          type="submit"
          fullWidth
          variant="contained"
          color="primary"
          className={classes.submit}
          //isLoading={isSendingCode}
          disabled={!validateCodeForm()}
        >
          Send Confirmation
        </Button>
      </form>
    );
  }

  function renderConfirmationForm() {
    return (
      <form className={classes.form} onSubmit={handleConfirmClick} noValidate>
        <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            id="code"
            label="Confirmation Code"
            type="tel"
            name="code"
            autoComplete="one-time-code"
            autoFocus
            value={fields.code}
            onChange={handleFieldChange}
          />
        <p className={classes.help}>Please check your email ({fields.email}) for the confirmation code.</p>
        <hr />
        <TextField
            variant="outlined"
            margin="normal"
            required
            fullWidth
            name="password"
            label="New Password"
            type="password"
            id="password"
            autoComplete="new-password"
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
            autoComplete="new-password"
            value={fields.confirmPassword}
            onChange={handleFieldChange}
          />
        <Button
          type="submit"
          fullWidth
          variant="contained"
          color="primary"
          className={classes.submit}
          //isLoading={isConfirming}
          disabled={!validateResetForm()}
        >
          Confirm
        </Button>
      </form>
    );
  }

  function renderSuccessMessage() {
    return (
      <div className="success">
        <CheckCircleOutlinedIcon style={{fill: 'green', width: '2em', height: '2em'}} />
        <p>Your password has been reset.</p>
        <p>
          <Link to="/login">
            Click here to login with your new credentials.
          </Link>
        </p>
      </div>
    );
  }

  return (
    <Container component="main" maxWidth="xs">
      <div className={classes.paper}>
        <div className="ResetPassword">
          <div className="Logo">
                <img src={logo} alt="Logo" />
            </div>
          {!codeSent
            ? renderRequestCodeForm()
            : !confirmed
            ? renderConfirmationForm()
            : renderSuccessMessage()}
        </div>
      </div>
    </Container>
  );
}
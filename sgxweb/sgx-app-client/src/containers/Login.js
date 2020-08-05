import React, { useState } from "react";
import { Link, useHistory } from "react-router-dom";
import { FormGroup, FormControl, ControlLabel, Grid, Row, Col } from "react-bootstrap";
import LoaderButton from "../components/LoaderButton";
import { Auth } from "aws-amplify";
import { useAppContext } from "../libs/contextLib";
import { useFormFields } from "../libs/hooksLib";
import { onError } from "../libs/errorLib";
import logo from "../images/synglyphx_logo_large.png";
import "./Login.css";

export default function Login() {
    const history = useHistory();
    const { userHasAuthenticated } = useAppContext();
    const [isLoading, setIsLoading] = useState(false);
    const [fields, handleFieldChange] = useFormFields({
      email: "",
      password: ""
    });

    function validateForm() {
        return fields.email.length > 0 && fields.password.length > 0;
    }

  async function handleSubmit(event) {
    event.preventDefault();
    setIsLoading(true);
  
    try {
        await Auth.signIn(fields.email, fields.password);
        userHasAuthenticated(true);
        history.push("/");
    } catch (e) {
        onError(e);
        setIsLoading(false);
    }
  }

  return (
    <div className="Login">
        <div className="Logo">
            <img src={logo} alt="Logo" />
        </div>
      <form onSubmit={handleSubmit}>
        <FormGroup controlId="email" bsSize="large">
          <ControlLabel>Email</ControlLabel>
          <FormControl
            autoFocus
            type="email"
            value={fields.email}
            onChange={handleFieldChange}
          />
        </FormGroup>
        <FormGroup controlId="password" bsSize="large">
          <ControlLabel>Password</ControlLabel>
          <FormControl
            type="password"
            value={fields.password}
            onChange={handleFieldChange}
          />
        </FormGroup>
        <LoaderButton
            block
            type="submit"
            bsSize="large"
            isLoading={isLoading}
            disabled={!validateForm()}
        >
            Login
        </LoaderButton>
        <Grid id="Login_Grid">
            <Row>
                <Col md={5}>
                    <Link to="/login/reset">Forgot password?</Link>
                </Col>
                <Col md={7}>
                    <Link to="/signup">Don't have an account? Sign Up</Link>
                </Col>
            </Row>
        </Grid>
      </form>
    </div>
  );
}
import React from "react";
import { Route, Switch } from "react-router-dom";
import Home from "./containers/Home";
import Login from "./containers/Login";
import ResetPassword from "./containers/ResetPassword";
import Signup from "./containers/Signup";
import NotFound from "./containers/NotFound";

export default function Routes(props) {
  return (
    <Switch>
        <Route exact path="/">
            <Home client={props.client} server={props.server} sendMessage={props.sendMessage}/>
        </Route>
        <Route exact path="/login">
            <Login />
        </Route>
        <Route exact path="/login/reset">
            <ResetPassword />
        </Route>
        <Route exact path="/signup">
            <Signup />
        </Route>
        {/* Finally, catch all unmatched routes */}
        <Route>
            <NotFound />
        </Route>
    </Switch>
  );
}
import React from "react";
import { Route, Switch } from "react-router-dom";
import Home from "./containers/Home";
import Login from "./containers/Login";
import ResetPassword from "./containers/ResetPassword";
import Signup from "./containers/Signup";
import Mapper from "./containers/Mapper";
import Visualization from "./containers/Visualization";
import Shareable from "./containers/Shareable";
import Cross from "./containers/Cross";
import NotFound from "./containers/NotFound";

export default function Routes() {
  return (
    <Switch>
        <Route exact path="/">
            <Home />
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
        <Route exact path="/mapper">
            <Mapper />
        </Route>
        <Route exact path="/visualization">
            <Visualization />
        </Route>
        <Route exact path="/shareable">
            <Shareable />
        </Route>
        <Route exact path="/cross">
            <Cross />
        </Route>
        {/* Finally, catch all unmatched routes */}
        <Route>
            <NotFound />
        </Route>
    </Switch>
  );
}
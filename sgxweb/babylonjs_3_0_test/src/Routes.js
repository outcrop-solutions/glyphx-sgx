import React from "react";
import { Route, Switch } from "react-router-dom";
import Home from "./containers/Home";
import Cross from "./containers/Cross";
import NotFound from "./containers/NotFound";

export default function Routes() {
  return (
    <Switch>
        <Route exact path="/">
            <Home />
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
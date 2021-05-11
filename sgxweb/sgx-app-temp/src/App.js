import React, { useState, useEffect } from "react";
import { useHistory} from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import { AppContext } from "./libs/contextLib";
import { onError } from "./libs/errorLib";
import { Auth } from "aws-amplify";
import Routes from "./Routes";
import MenuBar from "./components/MenuBar";
import "./App.css";

const useStyles = makeStyles((theme) => ({
  root: {
    flexGrow: 1,
  },
}));

function App() {

  const [isAuthenticating, setIsAuthenticating] = useState(true);
  const [isAuthenticated, userHasAuthenticated] = useState(false);
  const history = useHistory();
  const classes = useStyles();

  useEffect(() => {
    onLoad();
  }, []);
  
  async function onLoad() {

    try {
      await Auth.currentSession();
      
      userHasAuthenticated(true);
    }
    catch(e) {
      history.push("/login");
      if (e !== 'No current user') {
        onError(e);
      }
    }
    setIsAuthenticating(false);
  }

  async function handleLogout() {
    await Auth.signOut();
  
    userHasAuthenticated(false);
    history.push("/login");
  }

  return (
    !isAuthenticating &&
    <div className="App">
      {isAuthenticated
      ? <div className={classes.root}>
          <MenuBar handleLogout={handleLogout}/>
        </div>
      : <></>
      }
      <AppContext.Provider
        value={{ isAuthenticated, userHasAuthenticated }}
      >
        <Routes />
      </AppContext.Provider>
    </div>
  );
}

export default App;
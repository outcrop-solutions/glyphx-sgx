import React, { useState, useEffect } from "react";
import { useHistory} from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import { AppContext } from "./libs/contextLib";
import { onError } from "./libs/errorLib";
import { Auth } from "aws-amplify";
import Routes from "./Routes";
import MenuBar from "./components/MenuBar";
import "./App.css";
import Websocket from 'react-websocket';

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
  const [needsWebSocket, setNeedsWebSocket] = useState(false);
  const [client, setClient] = useState(null);
  const [server, setServer] = useState(null);
  var refWebSocket = null;

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

    if(window.location.href.includes('?')) {
      setServer(window.location.href.split('?')[1].split('server=')[1]);
      setNeedsWebSocket(true);
    }
  }

  async function handleLogout() {
    await Auth.signOut();
  
    userHasAuthenticated(false);
    history.push("/login");
  }

  function handleData(data) {
    let response = JSON.parse(data);
    if(response.type === "id"){
      setClient(response.message);
      //console.log(server);
      //console.log(response.message);
    }
    else if(response.type === "code"){
      console.log("code:", response.message);
    }
  }

  function handleOpen() {
    sendMessage('{"action" : "OnMessage" , "connectionId": "", "message" : ""}');
  }

  function sendMessage(message) {
    if(refWebSocket)
      refWebSocket.sendMessage(message);
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
        <Routes client={client} server={server} sendMessage={sendMessage}/>
      </AppContext.Provider>
      {needsWebSocket
      ?
      <Websocket url='wss://ggi3cm7i62.execute-api.us-east-1.amazonaws.com/production' 
        onOpen={handleOpen}
        onMessage={handleData}
        ref={WebSocket => {
          refWebSocket = WebSocket;
        }}
      />
      : <></>
      }
    </div>
  );
}

export default App;
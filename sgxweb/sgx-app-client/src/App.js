import React, { useState, useEffect } from "react";
import { useHistory, Link } from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import AccountCircle from '@material-ui/icons/AccountCircle';
import ExitToAppOutlinedIcon from '@material-ui/icons/ExitToAppOutlined';
import MenuItem from '@material-ui/core/MenuItem';
import Menu from '@material-ui/core/Menu';
import { AppContext } from "./libs/contextLib";
import { onError } from "./libs/errorLib";
import { Auth } from "aws-amplify";
import Routes from "./Routes";
import logo from "./images/synglyphx_logo_white_border.png";
import "./App.css";

const useStyles = makeStyles((theme) => ({
  root: {
    flexGrow: 1,
  },
  menuButton: {
    marginRight: theme.spacing(2),
  },
  title: {
    flexGrow: 1,
  },
}));

function App() {

  const [isAuthenticating, setIsAuthenticating] = useState(true);
  const [isAuthenticated, userHasAuthenticated] = useState(false);
  const history = useHistory();
  const classes = useStyles();
  const [anchorEl, setAnchorEl] = React.useState(null);
  const open = Boolean(anchorEl);

  useEffect(() => {
    onLoad();
  }, []);
  
  async function onLoad() {
    try {
      let result = await Auth.currentSession();
      
      //console.log(result.getAccessToken());
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

  const handleMenu = (event) => {
    setAnchorEl(event.currentTarget);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  async function handleLogout() {
    await Auth.signOut();
  
    handleClose();
    userHasAuthenticated(false);
    history.push("/login");
  }

  return (
    !isAuthenticating &&
    <div className="App">
      {isAuthenticated
      ? <div className={classes.root}>
          <AppBar position="static">
            <Toolbar style={{minHeight: 58}}>
              <Typography variant="h4" className={classes.title}>
                <Link to='/'><img style={{marginTop: 10}} alt="Logo" src={logo} /></Link>
              </Typography>
                <div>
                  <IconButton
                    aria-label="account of current user"
                    aria-controls="menu-appbar"
                    aria-haspopup="true"
                    onClick={handleMenu}
                    color="inherit"
                  >
                    <AccountCircle className="account-circle" />
                  </IconButton>
                  <Menu
                    id="menu-appbar"
                    anchorEl={anchorEl}
                    anchorOrigin={{
                      vertical: 'bottom',
                      horizontal: 'left',
                    }}
                    keepMounted
                    transformOrigin={{
                      vertical: 'bottom',
                      horizontal: 'left',
                    }}
                    open={open}
                    onClose={handleClose}
                  >
                    <MenuItem onClick={handleLogout} className="menu-account"><ExitToAppOutlinedIcon/>Logout</MenuItem>
                  </Menu>
                </div>
            </Toolbar>
          </AppBar>
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
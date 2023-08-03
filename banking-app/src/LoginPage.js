import './LoginPage.css';
import * as React from 'react';
import { 
  FormControl, InputLabel, Button,
  OutlinedInput, InputAdornment, IconButton,
  Grid
} from '@mui/material';
import {Visibility, VisibilityOff} from "@mui/icons-material"
import SendIcon from "@mui/icons-material/Send"


export default function LoginPage() {
  const [showPassword, setShowPassword] = React.useState(false);
  const [password, setPassword] = React.useState("");
  const [username, setUsername] = React.useState("");

  const handleClickShowPassword = () => setShowPassword((show) => !show);
  // const handleEnteredPassword = () => setPassword(password);
  // const handleEnteredUsername = () => setUsername(username);

  const handleLogin = () => {
    console.log("Hello there!");
    console.log("The username is " + username);
    console.log("The password is " + password);
  };

  return (
    <div className="App">
      <header className="App-header">
        <Grid container>
          <Grid item xs={12} justifyContent="center">
            <FormControl sx={{ m: 1, width: '24ch' }} variant="outlined">
              <InputLabel htmlFor="outlined-adornment-username">Username</InputLabel>
              <OutlinedInput
                id="outlined-adornment-username"
                label="Username"
                value={username}
                onChange={(e) => setUsername(e.target.value)}
              />
            </FormControl>
          </Grid>
          <Grid item xs={12} justifyContent="center">
            <FormControl sx={{ m: 1, width: '24ch' }} variant="outlined">
              <InputLabel htmlFor="outlined-adornment-password">Password</InputLabel>
              <OutlinedInput
                id="outlined-adornment-password"
                type={showPassword ? 'text' : 'password'}
                endAdornment={
                  <InputAdornment position="end">
                    <IconButton
                      aria-label="toggle password visibility"
                      onClick={handleClickShowPassword}
                      edge="end"
                    >
                      {showPassword ? <VisibilityOff /> : <Visibility />}
                    </IconButton>
                  </InputAdornment>
                }
                label="Password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
              />
            </FormControl>
          </Grid>
          <Grid item xs={4}>
          </Grid>
          <Grid item xs={8}>
            <Button onClick={() => {
              handleLogin()
            }} variant='outlined' endIcon={<SendIcon/>}>
              Login
            </Button>
          </Grid>
        </Grid>
      </header>
    </div>
  );
}
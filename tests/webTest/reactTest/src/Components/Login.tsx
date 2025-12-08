import "../login.css";
import React from "react";

function Login() {
  return (
    <div className="login-container">
      <div className="login-box">
        <h4>UA Baja Telemetry</h4>
        <form action="validate" method="POST">
          <div>
            <input type="text" name="username" placeholder={"username"} autoComplete="off"/>
          </div>
          <div>
            <input type="password" name="password" placeholder={"password"} />
          </div>
          <input type="submit" value="Login" />
        </form>
      </div>
    </div>
  );
}

export default Login;

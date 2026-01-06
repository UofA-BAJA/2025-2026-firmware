import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App";
import "./index.css";
import { BrowserRouter } from "react-router-dom";
import keycloak from "./Keycloak";

keycloak.init({ onLoad: "login-required", pkceMethod: "S256" }).then(() => {
  keycloak.onTokenExpired = () => {
    keycloak.updateToken(30);
  };
  ReactDOM.createRoot(document.getElementById("root") as HTMLElement).render(
    <React.StrictMode>
      <BrowserRouter>
        <App />
      </BrowserRouter>
    </React.StrictMode>
  );
});

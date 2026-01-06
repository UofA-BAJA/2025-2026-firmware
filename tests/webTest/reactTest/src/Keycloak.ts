import KeycloakJS from "keycloak-js";

const Keycloak = new KeycloakJS({
  // keycloak server location
  url: "http://localhost:8080",
  // realm keycloak using react-app cliant
  realm: "react-realm",
  // name of react app client
  clientId: "react-app",
});

export default Keycloak;

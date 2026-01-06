import { useEffect, useState } from "react";
import keycloak from "../Keycloak";

export default function TestSecure() {
  const [result, setResult] = useState("Calling /secure...");

  useEffect(() => {
    // refresh token if near expiry
    keycloak.updateToken(30).then(() => {
      console.log("TOKEN:", keycloak.token);
      fetch("http://localhost:5269/secure", {
        headers: {
          Authorization: `Bearer ${keycloak.token}`,
        },
      })
        .then((res) => {
          if (!res.ok) {
            throw new Error(`HTTP ${res.status}`);
          }
          return res.text();
        })
        .then((text) => setResult(text))
        .catch((err) => setResult("ERROR → " + err));
    });
  }, []);

  return (
    <div>
      <h2>Secure Endpoint Test</h2>
      <pre>{result}</pre>
    </div>
  );
}

import { Link } from "react-router-dom";

function Nave() {
  return (
    <nav
      style={{
        width: "100%",
        backgroundColor: "#1a1a1a",
        padding: "15px",
        display: "flex",
        justifyContent: "center",
        gap: "2rem",
        position: "fixed",
        top: 0,
        left: 0,
        zIndex: 1000,
      }}
    >
      <Link to="/">Home</Link>
      <Link to="/about">About</Link>
      <Link to="/contact">Contact</Link>
      <Link to="/login">Login</Link>
    </nav>
  );
}

export default Nave;

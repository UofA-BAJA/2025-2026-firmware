import { Route, Routes } from "react-router-dom";
import "./App.css";
import Home from "./Components/Home";
import Contact from "./Components/Contact";
import Nave from "./Components/Nave";
import About from "./Components/About";
import TestSecure from "./Components/TestSecure";

function App() {
  return (
    <div className="App">
      <Nave />
      <Routes>
        <Route path="/secure-test" element={<TestSecure />} />
        <Route path="/" element={<Home />} />
        <Route path="/about" element={<About />} />
        <Route path="/contact" element={<Contact />} />
      </Routes>
    </div>
  );
}

export default App;

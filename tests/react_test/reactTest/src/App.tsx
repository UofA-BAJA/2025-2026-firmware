import { useState } from "react";
import reactLogo from "./assets/react.svg";
import React from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
} from "recharts";
import "./App.css";

function App() {
  const [count, setCount] = useState(0);
  const data = [
    { id: 1, time: 0, speed: 3 },
    { id: 2, time: 5, speed: 8 },
    { id: 3, time: 10, speed: 14 },
    { id: 4, time: 15, speed: 19 },
    { id: 5, time: 20, speed: 17 },
  ];
  return (
    <div className="App">
      <div>
        <a href="https://vitejs.dev" target="_blank">
          <img src="/vite.svg" className="logo" alt="Vite logo" />
        </a>
        <a href="https://reactjs.org" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>Vite + React</h1>
      <div className="card">
        <button onClick={() => setCount((count) => count + 1)}>
          count is {count}
        </button>
        <p>
          Edit <code>src/App.tsx</code> and save to test HMR
        </p>
      </div>
      <p className="read-the-docs">
        Click on the Vite and React logos to learn more
      </p>
    </div>
  );
}

export default App;

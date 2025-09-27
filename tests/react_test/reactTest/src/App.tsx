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
  Legend,
} from "recharts";
import "./App.css";

function App() {
  const [count, setCount] = useState(0);
  const data = [
    { id: 1, time: 0, RPM: 3 },
    { id: 2, time: 5, RPM: 8 },
    { id: 3, time: 10, RPM: 14 },
    { id: 4, time: 15, RPM: 19 },
    { id: 5, time: 20, RPM: 17 },
  ];
  return (
    <div className="App">
      <div>
        <a
          href="https://github.com/UofA-BAJA/2025-2026-firmware/tree/main"
          target="_blank"
        >
          <img src="/baja_logo.jpg" className="logo" alt="Baja logo" />
        </a>
        <a href="https://reactjs.org" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>Line Graph Test</h1>
      <LineChart width={500} height={300} data={data}>
        <Line type="monotone" dataKey="RPM" stroke="#8884d8" strokeWidth={3} />
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="time" />
        <YAxis />
        <Tooltip />
        <Legend />
      </LineChart>
      <p className="read-the-docs">
        Click on the Baja and React logos to learn more
      </p>
    </div>
  );
}

export default App;

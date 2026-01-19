import { useState, useEffect } from "react";
import reactLogo from "../assets/react.svg";
import React from "react";
import axios from "axios";
import keycloak from "../Keycloak";
const rootURL = "http://localhost:5269";

import {
  LineChart,
  BarChart,
  Bar,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  Legend,
} from "recharts";

function Home() {
  type Book = {
    id: number;
    title: string;
    author: string;
    yearPublished: number;
  };

  type Temp = {
    id: number;
    epoch: number;
    value: number;
    beltId: number;
  };

  type Belt = {
    id: number;
    name: string;
  };

  const [beltName, setBeltName] = useState("");
  const [beltRef, setTempRef] = useState(0);
  const [tempValue, setTempValue] = useState(0);

  // post request to add a new belt
  async function createBelt(e: React.FormEvent) {
    await keycloak.updateToken(30);

    e.preventDefault();
    try {
      const response = await axios.post(
        `${rootURL}/Belts`,
        {
          name: beltName,
        },
        {
          headers: {
            Authorization: `Bearer ${keycloak.token}`,
            "Content-Type": "application/json",
          },
        },
      );

      //clear form input
      setBeltName("");
      alert("Belt Added");
    } catch (err) {
      alert(err);
    }
  }

  // post request to add a new belt
  async function createTemp(e: React.FormEvent) {
    e.preventDefault();
    try {
      await keycloak.updateToken(30);

      const response = await axios.post(
        `${rootURL}/temp`,
        {
          BeltId: beltRef,
          Value: tempValue,
        },
        {
          headers: {
            Authorization: `Bearer ${keycloak.token}`,
            "Content-Type": "application/json",
          },
        },
      );

      alert("Temp Added");
    } catch (err) {
      alert(err);
    }
  }

  const [count, setCount] = useState(0);
  const [belts, setBelts] = useState<Belt[]>([]);
  const [temps, setTemps] = useState<Temp[]>([]);

  // get data from localHost
  async function getAllBelts() {
    try {
      await keycloak.updateToken(30);
      const response = await axios.get(`${rootURL}/Belts`, {
        headers: {
          Authorization: `Bearer ${keycloak.token}`,
          "Content-Type": "application/json",
        },
      });
      // console.log(response.data);, might be in containers?
      setBelts(response.data);
    } catch (error) {
      console.error("Error fetching belts:", error);
    }
  }

  // get data from localHost
  async function getAllTemps() {
    try {
      await keycloak.updateToken(30);

      const response = await axios.get(`${rootURL}/temp`, {
        headers: {
          Authorization: `Bearer ${keycloak.token}`,
          "Content-Type": "application/json",
        },
      });
      // console.log(response.data);, might be in containers?
      setTemps(response.data);
    } catch (error) {
      console.error("Error fetching Temps:", error);
    }
  }

  // get all belts
  useEffect(() => {
    getAllBelts();
  }, []);

  // get all temps
  useEffect(() => {
    getAllTemps();
  }, []);

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

      <BarChart width={500} height={300} data={temps}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="beltId" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Bar dataKey="value" fill="#8884d8" />
      </BarChart>

      <div>
        <form onSubmit={createBelt}>
          <label htmlFor="belt">Belt Name: </label>
          <input
            type="text"
            id="belt"
            value={beltName}
            onChange={(e) => setBeltName(e.target.value)}
          />
          <button type="submit">Add belt</button>
        </form>
      </div>

      <div>
        <form onSubmit={createTemp}>
          <label htmlFor="tempValue">Temp value: </label>
          <input
            type="number"
            id="tempValue"
            value={tempValue}
            onChange={(e) => setTempValue(parseInt(e.target.value))}
          />
          <label htmlFor="beltRef">Belt Refrence: </label>
          <input
            type="number"
            id="beltRef"
            value={beltRef}
            onChange={(e) => setTempRef(parseInt(e.target.value))}
          />
          <button type="submit">Add Temp</button>
        </form>
      </div>
    </div>
  );
}

export default Home;

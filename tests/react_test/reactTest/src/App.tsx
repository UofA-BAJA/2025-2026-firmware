import { useState, useEffect } from "react";
import reactLogo from "./assets/react.svg";
import React from "react";
import axios from "axios";
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
import "./App.css";

function App() {
  type Book = {
    id: number;
    title: string;
    author: string;
    yearPublished: number;
  };

  const [count, setCount] = useState(0);
  const [books, setBooks] = useState<Book[]>([]);
  // get data from localHost
  async function getAllBooks() {
    try {
      const response = await axios.get(`${rootURL}/api/books`);
      console.log(response.data)
      setBooks(response.data);
    } catch (error) {
      console.error("Error fetching books:", error);
    }
  }

  useEffect(() => {
    getAllBooks();
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
      <LineChart width={500} height={300} data={books}>
        <Line type="monotone" dataKey="yearPublished" stroke="#8884d8" strokeWidth={3} />
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="id" />
        <YAxis />
        <Tooltip />
        <Legend />
      </LineChart>
      <p className="read-the-docs">
        Click on the Baja and React logos to learn more
      </p>
      {/* <h1>Books Publication Years</h1>
      <BarChart width={800} height={400} data={books}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="title" />
        <YAxis />
        <Tooltip
          formatter={(value, name, props) => [value, "Year Published"]}
          labelFormatter={(label) => {
            const book = books.find((b) => b.title === label);
            return book ? `${book.title} by ${book.author}` : label;
          }}
        />
        <Legend />
        <Bar dataKey="yearPublished" fill="#8884d8" />
      </BarChart> */}
    </div>
  );
}

export default App;

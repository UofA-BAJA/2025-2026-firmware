# Webapp Specification

This document will serve as a specifcation of the Baja Web App. It will describe the technology stack used and the architecture of the app.

https://github.com/anderspitman/awesome-tunneling?tab=readme-ov-file

ASP.NET for REST API

JWT Authentication

React for frontend

PostgreSQL

## Frontend

The frontend will serve multiple purposes. It will consist of two main modes: live and analysis.

Live mode will be used to see realtime car data that is being streamed from the car to the pit. Live mode needs to work even without internet connection, as internet connection is not guaranteed at testing sites or competition sites. Another (potential) feature of live mode would be to have one client sending data to the server over a web socket, and other clients reading that data. This way, one person would essentially be "broadcasting" the data, while all the other people are "streaming" it.

Analysis mode will be used to store and retrieve data from the past. The data will be stored and retrieved via REST endpoints. All of the data handling in analysis mode will be done from the backend. However, the frontend of analysis mode should handle a few things. First, it should be able to display the data in basic charts (scatter plot, line graph, bar chart, etc.). Next, it should give users the option to download the data as a csv file or a JSON file.

Realistically, it does not matter what framework the frontend is built on. As long as the framework can plot data. Other than that, it comes down to personal preference. React could be a solid choice because it is very well documented and supported, but some sources say that it is not necessarily the easiest to use. Vue, flutter, or any other framework are also perfectly viable. 

Note: All authentication will be handled in the backend

## Backend

The backend will be built using a REST API. It will have certain endpoints that the user can use to GET, POST, PUT, and DELETE data. When the user sends a query through the frontend, the query gets sent to the backend, where the server processes the request and returns whatever information the user should see.

Another optional feature of the backend is to have websockets that can stream live data, as described in the frontend section.

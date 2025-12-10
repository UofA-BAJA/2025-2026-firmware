# Webapp Specification

This document will serve as a specifcation of the Baja Web App. It will describe the technology stack used and the architecture of the app. None of this document is final, so everything is subject to change.

## Frontend

The frontend will serve multiple purposes. It will consist of two main modes: live and analysis.

Live mode will be used to see realtime car data that is being streamed from the car to the pit. Live mode needs to work even without internet connection, as internet connection is not guaranteed at testing sites or competition sites. Another (potential) feature of live mode would be to have one client sending data to the server over a web socket, and other clients reading that data. This way, one person would essentially be "broadcasting" the data, while all the other people are "streaming" it.

Analysis mode will be used tio store and retrieve data from the past. The data will be stored and retrieved via REST endpoints. All of the data handling in analysis mode will be done from the backend. However, the frontend of analysis mode should handle a few things. First, it should be able to display the data in basic charts (scatter plot, line graph, bar chart, etc.). Next, it should give users the option to download the data as a csv file or a JSON file.

Realistically, it does not matter what framework the frontend is built on. As long as the framework can plot data. Other than that, it comes down to personal preference. React could be a solid choice because it is very well documented and supported, but some sources say that it is not necessarily the easiest to use. Vue, flutter, or any other framework are also perfectly viable.

Note: All authentication will be handled in the backend

## Backend

The backend will be built using a REST API. It will have certain endpoints that the user can use to GET, POST, PUT, and DELETE data. When the user sends a query through the frontend, the query gets sent to the backend, where the server processes the request and returns whatever information the user should see.

Another optional feature of the backend is to have websockets that can stream live data, as described in the frontend section.

Endpoints might look like the following:

*www.bajatelemetry.com/api/imu*

*www.bajatelemetry.com/api/imu?{insert parameters}*

*www.bajatelemetry.com/api/temperature*

www.bajatelemetry.com/api/live?{insert parameters}

These endpoints would have the ability to query based on:

* Racing team
* Car
* Session (still deciding about session)
* Time

Furthermore, when ingesting data, these fields are not optional. 

Another important requirement is authentification and authorization. Realistically, anybody should be able to make an account for the website. However, not everybody should be able to interact with the endpoints in the same way. In a hypothetical situation, many teams could be using this platform. There are some teams that do not want to share their data at all, while other teams might be more open to sharing data. Teams need to have the ability to provide access

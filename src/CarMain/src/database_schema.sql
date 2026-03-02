------------------------------------------------------------------------------------
-- The big idea behind this schema is that the database is stored in a single file.
-- Every time the "new session" command is send from the pit, a new .db file is
-- created. Associated with this file is a timestamp in the form YYYY-MM-DDTHH:SSZ
-- (conforming to the ISO 8601 standard)
-- The z means "Zulu time", otherwise known as an offset of 0 from UTC. (The name of 
-- the file is really only used as a unique identifier, so it's not actually that
-- important)
--
-- [The paragraph below is still theoretical and a WIP]
-- All of these sessions are stored locally on a flash drive on the car. Once 
-- somebody with the flash drive has access to the Postgres server, they can sync
-- up all of the locally stored data with the rest of the data. The syncing process
-- will also clear the data on the flash drive, eliminating the need to manually 
-- clean it. Each .db file will be treated as its own unique session in the Postgres
-- database
--
-- [Fever dream goal below]
-- Eventually, it would be really cool to set up a web service that uses the postgres
-- database in combination with a data visualizer (Grafana?). It would also be really
-- cool to have this as a service that other teams could set up with their own 
-- telemetry systems.
--
-- Originally written by Matthew Larson
-- Started August 27, 2025
------------------------------------------------------------------------------------



-- Why aren't foreign keys enabled by default? No clue.
PRAGMA foreign_keys = ON;

-- Performance optimization (add more information here)
PRAGMA journal_mode = WAL;
-- Another performance optimization. Should probably give some more information
PRAGMA synchronous = NORMAL;


PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;

CREATE TABLE IF NOT EXISTS vehicle(
    name TEXT PRIMARY KEY,
    competition_year INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS sensor(
    name TEXT PRIMARY KEY,
    manufacturer TEXT NOT NULL,
    model TEXT NOT NULL,
    sensor_type TEXT NOT NULL,
    description TEXT
);

CREATE TABLE IF NOT EXISTS log(
    id INTEGER PRIMARY KEY,
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    type TEXT NOT NULL,
    message TEXT,
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS imu(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    sensor TEXT NOT NULL,
    ax REAL,
    ay REAL,
    az REAL,
    a_unit TEXT GENERATED ALWAYS AS ('m/s^2') VIRTUAL,
    PRIMARY KEY (epoch_ms, vehicle, sensor),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE,
    FOREIGN KEY(sensor) REFERENCES sensor(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS gps(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    sensor TEXT NOT NULL,
    lat REAL,
    lon REAL,
    alt REAL,
    PRIMARY KEY (epoch_ms, vehicle, sensor),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE,
    FOREIGN KEY(sensor) REFERENCES sensor(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS pressure(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    sensor TEXT NOT NULL,
    psi REAL,
    PRIMARY KEY (epoch_ms, vehicle, sensor),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE,
    FOREIGN KEY(sensor) REFERENCES sensor(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS linear_actuator(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    sensor TEXT NOT NULL,
    displacement REAL,
    displacement_unit TEXT GENERATED ALWAYS AS ('m') VIRTUAL,
    PRIMARY KEY (epoch_ms, vehicle, sensor),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE,
    FOREIGN KEY(sensor) REFERENCES sensor(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS temperature(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    sensor TEXT NOT NULL,
    temp REAL,
    temp_unit TEXT GENERATED ALWAYS AS ('°C') VIRTUAL,
    PRIMARY KEY (epoch_ms, vehicle, sensor),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE,
    FOREIGN KEY(sensor) REFERENCES sensor(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS tachometer(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    sensor TEXT NOT NULL,
    rpm REAL,
    PRIMARY KEY (epoch_ms, vehicle, sensor),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE,
    FOREIGN KEY(sensor) REFERENCES sensor(name) ON DELETE CASCADE
);


CREATE TABLE IF NOT EXISTS vehicle_state(
    epoch_ms INTEGER NOT NULL DEFAULT (unixepoch('now') * 1000),
    vehicle TEXT NOT NULL,
    speed REAL,
    dist REAL,
    speed_unit TEXT GENERATED ALWAYS AS ('m/s') VIRTUAL,
    dist_unit TEXT GENERATED ALWAYS AS ('m') VIRTUAL,
    PRIMARY KEY (epoch_ms, vehicle),
    FOREIGN KEY(vehicle) REFERENCES vehicle(name) ON DELETE CASCADE
);
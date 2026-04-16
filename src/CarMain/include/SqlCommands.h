#ifndef SQLCOMMANDS_H
#define SQLCOMMANDS_H

// It's hard to read from files during runtime because you don't necessarily know where the file is.
// Because of that, it's a lot easier to hard code the sql commands in some file like this one.

namespace BajaWildcatRacing
{

    // inline allows for multiple declarations
    inline const char* database_schema = R"SQL(
    PRAGMA journal_mode = WAL;
    PRAGMA synchronous = NORMAL;

    PRAGMA compile_options;

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
    
    )SQL";




}

#endif
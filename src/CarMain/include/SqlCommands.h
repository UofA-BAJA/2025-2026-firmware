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
        rx REAL,
        ry REAL,
        rz REAL,
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

    inline const char* insert_vehicle = R"SQL(
    INSERT INTO vehicle (name, competition_year)
    VALUES (?, ?);
    )SQL";

    inline const char* insert_sensor = R"SQL(
        INSERT INTO sensor (name, manufacturer, model, sensor_type, description)
        VALUES (?, ?, ?, ?, ?);
    )SQL";

    inline const char* insert_log = R"SQL(
        INSERT INTO log (epoch_ms, vehicle, type, message)
        VALUES (?, ?, ?, ?);
    )SQL";

    inline const char* insert_imu = R"SQL(
        INSERT INTO imu (epoch_ms, vehicle, sensor, ax, ay, az, rz, ry, rz)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
    )SQL";

    inline const char* insert_gps = R"SQL(
        INSERT INTO gps (epoch_ms, vehicle, sensor, lat, lon, alt)
        VALUES (?, ?, ?, ?, ?, ?);
    )SQL";

    inline const char* insert_pressure = R"SQL(
        INSERT INTO pressure (epoch_ms, vehicle, sensor, psi)
        VALUES (?, ?, ?, ?);
    )SQL";

    inline const char* insert_linear_actuator = R"SQL(
        INSERT INTO linear_actuator (epoch_ms, vehicle, sensor, displacement)
        VALUES (?, ?, ?, ?);
    )SQL";

    inline const char* insert_temperature = R"SQL(
        INSERT INTO temperature (epoch_ms, vehicle, sensor, temp)
        VALUES (?, ?, ?, ?);
    )SQL";

    inline const char* insert_tachometer = R"SQL(
        INSERT INTO tachometer (epoch_ms, vehicle, sensor, rpm)
        VALUES (?, ?, ?, ?);
    )SQL";

    inline const char* insert_vehicle_state = R"SQL(
        INSERT INTO vehicle_state (epoch_ms, vehicle, speed, dist)
        VALUES (?, ?, ?, ?);
    )SQL";

}

#endif
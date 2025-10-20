--belt table example
CREATE TABLE belt (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

--temperature of stuff example
CREATE TABLE temperature (
    id SERIAL PRIMARY KEY,
    --unix epoch time
    epoch INT NOT NULL,
    --temperature value
    value INT NOT NULL,
    --foreign key reference
    belt_id INT NOT NULL,
    FOREIGN KEY (belt_id) REFERENCES belt(id)
);

--starter belt data
INSERT INTO belt (name) VALUES
('Belt A'),
('Belt B'),
('Belt C');

--starter temperature data
INSERT INTO temperature (epoch, value, belt_id) VALUES
(1700000000, 75, 1),
(1700003600, 78, 2),
(1700007200, 80, 3)
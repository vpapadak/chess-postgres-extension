DROP DATABASE IF EXISTS games_db;
DROP TABLE IF EXISTS games_100;
DROP TABLE IF EXISTS games_25;
DROP TABLE IF EXISTS games_5;
DROP TABLE IF EXISTS games_1;
-- DROP EXTENSION IF EXISTS chess; <---- to uncomment to run our extension

-- CREATE EXTENSION chess; <---- also this has to be uncommented

CREATE DATABASE games_db;

CREATE TABLE games_100 (
    id SERIAL PRIMARY KEY,
    san VARCHAR(255),
    player VARCHAR(255),
    game_site VARCHAR(255)
);

CREATE TABLE games_20 (
    id SERIAL PRIMARY KEY,
    san VARCHAR(255),
    player VARCHAR(255),
    game_site VARCHAR(255)
);

CREATE TABLE games_5 (
    id SERIAL PRIMARY KEY,
    san VARCHAR(255),
    player VARCHAR(255),
    game_site VARCHAR(255)
);

CREATE TABLE games_1 (
    id SERIAL PRIMARY KEY,
    san VARCHAR(255),
    player VARCHAR(255),
    game_site VARCHAR(255)
);

COPY games_100(san, player, game_site)
FROM 'C:\\Users\\valer\\Desktop\\DBSA\\project\\chess-postgres-extension\\test_database\\csv_games_100.csv' DELIMITER ',' CSV HEADER;

COPY games_20(san, player, game_site)
FROM 'C:\\Users\\valer\\Desktop\\DBSA\\project\\chess-postgres-extension\\test_database\\csv_games_20.csv' DELIMITER ',' CSV HEADER;

COPY games_5(san, player, game_site)
FROM 'C:\\Users\\valer\\Desktop\\DBSA\\project\\chess-postgres-extension\\test_database\\csv_games_5.csv' DELIMITER ',' CSV HEADER;

COPY games_1(san, player, game_site)
FROM 'C:\\Users\\valer\\Desktop\\DBSA\\project\\chess-postgres-extension\\test_database\\csv_games_1.csv' DELIMITER ',' CSV HEADER;
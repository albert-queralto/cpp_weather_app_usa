#include "weather_database.h"
#include <pqxx/pqxx>
#include <iostream>
#include <thread>
#include <chrono>

void WeatherDatabase::store_observation(
    const std::string& station_id, 
    const std::string& parameter_name,
    const std::string& parameter_unit,
    double value,
    const std::string& observed_at
) {
    int retries = 10;
    while (retries--) {
        try {
            pqxx::connection conn("postgresql://weather:weatherpass@database:5432/weatherdb");
            pqxx::work txn(conn);

            txn.exec(R"(
                CREATE TABLE IF NOT EXISTS weather_observations (
                    id SERIAL PRIMARY KEY,
                    station VARCHAR(10),
                    parameter_name VARCHAR(50) DEFAULT 'temperature',
                    parameter_unit VARCHAR(10) DEFAULT 'Celsius',
                    value DOUBLE PRECISION,
                    observed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            )");

            txn.exec_params(
                "INSERT INTO weather_observations (station, parameter_name, parameter_unit, value, observed_at) VALUES ($1, $2, $3, $4, $5)",
                station_id, parameter_name, parameter_unit, value, observed_at
            );
            txn.commit();
            std::cout << "[DB] Inserted: " << station_id << " " << parameter_name << " = " << value << " " << parameter_unit << " at " << observed_at << std::endl;
            break;
        } catch (const std::exception& e) {
            std::cerr << "[DB ERROR] Database error: " << e.what() << std::endl;
            if (retries == 0) break;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cerr << "[DB] Retrying database connection..." << std::endl;
        }
    }
}

void WeatherDatabase::store_station_coordinates(
    const std::string& station_id,
    double longitude,
    double latitude
) {
    int retries = 10;
    while (retries--) {
        try {
            pqxx::connection conn("postgresql://weather:weatherpass@database:5432/weatherdb");
            pqxx::work txn(conn);

            txn.exec(R"(
                CREATE TABLE IF NOT EXISTS weather_stations (
                    id SERIAL PRIMARY KEY,
                    station_identifier VARCHAR(20) UNIQUE,
                    longitude DOUBLE PRECISION,
                    latitude DOUBLE PRECISION,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            )");

            txn.exec_params(
                "INSERT INTO weather_stations (station_identifier, longitude, latitude) VALUES ($1, $2, $3) "
                "ON CONFLICT (station_identifier) DO UPDATE SET longitude = EXCLUDED.longitude, latitude = EXCLUDED.latitude, updated_at = CURRENT_TIMESTAMP",
                station_id, longitude, latitude
            );
            txn.commit();
            std::cout << "[DB] Stored station: " << station_id << " (" << longitude << ", " << latitude << ")" << std::endl;
            break;
        } catch (const std::exception& e) {
            std::cerr << "[DB ERROR] Database error (station): " << e.what() << std::endl;
            if (retries == 0) break;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
}
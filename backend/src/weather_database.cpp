#include "weather_database.h"
#include <pqxx/pqxx>
#include <iostream>
#include <thread>
#include <chrono>

constexpr const char* DB_CONN_STR = "postgresql://weather:weatherpass@database:5432/weatherdb";

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
            pqxx::connection conn(DB_CONN_STR);
            pqxx::work txn(conn);

            txn.exec(R"(
                CREATE TABLE IF NOT EXISTS weather_observations (
                    id SERIAL PRIMARY KEY,
                    station VARCHAR(10),
                    parameter_name VARCHAR(50) DEFAULT 'temperature',
                    parameter_unit VARCHAR(50) DEFAULT 'Celsius',
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
            pqxx::connection conn(DB_CONN_STR);
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

nlohmann::json WeatherDatabase::fetch_all_stations() {
    nlohmann::json result = nlohmann::json::array();
    try {
        pqxx::connection conn(DB_CONN_STR);
        pqxx::work txn(conn);

        auto rows = txn.exec("SELECT station_identifier, longitude, latitude FROM weather_stations");
        for (const auto& row : rows) {
            nlohmann::json station;
            station["station_identifier"] = row["station_identifier"].c_str();
            station["longitude"] = row["longitude"].as<double>();
            station["latitude"] = row["latitude"].as<double>();
            result.push_back(station);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DB ERROR] Failed to fetch stations: " << e.what() << std::endl;
        return nlohmann::json::array();
    }
    return result;
}

nlohmann::json WeatherDatabase::fetch_latest_observation(const std::string& station_id) {
    nlohmann::json result;
    try {
        pqxx::connection conn(DB_CONN_STR);
        pqxx::work txn(conn);

        auto rows = txn.exec_params(
            "SELECT parameter_name, parameter_unit, value, observed_at "
            "FROM weather_observations "
            "WHERE station = $1 "
            "ORDER BY observed_at DESC LIMIT 1",
            station_id
        );
        if (rows.empty()) {
            const auto& row = rows[0];
            result["station"] = station_id;
            result["parameter_name"] = row["parameter_name"].c_str();
            result["parameter_unit"] = row["parameter_unit"].c_str();
            result["value"] = row["value"].as<double>();
            result["observed_at"] = row["observed_at"].c_str();
        }
    } catch (const std::exception& e) {
        std::cerr << "[DB ERROR] Failed to fetch latest observation for station " << station_id << ": " << e.what() << std::endl;
        return nlohmann::json();
    }
    return result;
}

nlohmann::json WeatherDatabase::fetch_observations_in_range(
    const std::string& station_id,
    const std::string& start_time,
    const std::string& end_time
) {
    nlohmann::json result = nlohmann::json::array();
    try {
        pqxx::connection conn(DB_CONN_STR);
        pqxx::work txn(conn);

        auto rows = txn.exec_params(
            "SELECT parameter_name, parameter_unit, value, observed_at "
            "FROM weather_observations "
            "WHERE station = $1 AND observed_at BETWEEN $2 AND $3 "
            "ORDER BY observed_at ASC",
            station_id, start_time, end_time
        );

        for (const auto& row : rows) {
            nlohmann::json obs;
            obs["station"] = station_id;
            obs["parameter_name"] = row["parameter_name"].c_str();
            obs["parameter_unit"] = row["parameter_unit"].c_str();
            obs["value"] = row["value"].as<double>();
            obs["observed_at"] = row["observed_at"].c_str();
            result.push_back(obs);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DB ERROR] Failed to fetch observations in range for station " << station_id << ": " << e.what() << std::endl;
        return nlohmann::json::array();
    }
    return result;
}

nlohmann::json WeatherDatabase::fetch_all_variables() {
    nlohmann::json result = nlohmann::json::array();
    try {
        pqxx::connection conn(DB_CONN_STR);
        pqxx::work txn(conn);

        auto rows = txn.exec("SELECT DISTINCT parameter_name FROM weather_observations");
        for (const auto& row : rows) {
            result.push_back(row["parameter_name"].c_str());
        }
    } catch (const std::exception& e) {
        std::cerr << "[DB ERROR] Failed to fetch all variables: " << e.what() << std::endl;
        return nlohmann::json::array();
    }
    return result;
}
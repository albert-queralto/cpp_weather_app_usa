#pragma once
#include <string>
#include <nlohmann/json.hpp>

class WeatherDatabase {
public:
    WeatherDatabase();

    void store_observation(
        const std::string& station_id, 
        const std::string& parameter_name,
        const std::string& parameter_unit,
        double value,
        const std::string& observed_at
    );
    
    void store_station_coordinates(
        const std::string& station_id,
        double longitude,
        double latitude
    );

    nlohmann::json fetch_all_stations();
    nlohmann::json fetch_latest_observation(const std::string& station_id);
    nlohmann::json fetch_observations_in_range(
        const std::string& station_id,
        const std::string& start_time,
        const std::string& end_time
    );
    nlohmann::json fetch_all_variables();
};
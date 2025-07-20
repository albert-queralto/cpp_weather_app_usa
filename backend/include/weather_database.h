#pragma once
#include <string>

class WeatherDatabase {
public:
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
};
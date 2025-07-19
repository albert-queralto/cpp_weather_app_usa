#pragma once
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <string>

class WeatherApiClient {
public:
    static constexpr const char* BASE_URL = "https://api.weather.gov";
    static const cpr::Header DEFAULT_HEADER;

    nlohmann::json fetch_all_stations() const;
    nlohmann::json fetch_observation_history(
        const std::string& station_id,
        const std::string& start_time = "",
        const std::string& end_time = "",
        int limit = 100
    ) const;
    nlohmann::json fetch_observation_latest(const std::string& station_id) const;

};
#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include "weather_database.h"
#include "weather_api_client.h"

namespace WeatherDataHelpers {

struct ParamInfo {
    std::string json_name;
    std::string db_name;
};

extern const std::vector<ParamInfo> WEATHER_PARAMS;

std::vector<std::string> extract_station_ids(const nlohmann::json& stations_json);

void store_all_parameters_from_feature(
    WeatherDatabase& db,
    const std::string& station_id,
    const nlohmann::json& feature
);

void store_latest_observation_for_station(
    WeatherDatabase& db,
    WeatherApiClient& api_client,
    const std::string& station_id
);

void store_all_station_coordinates(
    WeatherDatabase& db,
    const nlohmann::json& stations_json
);

std::string to_iso8601(std::time_t time);

}
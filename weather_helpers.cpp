#include "weather_helpers.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>

namespace WeatherDataHelpers {

const std::vector<ParamInfo> WEATHER_PARAMS = {
    {"temperature", "temperature"},
    {"dewpoint", "dewpoint"},
    {"windDirection", "windDirection"},
    {"windSpeed", "windSpeed"},
    {"windGust", "windGust"},
    {"barometricPressure", "barometricPressure"},
    {"seaLevelPressure", "seaLevelPressure"},
    {"visibility", "visibility"},
    {"maxTemperatureLast24Hours", "maxTemperatureLast24Hours"},
    {"minTemperatureLast24Hours", "minTemperatureLast24Hours"},
    {"precipitationLastHour", "precipitationLastHour"},
    {"precipitationLast3Hours", "precipitationLast3Hours"},
    {"precipitationLast6Hours", "precipitationLast6Hours"},
    {"relativeHumidity", "relativeHumidity"},
    {"windChill", "windChill"},
    {"heatIndex", "heatIndex"}
};

std::vector<std::string> extract_station_ids(const nlohmann::json& stations_json) {
    std::vector<std::string> ids;
    if (stations_json.contains("features") && stations_json["features"].is_array()) {
        for (const auto& feature : stations_json["features"]) {
            if (feature.contains("properties") &&
                feature["properties"].contains("stationIdentifier") &&
                feature["properties"]["stationIdentifier"].is_string()) {
                ids.push_back(feature["properties"]["stationIdentifier"].get<std::string>());
            }
        }
    }
    std::cout << "[INFO] Extracted " << ids.size() << " station IDs." << std::endl;
    return ids;
}

void store_all_parameters_from_feature(
    WeatherDatabase& db,
    const std::string& station_id,
    const nlohmann::json& feature
) {
    if (!feature.contains("properties")) return;
    const auto& properties = feature["properties"];
    std::string observed_at = properties.value("timestamp", "");
    for (const auto& param : WEATHER_PARAMS) {
        if (properties.contains(param.json_name) && properties[param.json_name].contains("value")) {
            double value = properties[param.json_name]["value"].is_null() ? std::numeric_limits<double>::quiet_NaN() : properties[param.json_name]["value"].get<double>();
            std::string unit = properties[param.json_name].value("unitCode", "");
            if (!std::isnan(value)) {
                std::cout << "[INFO] Storing parameter " << param.db_name << " for station " << station_id << std::endl;
                db.store_observation(station_id, param.db_name, unit, value, observed_at);
                std::cout << "[DB] Stored observation: " << station_id << " " << param.db_name
                          << " = " << value << " " << unit << " at " << observed_at << std::endl;
            }
        }
    }
}

void store_latest_observation_for_station(
    WeatherDatabase& db,
    WeatherApiClient& api_client,
    const std::string& station_id
) {
    auto obs_json = api_client.fetch_observation_latest(station_id);
    if (obs_json.is_null()) {
        std::cerr << "[ERROR] No latest observation data for station " << station_id << std::endl;
        return;
    }
    // The returned JSON is a single Feature, not a FeatureCollection
    WeatherDataHelpers::store_all_parameters_from_feature(db, station_id, obs_json);
}

void store_all_station_coordinates(
    WeatherDatabase& db,
    const nlohmann::json& stations_json
) {
    std::cout << "[INFO] Storing all station coordinates..." << std::endl;
    if (stations_json.contains("features") && stations_json["features"].is_array()) {
        for (const auto& feature : stations_json["features"]) {
            if (feature.contains("properties") &&
                feature["properties"].contains("stationIdentifier") &&
                feature["properties"]["stationIdentifier"].is_string() &&
                feature.contains("geometry") &&
                feature["geometry"].contains("coordinates") &&
                feature["geometry"]["coordinates"].is_array() &&
                feature["geometry"]["coordinates"].size() == 2) {
                std::string station_id = feature["properties"]["stationIdentifier"].get<std::string>();
                double longitude = feature["geometry"]["coordinates"][0].get<double>();
                double latitude = feature["geometry"]["coordinates"][1].get<double>();
                db.store_station_coordinates(station_id, longitude, latitude);
            }
        }
    }
    std::cout << "[INFO] Finished storing station coordinates." << std::endl;
}

std::string to_iso8601(std::time_t time) {
    std::ostringstream ss;
    ss << std::put_time(gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

} // namespace WeatherDataHelpers
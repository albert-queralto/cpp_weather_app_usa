#include "weather_api_client.h"
#include <iostream>

const cpr::Header WeatherApiClient::DEFAULT_HEADER = {{"User-Agent", "(myweatherapp, albert.queralto.lopez@gmail.com)"}};

nlohmann::json WeatherApiClient::fetch_all_stations() const {
    std::cout << "[INFO] Fetching all stations..." << std::endl;
    std::string url = std::string(BASE_URL) + "/stations/";
    cpr::Response r = cpr::Get(cpr::Url{url}, DEFAULT_HEADER);
    std::cout << "[INFO] Stations fetch HTTP status: " << r.status_code << std::endl;
    if (r.status_code != 200) {
        std::cerr << "[ERROR] Failed to fetch stations: " << r.status_code << std::endl;
        return nullptr;
    }
    std::cout << "[INFO] Successfully fetched stations." << std::endl;
    return nlohmann::json::parse(r.text);
}

nlohmann::json WeatherApiClient::fetch_observation_history(
    const std::string& station_id,
    const std::string& start_time,
    const std::string& end_time,
    int limit
) const {
    std::cout << "[INFO] Fetching observations for station: " << station_id
              << " from " << start_time << " to " << end_time << std::endl;
    std::string url = std::string(BASE_URL) + "/stations/" + station_id + "/observations";
    cpr::Parameters params{};
    if (!start_time.empty()) params.Add({"start", start_time});
    if (!end_time.empty()) params.Add({"end", end_time});
    params.Add({"limit", std::to_string(limit)});

    cpr::Response r = cpr::Get(
        cpr::Url{url},
        DEFAULT_HEADER,
        params
    );
    std::cout << "[INFO] Observations fetch HTTP status: " << r.status_code << std::endl;
    std::cout << "[DEBUG] Response text: " << r.text << std::endl;
    if (r.text.empty()) {
        std::cerr << "[ERROR] No data returned for station " << station_id << std::endl;
        return nullptr;
    }
    if (r.status_code != 200) {
        std::cerr << "[ERROR] Failed to fetch observation history for " << station_id << ": " << r.status_code << std::endl;
        return nullptr;
    }
    return nlohmann::json::parse(r.text);
}

nlohmann::json WeatherApiClient::fetch_observation_latest(
    const std::string& station_id
) const {
    std::cout << "[INFO] Fetching observations for station: " << station_id << std::endl;
    std::string url = std::string(BASE_URL) + "/stations/" + station_id + "/observations/latest";
    cpr::Response r = cpr::Get(cpr::Url{url}, DEFAULT_HEADER);
    std::cout << "[INFO] Latest observation fetch HTTP status: " << r.status_code << std::endl;
    if (r.status_code != 200) {
        std::cerr << "[ERROR] Failed to fetch latest observation for " << station_id << ": " << r.status_code << std::endl;
        return nullptr;
    }
    return nlohmann::json::parse(r.text);
}
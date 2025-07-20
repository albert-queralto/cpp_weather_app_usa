#include "weather_api_client.h"
#include "weather_database.h"
#include "weather_helpers.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "[START] Weather data fetcher started." << std::endl;
    WeatherApiClient api_client;
    WeatherDatabase db;

    auto stations_json = api_client.fetch_all_stations();
    if (stations_json.is_null()) {
        std::cerr << "[FATAL] No stations data available." << std::endl;
        return 1;
    }

    WeatherDataHelpers::store_all_station_coordinates(db, stations_json);
    std::vector<std::string> station_ids = WeatherDataHelpers::extract_station_ids(stations_json);

    while (true) {
        std::cout << "[LOOP] Starting new fetch cycle." << std::endl;
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto one_hour_ago = now - std::chrono::hours(1);
        auto one_hour_ago_time_t = std::chrono::system_clock::to_time_t(one_hour_ago);

        std::string end_time = WeatherDataHelpers::to_iso8601(now_time_t);
        std::string start_time = WeatherDataHelpers::to_iso8601(one_hour_ago_time_t);

        for (const auto& station_id : station_ids) {
            std::cout << "[LOOP] Fetching observations for station: " << station_id << std::endl;
            // auto obs_json = api_client.fetch_observation_history(
            //     station_id,
            //     start_time,
            //     end_time,
            //     100
            // );
            // if (!obs_json.is_null() && obs_json.contains("features")) {
            //     for (const auto& feature : obs_json["features"]) {
            //         WeatherDataHelpers::store_all_parameters_from_feature(db, station_id, feature);
            //     }
            // }
            WeatherDataHelpers::store_latest_observation_for_station(db, api_client, station_id);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "[LOOP] Cycle complete. Sleeping for 30 minutes..." << std::endl;
        std::this_thread::sleep_for(std::chrono::minutes(30));
    }

    return 0;
}
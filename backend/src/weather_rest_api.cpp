#include "weather_rest_api.h"

WeatherRestApi::WeatherRestApi(WeatherApiClient& api_client, WeatherDatabase& db)
    : api_client_(api_client), db_(db) {
    setup_routes();
}

void WeatherRestApi::setup_routes() {
    // GET /api/stations - list all stations from the database
    CROW_ROUTE(app_, "/api/stations")
    ([this]() {
        auto stations_json = db_.fetch_all_stations();
        return crow::response(stations_json.dump());
    });

    // GET /api/observations/latest?station_id=<id> - fetch latest observation for a station
    CROW_ROUTE(app_, "/api/observations/latest")
    .methods("GET"_method)
    ([this](const crow::request& req) {
        auto station_id = req.url_params.get("station_id");
        if (!station_id) {
            return crow::response(400, "Missing station_id parameter");
        }

        auto obs_json = db_.fetch_latest_observation(station_id);
        if (obs_json.is_null()) {
            return crow::response(404, "No observations found for station");
        }
        return crow::response(obs_json.dump());
    });

    // GET /api/observations/range?station_id=<id>&start=<iso8601>&end=<iso8601>
    CROW_ROUTE(app_, "/api/observations/range")
    .methods("GET"_method)
    ([this](const crow::request& req) {
        auto station_id = req.url_params.get("station_id");
        auto start_time = req.url_params.get("start");
        auto end_time = req.url_params.get("end");

        if (!station_id || !start_time || !end_time) {
            return crow::response(400, "Missing station_id, start, or end parameters");
        }

        auto obs_json = db_.fetch_observations_in_range(station_id, start_time, end_time);
        if (obs_json.empty()) {
            return crow::response(404, "No observations found for station in range");
        }
        return crow::response(obs_json.dump());
    });

    // GET /api/variables - list all variables from the database
    CROW_ROUTE(app_, "/api/variables")
    ([this]() {
        auto variables_json = db_.fetch_all_variables();
        crow::response res(variables_json.dump());
        res.add_header("Content-Type", "application/json");
        res.add_header("Cache-Control", "no-store");
        return res;
    });

    // GET /api/station_ids - list all station IDs from the database
    CROW_ROUTE(app_, "/api/station_ids")
    ([this]() {
        try {
            auto stations_json = db_.fetch_all_stations();
            nlohmann::json ids = nlohmann::json::array();
            for (const auto& station : stations_json) {
                if (station.contains("station_identifier")) {
                    ids.push_back(station["station_identifier"]);
                }
            }
            crow::response res(ids.dump());
            res.add_header("Content-Type", "application/json");
            res.add_header("Cache-Control", "no-store");
            return res;
        } catch (const std::exception& e) {
            crow::response res("{\"error\":\"Internal server error\"}");
            res.code = 500;
            res.add_header("Content-Type", "application/json");
            return res;
        }
    });
}

void WeatherRestApi::run(int port) {
    app_.port(port).multithreaded().run();
}
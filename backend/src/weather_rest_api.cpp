#include "weather_rest_api.h"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include "utils.h"
#include <pqxx/pqxx>
#include "db_config.h"


WeatherRestApi::WeatherRestApi(WeatherApiClient& api_client, WeatherDatabase& db)
    : api_client_(api_client), db_(db) {
    setup_routes();
}

void WeatherRestApi::setup_routes() {
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
        crow::response res(obs_json.dump());
        res.add_header("Content-Type", "application/json");
        return res;
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
        crow::response res(obs_json.dump());
        res.add_header("Content-Type", "application/json");
        return res;
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

    // GET /api/stations - list all stations with coordinates
    CROW_ROUTE(app_, "/api/stations")
    ([this]() {
        auto stations_json = db_.fetch_all_stations();
        crow::response res(stations_json.dump());
        res.add_header("Content-Type", "application/json");
        res.add_header("Cache-Control", "no-store");
        return res;
    });

    CROW_ROUTE(app_, "/api/signup").methods("POST"_method)
    ([this](const crow::request& req) {
        auto body = nlohmann::json::parse(req.body, nullptr, false);
        if (!body.contains("email") || !body.contains("password"))
            return crow::response(400, "Missing email or password");

        std::string email = body["email"];
        std::string password = body["password"];
        std::string hash = sha256(password);

        try {
            pqxx::connection conn(DB_CONN_STR);
            pqxx::work txn(conn);
            txn.exec_params(
                "INSERT INTO users (email, password_hash) VALUES ($1, $2) ON CONFLICT DO NOTHING",
                email, hash
            );
            txn.commit();
            return crow::response(201, "User created");
        } catch (...) {
            return crow::response(500, "Signup failed");
        }
    });

    CROW_ROUTE(app_, "/api/login").methods("POST"_method)
    ([this](const crow::request& req) {
        auto body = nlohmann::json::parse(req.body, nullptr, false);
        if (!body.contains("email") || !body.contains("password"))
            return crow::response(400, "Missing email or password");

        std::string email = body["email"];
        std::string password = body["password"];
        std::string hash = sha256(password);

        try {
            pqxx::connection conn(DB_CONN_STR);
            pqxx::work txn(conn);
            auto rows = txn.exec_params(
                "SELECT id FROM users WHERE email = $1 AND password_hash = $2",
                email, hash
            );
            if (rows.empty())
                return crow::response(401, "Invalid credentials");
            return crow::response(200, "Login successful");
        } catch (...) {
            return crow::response(500, "Login failed");
        }
    });



}

void WeatherRestApi::run(int port) {
    app_.port(port).multithreaded().run();
}
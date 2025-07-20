#pragma once
#include "weather_api_client.h"
#include "weather_database.h"
#include <crow_all.h>

class WeatherRestApi {
public:
    WeatherRestApi(WeatherApiClient& api_client, WeatherDatabase& db);

    void run(int port = 8080);

private:
    WeatherApiClient& api_client_;
    WeatherDatabase& db_;
    crow::SimpleApp app_;

    void setup_routes();
};
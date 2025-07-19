cmake_minimum_required(VERSION 3.14)
project(weather_fetcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(
    ${PQXX_INCLUDE_DIRS}
    /usr/include # For cpr/cpr.h (system install)
    /usr/local/include # For nlohmann/json.hpp
)

add_executable(weather_fetch
    main.cpp
    weather_api_client.cpp
    weather_database.cpp
    weather_helpers.cpp
)

# Link with system libraries directly
target_link_libraries(weather_fetch
    pqxx
    pq
    cpr
    curl
    ssl
    crypto
)
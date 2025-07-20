cmake_minimum_required(VERSION 3.14)
project(weather_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(
    ${PQXX_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    /usr/include # For cpr/cpr.h (system install)
    /usr/local/include # For nlohmann/json.hpp and crow_all.h
)

file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(weather_backend
    ${SOURCES}
)

# Link with system libraries directly
target_link_libraries(weather_backend
    pqxx
    pq
    cpr
    curl
    ssl
    crypto
    boost_system
    pthread
)
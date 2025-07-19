cmake_minimum_required(VERSION 3.14)
project(weather_fetch)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.12.0
)
FetchContent_MakeAvailable(cpr)

add_executable(weather_fetch main.cpp)
target_include_directories(weather_fetch PRIVATE /usr/local/include)
target_link_libraries(weather_fetch PRIVATE cpr::cpr pqxx pq)
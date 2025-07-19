FROM ubuntu:24.04

# Install system dependencies and all required libraries for CPR/curl
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        git \
        libbrotli-dev \
        libcurl4-openssl-dev \
        libidn2-0-dev \
        libnghttp2-dev \
        libpqxx-dev \
        libpsl-dev \
        libssl-dev \
        libunistring-dev \
        libzstd-dev \
        meson \
        ninja-build \
        pkg-config \
        wget \
        zlib1g-dev && \
    rm -rf /var/lib/apt/lists/*

# Install nlohmann/json (header-only)
RUN mkdir -p /usr/local/include/nlohmann && \
    wget https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp -O /usr/local/include/nlohmann/json.hpp

WORKDIR /app

COPY . .

RUN cmake -Bbuild -H. && cmake --build build

CMD ["./build/weather_fetch"]
FROM ubuntu:24.04

# Install system dependencies
RUN apt-get update && \
    apt-get install -y \
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

# Build and install CPR from source
RUN git clone --depth=1 https://github.com/libcpr/cpr.git /tmp/cpr && \
    cd /tmp/cpr && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCPR_USE_SYSTEM_CURL=ON . && \
    make -j$(nproc) && make install && \
    rm -rf /tmp/cpr

# Install nlohmann/json (header-only)
RUN mkdir -p /usr/local/include/nlohmann && \
    wget https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp -O /usr/local/include/nlohmann/json.hpp

WORKDIR /app/backend

COPY backend/ .

RUN cmake -Bbuild -H. && cmake --build build

CMD ["./build/weather_backend"]
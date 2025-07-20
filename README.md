# Weather Data USA

A full-stack application for fetching, storing, and visualizing weather station data across the USA.

## Project Structure

```
.
├── backend/      # C++ REST API server, data fetcher, and PostgreSQL integration
├── frontend/     # React app for interactive map and data visualization
├── nginx/        # Nginx reverse proxy configuration
├── docker-compose.yml
└── README.md
```

## Features

- Fetches weather station data from [api.weather.gov](https://api.weather.gov)
- Stores observations and station metadata in PostgreSQL
- REST API for querying stations, variables, and observations
- Interactive React map with station selection and charting
- Dockerized for easy deployment

## Prerequisites

- [Docker](https://www.docker.com/) and [Docker Compose](https://docs.docker.com/compose/)
- (Optional) [Node.js](https://nodejs.org/) for local frontend development

## Quick Start

1. **Clone the repository**
    ```sh
    git clone <your-repo-url>
    cd <repo-directory>
    ```

2. **Start all services with Docker Compose**
    ```sh
    docker-compose up --build -d
    ```

3. **Access the app**
    - Frontend: [http://localhost](http://localhost)
    - API: [http://localhost/api/](http://localhost/api/)

## API Endpoints

- `GET /api/stations` — List all stations with coordinates
- `GET /api/station_ids` — List all station IDs
- `GET /api/variables` — List all available weather variables
- `GET /api/observations/latest?station_id=...` — Latest observation for a station
- `GET /api/observations/range?station_id=...&start=...&end=...` — Observations in a time range

## Development

### Backend

- C++17, [Crow](https://crowcpp.org/) for REST API, [cpr](https://github.com/libcpr/cpr) for HTTP, [nlohmann/json](https://github.com/nlohmann/json) for JSON, [libpqxx](https://github.com/jtv/libpqxx) for PostgreSQL.
- Build and run in Docker, or locally with CMake.

### Frontend

- React, [react-leaflet](https://react-leaflet.js.org/) for maps, [chart.js](https://www.chartjs.org/) for charts.
- Local development:
    ```sh
    cd frontend
    npm install
    npm start
    ```

## Configuration

- Database credentials are set in `docker-compose.yml` (for testing/demo).
- Nginx proxies `/api/` to the backend and serves the frontend.

## License

MIT License

---

**Note:** This project is for educational/demo purposes. Do not use the provided database credentials in production.
import React, { useState } from "react";
import StationSelector from "./StationSelector";
import VariableSelector from "./VariableSelector";
import WeatherChart from "./WeatherChart";

function toISO8601(date, time) {
    if (!date) return "";
    if (!time) return `${date}T00:00:00Z`;
    return `${date}T${time}:00Z`;
}

function App() {
    const [stationId, setStationId] = useState("");
    const [variable, setVariable] = useState("");
    const [startDate, setStartDate] = useState("2025-07-01");
    const [startTime, setStartTime] = useState("00:00");
    const [endDate, setEndDate] = useState("2025-07-20");
    const [endTime, setEndTime] = useState("23:59");

    const start = toISO8601(startDate, startTime);
    const end = toISO8601(endDate, endTime);

    return (
        <div style={{ maxWidth: 800, margin: "auto" }}>
        <h1>Weather Data Plotter</h1>
        <div>
            <label>
            Station: <StationSelector onSelect={setStationId} />
            </label>
        </div>
        <div>
            <label>
            Start Date:{" "}
            <input type="date" value={startDate} onChange={e => setStartDate(e.target.value)} />
            {" "}Time:{" "}
            <input type="time" value={startTime} onChange={e => setStartTime(e.target.value)} />
            </label>
        </div>
        <div>
            <label>
            End Date:{" "}
            <input type="date" value={endDate} onChange={e => setEndDate(e.target.value)} />
            {" "}Time:{" "}
            <input type="time" value={endTime} onChange={e => setEndTime(e.target.value)} />
            </label>
        </div>
        <div>
            <label>
            Variable: <VariableSelector onSelect={setVariable} />
            </label>
        </div>
        <hr />
        {stationId && variable && (
            <WeatherChart
            stationId={stationId}
            start={start}
            end={end}
            variable={variable}
            />
        )}
        </div>
    );
}

export default App;
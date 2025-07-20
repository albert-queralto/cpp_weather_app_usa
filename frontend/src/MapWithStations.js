import React, { useEffect, useState } from "react";
import { MapContainer, TileLayer, Marker, Popup } from "react-leaflet";
import WeatherChart from "./WeatherChart";
import VariableSelector from "./VariableSelector";
import "./App.css";

function MapWithStations() {
    const [stations, setStations] = useState([]);
    const [selected, setSelected] = useState(null);
    const [variable, setVariable] = useState("");
    const [start, setStart] = useState("2025-07-01T00:00:00Z");
    const [end, setEnd] = useState("2025-07-20T23:59:59Z");

    useEffect(() => {
        fetch("/api/stations")
        .then(res => res.json())
        .then(setStations);
    }, []);

    return (
        <MapContainer center={[40, -100]} zoom={4} style={{ height: "600px", width: "100%" }}>
        <TileLayer
            url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        {stations.map(station => (
            <Marker
            key={station.station_identifier}
            position={[station.latitude, station.longitude]}
            eventHandlers={{
                click: () => setSelected(station),
            }}
            />
        ))}
        {selected && (
            <Popup
                position={[selected.latitude, selected.longitude]}
                onClose={() => setSelected(null)}
                className="large-popup"
            >
                <div className="popup-content">
                <strong>{selected.station_identifier}</strong>
                <div>
                    Start: <input type="datetime-local" value={start.slice(0,16)} onChange={e => setStart(e.target.value+":00Z")} />
                </div>
                <div>
                    End: <input type="datetime-local" value={end.slice(0,16)} onChange={e => setEnd(e.target.value+":00Z")} />
                </div>
                <div>
                    Variable: <VariableSelector onSelect={setVariable} />
                </div>
                {variable && (
                    <div className="weather-chart-container">
                    <WeatherChart
                        stationId={selected.station_identifier}
                        start={start}
                        end={end}
                        variable={variable}
                    />
                    </div>
                )}
                </div>
            </Popup>
        )}
        </MapContainer>
    );
}

export default MapWithStations;
import React from "react";
import MapWithStations from "./MapWithStations";

function App() {
    return (
        <div style={{ maxWidth: 1200, margin: "auto" }}>
        <h1>Weather Stations Map</h1>
        <MapWithStations />
        </div>
    );
}

export default App;
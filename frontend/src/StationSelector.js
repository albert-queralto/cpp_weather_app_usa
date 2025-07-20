import React, { useEffect, useState } from "react";

function StationSelector({ onSelect }) {
    const [stations, setStations] = useState([]);

    useEffect(() => {
        fetch("/api/station_ids")
            .then(async res => {
                const text = await res.text();
                console.log("Station IDs raw response:", text);
                return JSON.parse(text);
            })
            .then(setStations)
            .catch(e => console.error("Failed to load stations:", e));
    }, []);

    return (
        <select onChange={e => onSelect(e.target.value)}>
        <option value="">Select station</option>
        {stations.map(id => (
            <option key={id} value={id}>{id}</option>
        ))}
        </select>
    );
}

export default StationSelector;
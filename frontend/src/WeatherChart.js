import React, { useEffect, useState } from "react";
import { Line } from "react-chartjs-2";
import 'chart.js/auto';

function WeatherChart({ stationId, start, end, variable }) {
    const [data, setData] = useState(null);

    useEffect(() => {
        if (!stationId || !start || !end || !variable) return;
        fetch(
        `/api/observations/range?station_id=${stationId}&start=${start}&end=${end}`
        )
        .then((res) => res.json())
        .then((json) => setData(json));
    }, [stationId, start, end, variable]);

    if (!data) return <div>Loading...</div>;
    if (data.length === 0) return <div>No data found.</div>;

    const filtered = data.filter(d => d.parameter_name === variable);
    if (filtered.length === 0) return <div>No data for selected variable.</div>;

    const chartData = {
        labels: filtered.map(d => d.observed_at),
        datasets: [
        {
            label: variable,
            data: filtered.map(d => d.value),
            fill: false,
            borderColor: "rgb(75,192,192)",
            tension: 0.1,
        },
        ],
    };

    return (
        <div>
        <h3>{variable} for {stationId}</h3>
        <Line data={chartData} />
        </div>
    );
}

export default WeatherChart;
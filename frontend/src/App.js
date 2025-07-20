import React, { useState } from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Navbar from "./Navbar";
import Signup from "./Signup";
import Login from "./Login";
import MapWithStations from "./MapWithStations";

function App() {
    const [isAuthenticated, setIsAuthenticated] = useState(false);

    const handleLogin = () => setIsAuthenticated(true);
    const handleLogout = () => setIsAuthenticated(false);

    return (
        <Router>
        <Navbar isAuthenticated={isAuthenticated} onLogout={handleLogout} />
        <Routes>
            <Route path="/" element={<div style={{ padding: "2rem" }}>Welcome to the Weather App!</div>} />
            <Route path="/signup" element={<Signup />} />
            <Route path="/login" element={<Login onLogin={handleLogin} />} />
            <Route path="/weathermap" element={<MapWithStations />} />
        </Routes>
        </Router>
    );
}

export default App;
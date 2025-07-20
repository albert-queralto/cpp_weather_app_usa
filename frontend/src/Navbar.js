// Navbar.js
import React, { useEffect, useState } from "react";
import { Link } from "react-router-dom";

function Navbar({ isAuthenticated, onLogout }) {
    return (
        <nav style={{ padding: "1rem", background: "#1976d2", color: "#fff" }}>
        <Link to="/" style={{ color: "#fff", marginRight: "1rem" }}>Home</Link>
        <Link to="/weathermap" style={{ color: "#fff", marginRight: "1rem" }}>Weather Map</Link>
        {isAuthenticated ? (
            <button onClick={onLogout} style={{ marginLeft: "1rem" }}>Logout</button>
        ) : (
            <>
            <Link to="/login" style={{ color: "#fff", marginRight: "1rem" }}>Login</Link>
            <Link to="/signup" style={{ color: "#fff" }}>Signup</Link>
            </>
        )}
        </nav>
    );
}

export default Navbar;
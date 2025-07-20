import React, { useState, useEffect } from 'react';

function VariableSelector({ onSelect }) {
    const [variables, setVariables] = useState([]);

    useEffect(() => {
        fetch('/api/variables')
            .then(async res => {
                const text = await res.text();
                console.log("Variables raw response:", text);
                return JSON.parse(text);
            })
            .then(setVariables)
            .catch(e => console.error("Failed to load variables:", e));
    }, []);

    return (
        <select onChange={e => onSelect(e.target.value)}>
            <option value="">Select a variable</option>
            {variables.map(variable => (
                <option key={variable} value={variable}>
                    {variable}
                </option>
            ))}
        </select>
    );
}

export default VariableSelector;
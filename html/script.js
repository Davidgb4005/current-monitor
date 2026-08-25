function fetchAlternatorData() {
        fetch("http://localhost:8000/") // <- replace /data with your endpoint
            .then(res => res.json())
            .then(data => {
                document.getElementById("current").textContent = data.current;
                document.getElementById("currentMax").textContent = data.currentMax;
                document.getElementById("voltage").textContent = data.voltage;
                document.getElementById("voltageMax").textContent = data.voltageMax;
            })
        .catch(err => {console.error("Error fetching data:", err)
                document.getElementById("current").textContent = "Error";
                document.getElementById("currentMax").textContent = "Error";
                document.getElementById("voltage").textContent = "Error";
                document.getElementById("voltageMax").textContent = "Error";
        });
}

fetchAlternatorData();

setInterval(fetchAlternatorData, 500);

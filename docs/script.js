document.addEventListener("DOMContentLoaded", async () => {
    const firmwareSelect = document.getElementById("firmwareSelect");
    const flashButton = document.getElementById("flashButton");
    const basePath = window.location.pathname.replace(/\/[^/]*$/, '');
    const manifestBase = window.location.origin + basePath + "/firmware/";

    try {
        const response = await fetch("firmware/firmware.json");
        const firmwares = await response.json();

        if (!Array.isArray(firmwares)) throw new Error("Invalid JSON structure");

        // Sortiere nach neuester Version oben (optional)
        firmwares.sort((a, b) => b.version.localeCompare(a.version, undefined, { numeric: true, sensitivity: 'base' }));

        for (const fw of firmwares) {
            const opt = document.createElement("option");
            const label = `${fw.version}${fw.prerelease ? " (pre)" : ""}`;
            opt.value = fw.file;
            opt.textContent = label;
            firmwareSelect.appendChild(opt);
        }

        firmwareSelect.addEventListener("change", () => {
            const selected = firmwareSelect.value;
            if (selected) {
                flashButton.setAttribute("manifest", manifestBase + selected.replace(/\.bin$/, ".json"));
                flashButton.setAttribute("enabled", "");
            } else {
                flashButton.removeAttribute("manifest");
                flashButton.removeAttribute("enabled");
            }
        });
    } catch (err) {
        console.error("Fehler beim Laden der Firmware-Liste:", err);
        const status = document.getElementById("status");
        if (status) status.textContent = "Fehler beim Laden der Firmware-Dateien.";
    }
});
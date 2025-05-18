const centralRepoBaseURL = 'https://all-solutions.github.io/Flash2MQTT/firmware';

// Funktion zum Parsen der URL-Parameter
function getURLParameter(name) {
    return new URLSearchParams(window.location.search).get(name);
}

async function fetchFirmwareList() {
    const firmwareSelect = document.getElementById('firmwareSelect');

    // Abrufen der Firmware-Liste vom zentralen Repository
    try {
        const response = await fetch(`${centralRepoBaseURL}/firmware_list.json`);
        const firmwareList = await response.json();

        firmwareList.forEach(firmware => {
            const option = document.createElement('option');
            option.value = firmware.name;
            option.text = `${firmware.name} - Version ${firmware.version}`;
            firmwareSelect.add(option);
        });

        // Prüfen, ob ein 'get'-Parameter vorhanden ist
        const preselectFirmware = getURLParameter('get');
        if (preselectFirmware) {
            firmwareSelect.value = preselectFirmware;
            // Event manuell auslösen
            firmwareSelect.dispatchEvent(new Event('change'));
        }
    } catch (err) {
        console.error('Fehler beim Abrufen der Firmware-Liste:', err);
    }
}

fetchFirmwareList();

document.getElementById('firmwareSelect').addEventListener('change', async function () {
    const firmwareName = this.value;
    const variantSelect = document.getElementById('variantSelect');
    const flashButton = document.getElementById('flashButton');
    const variantLabel = document.querySelector('label[for="variantSelect"]');

    // Variante zurücksetzen
    variantSelect.style.display = 'none';
    variantLabel.style.display = 'none';
    variantSelect.innerHTML = '<option value="">Please select a variant</option>';
    flashButton.disabled = true;
    flashButton.removeAttribute('enabled');
    flashButton.manifest = '';

    if (!firmwareName) {
        return;
    }

    // Abrufen der Varianten für die ausgewählte Firmware
    try {
        const response = await fetch(`${centralRepoBaseURL}/${firmwareName}/variants.json`);
        const variants = await response.json();

        variants.forEach(variant => {
            const option = document.createElement('option');
            option.value = variant.file;
            option.text = variant.displayName;
            variantSelect.add(option);
        });

        variantSelect.style.display = 'block';
        variantLabel.style.display = 'block';

        // Prüfen, ob ein 'variant'-Parameter vorhanden ist
        const preselectVariant = getURLParameter('variant');
        if (preselectVariant) {
            variantSelect.value = preselectVariant;
            variantSelect.dispatchEvent(new Event('change'));
        }

    } catch (err) {
        console.error('Fehler beim Abrufen der Varianten:', err);
    }
});

document.getElementById('variantSelect').addEventListener('change', function () {
    const firmwareUrl = this.value;
    const flashButton = document.getElementById('flashButton');
    const firmwareSelect = document.getElementById('firmwareSelect');
    const firmwareName = firmwareSelect.options[firmwareSelect.selectedIndex].value;

    if (!firmwareUrl) {
        flashButton.disabled = true;
        flashButton.removeAttribute('enabled');
        flashButton.manifest = '';
        return;
    }

    const manifest = {
        "name": `${firmwareName} Firmware`,
        "builds": [
            {
                "chipFamily": "ESP8266",
                "parts": [
                    {
                        "path": firmwareUrl,
                        "offset": 0
                    }
                ]
            }
        ]
    };

    const blob = new Blob([JSON.stringify(manifest)], { type: 'application/json' });
    const manifestUrl = URL.createObjectURL(blob);

    flashButton.manifest = manifestUrl;
    flashButton.disabled = false;
    flashButton.setAttribute('enabled', 'true');
});


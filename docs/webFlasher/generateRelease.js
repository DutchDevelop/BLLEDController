const fs = require('fs');
const fetch = require('node-fetch');

const baseURL = "https://raw.githubusercontent.com/all-solutions/Flash2MQTT/main/firmware";
const firmwareListURL = `${baseURL}/firmware_list.json`;

async function fetchJSON(url) {
    const response = await fetch(url);
    if (!response.ok) {
        throw new Error(`Fehler beim Laden der URL: ${url}`);
    }
    return response.json();
}

async function createReleaseJSON() {
    const releaseData = { release: [] };

    try {
        const firmwareList = await fetchJSON(firmwareListURL);

        for (const firmware of firmwareList) {
            const { name, version } = firmware;
            const variantsURL = `${baseURL}/${name}/variants.json`;

            try {
                const variants = await fetchJSON(variantsURL);

                for (const variant of variants) {
                    const { displayName, file } = variant;
                    releaseData.release.push({
                        binary: `${name}${displayName}${version}`,
                        otaurl: file
                    });
                }
            } catch (error) {
                console.error(`Fehler beim Laden der Variants für ${name}:`, error);
            }
        }

        fs.writeFileSync('release.json', JSON.stringify(releaseData, null, 2));
        console.log("release.json wurde erfolgreich erstellt!");

    } catch (error) {
        console.error(`Fehler beim Erstellen der release.json: ${error.message}`);
    }
}

createReleaseJSON();

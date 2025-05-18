const fs = require('fs');
const path = require('path');

const firmwareDir = path.resolve(__dirname, 'firmware'); // z. B. ./docs/firmware

const binFiles = fs.readdirSync(firmwareDir).filter(file => file.endsWith('.bin'));

if (binFiles.length === 0) {
  console.log('Keine .bin Dateien gefunden.');
  process.exit(1);
}

for (const file of binFiles) {
  const binPath = path.join(firmwareDir, file);
  const baseName = path.basename(file, '.bin');
  const manifestPath = path.join(firmwareDir, `${baseName}.json`);

  const manifest = {
    name: baseName,
    version: extractVersion(file),
    build: Date.now().toString(),
    files: [
      {
        url: file,
        type: 'application/octet-stream',
        platform: 'esp32'
      }
    ]
  };

  fs.writeFileSync(manifestPath, JSON.stringify(manifest, null, 2));
  console.log(`Manifest erstellt: ${manifestPath}`);
}

function extractVersion(filename) {
  // Beispiel: BLLEDController_V1.0.2.bin → 1.0.2
  const match = filename.match(/_V?(\d+\.\d+\.\w*)\.bin$/i);
  return match ? match[1] : 'unknown';
}

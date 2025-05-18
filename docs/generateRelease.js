const fs = require('fs');
const path = require('path');

const firmwareDir = path.resolve(__dirname, 'firmware');
const binFiles = fs.readdirSync(firmwareDir).filter(file => file.endsWith('.bin'));

if (binFiles.length === 0) {
  console.log('No .bin files found.');
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
  console.log(`✔ Manifest written: ${manifestPath}`);
}

function extractVersion(filename) {
  const match = filename.match(/_V?([\w.\-]+)/);
  return match ? match[1] : 'unknown';
}

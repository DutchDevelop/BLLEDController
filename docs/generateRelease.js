const fs = require('fs');
const path = require('path');

const firmwareDir = path.resolve(__dirname, 'firmware');
const binFiles = fs.readdirSync(firmwareDir).filter(f => f.endsWith('.bin'));

if (binFiles.length === 0) {
  console.log('❌ No .bin files found in firmware folder.');
  process.exit(1);
}

const firmwareList = [];

for (const file of binFiles) {
  const versionMatch = file.match(/_V?([\w.\-]+)\.bin$/);
  const version = versionMatch ? versionMatch[1] : 'unknown';
  const isPre = file.toLowerCase().includes('nightly') || file.toLowerCase().includes('beta');

  // 👇 Add entry to firmware.json
  firmwareList.push({
    version,
    prerelease: isPre,
    file
  });

  // 👇 Create ESP Web Tools Manifest (merged .bin format)
  const manifest = {
    name: file.replace(/\.bin$/, ''),
    version,
    builds: [
      {
        chipFamily: "ESP32",
        parts: [
          {
            path: file,
            offset: 0
          }
        ]
      }
    ]
  };

  const manifestPath = path.join(firmwareDir, file.replace(/\.bin$/, '.json'));
  fs.writeFileSync(manifestPath, JSON.stringify(manifest, null, 2));
  console.log(`✅ Created manifest: ${manifestPath}`);
}

// 👇 Write firmware.json (for dropdown use in UI)
const firmwareJsonPath = path.join(firmwareDir, 'firmware.json');
fs.writeFileSync(firmwareJsonPath, JSON.stringify(firmwareList, null, 2));
console.log(`✅ Created firmware list: ${firmwareJsonPath}`);

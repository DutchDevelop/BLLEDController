
# 📘 BLLED Controller Manual

This document describes all configuration options and web interfaces provided by the BLLED firmware.

---

## ⚙️ Setup Page

This is the main configuration page of the controller.

### 🔆 Brightness
**ID:** `brightnessslider`  
Adjusts the global brightness of the LED output (0–100%).

### 🧪 Maintenance Mode
**ID:** `maintMode`  
Keeps LEDs permanently on (white), overrides other states.

### 🌈 RGB Cycle Mode
**ID:** `discoMode`  
Cycles through colors continuously for timelapse or decoration.

### 🔁 Replicate State
**ID:** `replicateLedState`  
Copies the printer’s internal chamber light status to the LEDs.

#### Running Color Parameters
- `runningRGB`, `runningWW`, `runningCW`

### 🧪 Color Test Mode
**ID:** `showtestcolor`  
Forces LEDs to show test color values.

#### Test Color Parameters
- `testRGB`, `testWW`, `testCW`

### 📶 WiFi Strength via LEDs
**ID:** `debugwifi`  
Displays signal strength using LED colors.

### 🎬 Finish Indication
**ID:** `finishIndication`  
Shows a color when print completes.

#### Finish Color & Exit Behavior
- `finishColor`, `finishWW`, `finishCW`
- `finishEndDoor`, `finishEndTimer`, `finishTimerMins`

### 🔕 Inactivity Timeout
**ID:** `inactivityEnabled`  
Turns off LEDs after period of inactivity.

- `inactivityMins`: Timeout duration

### 🔦 Control Chamber Light (via MQTT)
**ID:** `controlChamberLight`  
Synchronizes printer’s internal light with LED behavior.

### 🖨️ Printer Type: P1 Compatibility
**ID:** `p1Printer`  
Adjusts logic for P1 printers (no door sensor).

### 🔧 Door Switch Support
**ID:** `doorSwitch`  
Enables toggling LEDs via door open/close gesture.

### 🛠️ Stage-Specific LED Colors
Assign LED colors to internal printer stages:
- `stage1RGB`, `stage8RGB`, `stage9RGB`, etc.

### ❗ Error Detection
**ID:** `errorDetection`  
Uses LED colors to signal HMS or print errors.

### 🎨 Custom Error Colors
- `wifiRGB`, `pauseRGB`, `firstlayerRGB`, etc.

### 🚫 Ignore HMS Codes
**ID:** `hmsIgnoreList`  
List HMS codes (one per line) to suppress.

### 🐞 Debug Options
- `debuging`, `debugingchange`, `mqttdebug`

### 🧭 Navigation
| Page             | URL                |
|------------------|--------------------|
| WiFi Setup       | `/wifi`            |
| Firmware Update  | `/fwupdate`        |
| Backup & Restore | `/backuprestore`   |
| Web Log Console  | `/webserial`       |

---

## 📡 WiFi & Printer Setup (`/wifi`)

Configure WiFi and printer link:

- `ssid`, `password`: WiFi credentials
- `printerIP`, `printerSerial`, `accessCode`: Bambu printer link
- `username`, `userpassword`: WebUI access login
- `Test Printer Connection` button

---

## 💾 Backup & Restore (`/backuprestore`)

- **Backup** → Download current `blledconfig.json`
- **Restore** → Upload config and restart device

---

## 🔧 Firmware Update (`/fwupdate`)

Upload a `.bin` firmware file.  
Device reboots automatically.

---

## 🖥️ Web Serial Console (`/webserial`)

Live view of serial log messages.

- Log area with auto-scroll
- Buttons: Clear, Copy, Pause
- Command input

---

## ♻️ Factory Reset

Triggers full reset via:

```
http://[device-ip]/factoryreset
```

- Deletes all config and WiFi credentials
- Reboots into AP mode

---


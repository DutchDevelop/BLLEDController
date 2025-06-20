## ⚙️ BLLED Web Setup – Configuration Guide

This document describes each setting available in the BLLED Controller setup interface.

---

### 🔆 Brightness

**ID:** `brightnessslider`  
**Type:** Range (0–100)

Adjusts the global brightness level of the LED output.  
Value is in percent.

- `0` → LEDs off
- `100` → full brightness (default: `100`)

---

### 🧪 Maintenance Mode

**ID:** `maintMode`  
**Type:** Checkbox  
**Group:** `LEDBehavior`

When enabled, LEDs stay **permanently on** in white color, regardless of printer state or WiFi/MQTT connection.

- Always on (white)
- Overrides all other LED modes

---

### 🌈 RGB Cycle Mode

**ID:** `discoMode`  
**Type:** Checkbox  
**Group:** `LEDBehavior`

Activates a continuous RGB color transition cycle for decorative purposes.

- LED color cycles smoothly through red/green/blue
- Used for visual effects or time-lapse printing

---

### 🔁 Replicate State

**ID:** `replicateLedState`  
**Type:** Checkbox  
**Group:** `LEDBehavior`

Replicates the printer’s internal **chamber light state** on the external LEDs.

- When the printer turns its own light on/off, BLLED will follow
- Must be connected via MQTT

#### 🔧 Running Color

- `runningRGB`: LED color (HEX)
- `runningWW`: Warm white value (0–255)
- `runningCW`: Cold white value (0–255)

These are used while the replicate mode is active and printer light is on.

---

Möchtest du, dass ich weitermache mit:
- 🧪 Test Color Mode
- 📶 Wifi Strength Display
- 🎬 Finish Indication

### 🧪 Color Test Mode

**ID:** `showtestcolor`  
**Type:** Checkbox  
**Group:** `LEDBehavior`

Forces the LEDs to show a fixed test color at all times, regardless of printer state.  
Useful for hardware tests and verifying PWM color output.

#### 🔧 Test Color Parameters

- `testRGB`: HEX color value (e.g., `#3F3CFB`)
- `testWW`: Warm white value (0–255)
- `testCW`: Cold white value (0–255)

---

### 📶 Show WiFi Strength via LEDs

**ID:** `debugwifi`  
**Type:** Checkbox  
**Group:** `LEDBehavior`

LEDs display WiFi signal strength as color:

- Green: excellent signal
- Yellow/orange: medium
- Red: poor signal

This helps determine ESP placement for optimal reception.

---

### 🎬 Finish Indication

**ID:** `finishIndication`  
**Type:** Checkbox

When a print completes, LEDs change to a dedicated **"Finish" color** for visual feedback.

#### 🔧 Finish Color Parameters

- `finishColor`: HEX color (e.g., green)
- `finishWW`: Warm white level
- `finishCW`: Cold white level

#### ⏱ Finish Exit Behavior

You can configure how and when to leave the "finished" state:

---

### 🚪 Exit after Door Action

**ID:** `finishEndDoor`  
**Type:** Checkbox  
**Group:** `finishOption`

Leaves the finish state after the door is opened and closed once (or vice versa).

---

### ⏲️ Exit after Timeout

**ID:** `finishEndTimer`  
**Type:** Checkbox  
**Group:** `finishOption`

Leaves the finish state after a user-defined number of minutes.

- `finishTimerMins`: Duration in minutes (e.g., `1` = 1 min)

### 🔕 Inactivity Timeout

**ID:** `inactivityEnabled`  
**Type:** Checkbox

Automatically turns off the LEDs after a period of printer inactivity.

#### 🔧 Timeout Duration

- `inactivityMins`: Number of minutes without activity before LEDs turn off  
  *(e.g., `30` = 30 minutes)*

If the door is opened or a new print starts, the LEDs turn back on automatically.

---

### 🔦 Control Chamber Light

**ID:** `controlChamberLight`  
**Type:** Checkbox

When enabled, the BLLED controller will also send MQTT commands to toggle the printer's internal **chamber light** in sync with:

- 🚪 Door-based LED toggling (on/off)
- ✅ Print start → turns chamber light on
- ⏱ Print finish → turns chamber light off after timeout or door interaction

> Requires an active MQTT connection to your Bambu printer.

---

### 🖨️ Printer Type: P1 Compatibility

**ID:** `p1Printer`  
**Type:** Checkbox  
**Group:** `PrinterOptions`

If checked, adjusts behavior for P1 printers:

- Disables door switch-related features (P1 has no door sensor)
- Automatically sets all LIDAR-related stage colors to full white (255/255)

---

### 🔧 Door Switch Support

**ID:** `doorSwitch`  
**Type:** Checkbox  
**Group:** `PrinterOptions`

Enable this if your printer has a door sensor and you want to use:

- Double-close gesture to toggle LEDs
- Finish indication reset by door interaction

> Automatically disabled when `p1Printer` is active.

---

### 🛠️ Stage-Specific LED Colors

For advanced customization, you can assign LED colors to specific printer stages:

- `stage14RGB`, `stage14WW`, `stage14CW` → Cleaning Nozzle  
- `stage1RGB`,  `stage1WW`,  `stage1CW`  → Bed Leveling  
- `stage8RGB`,  `stage8WW`,  `stage8CW`  → Calibrating Extrusion  
- `stage9RGB`,  `stage9WW`,  `stage9CW`  → Scanning Bed Surface  
- `stage10RGB`, `stage10WW`, `stage10CW` → First Layer Inspection

Default values are OFF or white depending on printer type.

### ❗ Error Detection

**ID:** `errorDetection`  
**Type:** Checkbox  
**Group:** `PrinterOptions`

When enabled, BLLED reacts to known printer errors or HMS codes by switching to predefined **alert colors**.

---

### 🎨 Custom Error Colors

You can assign LED colors for specific error conditions:

| Condition                     | ID Prefix      | Description                                |
|------------------------------|----------------|--------------------------------------------|
| WiFi Scan / Setup            | `wifiRGB`      | Orange by default                          |
| Pause (User or Gcode)        | `pauseRGB`     | Blue                                        |
| First Layer Error            | `firstlayerRGB`| Blue                                        |
| Nozzle Clog                  | `nozzleclogRGB`| Blue                                        |
| HMS Serious Severity         | `hmsSeriousRGB`| Red                                         |
| HMS Fatal Severity           | `hmsFatalRGB`  | Red                                         |
| Filament Runout              | `filamentRunoutRGB` | Red                                  |
| Front Cover Removed          | `frontCoverRGB`| Red                                         |
| Nozzle Temperature Fail      | `nozzleTempRGB`| Red                                         |
| Bed Temperature Fail         | `bedTempRGB`   | Red                                         |

Each has associated warm/cold white (`WW`/`CW`) sliders to fine-tune tone and intensity.

---

### 🚫 Ignore Specific HMS Codes

**ID:** `hmsIgnoreList`  
**Type:** Textarea (multiline)

Enter one or more HMS codes (one per line or comma-separated) to suppress LED error response for those codes.

Example:
HMS_0300_1200_0002_0001
HMS_0700_2000_0003_0001


---

### 🐞 Debug Options

These options help track behavior for diagnostics:

| ID               | Description                                   |
|------------------|-----------------------------------------------|
| `debuging`       | Global debug log output                       |
| `debugingchange` | Logs only when a value or state changes       |
| `mqttdebug`      | Logs all MQTT messages to WebSerial/Console   |

---

### 🧭 Navigation Buttons

Located at the bottom of the config page:

| Button              | Target Page           | Description                              |
|---------------------|------------------------|------------------------------------------|
| **WiFi & Printer Setup** | `/wifi`              | Configure network and printer credentials |
| **Firmware Update** | `/fwupdate`           | Upload new firmware                      |
| **Backup & Restore** | `/backuprestore`      | Save or restore full configuration       |
| **Web Serial Log**  | `/webserial`          | Open debug log console                   |






## 💾 Backup & Restore Configuration

Access this page at: http://[device-ip]/backuprestore

Allows you to download or upload the full BLLED configuration (`blledconfig.json`).

---

### 📥 Backup Current Config

**Button Label:** `Download Config File`  
**Action:** Initiates download of the current `blledconfig.json` file from internal storage.

- Includes: WiFi, printer IP, serial number, LED settings, HMS ignores, etc.
- Format: JSON file, pretty-formatted

---

### 📤 Restore Config File

**Form Upload (Drag & Drop or File Picker)**  
**Action URL:** `/configrestore`  
**Method:** POST

Uploads a `blledconfig.json` backup file and replaces the current configuration.

- ⚠️ Device will **restart automatically** after upload
- File is validated for structure and required fields
- No merge – existing settings will be fully replaced

---

## 📡 WiFi & Printer Setup

Access this page at: http://[device-ip]/wifi


Use this page to configure the device’s WiFi connection and printer link.

---

### 📶 WiFi Network

#### 📥 SSID

**ID:** `ssid`  
**Type:** Text (required)  
**Description:**  
Name of the WiFi network the device should connect to.

#### 🔒 Password

**ID:** `password`  
**Type:** Password (required)  
**Description:**  
WiFi password. Will be stored encrypted in the local config.

> You can view available networks via the 🔍 Scan button.

---

### 🖨️ Printer Connection

#### 🌐 Printer IP

**ID:** `printerIP`  
**Type:** Text  
**Placeholder:** `e.g. 192.168.1.100`  
**Description:**  
IP address of the Bambu printer to receive MQTT status from. Must be reachable on the local network.

#### 🔢 Serial Number

**ID:** `printerSerial`  
**Type:** Text  
**Placeholder:** `e.g. BL123456789`  
**Description:**  
The unique serial number (USN) of your printer, used in MQTT topic handling.

#### 🔑 Access Code

**ID:** `accessCode`  
**Type:** Text  
**Placeholder:** `e.g. Access123`  
**Description:**  
Used for MQTT authentication with your printer. Obtain this from the Bambu app or device.

---

### 👤 Web Login (Optional)

#### 👥 Username

**ID:** `username`  
**Type:** Text  
Default is blank. If set, it will enable HTTP Basic Auth for accessing the web interface.

#### 🔑 Password

**ID:** `userpassword`  
**Type:** Password  
Used with the above username for securing access to all pages.

---

### 🧪 Test Connection

Button: `Test Printer Connection`  
Tests MQTT reachability of the printer with the current IP/serial.

---

## 🔧 Firmware Update

Access this page at: http://[device-ip]/fwupdate


Allows you to upload and install a new firmware binary file (`.bin`) directly to the controller.

---

### 📤 Firmware Upload

#### 📄 File Input

**Type:** File upload  
**Accepted File:** `.bin` (firmware binary)  
**Form Target:** `/update` (POST)

- Uploads a new firmware image to flash memory  
- File is verified and written immediately  
- Progress and status are displayed below

> ⚠️ The device will reboot automatically after a successful update.

---

### ⏹ Cancel Button

Cancels the upload process if not yet started.

---

### 💡 Tip

Ensure that your binary is built for the correct chip (e.g., ESP32) and compatible with the current hardware revision.


## 🖥️ Web Serial Debug Log

Access this page at: http://[device-ip]/webserial


This live interface displays system logs and debug messages from the controller in real time – without using a physical USB connection.

---

### 🪵 Log Output Console

**Element:** `textarea` (read-only)  
**Function:** Displays all serial output from the controller, including:

- MQTT messages
- LED state changes
- Door sensor triggers
- WiFi and printer status
- HMS error parsing results

Output scrolls live as messages are received.

---

### 🔧 Controls

| Button                | Function                                 |
|-----------------------|------------------------------------------|
| `Clear Console`       | Clears the current log view              |
| `Copy`               | Copies all visible log contents          |
| `Pause Log`           | Temporarily pauses real-time updates     |
| `Send Command` (input + button) | Sends a command string to the controller via WebSerial (if supported) |

> Useful for debugging MQTT, LEDs, and print events in real time.

---






























### 🔦 Control Chamber Light

When enabled, the BLLED Controller will automatically control the printer's internal chamber light via MQTT:

- ✅ Turns the chamber light **on** when a print starts  
- ❌ Turns it **off** after the print finishes (based on timeout or door interaction)  
- 🔁 Toggles the chamber light together with the LEDs when using the **door double-close gesture**

> This feature requires a working MQTT connection to your Bambu printer.  
> It will not interfere with manual light control or other automations.


### 💾 Backup & Restore Configuration

Use these options to save or restore your current BLLED configuration:

- ✅ **Backup**: Downloads the current configuration (`blledconfig.json`) to your computer  
- 🔁 **Restore**: Uploads a previously saved configuration to restore all LED and printer settings  
- 🔒 Includes printer IP, access code, and all LED behaviors

> Useful for transferring settings between devices or keeping a safety copy before updates.

---

### ♻️ Factory Reset

Performs a full reset of the BLLED controller:

- ❌ Deletes all saved configuration and WiFi credentials from internal storage  
- 🔄 Automatically restarts the device into Access Point (setup) mode  
- 🌐 You’ll need to reconnect and reconfigure via the setup page

**How to trigger:**  
Open your browser and navigate to: http://[device-ip]/factoryreset



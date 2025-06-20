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



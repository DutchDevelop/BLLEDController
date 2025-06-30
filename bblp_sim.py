import tkinter as tk
from tkinter import messagebox
import threading
import time
import json
import ssl
import paho.mqtt.client as mqtt

# === MQTT Configuration ===
mqtt_config = {
    "broker": "127.0.0.1",
    "port": 8883,
    "username": "bblp",
    "password": "bblp",
    "serial": "0000"
}

# === GUI Root and Variables ===
root = tk.Tk()
root.title("BLLED Printer MQTT Simulator")

chamber_light_state = tk.StringVar(value="ON")  # Light is ON by default after printer startup

# === MQTT Client Setup ===
client = mqtt.Client()  # Use default (v3.1.1) for compatibility
client.tls_set(cert_reqs=ssl.CERT_NONE)
client.tls_insecure_set(True)

# === MQTT Handlers ===
def on_connect(client, userdata, flags, rc):
    client.subscribe(f"device/{mqtt_config['serial']}/request")
    log_message("✅ Connected and subscribed")

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        if msg.topic.endswith("/request"):
            if "system" in payload:
                system = payload["system"]
                if system.get("command") == "ledctrl" and system.get("led_node") == "chamber_light":
                    mode = system.get("led_mode", "unknown")
                    chamber_light_state.set(mode.upper())
                    log_message(f"→ Light command received: {mode.upper()}")
                    # Simulate printer sending updated state back
                    send_status()
        if "print" in payload and "hms" in payload["print"]:
            hms_arr = payload["print"]["hms"]
            if isinstance(hms_arr, list) and len(hms_arr) > 0:
                h = hms_arr[0]
                code = ((int(h.get("attr", 0)) << 32) + int(h.get("code", 0)))
                chunk1 = (code >> 48) & 0xFFFF
                chunk2 = (code >> 32) & 0xFFFF
                chunk3 = (code >> 16) & 0xFFFF
                chunk4 = code & 0xFFFF
                parsed = f"HMS_{chunk1:04X}_{chunk2:04X}_{chunk3:04X}_{chunk4:04X}"
                parsed_hms_code.set(parsed)
                log_message(f"← HMS Received: {parsed}")
    except Exception as e:
        log_message(f"[Error] MQTT message: {e}")

client.on_connect = on_connect
client.on_message = on_message

# Track last HMS code
door_open_state = tk.BooleanVar(value=False)
parsed_hms_code = tk.StringVar(value="None")
simulation_running = tk.BooleanVar(value=False)

# === GUI Functions ===
def periodic_status_report():
    while True:
        send_status()
        time.sleep(1.0)

def connect_mqtt():
    # Start periodic reporting after connect
    threading.Thread(target=periodic_status_report, daemon=True).start()
    client.username_pw_set(mqtt_config["username"], mqtt_config["password"])
    try:
        client.connect(mqtt_config["broker"], mqtt_config["port"])
        threading.Thread(target=client.loop_forever, daemon=True).start()
        log_message("Connecting to MQTT...")
    except Exception as e:
        messagebox.showerror("MQTT Connection Failed", str(e))

def update_config():
    mqtt_config["broker"] = broker_entry.get()
    mqtt_config["port"] = int(port_entry.get())
    mqtt_config["username"] = user_entry.get()
    mqtt_config["password"] = pass_entry.get()
    mqtt_config["serial"] = serial_entry.get()
    connect_mqtt()

def send_payload(payload):
    topic = f"device/{mqtt_config['serial']}/report"
    client.publish(topic, json.dumps(payload))
    log_message(f"MQTT → {json.dumps(payload)}")

def send_status(stage=None, gcode_state=None, door_open=None, hms_code=None):
    payload = {"print": {}}
    payload["print"]["stg_cur"] = stage if stage is not None else 0
    payload["print"]["gcode_state"] = gcode_state if gcode_state is not None else "IDLE"
    payload["print"]["home_flag"] = (1 << 23) if door_open_state.get() else 0
    payload["print"]["lights_report"] = [{"node": "chamber_light", "mode": "on" if chamber_light_state.get() == "ON" else "off"}]
    current_hms = parsed_hms_code.get()
    if current_hms != "None":
        try:
            chunks = current_hms.split("_")[1:]
            parts = [int(x, 16) for x in chunks]
            hms_code_val = ((parts[0] << 48) + (parts[1] << 32) + (parts[2] << 16) + parts[3])
            payload["print"]["hms"] = [{"attr": 0x00000000, "code": hms_code_val}]
        except:
            pass
    send_payload(payload)

def simulate_print():
    simulation_running.set(True)
    simulate_btn.config(state=tk.DISABLED, text="Simulating...")
    steps = [
        (1, "RUNNING", "Bed Leveling"),
        (2, "RUNNING", "Preheating"),
        (8, "RUNNING", "Extrusion Calibration"),
        (0, "RUNNING", "Printing"),
        (10, "RUNNING", "First Layer Inspection"),
        (16, "PAUSE", "Paused"),
        (0, "RUNNING", "Resumed"),
        (-1, "FINISH", "Finished"),
        (-1, "IDLE", "Idle")
    ]
    for s, state, desc in steps:
        log_message(f"→ {desc}")
        send_status(stage=s, gcode_state=state)
        time.sleep(1.2)
    simulate_btn.config(state=tk.NORMAL, text="Simulate Print")
    simulation_running.set(False)

def simulate_print_thread():
    if not simulation_running.get():
        threading.Thread(target=simulate_print, daemon=True).start()

def simulate_light_off():
    payload = {
        "system": {
            "command": "ledctrl",
            "led_node": "chamber_light",
            "led_mode": "off"
        }
    }
    topic = f"device/{mqtt_config['serial']}/request"
    client.publish(topic, json.dumps(payload))
    log_message("← Simulated: chamber_light OFF")

def simulate_light_on():
    payload = {
        "system": {
            "command": "ledctrl",
            "led_node": "chamber_light",
            "led_mode": "on"
        }
    }
    topic = f"device/{mqtt_config['serial']}/request"
    client.publish(topic, json.dumps(payload))
    log_message("← Simulated: chamber_light ON")

def simulate_hms(event):
    sel = hms_listbox.curselection()
    if sel:
        code = hms_codes[sel[0]]
        name = hms_listbox.get(sel[0])
        parsed_hms_code.set(f"HMS_{(code >> 48) & 0xFFFF:04X}_{(code >> 32) & 0xFFFF:04X}_{(code >> 16) & 0xFFFF:04X}_{code & 0xFFFF:04X}")
        log_message(f"→ Simulating HMS: {name}")
        send_status(hms_code=code)

def log_message(text):
    log_output.insert(tk.END, text + "\n")
    log_output.see(tk.END)

# === GUI Layout ===
tk.Label(root, text="MQTT Broker IP:").grid(row=0, column=0, sticky="e")
broker_entry = tk.Entry(root)
broker_entry.insert(0, mqtt_config["broker"])
broker_entry.grid(row=0, column=1)

tk.Label(root, text="Port:").grid(row=0, column=2, sticky="e")
port_entry = tk.Entry(root, width=6)
port_entry.insert(0, str(mqtt_config["port"]))
port_entry.grid(row=0, column=3)

tk.Label(root, text="Username:").grid(row=1, column=0, sticky="e")
user_entry = tk.Entry(root)
user_entry.insert(0, mqtt_config["username"])
user_entry.grid(row=1, column=1)

tk.Label(root, text="Password:").grid(row=1, column=2, sticky="e")
pass_entry = tk.Entry(root, show="*")
pass_entry.insert(0, mqtt_config["password"])
pass_entry.grid(row=1, column=3)

tk.Label(root, text="Printer Serial:").grid(row=2, column=0, sticky="e")
serial_entry = tk.Entry(root)
serial_entry.insert(0, mqtt_config["serial"])
serial_entry.grid(row=2, column=1)

tk.Button(root, text="Connect", command=update_config).grid(row=2, column=3)

button_frame = tk.Frame(root)
button_frame.grid(row=3, column=0, columnspan=4, pady=5)

simulate_btn = tk.Button(button_frame, text="Simulate Print", command=simulate_print_thread)
simulate_btn.grid(row=0, column=5, padx=3)

buttons = [
    ("Door Open", lambda: door_open_state.set(True)),
    ("Door Close", lambda: door_open_state.set(False)),
    ("Pause", lambda: send_status(gcode_state="PAUSE")),
    ("Resume", lambda: send_status(gcode_state="RUNNING")),
    ("Finish", lambda: send_status(stage=-1, gcode_state="FINISH")),
    ("Simulate Light ON", simulate_light_on),
    ("Simulate Light OFF", simulate_light_off),
    ("Clear HMS", lambda: parsed_hms_code.set("None"))
]
for i, (txt, cmd) in enumerate(buttons):
    tk.Button(button_frame, text=txt, command=cmd).grid(row=0, column=i, padx=3)

# HMS List
hms_frame = tk.Frame(root)
hms_frame.grid(row=4, column=0, columnspan=4, sticky="w", padx=10)
tk.Label(hms_frame, text="HMS Errors:").pack(anchor="w")

hms_codes = [
    0x0300120000020001, 0x0C0003000003000B, 0x0700200000030001,
    0x0300020000010001, 0x0300010000010007, 0x0600100000010004,
    0x0500200000030001, 0x0800100000010001
]
hms_labels = [
    "Front Cover Removed", "First Layer Inspection", "Filament Runout",
    "Nozzle Temp Fail", "Bed Temp Fail", "Extruder Error",
    "Toolhead Error", "Camera Initialization Error"
]
hms_listbox = tk.Listbox(hms_frame, height=5, width=35)
for lbl in hms_labels:
    hms_listbox.insert(tk.END, lbl)
hms_listbox.pack(side="left")
hms_listbox.bind("<<ListboxSelect>>", simulate_hms)

# Log and status
tk.Label(root, text="HMS Code:").grid(row=5, column=2, sticky="e")
tk.Label(root, textvariable=parsed_hms_code, fg="red").grid(row=5, column=3, sticky="w")
tk.Label(root, text="Chamber Light State:").grid(row=5, column=0, sticky="e")
tk.Label(root, textvariable=chamber_light_state, fg="blue").grid(row=5, column=1, sticky="w")

log_output = tk.Text(root, height=12, width=100)
log_output.grid(row=6, column=0, columnspan=4, padx=10, pady=10)

# Start GUI
root.mainloop()

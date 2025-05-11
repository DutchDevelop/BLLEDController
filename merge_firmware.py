Import("env")
import os
import shutil
import gzip

# === Konfiguration ===
ENABLE_MERGE_BIN = True  # Steuert ob Merge ausgeführt wird (OTA .gz wird immer erstellt!)
# ======================

BUILD_DIR = env.subst("$BUILD_DIR")
PROGNAME = env.subst("${PROGNAME}")
APP_BIN = os.path.normpath(os.path.join(BUILD_DIR, f"{PROGNAME}.bin"))
MERGED_BIN = os.path.normpath(os.path.join(BUILD_DIR, f"{PROGNAME}_merged.bin"))
BOARD_CONFIG = env.BoardConfig()

project_name = env.GetProjectOption("custom_project_name") or "Firmware"
version = env.GetProjectOption("custom_version") or "0.0.0"
firmware_filename_merged = f"{project_name}_V{version}.bin"
firmware_filename_ota = f"{project_name}_V{version}.ota.gz"

firmware_path_merged = os.path.normpath(os.path.join(env.subst("$PROJECT_DIR"), ".firmware", firmware_filename_merged))
firmware_path_ota = os.path.normpath(os.path.join(env.subst("$PROJECT_DIR"), ".firmware", firmware_filename_ota))

def create_gz(input_file, output_file):
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(input_file, 'rb') as f_in:
        with gzip.open(output_file, 'wb', compresslevel=9) as f_out:
            shutil.copyfileobj(f_in, f_out)
    print(f'Firmware (OTA, gzipped) created at: {output_file}')

def copy_bin_and_gz(source, target, env):
    # Immer nur .gz für OTA, kein extra unkomprimiertes .ota
    create_gz(APP_BIN, firmware_path_ota)

def merge_bin(source, target, env):
    flash_images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", []))
    app_offset = env.subst("$ESP32_APP_OFFSET") or "0x10000"
    flash_images += [app_offset, APP_BIN]

    flash_args = [
        os.path.normpath(env.subst(str(x))) if not str(x).startswith("0x") else str(x)
        for x in flash_images
    ]

    cmd = [
        env.subst("$PYTHONEXE"),
        env.subst("$OBJCOPY"),
        "--chip", BOARD_CONFIG.get("build.mcu", "esp32"),
        "merge_bin",
        "--fill-flash-size", BOARD_CONFIG.get("upload.flash_size", "4MB"),
        "--output", MERGED_BIN,
    ] + flash_args

    command_str = " ".join(f'"{c}"' if " " in c and not c.startswith('"') else c for c in cmd)
    print("Running merge_bin:\n  ", command_str.replace('"', '\"'))
    result = env.Execute(command_str)

    os.makedirs(os.path.dirname(firmware_path_merged), exist_ok=True)
    shutil.copyfile(MERGED_BIN, firmware_path_merged)
    print(f'Firmware (merged) copied to: {firmware_path_merged}')
    return result

# Immer OTA.gz erstellen
env.AddPostAction(APP_BIN, copy_bin_and_gz)

# Optional merged .bin erzeugen
if ENABLE_MERGE_BIN:
    env.AddPostAction(APP_BIN, merge_bin)

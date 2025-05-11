Import("env")
import os
import shutil

# === Konfiguration ===
ENABLE_MERGE_BIN = True  # <<< NUR code flashen = False, ALLES flashen (inkl. FS) = True
# ======================

BUILD_DIR = env.subst("$BUILD_DIR")
PROGNAME = env.subst("${PROGNAME}")
APP_BIN = os.path.normpath(os.path.join(BUILD_DIR, f"{PROGNAME}.bin"))
MERGED_BIN = os.path.normpath(os.path.join(BUILD_DIR, f"{PROGNAME}_merged.bin"))
BOARD_CONFIG = env.BoardConfig()

project_name = env.GetProjectOption("custom_project_name") or "Firmware"
version = env.GetProjectOption("custom_version") or "0.0.0"
firmware_filename = f"{project_name}_V{version}.bin"
final_output_path = os.path.normpath(os.path.join(env.subst("$PROJECT_DIR"), ".firmware", firmware_filename))

def copy_bin_only(source, target, env):
    os.makedirs(os.path.dirname(final_output_path), exist_ok=True)
    shutil.copyfile(APP_BIN, final_output_path)
    print(f'Firmware (no merge) copied to: {final_output_path}')

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

    # Fix: sichere Shell-kompatible Zusammenführung
    command_str = " ".join(f'"{c}"' if " " in c and not c.startswith('"') else c for c in cmd)

    print("Running merge_bin:\n  ", command_str.replace('"', '\"'))
    result = env.Execute(command_str)

    os.makedirs(os.path.dirname(final_output_path), exist_ok=True)
    shutil.copyfile(MERGED_BIN, final_output_path)
    print(f'Firmware (merged) copied to: {final_output_path}')
    return result


# Richtige Datei verwenden
if ENABLE_MERGE_BIN:
    env.AddPostAction(APP_BIN, merge_bin)
    firmware_to_upload = MERGED_BIN
else:
    env.AddPostAction(APP_BIN, copy_bin_only)
    firmware_to_upload = APP_BIN

# Uploader definieren
#env.Replace(
#    UPLOADERFLAGS=[
#        "write_flash",
#        "--flash_freq", env.get("BOARD_F_FLASHFREQ", "40m"),
#        "--flash_mode", env.get("BOARD_F_FLASHMODE", "dio"),
#        "--flash_size", BOARD_CONFIG.get("upload.flash_size", "4MB"),
#        "0x0", firmware_to_upload
#    ],
#    UPLOADCMD='"$PYTHONEXE" "$UPLOADER" $UPLOADERFLAGS'
#)

Import("env")
import os
import shutil

# Saubere Pfaderstellung & Normalisierung
BUILD_DIR = env.subst("$BUILD_DIR")
PROGNAME = env.subst("${PROGNAME}")
APP_BIN = os.path.normpath(os.path.join(BUILD_DIR, f"{PROGNAME}.bin"))
MERGED_BIN = os.path.normpath(os.path.join(BUILD_DIR, f"{PROGNAME}_merged.bin"))

BOARD_CONFIG = env.BoardConfig()

def merge_bin(source, target, env):
    # Baue Liste der zu mergenenden Dateien
    flash_images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", []))
    app_offset = env.subst("$ESP32_APP_OFFSET") or "0x10000"
    flash_images += [app_offset, APP_BIN]

    # Konvertiere Pfade und Adressen für Shell-Aufruf
    flash_args = [
        f'"{os.path.normpath(env.subst(str(x)))}"' if not str(x).startswith("0x") else str(x)
        for x in flash_images
    ]

    # Merge-Befehl aufbauen
    cmd = [
        f'"{env.subst("$PYTHONEXE")}"',
        f'"{env.subst("$OBJCOPY")}"',
        "--chip", BOARD_CONFIG.get("build.mcu", "esp32"),
        "merge_bin",
        "--fill-flash-size", BOARD_CONFIG.get("upload.flash_size", "4MB"),
        "--output", f'"{MERGED_BIN}"',
    ] + flash_args

    print("Running merge_bin:\n  ", " \\\n   ".join(cmd))
    result = env.Execute(" ".join(cmd))

    # Projektname und Version aus platformio.ini holen
    project_name = env.GetProjectOption("custom_project_name") or "Firmware"
    version = env.GetProjectOption("custom_version") or "0.0.0"
    firmware_filename = f"{project_name}_V{version}.bin"

    # Zielpfad definieren
    final_output_path = os.path.normpath(os.path.join(env.subst("$PROJECT_DIR"), ".firmware", firmware_filename))
    os.makedirs(os.path.dirname(final_output_path), exist_ok=True)

    # Datei kopieren
    shutil.copyfile(MERGED_BIN, final_output_path)
    print(f'Firmware copied to: {final_output_path}')

    return result

# Merge nach Build ausführen
env.AddPostAction(APP_BIN, merge_bin)

# Uploader auf gemergte Firmware umstellen
env.Replace(
    UPLOADERFLAGS=[
        "write_flash",
        "--flash_freq", env.get("BOARD_F_FLASHFREQ", "40m"),
        "--flash_mode", env.get("BOARD_F_FLASHMODE", "dio"),
        "--flash_size", BOARD_CONFIG.get("upload.flash_size", "4MB"),
        "0x0", f'"{MERGED_BIN}"'
    ],
    UPLOADCMD=f'"{env.subst("$PYTHONEXE")}" "{env.subst("$UPLOADER")}" $UPLOADERFLAGS',
)

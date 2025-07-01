# ---------------------------------------------------------------------------- #
#        Compress files in src/www directory into a single header file.        #
# ---------------------------------------------------------------------------- #

import gzip
import os
import glob

WWW_DIR = os.path.join("src", "www")
OUTPUT_HEADER_NAME = "www.h"
OUTPUT_HEADER_FILE = os.path.join(WWW_DIR, OUTPUT_HEADER_NAME)

SUPPORTED_EXTENSIONS = ["*.html", "*.js", "*.css", "*.svg", "*.png"]

MIME_TYPES = {
    ".css":  "text/css",
    ".htm":  "text/html",
    ".html": "text/html",
    ".ico":  "image/x-icon",
    ".jpeg": "image/jpeg",
    ".jpg":  "image/jpeg",
    ".js":   "text/javascript",
    ".json": "application/json",
    ".png":  "image/png",
    ".svg":  "image/svg+xml",
}

def guess_mime_type(filename):
    _, ext = os.path.splitext(filename.lower())
    return MIME_TYPES.get(ext, "application/octet-stream")

def compress_and_generate_entry(input_file):
    # Compress in-memory, no temp file
    with open(input_file, "rb") as infile:
        data = infile.read()
        compressed_data = gzip.compress(data, compresslevel=9)

    # ------------ Generate a C array name based on the file name ------------ #
    # array_name = os.path.basename(input_file).replace(".", "_")
    # ---------- Generate a C array name based on the relative path ---------- #
    array_name = os.path.relpath(input_file, WWW_DIR).replace(os.sep, "_").replace(".", "_")

    entry = [f"const uint8_t {array_name}_gz[] PROGMEM = {{\n"]
    for i in range(0, len(compressed_data), 16):
        line = ', '.join(f'0x{b:02x}' for b in compressed_data[i:i+16])
        entry.append(f"  {line},\n")

    entry.append("};\n\n")
    entry.append(f"const unsigned int {array_name}_gz_len = {len(compressed_data)};\n")
    entry.append(f"const char * {array_name}_gz_mime = \"{guess_mime_type(input_file)}\";\n\n")
    file = os.path.relpath(input_file, WWW_DIR)
    print(f"Added: {file} as {array_name}_gz with MIME {guess_mime_type(input_file)}")
    return ''.join(entry)

def compress_files():

    if not os.path.isdir(WWW_DIR):
        print(f"❌ Error: {WWW_DIR} is not a valid directory")
        exit(1)

    files_to_process = set()
    for pattern in SUPPORTED_EXTENSIONS:
        files_to_process.update(glob.iglob(os.path.join(WWW_DIR, "**", pattern), recursive=True))

    files_to_process = list(files_to_process)
    if not files_to_process:
        print(f"☑️ No matching files found in {WWW_DIR}")
        exit(0)

    entries = []
    for fpath in files_to_process:
        entries.append(compress_and_generate_entry(fpath))

    with open(OUTPUT_HEADER_FILE, "w") as f:
        f.write("#ifndef WWW_H\n#define WWW_H\n\n#include <pgmspace.h>\n\n")
        for entry in entries:
            f.write(entry)

        f.write("\n#endif // WWW_H\n")

    print(f"\n✅ All files combined into: {OUTPUT_HEADER_FILE}")

compress_files()
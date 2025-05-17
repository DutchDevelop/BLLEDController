import gzip
import os
import glob

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
HTML_DIR = os.path.join(SCRIPT_DIR, "src", "www")

OUTPUT_HEADER_NAME = "www.h"
OUTPUT_HEADER = os.path.join(HTML_DIR, OUTPUT_HEADER_NAME)

SUPPORTED_EXTENSIONS = ["*.html", "*.js", "*.css","*.svg","*.png"]

MIME_TYPES = {
    ".html": "text/html",
    ".htm": "text/html",
    ".js": "application/javascript",
    ".css": "text/css",
    ".json": "application/json",
    ".png": "image/png",
    ".jpg": "image/jpeg",
    ".jpeg": "image/jpeg",
    ".svg": "image/svg+xml",
    ".ico": "image/x-icon",
}

def generate_header_start(f):
    f.write("#ifndef WWW_H\n#define WWW_H\n\n")
    f.write("#include <pgmspace.h>\n\n")

def generate_header_end(f):
    f.write("\n#endif // WWW_H\n")

def guess_mime_type(filename):
    _, ext = os.path.splitext(filename.lower())
    return MIME_TYPES.get(ext, "application/octet-stream")

def compress_and_append_file(input_file, f):
    compressed_file = input_file + ".gz"

    with open(input_file, "rb") as infile:
        with gzip.open(compressed_file, "wb", compresslevel=6) as outfile:
            outfile.write(infile.read())

    with open(compressed_file, "rb") as cf:
        data = cf.read()

    array_name = os.path.basename(input_file).replace(".", "_")

    f.write(f"const uint8_t {array_name}_gz[] PROGMEM = {{\n")
    for i in range(0, len(data), 16):
        line = ', '.join(f'0x{b:02x}' for b in data[i:i+16])
        f.write(f"  {line},\n")
    f.write("};\n\n")
    f.write(f"const unsigned int {array_name}_gz_len = {len(data)};\n")
    f.write(f"const char * {array_name}_gz_mime = \"{guess_mime_type(input_file)}\";\n\n")

    os.remove(compressed_file)
    print(f"Appended: {array_name}_gz to {OUTPUT_HEADER_NAME} with MIME {guess_mime_type(input_file)}")

if __name__ == "__main__":
    if not os.path.isdir(HTML_DIR):
        print(f"Error: {HTML_DIR} is not a valid directory")
        exit(1)

    files_to_process = []
    for pattern in SUPPORTED_EXTENSIONS:
        files_to_process.extend(glob.glob(os.path.join(HTML_DIR, pattern)))

    if not files_to_process:
        print(f"No matching files found in {HTML_DIR}")
        exit(0)

    with open(OUTPUT_HEADER, "w") as f:
        generate_header_start(f)

        for file in files_to_process:
            compress_and_append_file(file, f)

        generate_header_end(f)

    print(f"\n✅ All files combined into: {OUTPUT_HEADER}")

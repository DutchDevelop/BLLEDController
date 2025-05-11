import gzip
import os
import glob

# Verzeichnis RELATIV ZUM AKTUELLEN SKRIPT ermitteln
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
HTML_DIR = os.path.join(SCRIPT_DIR, "src", "www")

def compress_html(html_file):
    header_file = os.path.splitext(html_file)[0] + ".h"
    compressed_file = html_file + ".gz"

    # Komprimiere Datei als GZIP
    with open(html_file, "rb") as input_file:
        with gzip.open(compressed_file, "wb", compresslevel=6) as output_file:
            output_file.write(input_file.read())

    # Lese komprimierte Datei
    with open(compressed_file, "rb") as f:
        data = f.read()

    # Array-Name
    array_name = os.path.basename(html_file).replace(".", "_")

    # Schreibe Header-Datei
    with open(header_file, "w") as f:
        f.write("#include <pgmspace.h>\n\n")
        f.write(f"const uint8_t {array_name}_gz[] PROGMEM = {{\n")

        for i in range(0, len(data), 16):
            line = ', '.join(f'0x{b:02x}' for b in data[i:i+16])
            f.write(f"  {line},\n")

        f.write("};\n\n")
        f.write(f"const unsigned int {array_name}_gz_len = {len(data)};\n")

    os.remove(compressed_file)
    print("Generated:", header_file)

if __name__ == "__main__":
    if not os.path.isdir(HTML_DIR):
        print(f"Error: {HTML_DIR} is not a valid directory")
        exit(1)

    html_files = glob.glob(os.path.join(HTML_DIR, "*.html"))

    if not html_files:
        print(f"No .html files found in {HTML_DIR}")
        exit(0)

    for html_file in html_files:
        compress_html(html_file)

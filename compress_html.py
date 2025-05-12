import gzip
import os
import glob

# Verzeichnis RELATIV ZUM AKTUELLEN SKRIPT ermitteln
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
HTML_DIR = os.path.join(SCRIPT_DIR, "src", "www")

# Name der gemeinsamen Ausgabedatei (liegt IM HTML-DIR)
OUTPUT_HEADER_NAME = "www.h"
OUTPUT_HEADER = os.path.join(HTML_DIR, OUTPUT_HEADER_NAME)

def generate_header_start(f):
    f.write("#ifndef WWW_H\n#define WWW_H\n\n")
    f.write("#include <pgmspace.h>\n\n")

def generate_header_end(f):
    f.write("\n#endif // WWW_H\n")

def compress_and_append_html(html_file, f):
    compressed_file = html_file + ".gz"

    # Komprimiere Datei als GZIP
    with open(html_file, "rb") as input_file:
        with gzip.open(compressed_file, "wb", compresslevel=6) as output_file:
            output_file.write(input_file.read())

    # Lese komprimierte Datei
    with open(compressed_file, "rb") as cf:
        data = cf.read()

    # Erzeuge sauberen Array-Namen
    array_name = os.path.basename(html_file).replace(".", "_")

    # Schreibe Array in die geöffnete Datei f
    f.write(f"const uint8_t {array_name}_gz[] PROGMEM = {{\n")

    for i in range(0, len(data), 16):
        line = ', '.join(f'0x{b:02x}' for b in data[i:i+16])
        f.write(f"  {line},\n")

    f.write("};\n\n")
    f.write(f"const unsigned int {array_name}_gz_len = {len(data)};\n\n")

    os.remove(compressed_file)
    print(f"Appended: {array_name}_gz to {OUTPUT_HEADER_NAME}")

if __name__ == "__main__":
    if not os.path.isdir(HTML_DIR):
        print(f"Error: {HTML_DIR} is not a valid directory")
        exit(1)

    html_files = glob.glob(os.path.join(HTML_DIR, "*.html"))

    if not html_files:
        print(f"No .html files found in {HTML_DIR}")
        exit(0)

    # Öffne die gemeinsame Header-Datei (überschreibt immer komplett)
    with open(OUTPUT_HEADER, "w") as f:
        generate_header_start(f)

        for html_file in html_files:
            compress_and_append_html(html_file, f)

        generate_header_end(f)

    print(f"\n✅ All files combined into: {OUTPUT_HEADER}")

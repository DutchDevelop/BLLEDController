import sys
import gzip
import os

def compress_html(html_file):
    # Erzeuge Namen der Header-Datei
    header_file = os.path.splitext(html_file)[0] + ".h"

    # Komprimiere HTML-Datei als Gzip
    compressed_file = html_file + ".gz"
    with open(html_file, "rb") as input_file:
        with gzip.open(compressed_file, "wb", compresslevel=6) as output_file:
            output_file.write(input_file.read())

    # Lese komprimierte Datei
    with open(compressed_file, "rb") as f:
        data = f.read()

    # Erzeuge Array-Namen aus Dateinamen
    array_name = os.path.basename(html_file).replace(".", "_")

    # Schreibe C-Header-Datei mit PROGMEM Array
    with open(header_file, "w") as f:
        f.write("#include <pgmspace.h>\n\n")
        f.write(f"const uint8_t {array_name}_gz[] PROGMEM = {{\n")

        for i in range(0, len(data), 16):
            line = ', '.join(f'0x{b:02x}' for b in data[i:i+16])
            f.write(f"  {line},\n")

        f.write("};\n\n")
        f.write(f"const unsigned int {array_name}_gz_len = {len(data)};\n")

    # Entferne temporäre .gz-Datei
    os.remove(compressed_file)
    print("Generated:", header_file)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python compress_html.py <html_file>")
        sys.exit(1)

    html_file = sys.argv[1]
    compress_html(html_file)

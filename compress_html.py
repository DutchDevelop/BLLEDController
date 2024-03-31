import sys
import gzip
import subprocess
import os

def compress_html(html_file):
    # Compress HTML file
    compressed_file = html_file + ".gz"
    with open(html_file, "rb") as input_file:
        with gzip.open(compressed_file, "wb", compresslevel=6) as output_file:
            output_file.write(input_file.read())

    # Generate header file
    header_file = os.path.splitext(html_file)[0] + ".h"
    print("Header file path:", header_file)  # Debug print
    result = subprocess.run(["xxd", "-i", compressed_file], stdout=open(header_file, "w"))
    if result.returncode != 0:
        print("Error: xxd failed to generate the header file")
        return

    # Modify the generated header file to include PROGMEM attribute
    with open(header_file, "r") as file:
        content = file.read()
    
    with open(header_file, "w") as file:
        # Add PROGMEM attribute after the array declaration
        content = content.replace("unsigned char ", "const uint8_t ")
        # Remove "__" prefix from the length variable
        content = content.replace("unsigned int ", "unsigned int ")
        # Add PROGMEM attribute after the variable name
        content = content.replace("const uint8_t ", "const uint8_t ", 1).replace("{", "PROGMEM {")
        file.write(content)

    # Clean up
    os.remove(compressed_file)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python compress_html.py <html_file>")
        sys.exit(1)
    
    html_file = sys.argv[1]
    compress_html(html_file)

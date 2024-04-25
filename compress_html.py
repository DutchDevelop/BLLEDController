import sys
import gzip
import subprocess
import os

def should_compress(html_file, header_file):
    if not os.path.exists(header_file):
        return True
    
    html_modified_time = os.path.getmtime(html_file)
    header_modified_time = os.path.getmtime(header_file)
    
    return html_modified_time > header_modified_time

def compress_html(html_file):
    # Check if compression is necessary
    header_file = os.path.splitext(html_file)[0] + ".h"
    if not should_compress(html_file, header_file):
        print("No need to compress:", html_file)
        return
    
    # Compress HTML file
    compressed_file = html_file + ".gz"
    with open(html_file, "rb") as input_file:
        with gzip.open(compressed_file, "wb", compresslevel=6) as output_file:
            output_file.write(input_file.read())

    # Check if .h file exists and if HTML file is newer
    if os.path.exists(header_file):
        header_modified_time = os.path.getmtime(header_file)
        html_modified_time = os.path.getmtime(html_file)
        if html_modified_time < header_modified_time:
            print("No need to generate header file. HTML file is not modified.")
            os.remove(compressed_file)
            return

    # Generate header file
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

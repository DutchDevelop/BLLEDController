import subprocess
import os

def compress_html_files():
    # Change the working directory to the location of the compression script
    os.chdir("src/www")

    # List of HTML files to compress
    html_files = ["setuppage.html"]
    for html_file in html_files:
        print("Compressing file:", html_file)
        subprocess.run(["python", "../../compress_html.py", html_file])

compress_html_files()
## BL Led Controller

The BL Led Controller is an ESP32 based device that connects to your Bambulab X1,X1C,P1P Or P1S and controls the LED strip based on the state of the printer.

### Flashing and Setup
1. go to the [Web Flasher](https://softwarecrash.github.io/BLLED-Flasher/)
2. connect your ESP32
3. Select the Firmware build you want
4. Click on Flash
5. Search and connect to a WiFi Hotspot called "BLLED-AP"
6. Surf to http://192.168.4.1
7. Select your WiFi and Enter passwort, Optional enter all the Printer informations
8. enjoy :)

Questions? Read the [Docs](https://github.com/softwarecrash/BLLEDController-NG/blob/main/docs/manual.md)

![main setup_LED](https://github.com/user-attachments/assets/52778d28-98cd-496e-bfce-aca49cd4de89)
![main_setup_printer](https://github.com/user-attachments/assets/1969e4bd-0887-4a0c-a965-8ec575e23396)
![main_setup_error](https://github.com/user-attachments/assets/b026260c-5c33-4f5c-922a-fcc601cb9be1)
![main_setup_debug](https://github.com/user-attachments/assets/6d5a6e48-b052-4305-99f5-5d9c906eb8c1)
![main_setup_navigation](https://github.com/user-attachments/assets/78041d10-2edb-42e2-b600-6cde735d2d1f)
![setup_wifi](https://github.com/user-attachments/assets/4ef7631c-38f9-4424-ad88-72a335286c27)
![setup_wifi_printer](https://github.com/user-attachments/assets/da354544-710d-4e10-9dff-fc2cc1951857)
![ota](https://github.com/user-attachments/assets/7f223361-f811-4ff4-b3fc-45b2a1222c07)
![backup_restore](https://github.com/user-attachments/assets/b21899ba-c97f-48b6-8bc3-a87797029d14)
![navigation](https://github.com/user-attachments/assets/027fe724-ffc6-453b-ba73-c82c1ddb012e)





- Connects to Bambulab X1,X1C,P1P Or P1S
- Controls LED strip based on printer state

### Development Environment

To contribute to the BL Led Controller project, you'll need the following tools:

### Tools & Libraries Used

- [Visual Studio Code](https://code.visualstudio.com/): A lightweight and powerful source code editor.
- [PlatformIO](https://platformio.org/): An open-source ecosystem for IoT development.
- [Python](https://www.python.org/): A programming language used for scripting and automation.
- [qpdf](https://qpdf.sourceforge.io/): A command-line tool and library of compression tools (`gzip`)

### Building and Running the Project
1. Clone the repository to your local machine.
2. Open the project folder in Visual Studio Code.
3. Ensure that PlatformIO is installed and configured in your Visual Studio Code environment.
4. Connect your BLLED device ESP32 to your computer.
6. Build the project by clicking on the PlatformIO icon in the sidebar and selecting "Build" from the available options.
7. Once the build process is complete, upload the firmware to your device using the "Upload" option in PlatformIO.
8. After uploading the firmware, your BL Led Controller device should be ready to use.

### Setup Instructions
Once you have uploaded the firmware to your device, please visit the [dutchdevelop.com/bl-led-controller](https://dutchdevelop.com/bl-led-controller) website for detailed setup instructions.


### Development Notes

#### Generating .h Files for Compressed HTML

In embedded applications, HTML content is efficiently stored in PROGMEM memory. To achieve this, .h files are generated from compressed HTML files for webpages (i.e., `src/www/setuppage.html`) that are run on the device.

- The `compress_html.py` is used to compress HTML files and generate corresponding .h files and is integrated into the build process and executed as a pre-build step in `platform.ini`
- The generated .h files should not be checked into git (see `.gitignore`)

### License

The BL Led Controller is released under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) license. See the [LICENSE](https://github.com/DutchDevelop/BLLEDController/blob/main/LICENSE) file for more details.

### Credits
- **[DutchDeveloper](https://dutchdevelop.com/)**: Lead programmer
- **[Modbot](https://github.com/Modbot)**: Tester for X1C, P1P & P1S
- **[xps3riments](https://github.com/xps3riments)**: Inspiration for the foundation of the code
- **[longrackslabs](https://github.com/longrackslabs)**: Build process, documentation, developer & community support

### Author

This project was created by [DutchDeveloper](https://dutchdevelop.com/).

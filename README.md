## BL Led Controller

The BL Led Controller is an ESP8266 / ESP32 based device that connects to your Bambulab X1,X1C,P1P Or P1S and controls the LED strip based on the state of the printer.

### Features

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
4. Connect your BLLED device (ESP8266 or ESP32) to your computer.
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

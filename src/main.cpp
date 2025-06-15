#include <Arduino.h>
bool shouldRestart = false;
unsigned long restartRequestTime = 0;
#include "./blled/logSerial.h"
#include "./blled/leds.h"
#include "./blled/filesystem.h"
#include "./blled/types.h"
#include "./blled/bblPrinterDiscovery.h"
#include "./blled/web-server.h"
#include "./blled/mqttmanager.h"
#include "./blled/serialmanager.h"
#include "./blled/wifi-manager.h"
#include "./blled/ssdp.h"


int wifi_reconnect_count = 0;

void defaultcolors()
{
    LogSerial.println(F("Setting default customisable colors"));
    printerConfig.runningColor = hex2rgb("#000000", 255, 255); // WHITE Running
    printerConfig.testColor = hex2rgb("#3F3CFB");              // Violet Test
    printerConfig.finishColor = hex2rgb("#00FF00");            // Green Finish

    printerConfig.stage14Color = hex2rgb("#000000"); // OFF Cleaning Nozzle
    printerConfig.stage1Color = hex2rgb("#000055");  // OFF Bed Leveling
    printerConfig.stage8Color = hex2rgb("#000000");  // OFF Calibrating Extrusion
    printerConfig.stage9Color = hex2rgb("#000000");  // OFF Scanning Bed Surface
    printerConfig.stage10Color = hex2rgb("#000000"); // OFF First Layer Inspection

    printerConfig.wifiRGB = hex2rgb("#FFA500"); // Orange Wifi Scan

    printerConfig.pauseRGB = hex2rgb("#0000FF");          // Blue Pause
    printerConfig.firstlayerRGB = hex2rgb("#0000FF");     // Blue
    printerConfig.nozzleclogRGB = hex2rgb("#0000FF");     // Blue
    printerConfig.hmsSeriousRGB = hex2rgb("#FF0000");     // Red
    printerConfig.hmsFatalRGB = hex2rgb("#FF0000");       // Red
    printerConfig.filamentRunoutRGB = hex2rgb("#FF0000"); // Red
    printerConfig.frontCoverRGB = hex2rgb("#FF0000");     // Red
    printerConfig.nozzleTempRGB = hex2rgb("#FF0000");     // Red
    printerConfig.bedTempRGB = hex2rgb("#FF0000");        // Red
}

void setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println(F("Initializing"));
    Serial.println(ESP.getFreeHeap());
    Serial.println("");
    Serial.print(F("** Using firmware version: "));
    Serial.print(globalVariables.FWVersion);
    Serial.println(F(" **"));
    Serial.println("");
    defaultcolors();
    setupLeds();
    tweenToColor(100, 100, 100, 100, 100); // ALL LEDS ON
    Serial.println(F(""));

    tweenToColor(255, 0, 0, 0, 0); // RED
    setupFileSystem();
    loadFileSystem();
    Serial.println(F(""));

    tweenToColor(printerConfig.wifiRGB); // Customisable - Default is ORANGE
    setupSerial();

    if (strlen(globalVariables.SSID) == 0 || strlen(globalVariables.APPW) == 0)
    {
        Serial.println(F("SSID or password is missing. Please configure both by going to: https://dutchdevelop.com/blled-configuration-setup/"));
        tweenToColor(100, 0, 100, 0, 0); // PINK
        startAPMode();
        setupWebserver();
        return;
    }
    else

        if (!connectToWifi())
    {
        Serial.println(F("[WiFiManager] Not connected → AP Mode"));
        startAPMode();
        setupWebserver();
        return;
    }
    else
    {
        Serial.println(F("[WiFiManager] connected. Starting webUI."));
        tweenToColor(0, 0, 255, 0, 0); // BLUE
        setupWebserver();
    }

    start_ssdp();

    tweenToColor(34, 224, 238, 0, 0); // CYAN
    setupMqtt();

    Serial.println();
    Serial.print(F("** BLLED Controller started "));
    Serial.print(F("using firmware version: "));
    Serial.print(globalVariables.FWVersion);
    Serial.println(F(" **"));
    Serial.println();
    globalVariables.started = true;
    Serial.println(F("Updating LEDs from Setup"));
    updateleds();
}

void loop()
{
    serialLoop();
    if (globalVariables.started)
    {
        websocketLoop();
        ledsloop();

        if (WiFi.status() != WL_CONNECTED)
        {
            LogSerial.print(F("Wifi connection dropped.  "));
            LogSerial.print(F("Wifi Status: "));
            LogSerial.println(wl_status_to_string(WiFi.status()));
            LogSerial.println(F("Attempting to reconnect to WiFi..."));
            wifi_reconnect_count += 1;
            if (wifi_reconnect_count <= 2)
            {
                WiFi.disconnect();
                delay(100);
                WiFi.reconnect();
            }
            else
            {
                // Not connecting after 10 simple disconnect / reconnects
                // Do something more drastic in case needing to switch to new AP
                scanNetwork();
                connectToWifi();
                wifi_reconnect_count = 0;
            }
        }
        if (WiFi.getMode() == WIFI_AP)
        {
            dnsServer.processNextRequest();
        }
        if(WiFi.status() == WL_CONNECTED && WiFi.getMode() != WIFI_AP)
        {
            bblSearchPrinters();
        }
    }
    if (printerConfig.rescanWiFiNetwork)
    {
        LogSerial.println(F("Web submitted refresh of Wifi Scan (assigning Strongest AP)"));
        tweenToColor(printerConfig.wifiRGB); // Customisable - Default is ORANGE
        scanNetwork();                       // Sets the MAC address for following connection attempt
        printerConfig.rescanWiFiNetwork = false;
        updateleds();
    }
    if (shouldRestart && millis() - restartRequestTime > 1500)
    {
        LogSerial.println(F("[WiFiSetup] Restarting now..."));
        ESP.restart();
    }
}
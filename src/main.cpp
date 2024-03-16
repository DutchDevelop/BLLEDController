#include <Arduino.h>
#include "web-server.h"
#include "mqttmanager.h"
#include "filesystem.h"
#include "types.h"
#include "leds.h"
#include "serialmanager.h"
#include "wifi-manager.h"

void defaultcolors(){
    Serial.println(F("Setting default customisable colors"));
    strcpy(printerConfig.testRGB,           "#3F3CFB"); //Violet
    strcpy(printerConfig.wifiRGB,           "#FFA500"); //Orange
    strcpy(printerConfig.finishRGB,         "#00FF00"); //Green
    strcpy(printerConfig.pauseRGB,          "#0000FF"); //Blue
    strcpy(printerConfig.firstlayerRGB,     "#0000FF"); //Blue
    strcpy(printerConfig.nozzleclogRGB,     "#0000FF"); //Blue
    strcpy(printerConfig.hmsSeriousRGB,     "#FF0000"); //Red
    strcpy(printerConfig.hmsFatalRGB,       "#FF0000"); //Red
    strcpy(printerConfig.filamentRunoutRGB, "#FF0000"); //Red
    strcpy(printerConfig.frontCoverRGB,     "#FF0000"); //Red
    strcpy(printerConfig.nozzleTempRGB,     "#FF0000"); //Red
    strcpy(printerConfig.bedTempRGB,        "#FF0000"); //Red
}

void setup(){
    Serial.begin(115200);
    delay(100);
    Serial.println(F("Initializing"));
    Serial.println(ESP.getFreeHeap());
    defaultcolors();
    setupLeds();
    tweenToColor(255,255,255,255,255); //ALL LEDS ON
    delay(1000);

    tweenToColor(255,0,0,0,0); //RED
    setupFileSystem();
    loadFileSystem();
    delay(500);

    tweenToColor(printerConfig.wifiRGB); //Customisable - Default is ORANGE
    setupSerial();

    if (strlen(globalVariables.SSID) == 0 || strlen(globalVariables.APPW) == 0) {
        Serial.println(F("SSID or password is missing. Please configure both by going to: https://dutchdevelop.com/blled-configuration-setup/"));
        tweenToColor(255,0,255,0,0); //PINK
        return;
    }
   
    scanNetwork(); //Sets the MAC address for following connection attempt
    if(!connectToWifi()){
        return;
    }

    tweenToColor(0,0,255,0,0); //BLUE
    setupWebserver();
    delay(500);

    
    tweenToColor(34,224,238,0,0); //CYAN
    setupMqtt();

    Serial.println();
    Serial.println(F("BLLED Controller started"));
    Serial.println();
    tweenToColor(0,0,0,0,0,3000); //Fade to Black before starting
    globalVariables.started = true;
}

void loop(){
    serialLoop();
    if(printerConfig.maintMode){
        //Doesn't require monitoring of Wifi or MQTT if LEDs only need to be ON
        webserverloop();
        if(printerConfig.updateMaintenance) updateleds();
        printerVariables.inactivityStartms = millis(); // Don't turn light off (due to inactivity) if in Maintenance Mode
    }
    else if (printerVariables.testcolorEnabled){
        //Doesn't require monitoring of Wifi or MQTT if LEDs set to a custom color
        webserverloop();
        if(printerConfig.updateTestLEDS) ledsloop();
    }
    else if (globalVariables.started){
        mqttloop();
        webserverloop();
        ledsloop();
        
        if (WiFi.status() != WL_CONNECTED){
            Serial.println(F("Reconnecting to WiFi..."));
            WiFi.disconnect();
            delay(10);
            WiFi.reconnect();
        }
    }
    if(printerConfig.rescanWiFiNetwork)
    {
        Serial.println(F("Web submitted refresh of Wifi Scan (assigning Strongest AP)"));
        tweenToColor(printerConfig.wifiRGB); //Customisable - Default is ORANGE
        scanNetwork(); //Sets the MAC address for following connection attempt
        printerConfig.rescanWiFiNetwork = false;
        updateleds();
    }
}

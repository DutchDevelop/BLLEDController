#ifndef _BLLEDWIFI_MANAGER
#define _BLLEDWIFI_MANAGER

#include <Arduino.h>
#include <ArduinoJson.h> 

#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include "filesystem.h"
#include "types.h"

bool shouldSaveConfig = true;
int connectionAttempts = 0;
const int maxConnectionAttempts = 10;

void restartprocess(){
    deleteFileSystem();
    delay(3000);
}

void configModeCallback() {
  Serial.println("Entered config mode");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

bool setupWifi(){
    loadFileSystem();
    delay(1000);
    Serial.println(F("-------------------------------------"));

    if (strlen(globalVariables.SSID) == 0 || strlen(globalVariables.APPW) == 0) {
        Serial.println(F("SSID or password is missing. Please configure both by going to: https://dutchdevelop.com/blled-configuration-setup/"));
        return false;
    }

    while (connectionAttempts < maxConnectionAttempts) {
        if (WiFi.status() == WL_CONNECTED)
            break;
        
        //WiFi.mode(WIFI_STA);
        WiFi.begin(globalVariables.SSID, globalVariables.APPW);
        delay(1000);
        
        Serial.print(F("Connecting to WIFI.. "));
        Serial.println(globalVariables.SSID);
        Serial.println(globalVariables.APPW);
        delay(8000); // can probably be lower?
        connectionAttempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED){
        Serial.println(F("Failed to connect to wifi."));
        return false;
    }
    
    int signalStrength = WiFi.RSSI();
    Serial.println(F("Connected To Wifi:"));
    Serial.println(signalStrength);
    Serial.println(F(" With Signal Strength"));
    Serial.println(F("-------------------------------------"));
    Serial.print(F("Head over to http://"));
    Serial.println(WiFi.localIP());
    //Serial.print(F("Login Details User: BLLC, Password: "));
    //Serial.println(printerConfig.webpagePassword);
    Serial.println(F("To configure further"));
    Serial.println(F("-------------------------------------"));


    #ifdef ARDUINO_ARCH_ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif
    
    return true;
}

#endif

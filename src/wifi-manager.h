#ifndef _BLLEDWIFI_MANAGER
#define _BLLEDWIFI_MANAGER

#include <Arduino.h>
#include <ArduinoJson.h> 

#include <WiFi.h>
#include "filesystem.h"
#include "types.h"

bool shouldSaveConfig = true;
int connectionAttempts = 0;

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

    while (true) { // stay in this loop until wifi connects
        if (WiFi.status() == WL_CONNECTED)
            break;
        
        //WiFi.mode(WIFI_STA);
        WiFi.begin(globalVariables.SSID, globalVariables.APPW);
        delay(1000);
        
        Serial.print(F("Connecting to WIFI.. Attempt #"));
        Serial.print(connectionAttempts);
        Serial.print(F(" SSID: "));
        Serial.print(globalVariables.SSID);
        Serial.print(F(" APPW: "));
        Serial.println(globalVariables.APPW);
        delay(10000); // Giving enough time to connect
        connectionAttempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED){
        Serial.println(F("Failed to connect to wifi."));
        return false;
    }
    
    int signalStrength = WiFi.RSSI();
    Serial.println(F("Connected To Wifi With Signal Strength: "));
    Serial.println(signalStrength);
    Serial.println();
    delay(10);
    Serial.print("IP_ADDRESS:"); // Unique identifier for the IP address
    Serial.println(WiFi.localIP());

    #ifdef ARDUINO_ARCH_ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif
    
    #ifdef ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif
    
    return true;
}

#endif

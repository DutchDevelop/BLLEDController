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
const int maxConnectionAttempts = 3;

void restartprocess(){
    deleteFileSystem();
    delay(3000);
}

void configModeCallback() {
  Serial.println("Entered config mode");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void setupWifi(){
    loadFileSystem();

    Serial.println(F("-------------------------------------"));
    WiFi.begin(globalVariables.SSID, globalVariables.APPW);
    delay(8000);

    while (connectionAttempts < maxConnectionAttempts) {
        if (WiFi.status() != WL_CONNECTED){
            Serial.print(F("Connecting to WIFI.. "));
            Serial.println(globalVariables.SSID);
            Serial.println(globalVariables.APPW);
            delay(8000);
        };
        connectionAttempts++;
    }
    if (WiFi.status() != WL_CONNECTED){
        Serial.println(F("Failed to connect to wifi."));
        return;
    }
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
}

#endif
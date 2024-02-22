#ifndef _BLLEDWIFI_MANAGER
#define _BLLEDWIFI_MANAGER

#include <Arduino.h>
#include <ArduinoJson.h> 

#include <WiFi.h>
#include "filesystem.h"
#include "types.h"

bool shouldSaveConfig = true;
int connectionAttempts = 0;

void configModeCallback() {
  Serial.println("Entered config mode");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

bool connectToWifi(){
    Serial.println(F("-------------------------------------"));
    WiFi.mode(WIFI_STA);
    delay(10);
    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(globalVariables.SSID, globalVariables.APPW);
        delay(1000);

        wl_status_t status = WiFi.status();
        switch (status)
        {
            case WL_IDLE_STATUS:
                Serial.print(F("Connecting to WIFI.. Attempt #"));
                Serial.print(connectionAttempts);
                Serial.print(F(" SSID: "));
                Serial.print(globalVariables.SSID);
                Serial.print(F(" APPW: "));
                Serial.println(globalVariables.APPW);
                break;
            case WL_NO_SSID_AVAIL:
                Serial.print(F("Bad WiFi credentials"));
                return false;
            case WL_CONNECT_FAILED:
            case WL_DISCONNECTED:
                Serial.print(F("Check your WiFi credentials."));
                return false;
            default:
                break;
        }

        delay(10000); // Giving enough time to connect
        connectionAttempts++;
    }

    #ifdef ARDUINO_ARCH_ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif
    
    #ifdef ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif

    delay(1000);
    int signalStrength = WiFi.RSSI();
    Serial.println(F("Connected To Wifi With Signal Strength: "));
    Serial.println(signalStrength);
    Serial.println();
    delay(10);
    Serial.print("IP_ADDRESS:"); // Unique identifier for the IP address
    Serial.println(WiFi.localIP());
    
    return true;
};

#endif

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

int str2mac(const char* mac, uint8_t* values){
    if( 6 == sscanf( mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&values[0], &values[1], &values[2],&values[3], &values[4], &values[5] ) ){
        return 1;
    }else{
        return 0;
    }
}

bool connectToWifi(){
    Serial.println(F("-------------------------------------"));
    WiFi.mode(WIFI_STA);
    delay(10);
    while (WiFi.status() != WL_CONNECTED) {
        //If optional MAC address for AP not set, connect to first available AP with SSID Name
        if(strlen(printerConfig.BSSID) == 0){
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("Plain Wifi connection attempt to "));
                Serial.println(globalVariables.SSID);
            }
            WiFi.begin(globalVariables.SSID, globalVariables.APPW);
        }
        else{
            uint8_t bssid[6] = {0};

            //Convert to Text (Debugging code to check contacts of array)
            //char tempBSSID[18];
            //snprintf(tempBSSID, sizeof(tempBSSID), "%02x:%02x:%02x:%02x:%02x:%02x", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
            //Serial.print(F("Ary to STR: "));
            //Serial.println(tempBSSID);

            //Function to convert string MAC address to 6 byte array
            if(str2mac(printerConfig.BSSID,bssid)){
                if (printerConfig.debuging || printerConfig.debugingchange){
                    Serial.print(F("Alternate Wifi connection attempt to MAC: "));
                    Serial.println(printerConfig.BSSID);
                }   
                WiFi.begin(globalVariables.SSID, globalVariables.APPW, 0, bssid);
            }
            else{
                if (printerConfig.debuging || printerConfig.debugingchange){
                    Serial.print(F("Alternate MAC converion Failed, reverting to "));
                    Serial.println(globalVariables.SSID);
                } 
                WiFi.begin(globalVariables.SSID, globalVariables.APPW);
            }

                
        }
        
        // Reference https://stackoverflow.com/questions/70415075/esp32-select-one-from-multiple-wifi-ap-with-same-name-ssid
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
                Serial.print(F("ConnectFailed."));
            case WL_DISCONNECTED:
                Serial.print(F("Disconnected - will attempt again."));
                //return false; - This was blocking any attempt to reconnect
            default:
                break;
        }
        Serial.println(F("Waiting 10sec"));
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
    Serial.print(F("Connected To Wifi '"));
    Serial.print(globalVariables.SSID);
    Serial.print(F("' using MAC address: '"));
    Serial.print(WiFi.BSSIDstr());
    Serial.print(F("' with Signal Strength: "));
    Serial.println(signalStrength);
    delay(10);
    Serial.print("IP_ADDRESS:"); // Unique identifier for the IP address
    Serial.println(WiFi.localIP());
    
    return true;
};

#endif

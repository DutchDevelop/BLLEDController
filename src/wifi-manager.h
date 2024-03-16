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
  Serial.println(F("Entered config mode"));
  Serial.print(F("AP IP address: "));
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
    WiFi.mode(WIFI_STA);   //ESP32 connects to an access point
    delay(10);

    if(strlen(printerConfig.BSSID) == 0){
        WiFi.begin(globalVariables.SSID, globalVariables.APPW);
    }
    else{
        // Reference https://stackoverflow.com/questions/70415075/esp32-select-one-from-multiple-wifi-ap-with-same-name-ssid
        uint8_t bssid[6] = {0};

        //Function to convert String MAC address to 6 byte array
        if(str2mac(printerConfig.BSSID,bssid)){
            WiFi.begin(globalVariables.SSID, globalVariables.APPW, 0, bssid);
        }
        else{
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("Parsing MAC Address Failed, reverting to "));
                Serial.println(globalVariables.SSID);
            } 
            WiFi.begin(globalVariables.SSID, globalVariables.APPW);
        }
    }

    wl_status_t status = WiFi.status();

    while (status != WL_CONNECTED) {

        if(status != WiFi.status()){
            status = WiFi.status();
            switch (status)
            {
                case WL_IDLE_STATUS:
                    Serial.print(F("Connecting to WIFI.. Attempt #"));
                    Serial.println(connectionAttempts);
                    Serial.print(F(" SSID: "));
                    Serial.print(globalVariables.SSID);
                    Serial.print(F("   APPW: "));
                    Serial.print(globalVariables.APPW);
                    if(strlen(printerConfig.BSSID) > 0){
                        Serial.print(F("   BSSID: "));
                        Serial.print(globalVariables.SSID);
                    }
                    Serial.println();
                    break;
                case WL_NO_SSID_AVAIL:
                    Serial.print(F("Bad WiFi credentials"));
                    return false;
                case WL_CONNECT_FAILED:
                    Serial.print(F("Connect Failed."));
                    break;
                case WL_DISCONNECTED:
                    Serial.print(F("Disconnected. (Check low RSSI)"));
                default:
                    break;
            }
        }
        delay(1000); // Giving time to connect
        connectionAttempts++;
    }

    #ifdef ARDUINO_ARCH_ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif
    
    #ifdef ESP32
        WiFi.setTxPower(WIFI_POWER_19_5dBm); // https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
    #endif

    Serial.print(F("Connected To Wifi Access Point:  "));
    Serial.println(globalVariables.SSID);
    Serial.print(F("Specific BSSID:                  "));  //MAC address of connected AP
    Serial.println(WiFi.BSSIDstr());
    Serial.print(F("RSSI (Signal Strength):          "));
    Serial.println(WiFi.RSSI());
    Serial.print(F("IP_ADDRESS:                      ")); // Unique identifier for the IP address
    Serial.println(WiFi.localIP());
    Serial.println();
    return true;
};

void scanNetwork()
{
    //Reference / Credit - https://www.esp32.com/viewtopic.php?t=18979#p70299
    int bestRSSI = -200;
    String bestBSSID = "";

    Serial.println(F("Wifi network scan start"));
    int n = WiFi.scanNetworks();
    Serial.println(F("Network scan complete"));

    if (n == 0) {
        Serial.println(F("No wifi networks found :("));
    } else {
        Serial.print(n); // Count
        Serial.println(F(" Wifi networks found"));
        Serial.println(F("-------------------------------------------------------------------------"));
        for (int i = 0; i < n; ++i) {                // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(F(": "));
            Serial.print(WiFi.SSID(i));
            Serial.print(F(" ("));
            Serial.print(WiFi.RSSI(i));
            Serial.print(F("dBm, "));
            if (WiFi.RSSI(i) < -50) Serial.print(F(" ")); //Spacing... because I just can't look at the results when misaligned.
            Serial.print(constrain(2 * (WiFi.RSSI(i) + 100), 0, 100));
            Serial.print(F("%)      "));
            Serial.print(F("BSSID: "));
            Serial.print(WiFi.BSSIDstr(i));
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " open" : " encrypted");

            if(WiFi.SSID(i) == globalVariables.SSID && WiFi.RSSI(i) > bestRSSI)
            {
                bestBSSID = WiFi.BSSIDstr(i);
                bestRSSI = WiFi.RSSI(i); 
            }
            delay(10);
        }
        Serial.println();
        if(printerConfig.BSSID == bestBSSID.c_str()){
                Serial.print(F("BSSID already set to: "));
                Serial.print(printerConfig.BSSID);
        }
        else{
            if(strlen(printerConfig.BSSID) == 0 || printerConfig.rescanWiFiNetwork){
                strcpy(printerConfig.BSSID,bestBSSID.c_str());
                Serial.print(F("Saving strongest AP for: "));
                Serial.println(globalVariables.SSID);
                Serial.print(F("Strongest BSSID (MAC Address): "));
                Serial.print(printerConfig.BSSID);
            }
            else{
                Serial.print(F("Ignoring strongest AP for: "));
                Serial.println(globalVariables.SSID);
                Serial.print(F("Using alrady saved BSSID (MAC Address): "));
                Serial.print(printerConfig.BSSID);
            }
            Serial.print(F("     RSSI (Signal Strength): "));
            Serial.print(bestRSSI);
            Serial.println(F("dBm"));
        }
    }
    Serial.println();
}


#endif

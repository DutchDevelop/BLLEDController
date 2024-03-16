#ifndef _BLLEDSERIAL_MANAGER
#define _BLLEDSERIAL_MANAGER

#include <Arduino.h>
#include <ArduinoJson.h> 

#include "filesystem.h"
#include "types.h"

void setupSerial(){
   while (!Serial);
}

void serialLoop(){
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        JsonDocument doc;
        deserializeJson(doc, input);
        if (doc.containsKey("ssid") && doc.containsKey("pass")) {
            Serial.print(F("SSID "));
            Serial.println(doc["ssid"].as<String>());
            Serial.print(F("PASS "));
            Serial.println(doc["pass"].as<String>());

            Serial.println(doc["printerip"].as<String>());
            Serial.println(doc["printercode"].as<String>());
            Serial.println(doc["printerserial"].as<String>());

            strcpy(globalVariables.SSID, doc["ssid"]);
            strcpy(globalVariables.APPW, doc["pass"]);

            strcpy(printerConfig.printerIP, doc["printerip"]);
            strcpy(printerConfig.accessCode, doc["printercode"]);
            strcpy(printerConfig.serialNumber, doc["printerserial"]);

            saveFileSystem();
            Serial.println(F("Restarting Device"));
            delay(1000);
            ESP.restart();
       }
      
    };
    delay(10);
}

#endif
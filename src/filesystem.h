#ifndef _BLLEDFILESYSTEM
#define _BLLEDFILESYSTEM

#include <WiFi.h>
#include "FS.h"

#include <Arduino.h>
#include <LittleFS.h>
#include "types.h"

const char *configPath = "/blledconfig.json";

char* generateRandomString(int length) {
  static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  int charsetLength = strlen(charset);

  char* randomString = new char[length + 1];
  for (int i = 0; i < length; i++) {
    int randomIndex = random(0, charsetLength);
    randomString[i] = charset[randomIndex];
  }
  randomString[length] = '\0';

  return randomString;
}

void saveFileSystem(){
    Serial.println(F("Saving config"));
    
    JsonDocument json;
    json["printerIp"] = printerConfig.printerIP;
    json["accessCode"] = printerConfig.accessCode;
    json["serialNumber"] = printerConfig.serialNumber;
    json["ssid"] = globalVariables.SSID;
    json["appw"] = globalVariables.APPW;
    //json["webpagePassword"] = printerConfig.webpagePassword;
    json["bssi"] = printerConfig.BSSID;
    json["brightness"] = printerConfig.brightness;
    // BLLED Settings
    json["replicatestate"] = printerConfig.replicatestate;
    json["errordetection"] = printerConfig.errordetection;
    json["finishindication"] = printerConfig.finishindication;
    json["lidarLightsOff"] = printerConfig.lidarLightsOff;
    json["inactivityEnabled"] = printerConfig.inactivityEnabled;
    json["inactivityTimeOut"] = printerConfig.inactivityTimeOut;
    json["discoMode"] = printerConfig.discoMode;
    json["maintMode"] = printerConfig.maintMode;
    // Debugging
    json["debuging"] = printerConfig.debuging;
    json["debugingchange"] = printerConfig.debugingchange;
    json["mqttdebug"] = printerConfig.mqttdebug;
    // Test - Fixed LED Colors
    json["testRGB"] = printerConfig.testRGB;
    json["testwarmwhite"] = printerConfig.testwarmwhite;
    json["testcoldwhite"] = printerConfig.testcoldwhite;
    // Customise LED Colors
    json["wifiRGB"] = printerConfig.wifiRGB;
    json["finishRGB"] = printerConfig.finishRGB;
    json["pauseRGB"] = printerConfig.pauseRGB;
    json["firstlayerRGB"] = printerConfig.firstlayerRGB;
    json["nozzleclogRGB"] = printerConfig.nozzleclogRGB;
    json["hmsSeriousRGB"] = printerConfig.hmsSeriousRGB;
    json["hmsFatalRGB"] = printerConfig.hmsFatalRGB;
    json["filamentRunoutRGB"] = printerConfig.filamentRunoutRGB;
    json["frontCoverRGB"] = printerConfig.frontCoverRGB;
    json["nozzleTempRGB"] = printerConfig.nozzleTempRGB;
    json["bedTempRGB"] = printerConfig.bedTempRGB;
    File configFile = LittleFS.open(configPath, "w");
    if (!configFile) {
        Serial.println(F("Failed to save config"));
        return;
    }
    serializeJson(json, configFile);
    configFile.close();
    Serial.println(F("Config Saved"));
}

void loadFileSystem(){
    Serial.println(F("Loading config"));
    
    File configFile;
    int attempts = 0;
    while (attempts < 2) {
        configFile = LittleFS.open(configPath, "r");
        if (configFile) {
            break;
        }
        attempts++;
        Serial.println(F("Failed to open config file, retrying.."));
        delay(2000);
    }
    if (!configFile) {
        Serial.print(F("Failed to open config file after "));
        Serial.print(attempts);
        Serial.println(F(" retries"));
        
        Serial.println(F("Clearing config"));
        // LittleFS.remove(configPath);
        saveFileSystem();
        return;
    }
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);

    JsonDocument json;
    auto deserializeError = deserializeJson(json, buf.get());

    if (!deserializeError) {
        strcpy(globalVariables.SSID, json["ssid"]);
        strcpy(globalVariables.APPW, json["appw"]);
        strcpy(printerConfig.printerIP, json["printerIp"]);
        strcpy(printerConfig.accessCode, json["accessCode"]);
        strcpy(printerConfig.serialNumber, json["serialNumber"]);
        //strcpy(printerConfig.webpagePassword, json["webpagePassword"]);
        strcpy(printerConfig.BSSID, json["bssi"]);
        printerConfig.brightness = json["brightness"];
        //BLLED Settings
        printerConfig.replicatestate = json["replicatestate"];
        printerConfig.errordetection = json["errordetection"];
        printerConfig.finishindication = json["finishindication"];
        printerConfig.lidarLightsOff = json["lidarLightsOff"];
        printerConfig.inactivityEnabled = json["inactivityEnabled"];
        printerConfig.inactivityTimeOut = json["inactivityTimeOut"];
        printerConfig.discoMode = json["discoMode"];
        printerConfig.maintMode = json["maintMode"];
        //Debugging
        printerConfig.debuging = json["debuging"];
        printerConfig.debugingchange = json["debugingchange"];
        printerConfig.mqttdebug = json["mqttdebug"];
        //Test - Fixed LED Colors
        strcpy(printerConfig.testRGB, json["testRGB"]);
        printerConfig.testwarmwhite = json["testwarmwhite"];
        printerConfig.testcoldwhite = json["testcoldwhite"];
        // Customise LED Colors
        strcpy(printerConfig.wifiRGB, json["wifiRGB"]);
        strcpy(printerConfig.finishRGB, json["finishRGB"]);
        strcpy(printerConfig.pauseRGB, json["pauseRGB"]);
        strcpy(printerConfig.firstlayerRGB, json["firstlayerRGB"]);
        strcpy(printerConfig.nozzleclogRGB, json["nozzleclogRGB"]);
        strcpy(printerConfig.hmsSeriousRGB, json["hmsSeriousRGB"]);
        strcpy(printerConfig.hmsFatalRGB, json["hmsFatalRGB"]);
        strcpy(printerConfig.filamentRunoutRGB, json["filamentRunoutRGB"]);
        strcpy(printerConfig.frontCoverRGB, json["frontCoverRGB"]);
        strcpy(printerConfig.nozzleTempRGB, json["nozzleTempRGB"]);
        strcpy(printerConfig.bedTempRGB, json["bedTempRGB"]);

        Serial.println(F("Loaded config"));
    } else {
        Serial.println(F("Failed loading config"));
        Serial.println(F("Clearing config"));
        LittleFS.remove(configPath);

        //Serial.println(F("Generating new password"));
        //char* pw = generateRandomString(8);
        //strcpy(printerConfig.webpagePassword, pw);
    }

    configFile.close();
}

void deleteFileSystem(){
    Serial.println(F("Deleting LittleFS"));
    LittleFS.remove(configPath);
}

bool hasFileSystem(){
    return LittleFS.exists(configPath);
}

void setupFileSystem(){
    Serial.println(F("Mounting LittleFS"));
    if (!LittleFS.begin()) {
        Serial.println(F("Failed to mount LittleFS"));
        LittleFS.format();
        Serial.println(F("Formatting LittleFS"));
        Serial.println(F("Restarting Device"));
        delay(1000);
        ESP.restart();
    }
    Serial.println(F("Mounted LittleFS"));
};

#endif
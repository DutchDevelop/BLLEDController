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
    json["ssid"] = globalVariables.SSID;
    json["appw"] = globalVariables.APPW;
    json["printerIp"] = printerConfig.printerIP;
    json["accessCode"] = printerConfig.accessCode;
    json["serialNumber"] = printerConfig.serialNumber;
    //json["webpagePassword"] = printerConfig.webpagePassword;
    json["bssi"] = printerConfig.BSSID;
    json["brightness"] = printerConfig.brightness;
    // LED Behaviour (Choose One)
    json["maintMode"] = printerConfig.maintMode;
    json["discoMode"] = printerConfig.discoMode;
    json["replicatestate"] = printerConfig.replicatestate;
    // Running Color
    json["runningRGB"] = printerConfig.runningColor.RGBhex;
    json["runningWW"] = printerConfig.runningColor.ww;
    json["runningCW"] = printerConfig.runningColor.cw;
    // Test LED Colors
    json["showtestcolor"] = printerConfig.testcolorEnabled;
    json["testRGB"] = printerConfig.testColor.RGBhex;
    json["testWW"] = printerConfig.testColor.ww;
    json["testCW"] = printerConfig.testColor.cw;
    json["debugwifi"] = printerConfig.debugwifi;
    // Options
    json["finishindication"] = printerConfig.finishindication;
    json["finishColor"] = printerConfig.finishColor.RGBhex;
    json["finishWW"] = printerConfig.finishColor.ww;
    json["finishCW"] = printerConfig.finishColor.cw;
    json["finishExit"] = printerConfig.finishExit;
    json["finish_check"] = printerConfig.finish_check;
    json["finishTimerMins"] = printerConfig.finishTimeOut;
    json["inactivityEnabled"] = printerConfig.inactivityEnabled;
    json["inactivityTimeOut"] = printerConfig.inactivityTimeOut;
    // Debugging
    json["debuging"] = printerConfig.debuging;
    json["debugingchange"] = printerConfig.debugingchange;
    json["mqttdebug"] = printerConfig.mqttdebug;
    // Printer Dependant
    json["p1Printer"] = printerVariables.isP1Printer;
    json["doorSwitch"] =  printerVariables.useDoorSwtich;
    // Customise LED Colors (during Lidar)
    json["stage14RGB"] = printerConfig.stage14Color.RGBhex;
    json["stage14WW"] = printerConfig.stage14Color.ww;
    json["stage14CW"] = printerConfig.stage14Color.cw;
    json["stage1RGB"] = printerConfig.stage1Color.RGBhex;
    json["stage1WW"] = printerConfig.stage1Color.ww;
    json["stage1CW"] = printerConfig.stage1Color.cw;
    json["stage8RGB"] = printerConfig.stage8Color.RGBhex;
    json["stage8WW"] = printerConfig.stage8Color.ww;
    json["stage8CW"] = printerConfig.stage8Color.cw;
    json["stage9RGB"] = printerConfig.stage9Color.RGBhex;
    json["stage9WW"] = printerConfig.stage9Color.ww;
    json["stage9CW"] = printerConfig.stage9Color.cw;
    json["stage10RGB"] = printerConfig.stage10Color.RGBhex;
    json["stage10WW"] = printerConfig.stage10Color.ww;
    json["stage10CW"] = printerConfig.stage10Color.cw;
    // Customise LED Colors
    json["errordetection"] = printerConfig.errordetection;
    json["wifiRGB"] = printerConfig.wifiRGB.RGBhex;
    json["wifiWW"] = printerConfig.wifiRGB.ww;
    json["wifiCW"] = printerConfig.wifiRGB.cw;
    json["pauseRGB"] = printerConfig.pauseRGB.RGBhex;
    json["pauseWW"] = printerConfig.pauseRGB.ww;
    json["pauseCW"] = printerConfig.pauseRGB.cw;
    json["firstlayerRGB"] = printerConfig.firstlayerRGB.RGBhex;
    json["firstlayerWW"] = printerConfig.firstlayerRGB.ww;
    json["firstlayerCW"] = printerConfig.firstlayerRGB.cw;
    json["nozzleclogRGB"] = printerConfig.nozzleclogRGB.RGBhex;
    json["nozzleclogWW"] = printerConfig.nozzleclogRGB.ww;
    json["nozzleclogCW"] = printerConfig.nozzleclogRGB.cw;
    json["hmsSeriousRGB"] = printerConfig.hmsSeriousRGB.RGBhex;
    json["hmsSeriousWW"] = printerConfig.hmsSeriousRGB.ww;
    json["hmsSeriousCW"] = printerConfig.hmsSeriousRGB.cw;
    json["hmsFatalRGB"] = printerConfig.hmsFatalRGB.RGBhex;
    json["hmsFatalWW"] = printerConfig.hmsFatalRGB.ww;
    json["hmsFatalCW"] = printerConfig.hmsFatalRGB.cw;
    json["filamentRunoutRGB"] = printerConfig.filamentRunoutRGB.RGBhex;
    json["filamentRunoutWW"] = printerConfig.filamentRunoutRGB.ww;
    json["filamentRunoutCW"] = printerConfig.filamentRunoutRGB.cw;
    json["frontCoverRGB"] = printerConfig.frontCoverRGB.RGBhex;
    json["frontCoverWW"] = printerConfig.frontCoverRGB.ww;
    json["frontCoverCW"] = printerConfig.frontCoverRGB.cw;
    json["nozzleTempRGB"] = printerConfig.nozzleTempRGB.RGBhex;
    json["nozzleTempWW"] = printerConfig.nozzleTempRGB.ww;
    json["nozzleTempCW"] = printerConfig.nozzleTempRGB.cw;
    json["bedTempRGB"] = printerConfig.bedTempRGB.RGBhex;
    json["bedTempWW"] = printerConfig.bedTempRGB.ww;
    json["bedTempCW"] = printerConfig.bedTempRGB.cw;

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
        // LED Behaviour (Choose One)
        printerConfig.maintMode = json["maintMode"];
        printerConfig.discoMode = json["discoMode"];
        printerConfig.replicatestate = json["replicatestate"];
        // Running Color
        printerConfig.runningColor = hex2rgb(json["runningRGB"],json["runningWW"],json["runningCW"]);
        // Test LED Colors
        printerConfig.testcolorEnabled = json["showtestcolor"];
        printerConfig.testColor = hex2rgb(json["testRGB"],json["testWW"],json["testCW"]);
        printerConfig.debugwifi = json["debugwifi"];
        // Options
        printerConfig.finishindication = json["finishindication"];
        printerConfig.finishColor = hex2rgb(json["finishColor"],json["finishWW"],json["finishCW"]);
        printerConfig.finishExit = json["finishExit"];
        printerConfig.finishTimeOut = json["finishTimerMins"];
        printerConfig.finish_check = json["finish_check"];
        printerConfig.inactivityEnabled = json["inactivityEnabled"];
        printerConfig.inactivityTimeOut = json["inactivityTimeOut"];
        // Debugging
        printerConfig.debuging = json["debuging"];
        printerConfig.debugingchange = json["debugingchange"];
        printerConfig.mqttdebug = json["mqttdebug"];
        // Printer Dependant
        printerVariables.isP1Printer = json["p1Printer"];
        printerVariables.useDoorSwtich = json["doorSwitch"];
        printerConfig.stage14Color = hex2rgb(json["stage14RGB"],json["stage14WW"],json["stage14CW"]);
        printerConfig.stage1Color = hex2rgb(json["stage1RGB"],json["stage1WW"],json["stage1CW"]);
        printerConfig.stage8Color = hex2rgb(json["stage8RGB"],json["stage8WW"],json["stage8CW"]);
        printerConfig.stage9Color = hex2rgb(json["stage9RGB"],json["stage9WW"],json["stage9CW"]);
        printerConfig.stage10Color = hex2rgb(json["stage10RGB"],json["stage10WW"],json["stage10CW"]);
        // Customise LED Colors
        printerConfig.errordetection = json["errordetection"];
        printerConfig.wifiRGB = hex2rgb(json["wifiRGB"],json["wifiWW"],json["wifiCW"]);

        printerConfig.pauseRGB = hex2rgb(json["pauseRGB"],json["pauseWW"],json["pauseCW"]);
        printerConfig.firstlayerRGB = hex2rgb(json["firstlayerRGB"],json["firstlayerWW"],json["firstlayerCW"]);
        printerConfig.nozzleclogRGB = hex2rgb(json["nozzleclogRGB"],json["nozzleclogWW"],json["nozzleclogCW"]);
        printerConfig.hmsSeriousRGB = hex2rgb(json["hmsSeriousRGB"],json["hmsSeriousWW"],json["hmsSeriousCW"]);
        printerConfig.hmsFatalRGB = hex2rgb(json["hmsFatalRGB"],json["hmsFatalWW"],json["hmsFatalCW"]);
        printerConfig.filamentRunoutRGB = hex2rgb(json["filamentRunoutRGB"],json["filamentRunoutWW"],json["filamentRunoutCW"]);
        printerConfig.frontCoverRGB = hex2rgb(json["frontCoverRGB"],json["frontCoverWW"],json["frontCoverCW"]);
        printerConfig.nozzleTempRGB = hex2rgb(json["nozzleTempRGB"],json["nozzleTempWW"],json["nozzleTempCW"]);
        printerConfig.bedTempRGB = hex2rgb(json["bedTempRGB"],json["bedTempWW"],json["bedTempCW"]);
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
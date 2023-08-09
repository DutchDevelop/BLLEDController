#ifndef _BLLEDFILESYSTEM
#define _BLLEDFILESYSTEM

#ifdef ESP32
    #include <WiFi.h>
    #include "FS.h"
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

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
    DynamicJsonDocument json(256);
    json["printerIp"] = printerConfig.printerIP;
    json["accesCode"] = printerConfig.accessCode;
    json["serialNumber"] = printerConfig.serialNumber;
    json["webpagePassword"] = printerConfig.webpagePassword;

    json["replicatestate"] =  printerConfig.replicatestate;
    json["errordetection"] =  printerConfig.errordetection;
    json["finishindication"] =  printerConfig.finishindication;

    File configFile = LittleFS.open(configPath, "w");

    if (!configPath){
        Serial.println(F("Failed to save config"));
        return;
    }
    serializeJson(json, configFile);
    configFile.close();
    Serial.println(F("Config Saved"));
}

void loadFileSystem(){
    Serial.println(F("Loading config"));
    File configFile = LittleFS.open(configPath, "r");
    size_t size = configFile.size();

    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DynamicJsonDocument json(1024);

    auto deserializeError = deserializeJson(json, buf.get());
    if (!deserializeError){
        strcpy(printerConfig.printerIP, json["printerIp"]);
        strcpy(printerConfig.accessCode, json["accesCode"]);
        strcpy(printerConfig.serialNumber, json["serialNumber"]);
        strcpy(printerConfig.webpagePassword,json["webpagePassword"]);

        printerConfig.replicatestate = json["replicatestate"];
        printerConfig.errordetection = json["errordetection"];
        printerConfig.finishindication =  json["finishindication"];


        Serial.println(F("Loaded config"));
    }else{
        Serial.println(F("Failed loading config"));
        Serial.println(F("Clearing config"));
        LittleFS.remove(configPath);

        Serial.println(F("Generating new password"));
        char* pw = generateRandomString(8);
        strcpy(printerConfig.webpagePassword,pw);

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
        ESP.restart();
    }
    Serial.println(F("Mounted LittleFS"));
};

#endif
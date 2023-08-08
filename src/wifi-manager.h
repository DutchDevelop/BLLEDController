#ifndef _BLLEDWIFI_MANAGER
#define _BLLEDWIFI_MANAGER

#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 

#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include "filesystem.h"
#include "types.h"

WiFiManager wifiManager;

bool shouldSaveConfig = true;

void processConfig(){
    if(hasFileSystem()){
        loadFileSystem();
    }else{
        saveFileSystem();
        loadFileSystem();
    }
}

void restartprocess(){
    wifiManager.resetSettings();
    deleteFileSystem();
    delay(3000);
}

void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void setupWifi(){
   // restartprocess();

    processConfig();

    WiFiManagerParameter printerIpParameter("printerIp", "Printer Ip", printerConfig.printerIP, 16);
    WiFiManagerParameter accessCodeParameter("accessCode", "Access Code", printerConfig.accessCode, 9);
    WiFiManagerParameter serialNumberParameter("serialNumber", "Serial", printerConfig.serialNumber, 16);
    WiFiManagerParameter configPasswordParameter("configpassword", "Config Password", printerConfig.password, 8);

    wifiManager.addParameter(&printerIpParameter);
    wifiManager.addParameter(&accessCodeParameter);
    wifiManager.addParameter(&serialNumberParameter);
    wifiManager.addParameter(&configPasswordParameter);

    wifiManager.setConfigPortalTimeout(180);
    wifiManager.setAPCallback(configModeCallback);

    if (!wifiManager.autoConnect("BLLEDController")){
        Serial.println(F("Failed to create accesspoint"));
        delay(5000);
        restartprocess();
        ESP.restart();
        delay(5000);
        return;
    }

    strcpy(printerConfig.printerIP,printerIpParameter.getValue());
    strcpy(printerConfig.accessCode,accessCodeParameter.getValue());
    strcpy(printerConfig.serialNumber,serialNumberParameter.getValue());
    strcpy(printerConfig.password,configPasswordParameter.getValue());

    saveFileSystem();

    Serial.println(F("-------------------------------------"));
    Serial.print(F("Head over to http://"));
    Serial.println(WiFi.localIP());
    Serial.print(F("Login Details User: BLLC, Password: "));
    Serial.println(printerConfig.password);
    Serial.println(F("To configure further"));
    Serial.println(F("-------------------------------------"));
}

#endif
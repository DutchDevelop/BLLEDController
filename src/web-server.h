#ifndef _BLLEDWEB_SERVER
#define _BLLEDWEB_SERVER

#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 
#include "leds.h"

#if defined(ESP32)
    #include <WiFi.h>
    #include <WebServer.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
#endif

#if defined(ESP32)
    WebServer webServer(80);
#elif defined(ESP8266)
    ESP8266WebServer webServer(80);
#endif

#include "www/setupPage.h"

bool isAuthorized() {
  return webServer.authenticate("BLLC", printerConfig.password);
}

void handleSetup(){
    if (!isAuthorized()){
        webServer.requestAuthentication();
        return;
    }
    String dataType = "text/html";
    webServer.send(200, dataType, (const char*)setupPage_html);
}

void submitSetup(){
    if (webServer.args() > 0) {

        strcpy(printerConfig.printerIP,webServer.arg("ip").c_str());
        strcpy(printerConfig.accessCode,webServer.arg("code").c_str());
        strcpy(printerConfig.serialNumber,webServer.arg("id").c_str());

        printerConfig.replicatestate = webServer.arg("replicateLedState") == "on";
        printerConfig.errordetection = webServer.arg("errorDetection") == "on";
        printerConfig.finishindication = webServer.arg("finishIndication") == "on";

        saveFileSystem();
        updateleds();
        handleSetup();
    }
}

void handleGetConfig(){
    if (!isAuthorized()){
        webServer.requestAuthentication();
        return;
    }

    DynamicJsonDocument doc(200);
    doc["ip"] = printerConfig.printerIP;
    doc["code"] = printerConfig.accessCode;
    doc["id"] = printerConfig.serialNumber;
    doc["replicateled"] = printerConfig.replicatestate;
    doc["errordetection"] = printerConfig.errordetection;
    doc["finishindication"] = printerConfig.finishindication;

    String jsonString;
    serializeJson(doc, jsonString);

    webServer.send(200, "application/json", jsonString);
}

void setupWebserver(){
    Serial.println(F("Setting up webserver"));

    webServer.on("/", handleSetup);
    webServer.on("/submitSetup",HTTP_POST,submitSetup);
    webServer.on("/getConfig", handleGetConfig);
    webServer.begin();

    Serial.println(F("Webserver started"));
}

void webserverloop(){
    webServer.handleClient();
}

#endif
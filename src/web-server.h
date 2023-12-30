#ifndef _BLLEDWEB_SERVER
#define _BLLEDWEB_SERVER

#include <Arduino.h>
#include <ArduinoJson.h> 
#include "leds.h"

#if defined(ESP32)
    #include <WiFi.h>
    #include <WebServer.h> 
    #include <ESPHTTPUpdateServer.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266HTTPUpdateServer.h>
#endif

#if defined(ESP32)
    WebServer webServer(80);
    ESPHTTPUpdateServer httpUpdater;
#elif defined(ESP8266)
    ESP8266WebServer webServer(80);
    ESP8266HTTPUpdateServer httpUpdater; 
#endif

#include "www/setupPage.h"
#include "www/updatePage.h"

bool isAuthorized() {
  return true; //webServer.authenticate("BLLC", printerConfig.webpagePassword);
}

void handleSetup(){
    if (!isAuthorized()){
        webServer.requestAuthentication();
        return;
    }
    webServer.sendHeader(F("Content-Encoding"), F("gzip"));
    webServer.send_P(200, "text/html", (const char*)setupPage_html_gz, (int)setupPage_html_gz_len);
}

void handleUpdate(){
    if (!isAuthorized()){
        webServer.requestAuthentication();
        return;
    }
    webServer.sendHeader(F("Content-Encoding"), F("gzip"));
    webServer.send_P(200, "text/html", (const char*)updatepPage_html_gz, (int)updatePage_html_gz_len);
}

void submitSetup(){
    if (webServer.args() > 0) {

        strcpy(printerConfig.printerIP,webServer.arg("ip").c_str());
        strcpy(printerConfig.accessCode,webServer.arg("code").c_str());
        strcpy(printerConfig.serialNumber,webServer.arg("id").c_str());
        printerConfig.turbo = webServer.arg("turbo") == "on"; 
        printerConfig.replicatestate = webServer.arg("replicateLedState") == "on";
        printerConfig.errordetection = webServer.arg("errorDetection") == "on";
        printerConfig.finishindication = webServer.arg("finishIndication") == "on";
        printerConfig.debuging = webServer.arg("debuging") == "on";
        printerConfig.brightness = webServer.arg("brightnessslider").toInt();

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

    DynamicJsonDocument doc(300);
    doc["brightness"] = printerConfig.brightness;
    doc["turbo"] = printerConfig.turbo;
    doc["ip"] = printerConfig.printerIP;
    doc["code"] = printerConfig.accessCode;
    doc["id"] = printerConfig.serialNumber;
    doc["replicateled"] = printerConfig.replicatestate;
    doc["errordetection"] = printerConfig.errordetection;
    doc["finishindication"] = printerConfig.finishindication;
    doc["debuging"] = printerConfig.debuging;
    
    const char* firmwareVersionChar = globalVariables.FWVersion.c_str();
    doc["firmwareversion"] = firmwareVersionChar;

    String jsonString;
    serializeJson(doc, jsonString);

    webServer.send(200, "application/json", jsonString);
}

void setupWebserver(){
    Serial.println(F("Setting up webserver"));
    
    webServer.on("/", handleSetup);
    webServer.on("/submitSetup",HTTP_POST,submitSetup);
    webServer.on("/getConfig", handleGetConfig);
    webServer.on("/update",HTTP_GET,handleUpdate);

    httpUpdater.setup(&webServer);
    webServer.begin();

    Serial.println(F("Webserver started"));
}

void webserverloop(){
    webServer.handleClient();
}

#endif
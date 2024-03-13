#ifndef _BLLEDWEB_SERVER
#define _BLLEDWEB_SERVER

#include <Arduino.h>
#include <ArduinoJson.h> 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "leds.h"
#include "filesystem.h"

WebServer webServer(80);

#include "www/setupPage.h"

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

void submitSetup(){
    if (webServer.args() > 0) {

        strcpy(printerConfig.printerIP,webServer.arg("ip").c_str());
        strcpy(printerConfig.accessCode,webServer.arg("code").c_str());
        strcpy(printerConfig.serialNumber,webServer.arg("id").c_str());
        strcpy(printerConfig.BSSID,webServer.arg("apMAC").c_str());

        printerConfig.replicatestate = webServer.arg("replicateLedState") == "on";
        printerConfig.errordetection = webServer.arg("errorDetection") == "on";
        printerConfig.finishindication = webServer.arg("finishIndication") == "on";
        printerConfig.discoMode = webServer.arg("discoMode") == "on";
        printerConfig.debuging = webServer.arg("debuging") == "on";
        printerConfig.debugingchange = webServer.arg("debugingchange") == "on";
        printerConfig.mqttdebug = webServer.arg("mqttdebug") == "on";
        
        printerVariables.overrideLEDstate = !printerConfig.replicatestate;

        printerConfig.overrideRed = webServer.arg("overrideRed") == "on";
        printerConfig.overrideGreen = webServer.arg("overrideGreen") == "on";
        printerConfig.overrideBlue = webServer.arg("overrideBlue") == "on";
        printerConfig.overrideWarmWhite = webServer.arg("overrideWarmWhite") == "on";
        printerConfig.overrideColdWhite = webServer.arg("overrideColdWhite") == "on";
        printerConfig.debugwifi = webServer.arg("debugwifi") == "on";        

        printerConfig.brightness = webServer.arg("brightnessslider").toInt();

        saveFileSystem();
        Serial.println(F("Updating from webpage"));
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
    doc["ip"] = printerConfig.printerIP;
    doc["code"] = printerConfig.accessCode;
    doc["id"] = printerConfig.serialNumber;
    doc["apMAC"] = printerConfig.BSSID;

    doc["replicateled"] = printerConfig.replicatestate;
    doc["errordetection"] = printerConfig.errordetection;
    doc["finishindication"] = printerConfig.finishindication;
    doc["discoMode"] = printerConfig.discoMode;
    doc["debuging"] = printerConfig.debuging;
    doc["debugingchange"] = printerConfig.debugingchange;
    doc["mqttdebug"] = printerConfig.mqttdebug;

    doc["overrideRed"] = printerConfig.overrideRed;
    doc["overrideGreen"] = printerConfig.overrideGreen;
    doc["overrideBlue"] = printerConfig.overrideBlue;
    doc["overrideWarmWhite"] = printerConfig.overrideWarmWhite;
    doc["overrideColdWhite"] = printerConfig.overrideColdWhite;
    doc["debugwifi"] = printerConfig.debugwifi;

    doc["brightness"] = printerConfig.brightness;
    const char* firmwareVersionChar = globalVariables.FWVersion.c_str();
    doc["firmwareversion"] = firmwareVersionChar;

    String jsonString;
    serializeJson(doc, jsonString);

    webServer.send(200, "application/json", jsonString);
}

void setupWebserver(){
    if (!MDNS.begin(globalVariables.Host.c_str())) {
        Serial.println(F("Error setting up MDNS responder!"));
        while (1) {
        delay(1000);
        }
    }

    Serial.println(F("Setting up webserver"));
    
    webServer.on("/", handleSetup);
    webServer.on("/submitSetup",HTTP_POST,submitSetup);
    webServer.on("/getConfig", handleGetConfig);

    webServer.on("/update", HTTP_POST, []() {
        webServer.sendHeader("Connection", "close");
        webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        Serial.println(F("Restarting Device"));
        ESP.restart();
    }, []() {
        HTTPUpload& upload = webServer.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
        
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        }
    });

    webServer.begin();

    Serial.println(F("Webserver started"));
    Serial.println(F(""));
}

void webserverloop(){
    webServer.handleClient();
    delay(10);
}

#endif
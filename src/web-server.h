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
    webServer.send_P(200, "text/html", (const char*)setuppage_html_gz, (int)setuppage_html_gz_len);
}

void submitSetup(){
    bool newBSSID = false;
    if (webServer.args() > 0) {
        if( strcmp(printerConfig.BSSID,webServer.arg("apMAC").c_str()) != 0 ){
            newBSSID = true;
        }
        strcpy(printerConfig.printerIP,webServer.arg("ip").c_str());
        strcpy(printerConfig.accessCode,webServer.arg("code").c_str());
        strcpy(printerConfig.serialNumber,webServer.arg("id").c_str());

        strcpy(printerConfig.BSSID,webServer.arg("apMAC").c_str());
        printerConfig.brightness = webServer.arg("brightnessslider").toInt();
        printerConfig.rescanWiFiNetwork = webServer.arg("rescanWiFiNetwork") == "on";
        // BLLED Settings
        printerConfig.replicatestate = webServer.arg("replicateLedState") == "on";
        printerConfig.errordetection = webServer.arg("errorDetection") == "on";
        printerConfig.finishindication = webServer.arg("finishIndication") == "on";
        printerConfig.lidarLightsOff = webServer.arg("lidarLightsOff") == "on";
        printerConfig.inactivityEnabled = webServer.arg("inactivityEnabled") == "on";
        printerConfig.inactivityTimeOut = (webServer.arg("inactivityMins").toInt() * 60000);
        if(printerConfig.maintMode != (webServer.arg("maintMode") == "on"))
        {
            printerConfig.maintMode = webServer.arg("maintMode") == "on";
            printerConfig.updateMaintenance = printerConfig.maintMode;
            if(printerConfig.maintMode && printerConfig.debugingchange){
                Serial.println(F("Maintence Mode is ON"));
                Serial.println(F("**No MQTT msgs, TEST colors or Wifi strength visualisation**"));
                Serial.println();
            }
        }
        
        printerConfig.discoMode = webServer.arg("discoMode") == "on";
        printerConfig.debuging = webServer.arg("debuging") == "on";
        printerConfig.debugingchange = webServer.arg("debugingchange") == "on";
        printerConfig.mqttdebug = webServer.arg("mqttdebug") == "on";
        // Test - Fixed LED Colors
        printerVariables.testcolorEnabled = !printerConfig.replicatestate;
        printerConfig.updateTestLEDS = !printerConfig.replicatestate;
        strcpy(printerConfig.testRGB,webServer.arg("testRGB").c_str());
        printerConfig.testcoldwhite = webServer.arg("cw_slider").toInt();
        printerConfig.testwarmwhite = webServer.arg("ww_slider").toInt();
        
        printerConfig.debugwifi = webServer.arg("debugwifi") == "on";
        // Customise LED Colors
        strcpy(printerConfig.wifiRGB,webServer.arg("wifiRGB").c_str());
        strcpy(printerConfig.finishRGB, webServer.arg("finishRGB").c_str());
        strcpy(printerConfig.pauseRGB, webServer.arg("pauseRGB").c_str());
        strcpy(printerConfig.firstlayerRGB, webServer.arg("firstlayerRGB").c_str());
        strcpy(printerConfig.nozzleclogRGB, webServer.arg("nozzleclogRGB").c_str());
        strcpy(printerConfig.hmsSeriousRGB, webServer.arg("hmsSeriousRGB").c_str());
        strcpy(printerConfig.hmsFatalRGB, webServer.arg("hmsFatalRGB").c_str());
        strcpy(printerConfig.filamentRunoutRGB, webServer.arg("filamentRunoutRGB").c_str());
        strcpy(printerConfig.frontCoverRGB, webServer.arg("frontCoverRGB").c_str());
        strcpy(printerConfig.nozzleTempRGB, webServer.arg("nozzleTempRGB").c_str());
        strcpy(printerConfig.bedTempRGB, webServer.arg("bedTempRGB").c_str());

        saveFileSystem();
        Serial.println(F("Updating LEDS from webpage form submit"));
        updateleds();
        handleSetup();
        if(newBSSID)
        {
            Serial.println(F("New MAC address (BSSID) assigned. Restarting..."));
            delay(1000);
            ESP.restart();
        }
    }
}

void handleGetConfig(){
    if (!isAuthorized()){
        webServer.requestAuthentication();
        return;
    }

    JsonDocument doc;
    doc["ip"] = printerConfig.printerIP;
    doc["code"] = printerConfig.accessCode;
    doc["id"] = printerConfig.serialNumber;

    doc["apMAC"] = printerConfig.BSSID;
    doc["brightness"] = printerConfig.brightness;
    // BLLED Settings
    doc["replicateled"] = printerConfig.replicatestate;
    doc["errordetection"] = printerConfig.errordetection;
    doc["finishindication"] = printerConfig.finishindication;
    doc["lidarLightsOff"] = printerConfig.lidarLightsOff;
    doc["inactivityEnabled"] = printerConfig.inactivityEnabled;
    doc["inactivityMins"] =(int)( printerConfig.inactivityTimeOut / 60000);
    doc["discoMode"] = printerConfig.discoMode;
    doc["maintMode"] = printerConfig.maintMode;
    // Debugging
    doc["debuging"] = printerConfig.debuging;
    doc["debugingchange"] = printerConfig.debugingchange;
    doc["mqttdebug"] = printerConfig.mqttdebug;
    // Test - Fixed LED Colors    
    doc["testRGB"] = printerConfig.testRGB;
    doc["cw_slider"] = printerConfig.testcoldwhite;
    doc["ww_slider"] = printerConfig.testwarmwhite;

    doc["debugwifi"] = printerConfig.debugwifi;
    // Customise LED Colors
    doc["wifiRGB"] = printerConfig.wifiRGB;
    doc["finishRGB"] = printerConfig.finishRGB;
    doc["pauseRGB"] = printerConfig.pauseRGB;
    doc["firstlayerRGB"] = printerConfig.firstlayerRGB;
    doc["nozzleclogRGB"] = printerConfig.nozzleclogRGB;
    doc["hmsSeriousRGB"] = printerConfig.hmsSeriousRGB;
    doc["hmsFatalRGB"] = printerConfig.hmsFatalRGB;
    doc["filamentRunoutRGB"] = printerConfig.filamentRunoutRGB;
    doc["frontCoverRGB"] = printerConfig.frontCoverRGB;
    doc["nozzleTempRGB"] = printerConfig.nozzleTempRGB;
    doc["bedTempRGB"] = printerConfig.bedTempRGB;

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
        delay(1000);
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
    Serial.println();
}

void webserverloop(){
    webServer.handleClient();
    delay(10);
}

#endif
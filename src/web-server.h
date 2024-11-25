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



#include "www/setuppage.h"

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
        //Force Uppercase
        char temperserial[20];
        strcpy(temperserial,webServer.arg("id").c_str());
        for (int x=0; x<strlen(temperserial); x++)
            temperserial[x] = toupper(temperserial[x]);
        strcpy(printerConfig.serialNumber,temperserial);

        strcpy(printerConfig.BSSID,webServer.arg("apMAC").c_str());
        printerConfig.brightness = webServer.arg("brightness").toInt();
        printerConfig.rescanWiFiNetwork = (webServer.arg("rescanWiFiNetwork") == "on");
        // LED Behaviour (Choose One)
        printerConfig.maintMode = (webServer.arg("maintMode") == "on");
        printerConfig.discoMode = (webServer.arg("discoMode") == "on");
        printerConfig.replicatestate = (webServer.arg("replicateled") == "on");
        // Running Color
        printerConfig.runningColor = hex2rgb(webServer.arg("runningRGB").c_str(),webServer.arg("runningWW").toInt(),webServer.arg("runningCW").toInt());
        // Test - Fixed LED Colors
        printerConfig.testcolorEnabled = (webServer.arg("showtestcolor") == "on");
        printerConfig.testColor = hex2rgb(webServer.arg("testRGB").c_str(),webServer.arg("testWW").toInt(),webServer.arg("testCW").toInt());
        printerConfig.debugwifi = webServer.arg("debugwifi") == "on";
        // Options
        printerConfig.finishindication = (webServer.arg("finishindication") == "on");
        printerConfig.finishColor = hex2rgb(webServer.arg("finishColor").c_str(),webServer.arg("finishWW").toInt(),webServer.arg("finishCW").toInt());
        if(webServer.arg("finishEndTimer") == "on"){
            printerConfig.finishExit = false;
        } else {
            printerConfig.finishExit = true;
        }
        printerConfig.finishTimeOut = (webServer.arg("finishTimerMins").toInt() * 60000);
        printerConfig.inactivityEnabled = (webServer.arg("inactivityEnabled") == "on");
        printerConfig.inactivityTimeOut = (webServer.arg("inactivityMins").toInt() * 60000);
        // Debugging
        printerConfig.debuging = (webServer.arg("debuging") == "on");
        printerConfig.debugingchange = (webServer.arg("debugingchange") == "on");
        printerConfig.mqttdebug = (webServer.arg("mqttdebug") == "on");
        // Printer Dependant
        printerVariables.isP1Printer = (webServer.arg("p1Printer") == "on");
        printerVariables.useDoorSwtich = (webServer.arg("doorSwitch") == "on");
        // Customise LED Colors (during Lidar)
        printerConfig.stage14Color = hex2rgb(webServer.arg("stage14RGB").c_str(),webServer.arg("stage14WW").toInt(),webServer.arg("stage14CW").toInt());
        printerConfig.stage1Color = hex2rgb(webServer.arg("stage1RGB").c_str(),webServer.arg("stage1WW").toInt(),webServer.arg("stage1CW").toInt());
        printerConfig.stage8Color = hex2rgb(webServer.arg("stage8RGB").c_str(),webServer.arg("stage8WW").toInt(),webServer.arg("stage8CW").toInt());
        printerConfig.stage9Color = hex2rgb(webServer.arg("stage9RGB").c_str(),webServer.arg("stage9WW").toInt(),webServer.arg("stage9CW").toInt());
        printerConfig.stage10Color = hex2rgb(webServer.arg("stage10RGB").c_str(),webServer.arg("stage10WW").toInt(),webServer.arg("stage10CW").toInt());
        // Customise LED Colors
        printerConfig.errordetection = (webServer.arg("errordetection") == "on");
        printerConfig.wifiRGB = hex2rgb(webServer.arg("wifiRGB").c_str(),webServer.arg("wifiWW").toInt(),webServer.arg("wifiCW").toInt());

        printerConfig.pauseRGB = hex2rgb(webServer.arg("pauseRGB").c_str(),webServer.arg("pauseWW").toInt(),webServer.arg("pauseCW").toInt());
        printerConfig.firstlayerRGB = hex2rgb(webServer.arg("firstlayerRGB").c_str(),webServer.arg("firstlayerWW").toInt(),webServer.arg("firstlayerCW").toInt());
        printerConfig.nozzleclogRGB = hex2rgb(webServer.arg("nozzleclogRGB").c_str(),webServer.arg("nozzleclogWW").toInt(),webServer.arg("nozzleclogCW").toInt());
        printerConfig.hmsSeriousRGB = hex2rgb(webServer.arg("hmsSeriousRGB").c_str(),webServer.arg("hmsSeriousWW").toInt(),webServer.arg("hmsSeriousCW").toInt());
        printerConfig.hmsFatalRGB = hex2rgb(webServer.arg("hmsFatalRGB").c_str(),webServer.arg("hmsFatalWW").toInt(),webServer.arg("hmsFatalCW").toInt());
        printerConfig.filamentRunoutRGB = hex2rgb(webServer.arg("filamentRunoutRGB").c_str(),webServer.arg("filamentRunoutWW").toInt(),webServer.arg("filamentRunoutCW").toInt());
        printerConfig.frontCoverRGB = hex2rgb(webServer.arg("frontCoverRGB").c_str(),webServer.arg("frontCoverWW").toInt(),webServer.arg("frontCoverCW").toInt());
        printerConfig.nozzleTempRGB = hex2rgb(webServer.arg("nozzleTempRGB").c_str(),webServer.arg("nozzleTempWW").toInt(),webServer.arg("nozzleTempCW").toInt());
        printerConfig.bedTempRGB = hex2rgb(webServer.arg("bedTempRGB").c_str(),webServer.arg("bedTempWW").toInt(),webServer.arg("bedTempCW").toInt());

        saveFileSystem();
        Serial.println(F("Packet received from setuppage"));
        printerConfig.inactivityStartms = millis();  //restart idle timer
        printerConfig.isIdleOFFActive = false;
        printerConfig.replicate_update=true;
        printerConfig.maintMode_update=true;
        printerConfig.discoMode_update=true;
        printerConfig.testcolor_update=true;
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

    JsonDocument result;
    JsonDocument docDefault;
    JsonDocument doc;
    const char* firmwareVersionChar = globalVariables.FWVersion.c_str();
    doc["firmwareversion"] = firmwareVersionChar;
    doc["wifiStrength"] = WiFi.RSSI();
    doc["ip"] = printerConfig.printerIP;
    doc["code"] = printerConfig.accessCode;
    doc["id"] = printerConfig.serialNumber;

    doc["apMAC"] = printerConfig.BSSID;
    doc["ssid"] = globalVariables.SSID;
    doc["brightness"] = printerConfig.brightness;
    // LED Behaviour (Choose One)
    doc["maintMode"] = printerConfig.maintMode;
    doc["discoMode"] = printerConfig.discoMode;
    doc["replicateled"] = printerConfig.replicatestate;
    // Running Color
    doc["runningRGB"] = printerConfig.runningColor.RGBhex;
    doc["runningWW"] = printerConfig.runningColor.ww;
    doc["runningCW"] = printerConfig.runningColor.cw;
    // LED Test Colors
    doc["showtestcolor"] = printerConfig.testcolorEnabled;
    doc["testRGB"] = printerConfig.testColor.RGBhex;
    doc["testWW"] = printerConfig.testColor.ww;
    doc["testCW"] = printerConfig.testColor.cw;
    doc["debugwifi"] = printerConfig.debugwifi;
    // Options
    doc["finishindication"] = printerConfig.finishindication;
    doc["finishColor"] = printerConfig.finishColor.RGBhex;
    doc["finishWW"] = printerConfig.finishColor.ww;
    doc["finishCW"] = printerConfig.finishColor.cw;
    doc["finishExit"] = printerConfig.finishExit;
    doc["finishTimerMins"] = (int)(printerConfig.finishTimeOut / 60000);
    doc["inactivityEnabled"] = printerConfig.inactivityEnabled;
    doc["inactivityMins"] =(int)( printerConfig.inactivityTimeOut / 60000);
    // Debugging
    doc["debuging"] = printerConfig.debuging;
    doc["debugingchange"] = printerConfig.debugingchange;
    doc["mqttdebug"] = printerConfig.mqttdebug;
    // Printer Dependant
    doc["p1Printer"] = printerVariables.isP1Printer;
    doc["doorSwitch"] =  printerVariables.useDoorSwtich;
    // Customise LED Colors (during Lidar)
    doc["stage14RGB"] = printerConfig.stage14Color.RGBhex;
    doc["stage14WW"] = printerConfig.stage14Color.ww;
    doc["stage14CW"] = printerConfig.stage14Color.cw;
    doc["stage1RGB"] = printerConfig.stage1Color.RGBhex;
    doc["stage1WW"] = printerConfig.stage1Color.ww;
    doc["stage1CW"] = printerConfig.stage1Color.cw;
    doc["stage8RGB"] = printerConfig.stage8Color.RGBhex;
    doc["stage8WW"] = printerConfig.stage8Color.ww;
    doc["stage8CW"] = printerConfig.stage8Color.cw;
    doc["stage9RGB"] = printerConfig.stage9Color.RGBhex;
    doc["stage9WW"] = printerConfig.stage9Color.ww;
    doc["stage9CW"] = printerConfig.stage9Color.cw;
    doc["stage10RGB"] = printerConfig.stage10Color.RGBhex;
    doc["stage10WW"] = printerConfig.stage10Color.ww;
    doc["stage10CW"] = printerConfig.stage10Color.cw;
    // Customise LED Colors
    doc["errordetection"] = printerConfig.errordetection;
    doc["wifiRGB"] = printerConfig.wifiRGB.RGBhex;
    doc["wifiWW"] = printerConfig.wifiRGB.ww;
    doc["wifiCW"] = printerConfig.wifiRGB.cw;
    doc["pauseRGB"] = printerConfig.pauseRGB.RGBhex;
    doc["pauseWW"] = printerConfig.pauseRGB.ww;
    doc["pauseCW"] = printerConfig.pauseRGB.cw;
    doc["firstlayerRGB"] = printerConfig.firstlayerRGB.RGBhex;
    doc["firstlayerWW"] = printerConfig.firstlayerRGB.ww;
    doc["firstlayerCW"] = printerConfig.firstlayerRGB.cw;
    doc["nozzleclogRGB"] = printerConfig.nozzleclogRGB.RGBhex;
    doc["nozzleclogWW"] = printerConfig.nozzleclogRGB.ww;
    doc["nozzleclogCW"] = printerConfig.nozzleclogRGB.cw;
    doc["hmsSeriousRGB"] = printerConfig.hmsSeriousRGB.RGBhex;
    doc["hmsSeriousWW"] = printerConfig.hmsSeriousRGB.ww;
    doc["hmsSeriousCW"] = printerConfig.hmsSeriousRGB.cw;
    doc["hmsFatalRGB"] = printerConfig.hmsFatalRGB.RGBhex;
    doc["hmsFatalWW"] = printerConfig.hmsFatalRGB.ww;
    doc["hmsFatalCW"] = printerConfig.hmsFatalRGB.cw;
    doc["filamentRunoutRGB"] = printerConfig.filamentRunoutRGB.RGBhex;
    doc["filamentRunoutWW"] = printerConfig.filamentRunoutRGB.ww;
    doc["filamentRunoutCW"] = printerConfig.filamentRunoutRGB.cw;
    doc["frontCoverRGB"] = printerConfig.frontCoverRGB.RGBhex;
    doc["frontCoverWW"] = printerConfig.frontCoverRGB.ww;
    doc["frontCoverCW"] = printerConfig.frontCoverRGB.cw;
    doc["nozzleTempRGB"] = printerConfig.nozzleTempRGB.RGBhex;
    doc["nozzleTempWW"] = printerConfig.nozzleTempRGB.ww;
    doc["nozzleTempCW"] = printerConfig.nozzleTempRGB.cw;
    doc["bedTempRGB"] = printerConfig.bedTempRGB.RGBhex;
    doc["bedTempWW"] = printerConfig.bedTempRGB.ww;
    doc["bedTempCW"] = printerConfig.bedTempRGB.cw;

    docDefault["brightness"] = printerConfigDefault.brightness;
    // LED Behaviour (Choose One)
    docDefault["maintMode"] = printerConfigDefault.maintMode;
    docDefault["discoMode"] = printerConfigDefault.discoMode;
    docDefault["replicateled"] = printerConfigDefault.replicatestate;
    // Running Color
    docDefault["runningRGB"] = printerConfigDefault.runningColor.RGBhex;
    docDefault["runningWW"] = printerConfigDefault.runningColor.ww;
    docDefault["runningCW"] = printerConfigDefault.runningColor.cw;
    // LED Test Colors
    docDefault["showtestcolor"] = printerConfigDefault.testcolorEnabled;
    docDefault["testRGB"] = printerConfigDefault.testColor.RGBhex;
    docDefault["testWW"] = printerConfigDefault.testColor.ww;
    docDefault["testCW"] = printerConfigDefault.testColor.cw;
    docDefault["debugwifi"] = printerConfigDefault.debugwifi;
    // Options
    docDefault["finishindication"] = printerConfigDefault.finishindication;
    docDefault["finishColor"] = printerConfigDefault.finishColor.RGBhex;
    docDefault["finishWW"] = printerConfigDefault.finishColor.ww;
    docDefault["finishCW"] = printerConfigDefault.finishColor.cw;
    docDefault["finishExit"] = printerConfigDefault.finishExit;
    docDefault["inactivityEnabled"] = printerConfigDefault.inactivityEnabled;
    // Printer Dependant
    docDefault["debuging"] = printerConfigDefault.debuging;
    docDefault["debugingchange"] = printerConfigDefault.debugingchange;
    docDefault["mqttdebug"] = printerConfigDefault.mqttdebug;
    // Customise LED Colors (during Lidar)
    docDefault["stage14RGB"] = printerConfigDefault.stage14Color.RGBhex;
    docDefault["stage14WW"] = printerConfigDefault.stage14Color.ww;
    docDefault["stage14CW"] = printerConfigDefault.stage14Color.cw;
    docDefault["stage1RGB"] = printerConfigDefault.stage1Color.RGBhex;
    docDefault["stage1WW"] = printerConfigDefault.stage1Color.ww;
    docDefault["stage1CW"] = printerConfigDefault.stage1Color.cw;
    docDefault["stage8RGB"] = printerConfigDefault.stage8Color.RGBhex;
    docDefault["stage8WW"] = printerConfigDefault.stage8Color.ww;
    docDefault["stage8CW"] = printerConfigDefault.stage8Color.cw;
    docDefault["stage9RGB"] = printerConfigDefault.stage9Color.RGBhex;
    docDefault["stage9WW"] = printerConfigDefault.stage9Color.ww;
    docDefault["stage9CW"] = printerConfigDefault.stage9Color.cw;
    docDefault["stage10RGB"] = printerConfigDefault.stage10Color.RGBhex;
    docDefault["stage10WW"] = printerConfigDefault.stage10Color.ww;
    docDefault["stage10CW"] = printerConfigDefault.stage10Color.cw;
    // Customise LED Colors
    docDefault["errordetection"] = printerConfigDefault.errordetection;
    docDefault["wifiRGB"] = printerConfigDefault.wifiRGB.RGBhex;
    docDefault["wifiWW"] = printerConfigDefault.wifiRGB.ww;
    docDefault["wifiCW"] = printerConfigDefault.wifiRGB.cw;
    docDefault["pauseRGB"] = printerConfigDefault.pauseRGB.RGBhex;
    docDefault["pauseWW"] = printerConfigDefault.pauseRGB.ww;
    docDefault["pauseCW"] = printerConfigDefault.pauseRGB.cw;
    docDefault["firstlayerRGB"] = printerConfigDefault.firstlayerRGB.RGBhex;
    docDefault["firstlayerWW"] = printerConfigDefault.firstlayerRGB.ww;
    docDefault["firstlayerCW"] = printerConfigDefault.firstlayerRGB.cw;
    docDefault["nozzleclogRGB"] = printerConfigDefault.nozzleclogRGB.RGBhex;
    docDefault["nozzleclogWW"] = printerConfigDefault.nozzleclogRGB.ww;
    docDefault["nozzleclogCW"] = printerConfigDefault.nozzleclogRGB.cw;
    docDefault["hmsSeriousRGB"] = printerConfigDefault.hmsSeriousRGB.RGBhex;
    docDefault["hmsSeriousWW"] = printerConfigDefault.hmsSeriousRGB.ww;
    docDefault["hmsSeriousCW"] = printerConfigDefault.hmsSeriousRGB.cw;
    docDefault["hmsFatalRGB"] = printerConfigDefault.hmsFatalRGB.RGBhex;
    docDefault["hmsFatalWW"] = printerConfigDefault.hmsFatalRGB.ww;
    docDefault["hmsFatalCW"] = printerConfigDefault.hmsFatalRGB.cw;
    docDefault["filamentRunoutRGB"] = printerConfigDefault.filamentRunoutRGB.RGBhex;
    docDefault["filamentRunoutWW"] = printerConfigDefault.filamentRunoutRGB.ww;
    docDefault["filamentRunoutCW"] = printerConfigDefault.filamentRunoutRGB.cw;
    docDefault["frontCoverRGB"] = printerConfigDefault.frontCoverRGB.RGBhex;
    docDefault["frontCoverWW"] = printerConfigDefault.frontCoverRGB.ww;
    docDefault["frontCoverCW"] = printerConfigDefault.frontCoverRGB.cw;
    docDefault["nozzleTempRGB"] = printerConfigDefault.nozzleTempRGB.RGBhex;
    docDefault["nozzleTempWW"] = printerConfigDefault.nozzleTempRGB.ww;
    docDefault["nozzleTempCW"] = printerConfigDefault.nozzleTempRGB.cw;
    docDefault["bedTempRGB"] = printerConfigDefault.bedTempRGB.RGBhex;
    docDefault["bedTempWW"] = printerConfigDefault.bedTempRGB.ww;
    docDefault["bedTempCW"] = printerConfigDefault.bedTempRGB.cw;

    result["config"] = doc;
    result["default"] = docDefault;

    String jsonString;
    serializeJson(result, jsonString);
    webServer.send(200, "application/json", jsonString);

    Serial.println(F("Packet sent to setuppage"));
    //serializeJson(doc, Serial);
    //Serial.println();
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

#ifndef _BLLEDWEB_SERVER
#define _BLLEDWEB_SERVER

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include "leds.h"
#include "filesystem.h"

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");

#include "../www/www.h"
#include "../www/blled_svg.h"
#include "../www/favicon.h"

unsigned long lastWsPush = 0;
const unsigned long wsPushInterval = 1000; // alle 1000ms

bool isAuthorized(AsyncWebServerRequest *request)
{
    if (strlen(securityVariables.HTTPUser) == 0 || strlen(securityVariables.HTTPPass) == 0)
    {
        return true;
    }
    return request->authenticate(securityVariables.HTTPUser, securityVariables.HTTPPass);
}

void handleSetup(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, setupPage_html_gz_mime, setupPage_html_gz, setupPage_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleOldSetup(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, setupPageOld_html_gz_mime, setupPageOld_html_gz, setupPageOld_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleUpdatePage(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, updatePage_html_gz_mime, updatePage_html_gz, updatePage_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleGetIcon(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, blled_svg_gz_mime, blled_svg_gz, blled_svg_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleGetfavicon(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, favicon_png_gz_mime, favicon_png_gz, favicon_png_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleGetPCC(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, particleCanvas_js_gz_mime, (const uint8_t *)particleCanvas_js_gz, particleCanvas_js_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleGetConfig(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }

    JsonDocument doc;

    doc["firmwareversion"] = globalVariables.FWVersion.c_str();
    doc["wifiStrength"] = WiFi.RSSI();
    doc["ip"] = printerConfig.printerIP;
    doc["code"] = printerConfig.accessCode;
    doc["id"] = printerConfig.serialNumber;
    doc["apMAC"] = printerConfig.BSSID;
    doc["brightness"] = printerConfig.brightness;
    doc["maintMode"] = printerConfig.maintMode;
    doc["discoMode"] = printerConfig.discoMode;
    doc["replicateled"] = printerConfig.replicatestate;
    doc["runningRGB"] = printerConfig.runningColor.RGBhex;
    doc["runningWW"] = printerConfig.runningColor.ww;
    doc["runningCW"] = printerConfig.runningColor.cw;
    doc["showtestcolor"] = printerConfig.testcolorEnabled;
    doc["testRGB"] = printerConfig.testColor.RGBhex;
    doc["testWW"] = printerConfig.testColor.ww;
    doc["testCW"] = printerConfig.testColor.cw;
    doc["debugwifi"] = printerConfig.debugwifi;
    doc["finishindication"] = printerConfig.finishindication;
    doc["finishColor"] = printerConfig.finishColor.RGBhex;
    doc["finishWW"] = printerConfig.finishColor.ww;
    doc["finishCW"] = printerConfig.finishColor.cw;
    doc["finishExit"] = printerConfig.finishExit;
    doc["finishTimerMins"] = (int)(printerConfig.finishTimeOut / 60000);
    doc["inactivityEnabled"] = printerConfig.inactivityEnabled;
    doc["inactivityMins"] = (int)(printerConfig.inactivityTimeOut / 60000);
    doc["debuging"] = printerConfig.debuging;
    doc["debugingchange"] = printerConfig.debugingchange;
    doc["mqttdebug"] = printerConfig.mqttdebug;
    doc["p1Printer"] = printerVariables.isP1Printer;
    doc["doorSwitch"] = printerVariables.useDoorSwtich;
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

    String jsonString;
    serializeJson(doc, jsonString);
    request->send(200, "application/json", jsonString);
}

void handlePrinterConfigJson(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    JsonDocument doc;
    doc["ssid"] = globalVariables.SSID;
    doc["pass"] = globalVariables.APPW;
    doc["printerIP"] = printerConfig.printerIP;
    doc["printerSerial"] = printerConfig.serialNumber;
    doc["accessCode"] = printerConfig.accessCode;
    doc["webUser"] = securityVariables.HTTPUser;
    doc["webPass"] = securityVariables.HTTPPass;

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void handleSubmitConfig(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }

    bool newBSSID = false;
    printerConfig.brightness = request->getParam("brightnessslider", true)->value().toInt();
    printerConfig.rescanWiFiNetwork = (request->hasParam("rescanWiFiNetwork", true));
    printerConfig.maintMode = (request->hasParam("maintMode", true));
    printerConfig.discoMode = (request->hasParam("discoMode", true));
    printerConfig.replicatestate = (request->hasParam("replicateLedState", true));
    printerConfig.runningColor = hex2rgb(request->getParam("runningRGB", true)->value().c_str(),
                                         request->getParam("runningWW", true)->value().toInt(),
                                         request->getParam("runningCW", true)->value().toInt());
    printerConfig.testcolorEnabled = (request->hasParam("showtestcolor", true));
    printerConfig.testColor = hex2rgb(request->getParam("testRGB", true)->value().c_str(),
                                      request->getParam("testWW", true)->value().toInt(),
                                      request->getParam("testCW", true)->value().toInt());
    printerConfig.debugwifi = (request->hasParam("debugwifi", true));
    printerConfig.finishindication = (request->hasParam("finishIndication", true));
    printerConfig.finishColor = hex2rgb(request->getParam("finishColor", true)->value().c_str(),
                                        request->getParam("finishWW", true)->value().toInt(),
                                        request->getParam("finishCW", true)->value().toInt());
    printerConfig.finishExit = !(request->hasParam("finishEndTimer", true));
    printerConfig.finishTimeOut = request->getParam("finishTimerMins", true)->value().toInt() * 60000;
    printerConfig.inactivityEnabled = (request->hasParam("inactivityEnabled", true));
    printerConfig.inactivityTimeOut = request->getParam("inactivityMins", true)->value().toInt() * 60000;
    printerConfig.debuging = (request->hasParam("debuging", true));
    printerConfig.debugingchange = (request->hasParam("debugingchange", true));
    printerConfig.mqttdebug = (request->hasParam("mqttdebug", true));
    printerVariables.isP1Printer = (request->hasParam("p1Printer", true));
    printerVariables.useDoorSwtich = (request->hasParam("doorSwitch", true));

    printerConfig.stage14Color = hex2rgb(request->getParam("stage14RGB", true)->value().c_str(),
                                         request->getParam("stage14WW", true)->value().toInt(),
                                         request->getParam("stage14CW", true)->value().toInt());
    printerConfig.stage1Color = hex2rgb(request->getParam("stage1RGB", true)->value().c_str(),
                                        request->getParam("stage1WW", true)->value().toInt(),
                                        request->getParam("stage1CW", true)->value().toInt());
    printerConfig.stage8Color = hex2rgb(request->getParam("stage8RGB", true)->value().c_str(),
                                        request->getParam("stage8WW", true)->value().toInt(),
                                        request->getParam("stage8CW", true)->value().toInt());
    printerConfig.stage9Color = hex2rgb(request->getParam("stage9RGB", true)->value().c_str(),
                                        request->getParam("stage9WW", true)->value().toInt(),
                                        request->getParam("stage9CW", true)->value().toInt());
    printerConfig.stage10Color = hex2rgb(request->getParam("stage10RGB", true)->value().c_str(),
                                         request->getParam("stage10WW", true)->value().toInt(),
                                         request->getParam("stage10CW", true)->value().toInt());
    printerConfig.errordetection = (request->hasParam("errorDetection", true));
    printerConfig.wifiRGB = hex2rgb(request->getParam("wifiRGB", true)->value().c_str(),
                                    request->getParam("wifiWW", true)->value().toInt(),
                                    request->getParam("wifiCW", true)->value().toInt());
    printerConfig.pauseRGB = hex2rgb(request->getParam("pauseRGB", true)->value().c_str(),
                                     request->getParam("pauseWW", true)->value().toInt(),
                                     request->getParam("pauseCW", true)->value().toInt());
    printerConfig.firstlayerRGB = hex2rgb(request->getParam("firstlayerRGB", true)->value().c_str(),
                                          request->getParam("firstlayerWW", true)->value().toInt(),
                                          request->getParam("firstlayerCW", true)->value().toInt());
    printerConfig.nozzleclogRGB = hex2rgb(request->getParam("nozzleclogRGB", true)->value().c_str(),
                                          request->getParam("nozzleclogWW", true)->value().toInt(),
                                          request->getParam("nozzleclogCW", true)->value().toInt());
    printerConfig.hmsSeriousRGB = hex2rgb(request->getParam("hmsSeriousRGB", true)->value().c_str(),
                                          request->getParam("hmsSeriousWW", true)->value().toInt(),
                                          request->getParam("hmsSeriousCW", true)->value().toInt());
    printerConfig.hmsFatalRGB = hex2rgb(request->getParam("hmsFatalRGB", true)->value().c_str(),
                                        request->getParam("hmsFatalWW", true)->value().toInt(),
                                        request->getParam("hmsFatalCW", true)->value().toInt());
    printerConfig.filamentRunoutRGB = hex2rgb(request->getParam("filamentRunoutRGB", true)->value().c_str(),
                                              request->getParam("filamentRunoutWW", true)->value().toInt(),
                                              request->getParam("filamentRunoutCW", true)->value().toInt());
    printerConfig.frontCoverRGB = hex2rgb(request->getParam("frontCoverRGB", true)->value().c_str(),
                                          request->getParam("frontCoverWW", true)->value().toInt(),
                                          request->getParam("frontCoverCW", true)->value().toInt());
    printerConfig.nozzleTempRGB = hex2rgb(request->getParam("nozzleTempRGB", true)->value().c_str(),
                                          request->getParam("nozzleTempWW", true)->value().toInt(),
                                          request->getParam("nozzleTempCW", true)->value().toInt());
    printerConfig.bedTempRGB = hex2rgb(request->getParam("bedTempRGB", true)->value().c_str(),
                                       request->getParam("bedTempWW", true)->value().toInt(),
                                       request->getParam("bedTempCW", true)->value().toInt());

    saveFileSystem();
    Serial.println(F("Packet received from setuppage"));
    printerConfig.inactivityStartms = millis();
    printerConfig.isIdleOFFActive = false;
    printerConfig.replicate_update = true;
    printerConfig.maintMode_update = true;
    printerConfig.discoMode_update = true;
    printerConfig.testcolor_update = true;
    updateleds();
    request->redirect("/");
}

void sendJsonToAll(JsonDocument &doc)
{
    String jsonString;
    serializeJson(doc, jsonString);
    ws.textAll(jsonString);
}

void handleWiFiScan(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonArray networks = doc["networks"].to<JsonArray>();

    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i)
    {
        JsonObject net = networks.add<JsonObject>();
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["enc"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? false : true;
    }

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void handleWiFiSetupPage(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse(200, wifiSetup_html_gz_mime, wifiSetup_html_gz, wifiSetup_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleSubmitWiFi(AsyncWebServerRequest *request)
{
    // Check for SSID + PASS optional handling
    bool hasSSID = request->hasParam("ssid", true);
    bool hasPASS = request->hasParam("pass", true);

    if (hasSSID && hasPASS)
    {
        String ssid = request->getParam("ssid", true)->value();
        String pass = request->getParam("pass", true)->value();
        ssid.trim();
        pass.trim();

        if (ssid.length() > 0 && pass.length() > 0)
        {
            Serial.println(F("[WiFiSetup] Updating WiFi credentials:"));
            Serial.print(F("SSID: "));
            Serial.println(ssid);
            Serial.print(F("Password: "));
            Serial.println(pass);

            strlcpy(globalVariables.SSID, ssid.c_str(), sizeof(globalVariables.SSID));
            strlcpy(globalVariables.APPW, pass.c_str(), sizeof(globalVariables.APPW));
        }
        else
        {
            Serial.println(F("[WiFiSetup] Empty SSID or PASS received → ignoring WiFi update."));
        }
    }
    else
    {
        Serial.println(F("[WiFiSetup] No SSID or PASS provided → keeping existing WiFi credentials."));
    }

    // Optional other fields (printerIP, printerSerial, etc.)
    String printerIP = request->hasParam("printerIP", true) ? request->getParam("printerIP", true)->value() : "";
    String printerSerial = request->hasParam("printerSerial", true) ? request->getParam("printerSerial", true)->value() : "";
    String accessCode = request->hasParam("accessCode", true) ? request->getParam("accessCode", true)->value() : "";
    String webUser = request->hasParam("webUser", true) ? request->getParam("webUser", true)->value() : "";
    String webPass = request->hasParam("webPass", true) ? request->getParam("webPass", true)->value() : "";

    if (printerIP.length() > 0)
        strlcpy(printerConfig.printerIP, printerIP.c_str(), sizeof(printerConfig.printerIP));
    if (printerSerial.length() > 0)
        strlcpy(printerConfig.serialNumber, printerSerial.c_str(), sizeof(printerConfig.serialNumber));
    if (accessCode.length() > 0)
        strlcpy(printerConfig.accessCode, accessCode.c_str(), sizeof(printerConfig.accessCode));

    strlcpy(securityVariables.HTTPUser, webUser.c_str(), sizeof(securityVariables.HTTPUser));
    strlcpy(securityVariables.HTTPPass, webPass.c_str(), sizeof(securityVariables.HTTPPass));

    saveFileSystem();

    request->send(200, "text/plain", "Settings saved, restarting...");
    shouldRestart = true;
    restartRequestTime = millis();
}

void websocketLoop()
{
    if (ws.count() == 0)
        return;
    if (millis() - lastWsPush > wsPushInterval)
    {
        lastWsPush = millis();

        JsonDocument doc;
        doc["wifi_rssi"] = WiFi.RSSI();
        doc["ip"] = WiFi.localIP().toString();
        doc["uptime"] = millis() / 1000;
        doc["doorOpen"] = printerVariables.doorOpen;
        doc["printerConnection"] = printerVariables.online;
        doc["clients"] = ws.count();

        sendJsonToAll(doc);

        Serial.println(F("[WS] JSON Status Push send."));
    }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("[WS] Client connected: %u\n", client->id());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("[WS] Client disconnected: %u\n", client->id());
        ws.cleanupClients();
        break;
    case WS_EVT_DATA:
        Serial.printf("[WS] Data received from client %u\n", client->id());
        break;
    case WS_EVT_PONG:
        Serial.printf("[WS] Pong received from %u\n", client->id());
        break;
    case WS_EVT_ERROR:
        Serial.printf("[WS] Error on connection %u\n", client->id());
        ws.cleanupClients();
        break;
    }
}

void setupWebserver()
{
    if (!MDNS.begin(globalVariables.Host.c_str()))
    {
        Serial.println(F("Error setting up MDNS responder!"));
        while (1)
            delay(500);
    }

    Serial.println(F("Setting up webserver"));

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
    if (WiFi.getMode() == WIFI_AP) {
        Serial.println(F("[WebServer] Captive Portal activ – redirect to /wifi"));
        request->redirect("/wifi");
    } else {
        handleSetup(request);
    } });
    webServer.on("/old", HTTP_GET, handleOldSetup);
    webServer.on("/fwupdate", HTTP_GET, handleUpdatePage);
    webServer.on("/getConfig", HTTP_GET, handleGetConfig);
    webServer.on("/submitConfig", HTTP_POST, handleSubmitConfig);
    webServer.on("/blled.png", HTTP_GET, handleGetIcon);
    webServer.on("/favicon.ico", HTTP_GET, handleGetfavicon);
    webServer.on("/particleCanvas.js", HTTP_GET, handleGetPCC);
    webServer.on("/config.json", HTTP_GET, handlePrinterConfigJson);
    webServer.on("/wifi", HTTP_GET, handleWiFiSetupPage);
    webServer.on("/wifiScan", HTTP_GET, handleWiFiScan);
    webServer.on("/submitWiFi", HTTP_POST, handleSubmitWiFi);

    webServer.on("/update", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
        request->send(200, "text/plain", "OK");
        Serial.println(F("Restarting Device"));
        delay(1000);
        ESP.restart(); });

    webServer.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
                           {
        if (!index) {
            Serial.printf("Update: %s\n", filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        }
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        }
        if (final) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u\nRebooting...\n", index + len);
            } else {
                Update.printError(Serial);
            }
        } });

    ws.onEvent(onWsEvent);
    webServer.addHandler(&ws);
    webServer.begin();

    Serial.println(F("Webserver started"));
}

#endif

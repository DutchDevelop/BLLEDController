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
    doc["turnOnLightWhenDoorIsOpen"] = printerConfig.turnOnLightWhenDoorIsOpen;
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
    // HMS Error Handling
    doc["hmsIgnoreList"] = printerConfig.hmsIgnoreList;
    // control chamber light
    doc["controlChamberLight"] = printerConfig.controlChamberLight;

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
    doc["isAPMode"] = (WiFi.getMode() & WIFI_AP);

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void handleStyleCss(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }
    AsyncWebServerResponse *response = request->beginResponse(200, style_css_gz_mime, style_css_gz, style_css_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleSubmitConfig(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }

    auto getSafeParamValue = [](AsyncWebServerRequest *req, const char *name, const char *fallback = "") -> String
    {
        return req->hasParam(name, true) ? req->getParam(name, true)->value() : fallback;
    };

    auto getSafeParamInt = [](AsyncWebServerRequest *req, const char *name, int fallback = 0) -> int
    {
        return req->hasParam(name, true) ? req->getParam(name, true)->value().toInt() : fallback;
    };

    printerConfig.brightness = getSafeParamInt(request, "brightnessslider");
    printerConfig.rescanWiFiNetwork = request->hasParam("rescanWiFiNetwork", true);
    printerConfig.maintMode = request->hasParam("maintMode", true);
    printerConfig.discoMode = request->hasParam("discoMode", true);
    printerConfig.replicatestate = request->hasParam("replicateLedState", true);
    printerConfig.runningColor = hex2rgb(getSafeParamValue(request, "runningRGB").c_str(), getSafeParamInt(request, "runningWW"), getSafeParamInt(request, "runningCW"));
    printerConfig.testcolorEnabled = request->hasParam("showtestcolor", true);
    printerConfig.testColor = hex2rgb(getSafeParamValue(request, "testRGB").c_str(), getSafeParamInt(request, "testWW"), getSafeParamInt(request, "testCW"));
    printerConfig.debugwifi = request->hasParam("debugwifi", true);
    printerConfig.finishindication = request->hasParam("finishIndication", true);
    printerConfig.turnOnLightWhenDoorIsOpen = request->hasParam("turnOnLightWhenDoorIsOpen", true);
    printerConfig.finishColor = hex2rgb(getSafeParamValue(request, "finishColor").c_str(), getSafeParamInt(request, "finishWW"), getSafeParamInt(request, "finishCW"));
    printerConfig.finishExit = !request->hasParam("finishEndTimer", true);
    printerConfig.finishTimeOut = getSafeParamInt(request, "finishTimerMins") * 60000;
    printerConfig.inactivityEnabled = request->hasParam("inactivityEnabled", true);
    printerConfig.inactivityTimeOut = getSafeParamInt(request, "inactivityMins") * 60000;
    printerConfig.debuging = request->hasParam("debuging", true);
    printerConfig.debugingchange = request->hasParam("debugingchange", true);
    printerConfig.mqttdebug = request->hasParam("mqttdebug", true);
    printerVariables.isP1Printer = request->hasParam("p1Printer", true);
    printerVariables.useDoorSwtich = request->hasParam("doorSwitch", true);

    printerConfig.stage14Color = hex2rgb(getSafeParamValue(request, "stage14RGB").c_str(), getSafeParamInt(request, "stage14WW"), getSafeParamInt(request, "stage14CW"));
    printerConfig.stage1Color = hex2rgb(getSafeParamValue(request, "stage1RGB").c_str(), getSafeParamInt(request, "stage1WW"), getSafeParamInt(request, "stage1CW"));
    printerConfig.stage8Color = hex2rgb(getSafeParamValue(request, "stage8RGB").c_str(), getSafeParamInt(request, "stage8WW"), getSafeParamInt(request, "stage8CW"));
    printerConfig.stage9Color = hex2rgb(getSafeParamValue(request, "stage9RGB").c_str(), getSafeParamInt(request, "stage9WW"), getSafeParamInt(request, "stage9CW"));
    printerConfig.stage10Color = hex2rgb(getSafeParamValue(request, "stage10RGB").c_str(), getSafeParamInt(request, "stage10WW"), getSafeParamInt(request, "stage10CW"));
    printerConfig.errordetection = request->hasParam("errorDetection", true);
    printerConfig.wifiRGB = hex2rgb(getSafeParamValue(request, "wifiRGB").c_str(), getSafeParamInt(request, "wifiWW"), getSafeParamInt(request, "wifiCW"));
    printerConfig.pauseRGB = hex2rgb(getSafeParamValue(request, "pauseRGB").c_str(), getSafeParamInt(request, "pauseWW"), getSafeParamInt(request, "pauseCW"));
    printerConfig.firstlayerRGB = hex2rgb(getSafeParamValue(request, "firstlayerRGB").c_str(), getSafeParamInt(request, "firstlayerWW"), getSafeParamInt(request, "firstlayerCW"));
    printerConfig.nozzleclogRGB = hex2rgb(getSafeParamValue(request, "nozzleclogRGB").c_str(), getSafeParamInt(request, "nozzleclogWW"), getSafeParamInt(request, "nozzleclogCW"));
    printerConfig.hmsSeriousRGB = hex2rgb(getSafeParamValue(request, "hmsSeriousRGB").c_str(), getSafeParamInt(request, "hmsSeriousWW"), getSafeParamInt(request, "hmsSeriousCW"));
    printerConfig.hmsFatalRGB = hex2rgb(getSafeParamValue(request, "hmsFatalRGB").c_str(), getSafeParamInt(request, "hmsFatalWW"), getSafeParamInt(request, "hmsFatalCW"));
    printerConfig.filamentRunoutRGB = hex2rgb(getSafeParamValue(request, "filamentRunoutRGB").c_str(), getSafeParamInt(request, "filamentRunoutWW"), getSafeParamInt(request, "filamentRunoutCW"));
    printerConfig.frontCoverRGB = hex2rgb(getSafeParamValue(request, "frontCoverRGB").c_str(), getSafeParamInt(request, "frontCoverWW"), getSafeParamInt(request, "frontCoverCW"));
    printerConfig.nozzleTempRGB = hex2rgb(getSafeParamValue(request, "nozzleTempRGB").c_str(), getSafeParamInt(request, "nozzleTempWW"), getSafeParamInt(request, "nozzleTempCW"));
    printerConfig.bedTempRGB = hex2rgb(getSafeParamValue(request, "bedTempRGB").c_str(), getSafeParamInt(request, "bedTempWW"), getSafeParamInt(request, "bedTempCW"));
    // HMS Error handling
    printerConfig.hmsIgnoreList = getSafeParamValue(request, "hmsIgnoreList");
    // Control Chamber Light
    printerConfig.controlChamberLight = request->hasParam("controlChamberLight", true);

    saveFileSystem();
    LogSerial.println(F("Packet received from setuppage"));
    printerConfig.inactivityStartms = millis();
    printerConfig.isIdleOFFActive = false;
    printerConfig.replicate_update = true;
    printerConfig.maintMode_update = true;
    printerConfig.discoMode_update = true;
    printerConfig.testcolor_update = true;
    updateleds();
    request->send(200, "text/plain", "OK");
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
        net["bssid"] = WiFi.BSSIDstr(i);
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
        String bssid = request->hasParam("bssid", true) ? request->getParam("bssid", true)->value() : "";
        if (bssid.length() > 0)
            strlcpy(printerConfig.BSSID, bssid.c_str(), sizeof(printerConfig.BSSID));

        ssid.trim();
        pass.trim();

        if (ssid.length() > 0 && pass.length() > 0)
        {
            LogSerial.println(F("[WiFiSetup] Updating WiFi credentials:"));
            LogSerial.print(F("SSID: "));
            LogSerial.println(ssid);
            LogSerial.print(F("Password: "));
            LogSerial.println(pass);

            strlcpy(globalVariables.SSID, ssid.c_str(), sizeof(globalVariables.SSID));
            strlcpy(globalVariables.APPW, pass.c_str(), sizeof(globalVariables.APPW));
        }
        else
        {
            LogSerial.println(F("[WiFiSetup] Empty SSID or PASS received → ignoring WiFi update."));
        }
    }
    else
    {
        LogSerial.println(F("[WiFiSetup] No SSID or PASS provided → keeping existing WiFi credentials."));
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
        doc["stg_cur"] = printerVariables.stage;
        sendJsonToAll(doc);
    }
}

void handleConfigPage(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
        return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, backupRestore_html_gz_mime, backupRestore_html_gz, backupRestore_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleDownloadConfigFile(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
    {
        return request->requestAuthentication();
    }

    if (!LittleFS.exists(configPath))
    {
        request->send(404, "text/plain", "Config file not found");
        return;
    }

    File configFile = LittleFS.open(configPath, "r");
    if (!configFile)
    {
        request->send(500, "text/plain", "Failed to open config file");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error)
    {
        request->send(500, "text/plain", "Failed to parse config file");
        return;
    }

    String jsonString;
    serializeJsonPretty(doc, jsonString);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonString);
    response->addHeader("Content-Disposition", "attachment; filename=\"blledconfig.json\"");
    request->send(response);
}

void handleWebSerialPage(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
        return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, webSerialPage_html_gz_mime, webSerialPage_html_gz, webSerialPage_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handlePrinterList(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (int i = 0; i < bblKnownPrinterCount; i++)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["ip"] = bblLastKnownPrinters[i].ip.toString();
        obj["usn"] = bblLastKnownPrinters[i].usn;
    }

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void handleFactoryReset(AsyncWebServerRequest *request)
{
    if (!isAuthorized(request))
        return request->requestAuthentication();

    LogSerial.println(F("[FactoryReset] Performing full reset..."));

    deleteFileSystem(); // delete LittleFS config
    request->send(200, "text/plain", "Factory reset complete. Restarting...");

    shouldRestart = true;
    restartRequestTime = millis();
}

void handleUploadConfigFileData(AsyncWebServerRequest *request, const String &filename,
                                size_t index, uint8_t *data, size_t len, bool final)
{
    static File uploadFile;

    if (!index)
    {
        LogSerial.println(F("[ConfigUpload] Start"));
        uploadFile = LittleFS.open(configPath, "w");
    }
    if (uploadFile)
    {
        uploadFile.write(data, len);
    }
    if (final)
    {
        uploadFile.close();
        LogSerial.println(F("[ConfigUpload] Finished"));
    }
    shouldRestart = true;
    restartRequestTime = millis();
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        LogSerial.printf("[WS] Client connected: %u\n", client->id());
        websocketLoop();
        break;
    case WS_EVT_DISCONNECT:
        LogSerial.printf("[WS] Client disconnected: %u\n", client->id());
        ws.cleanupClients();
        break;
    case WS_EVT_DATA:
        LogSerial.printf("[WS] Data received from client %u\n", client->id());
        break;
    case WS_EVT_PONG:
        LogSerial.printf("[WS] Pong received from %u\n", client->id());
        break;
    case WS_EVT_ERROR:
        LogSerial.printf("[WS] Error on connection %u\n", client->id());
        ws.cleanupClients();
        break;
    }
}

void setupWebserver()
{
    if (!MDNS.begin(globalVariables.Host.c_str()))
    {
        LogSerial.println(F("Error setting up MDNS responder!"));
        while (1)
            delay(500);
    }

    LogSerial.println(F("Setting up webserver"));

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
    if (WiFi.getMode() == WIFI_AP) {
        LogSerial.println(F("[WebServer] Captive Portal activ – redirect to /wifi"));
        request->redirect("/wifi");
    } else {
        handleSetup(request);
    } });
    webServer.on("/fwupdate", HTTP_GET, handleUpdatePage);
    webServer.on("/getConfig", HTTP_GET, handleGetConfig);
    webServer.on("/submitConfig", HTTP_POST, handleSubmitConfig);
    webServer.on("/blled.svg", HTTP_GET, handleGetIcon);
    webServer.on("/favicon.ico", HTTP_GET, handleGetfavicon);
    webServer.on("/particleCanvas.js", HTTP_GET, handleGetPCC);
    webServer.on("/config.json", HTTP_GET, handlePrinterConfigJson);
    webServer.on("/wifi", HTTP_GET, handleWiFiSetupPage);
    webServer.on("/wifiScan", HTTP_GET, handleWiFiScan);
    webServer.on("/submitWiFi", HTTP_POST, handleSubmitWiFi);
    webServer.on("/style.css", HTTP_GET, handleStyleCss);
    webServer.on("/backuprestore", HTTP_GET, handleConfigPage);
    webServer.on("/configfile.json", HTTP_GET, handleDownloadConfigFile);
    webServer.on("/webserial", HTTP_GET, handleWebSerialPage);
    webServer.on("/printerList", HTTP_GET, handlePrinterList);
    webServer.on("/factoryreset", HTTP_GET, handleFactoryReset);
    webServer.on("/configrestore", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
        if (!isAuthorized(request)) {
            return request->requestAuthentication();
        }
        request->send(200, "text/plain", "Config uploaded. Restarting...");
        shouldRestart = true;
        restartRequestTime = millis(); }, [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
                 {
        static File uploadFile;

        if (!index) {
            LogSerial.printf("[ConfigUpload] Start: %s\n", filename.c_str());
            uploadFile = LittleFS.open(configPath, "w");
        }
        if (uploadFile) {
            uploadFile.write(data, len);
        }
        if (final) {
            uploadFile.close();
            LogSerial.println(F("[ConfigUpload] Finished"));
        } });

    webServer.on("/update", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
        request->send(200, "text/plain", "OK");
        LogSerial.println(F("OTA Upload done. Marking for restart."));
        shouldRestart = true;
        restartRequestTime = millis(); }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
                 {
        if (!index) {
            LogSerial.printf("[OTA] Start: %s\n", filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(LogSerial);
            }
        }

        if (Update.write(data, len) != len) {
            Update.printError(LogSerial);
        }

        if (final) {
            if (Update.end(true)) {
                LogSerial.printf("[OTA] Success (%u bytes). Awaiting reboot...\n", index + len);
            } else {
                Update.printError(LogSerial);
            }
        } });

    LogSerial.begin(&webServer);

    ws.onEvent(onWsEvent);
    webServer.addHandler(&ws);

    webServer.begin();

    LogSerial.println(F("Webserver started"));
}

#endif

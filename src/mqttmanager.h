#ifndef _MQTTMANAGER
#define _MQTTMANAGER

#include <Arduino.h>
#include <WiFi.h>
static int mqttbuffer = 32768;
static int mqttdocument = 16384;

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 

#include "mqttparsingutility.h"
#include "types.h"
#include "leds.h"

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);

String device_topic;
String report_topic;
String clientId = "BLLED-";

unsigned long mqttattempt = (millis()-3000);

void connectMqtt(){
    if (!mqttClient.connected() && (millis() - mqttattempt) >= 3000){   
        Serial.println(F("Connecting to mqtt"));
        if (mqttClient.connect(clientId.c_str(),"bblp",printerConfig.accessCode)){
            Serial.println(F("MWTT connected, subscribing to topic:"));
            Serial.println(report_topic);
            mqttClient.subscribe(report_topic.c_str());
            printerVariables.online = true;
            updateleds();
        }else{
            Serial.println(F("Failed to connect with error code: "));
            Serial.println(mqttClient.state());
            switch (mqttClient.state())
            {
            case -4: // MQTT_CONNECTION_TIMEOUT
                Serial.println(F("MQTT TIMEOUT"));
                break;
            case -2: // MQTT_CONNECT_FAILED
                Serial.println(F("MQTT CONNECT_FAILED"));
                break;
            case -3: // MQTT_CONNECTION_LOST
                Serial.println(F("MQTT CONNECTION_LOST"));
                break;
            case -1: // MQTT_DISCONNECTED
                Serial.println(F("MQTT DISCONNECTED"));
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5: // MQTT UNAUTHORIZED
                Serial.println(F("MQTT UNAUTHORIZED"));
                ESP.restart();
                break;
            }
        }
    }
}

void ParseCallback(JsonDocument &messageobject){

    if (printerConfig.debuging){
        Serial.println(F("Mqtt message received,"));
        Serial.println(F("FreeHeap: "));
        Serial.print(ESP.getFreeHeap());
        Serial.println();
    }

    if (printerConfig.mqttdebug){
        Serial.println(F("Mqtt payload:"));
        Serial.println();
        serializeJson(messageobject, Serial);
        Serial.println();
    }

    bool Changed = false;
    if (messageobject["print"].containsKey("stg_cur")){
        printerVariables.stage = messageobject["print"]["stg_cur"];
        Changed = true;
    }else{
        if (printerConfig.debuging){
            Serial.println(F("stg_cur not in message"));
        }
    }

    if (messageobject["print"].containsKey("gcode_state")){
        printerVariables.gcodeState = messageobject["print"]["gcode_state"].as<String>();
        if (printerVariables.gcodeState == "FINISH"){
            if (printerVariables.finished == false){
                printerVariables.finished = true;
                printerVariables.finishstartms = millis();
            }
        }else{
            printerVariables.finished = false;
        }
        Changed = true;
    }

    if (messageobject["print"].containsKey("lights_report")) {
        JsonArray lightsReport = messageobject["print"]["lights_report"];

        for (JsonObject light : lightsReport) {
            if (light["node"] == "chamber_light") {
                printerVariables.ledstate = light["mode"] == "on";
                Changed = true;
            }
        }
    }else{
        if (printerConfig.debuging){
            Serial.println(F("lights_report not in message"));
        }
    }

    if (messageobject["print"].containsKey("hms")){
        printerVariables.hmsstate = false;
        printerVariables.parsedHMS = "";
        for (const auto& hms : messageobject["print"]["hms"].as<JsonArray>()) {
            if (ParseHMSSeverity(hms["code"]) != ""){
                printerVariables.hmsstate = true;
                printerVariables.parsedHMS = ParseHMSSeverity(hms["code"]);
            }
        }
        Changed = true;
    }

    if (Changed == true){
        updateleds();
    }
}

StaticJsonDocument<64> getMqttPayloadFilter()
{
    StaticJsonDocument<64> filter;
    filter["print"]["stg_cur"] = true;
    filter["print"]["gcode_state"] = true;
    filter["print"]["lights_report"] = true;
    filter["print"]["hms"] = true;
    // Make sure to add more here when needed
    return filter;
}

void mqttCallback(char *topic, byte *payload, unsigned int length){
    DynamicJsonDocument messageobject(mqttdocument);
    
    auto deserializeError = deserializeJson(messageobject, payload, length, DeserializationOption::Filter(getMqttPayloadFilter()));
    if (!deserializeError){
        ParseCallback(messageobject);
    }else{
        Serial.println(F("Deserialize error while parsing mqtt"));
    }
}

void setupMqtt(){
    clientId += String(random(0xffff), HEX);
    Serial.println(F("Setting up MQTT with ip: "));
    Serial.println(printerConfig.printerIP);

    device_topic = String("device/") + printerConfig.serialNumber;
    report_topic = device_topic + String("/report");

    wifiSecureClient.setInsecure();
    mqttClient.setBufferSize(mqttbuffer); //4096
    mqttClient.setServer(printerConfig.printerIP, 8883);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setSocketTimeout(20);
    Serial.println(F("Finished setting up MQTT"));
    connectMqtt();
}

void mqttloop(){
    if (!mqttClient.connected()){
        printerVariables.online = false;
        updateleds();
        connectMqtt();
        return;
    }

    mqttClient.loop();
    delay(10);
}

#endif

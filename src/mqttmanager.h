#ifndef _MQTTMANAGER
#define _MQTTMANAGER

#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 

#include "mqttparsingutility.h"
#include "types.h"
#include "leds.h"

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);

String device_topic;
String report_topic;
String request_topic;
String clientId = "BLLED-";
unsigned long mqttattempt = (millis()-3000);

void pushall(){ //Provided by WolfwithSword
    String message = "{\"pushing\":{\"sequence_id\":\"0\",\"command\":\"start\"}}";
    mqttClient.publish(request_topic.c_str(), message.c_str()); //Awake Machine incase its in a deepsleep
    message = "{\"pushing\":{\"sequence_id\":\"1\",\"command\":\"pushall\"}}";
    mqttClient.publish(request_topic.c_str(), message.c_str()); //Send pushall
}

void connectMqtt(){
    device_topic = String("device/") + printerConfig.serialNumber;
    report_topic = device_topic + String("/report");
    request_topic = device_topic + String("/request");
    if (!mqttClient.connected() && (millis() - mqttattempt) >= 3000){   
        if (mqttClient.connect(clientId.c_str(),"bblp",printerConfig.accessCode)){
            Serial.println(F("Connected to mqtt"));
            Serial.println(report_topic);
            mqttClient.subscribe(report_topic.c_str());
            //pushall();
            printerVariables.online = true;
            updateleds();
        }else{
            Serial.println(F("Failed to connect to mqtt"));
            Serial.println(mqttClient.state());
        }
    }
}

void ParseCallback(JsonDocument &messageobject){

    if (printerConfig.debuging){
        Serial.println("");
        serializeJson(messageobject, Serial);
        Serial.println("");
    }

    bool Changed = false;
    if (messageobject["print"].containsKey("stg_cur")){
        printerVariables.stage = messageobject["print"]["stg_cur"];
        Changed = true;
    }

    if (messageobject["print"].containsKey("gcode_state")){
        printerVariables.gcodeState = messageobject["print"]["gcode_state"].as<String>();
        if (printerVariables.gcodeState == "FINISH"){
            printerVariables.finishstartms = millis();
        }
        Changed = true;
    }

    if (messageobject["print"].containsKey("lights_report")){
        if (messageobject["print"]["lights_report"][0]["node"] == "chamber_light"){
            printerVariables.ledstate = messageobject["print"]["lights_report"][0]["mode"] == "on";
            Changed = true;
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

void mqttCallback(char *topic, byte *payload, unsigned int length){
    DynamicJsonDocument messageobject(4096);
    auto deserializeError = deserializeJson(messageobject, payload, length);
    if (!deserializeError){
        if (!messageobject.containsKey("print")) {
            return;
        }
        ParseCallback(messageobject);
    }else{
        Serial.println(F("Deserialize error while parsing mqtt"));
    }
}

void setupMqtt(){
    clientId += String(random(0xffff), HEX);
    Serial.println(F("Setting up MQTT with ip: "));
    Serial.println(printerConfig.printerIP);
    wifiSecureClient.setInsecure();
    mqttClient.setBufferSize(2096); //4096
    mqttClient.setServer(printerConfig.printerIP, 8883);
    mqttClient.setCallback(mqttCallback);
    //mqttClient.setSocketTimeout(20);
    Serial.println(F("Finished setting up MQTT, Attempting to connect"));
    connectMqtt();
}

void mqttloop(){
    if (!mqttClient.connected()){
        printerVariables.online = false;
        updateleds();
        connectMqtt();
    }else{
        mqttClient.loop();
    }
    delay(10);
}

#endif
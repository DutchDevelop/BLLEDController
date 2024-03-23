#ifndef _MQTTMANAGER
#define _MQTTMANAGER

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 

#include "mqttparsingutility.h"
#include "AutoGrowBufferStream.h"
#include "types.h"
#include "leds.h"

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);

String device_topic;
String report_topic;
String clientId = "BLLED-";

AutoGrowBufferStream stream;

unsigned long mqttattempt = (millis()-3000);

//With a Default BLLED
//Expected information when viewing MQTT status messages
			
//gcode_state	stg_cur	    BLLED LED control	    Comments
//------------------------------------------------------------------------
//IDLE	        -1	        White	                Printer just powered on
//RUNNING	    -1	        White	                Printer sent print file
//RUNNING	     2	        White	                PREHEATING BED
//RUNNING	    14	        OFF (for Lidar)	        CLEANING NOZZLE
//RUNNING	     1	        OFF (for Lidar)	        BED LEVELING
//RUNNING	     8	        OFF (for Lidar)	        CALIBRATING EXTRUSION
//RUNNING	     0	        White	                All the printing happens here
//FINISH	    -1	        Green	                After bed is lowered and filament retracted
//FINISH	    -1	        Green	                BLLED logic waits for a door interaction
//FINISH	    -1	        White	                After door interaction
//FINISH	    -1	        OFF                     Inactivity after 30mins


void connectMqtt(){
    if(WiFi.status() != WL_CONNECTED){
        //Abort MQTT connection attempt when no Wifi
        return;
    }
    if (!mqttClient.connected() && (millis() - mqttattempt) >= 3000){   
        Serial.println(F("Connecting to mqtt..."));
        if (mqttClient.connect(clientId.c_str(),"bblp",printerConfig.accessCode)){
            Serial.print(F("MQTT connected, subscribing to MQTT Topic:  "));
            Serial.println(report_topic);
            mqttClient.subscribe(report_topic.c_str());
            printerVariables.online = true;
            printerVariables.disconnectMQTTms = 0;
            Serial.println(F("Updating LEDs from MQTT connect"));
            updateleds();
        }else{
            Serial.println(F("Failed to connect with error code: "));
            Serial.print(mqttClient.state());
            Serial.print(F("  "));
            ParseMQTTState(mqttClient.state());
            if(mqttClient.state() == 5){
                Serial.println(F("Restarting Device"));
                delay(1000);
                ESP.restart();                
            }
        }
    }
}

void ParseCallback(char *topic, byte *payload, unsigned int length){
    JsonDocument messageobject;

    JsonDocument filter;
    //Rather than showing the entire message to Serial - grabbing only the pertinent bits for BLLED.
    //Device Status
    filter["print"]["command"] =  true;
    filter["print"]["fail_reason"] =  true;
    filter["print"]["gcode_state"] =  true;
    filter["print"]["print_gcode_action"] =  true;
    filter["print"]["print_real_action"] =  true;
    filter["print"]["hms"] =  true;
    filter["print"]["lights_report"] =  true;
    filter["print"]["home_flag"] =  true;
    filter["print"]["stg_cur"] =  true;
    filter["print"]["print_error"] =  true;
    filter["print"]["wifi_signal"] =  true;

    //Adding extra filters that looking interesting
    //Could be useful for additional functionality that might be developed at a later date
    //Color blend, the closer to 100% - I dunno

    //Print Job Status
    //filter["print"]["subtask_name"] =  true;  //Print Name
    //filter["print"]["layer_num"] =  true;
    //filter["print"]["total_layer_num"] =  true;
    //filter["print"]["mc_percent"] =  true;
    //filter["print"]["mc_print_error_code"] =  true;
    //filter["print"]["mc_print_stage"] =  true;
    //filter["print"]["mc_print_sub_stage"] =  true;
    //filter["print"]["mc_remaining_time"] =  true;

    //Temperatures
    //filter["print"]["bed_target_temper"] =  true;
    //filter["print"]["bed_temper"] =  true;
    //filter["print"]["nozzle_target_temper"] =  true;
    //filter["print"]["nozzle_temper"] =  true;
    //filter["print"]["chamber_temper"] =  true;

    auto deserializeError = deserializeJson(messageobject, payload, length, DeserializationOption::Filter(filter));
    if (!deserializeError){

        if (printerConfig.debuging){
            Serial.print(F("Mqtt message received,  "));
            Serial.print(F("FreeHeap: "));
            Serial.println(ESP.getFreeHeap());
        }

        bool Changed = false;

        if (messageobject["print"].containsKey("command")){
            if (messageobject["print"]["command"].as<String>() == "gcode_line"){
                //gcode_line used a lot during print initialisations - Skip these
                return;
            }
        }
        if(messageobject.size() == 0)
        {
            //Null or Filtered essage that is not 'print' - Ignore
            return;
        }

        //Output Filtered MQTT message
        if (printerConfig.mqttdebug){
            Serial.print(F("(Filtered) MQTT payload, "));
            Serial.print(messageobject.size());
            Serial.print(F(", "));
            serializeJson(messageobject, Serial);
            Serial.println();
        }

        //Check BBLP Stage
        if (messageobject["print"].containsKey("stg_cur")){
            if (printerVariables.stage != messageobject["print"]["stg_cur"].as<int>() ){
                printerVariables.inactivityStartms = millis();  //restart idle timer
                printerVariables.stage = messageobject["print"]["stg_cur"];
                
                if (printerConfig.debugingchange || printerConfig.debuging){
                    Serial.print(F("MQTT update - stg_cur now: "));
                    Serial.println(printerVariables.stage);
                }
                Changed = true;
            }
        }else{
            if (printerConfig.debuging){
                Serial.println(F("MQTT stg_cur not in message"));
            }
        }

        //Check BBLP GCode State
        if (messageobject["print"].containsKey("gcode_state")){
            String mqttgcodeState = messageobject["print"]["gcode_state"].as<String>();

            if(mqttgcodeState =="RUNNING" || mqttgcodeState =="PAUSE"){
                    //Never turn off light (due to idle timer) while in this state
                    printerVariables.inactivityStartms = millis();
            }

            // Onchange of gcodeState...
            if(printerVariables.gcodeState != mqttgcodeState){
                printerVariables.inactivityStartms = millis();  //restart idle timer

                if(mqttgcodeState =="FINISH"){
                    printerVariables.finished = true;
                    if(printerConfig.finishindication == true) printerVariables.waitingForDoor = true;
                }
                printerVariables.gcodeState = mqttgcodeState;

                if (printerConfig.debugingchange || printerConfig.debuging){
                    Serial.print(F("MQTT update - gcode_state now: "));
                    Serial.println(printerVariables.gcodeState);
                }
                Changed = true;
            }
        }


        if (messageobject["print"].containsKey("lights_report")) {
            JsonArray lightsReport = messageobject["print"]["lights_report"];

            for (JsonObject light : lightsReport) {
                if (light["node"] == "chamber_light") {
                    if(printerVariables.printerledstate != (light["mode"] == "on")){
                        printerVariables.printerledstate = light["mode"] == "on";
                        if (printerConfig.debugingchange || printerConfig.debuging){
                            Serial.print(F("MQTT chamber_light now: "));
                            Serial.println(printerVariables.printerledstate);
                        }
                        Changed = true;
                    }
                }
            }
        }else{
            if (printerConfig.debuging){
                Serial.println(F("MQTT lights_report not in message"));
            }
        }
        //Bambu Health Management System (HMS)
        if (messageobject["print"].containsKey("hms")){
            String oldHMS = "";
            oldHMS = printerVariables.parsedHMS;

            printerVariables.hmsstate = false;
            printerVariables.parsedHMS = "";
            for (const auto& hms : messageobject["print"]["hms"].as<JsonArray>()) {
                if (ParseHMSSeverity(hms["code"]) != ""){
                    printerVariables.hmsstate = true;
                    printerVariables.parsedHMS = ParseHMSSeverity(hms["code"]);
                    int attrib = hms["attr"];
                    printerVariables.parsedHMSattrib = (attrib>>16);
                }
            }
            if(oldHMSlevel != printerVariables.parsedHMSlevel){

                if(printerVariables.parsedHMScode == 0x0C0003000003000B) printerVariables.overridestage = 10;
                if(printerVariables.parsedHMScode == 0x0300120000020001) printerVariables.overridestage = 17;
                if(printerVariables.parsedHMScode == 0x0700200000030001) printerVariables.overridestage = 6;
                if(printerVariables.parsedHMScode == 0x0300020000010001) printerVariables.overridestage = 20;
                if(printerVariables.parsedHMScode == 0x0300010000010007) printerVariables.overridestage = 21;
                
                if (printerConfig.debuging  || printerConfig.debugingchange){
                    Serial.print(F("MQTT update - parsedHMSlevel now: "));
                    if (printerVariables.parsedHMSlevel.length() > 0) {
                        Serial.print(printerVariables.parsedHMSlevel);
                        Serial.print(F("      Error Code: "));
                        //Serial.println(F("https://wiki.bambulab.com/en/x1/troubleshooting/how-to-enter-the-specific-code-page"));
                        int chunk1 = (printerVariables.parsedHMScode >> 48);
                        int chunk2 = (printerVariables.parsedHMScode >> 32) & 0xFFFF;
                        int chunk3 = (printerVariables.parsedHMScode >> 16) & 0xFFFF;
                        int chunk4 = printerVariables.parsedHMScode & 0xFFFF;
                        char strHMScode[20];
                        sprintf(strHMScode, "%04X_%04X_%04X_%04X", chunk1, chunk2, chunk3, chunk4);
                        Serial.print(strHMScode);
                        if(printerVariables.overridestage != printerVariables.stage){
                            Serial.println(F(" **"));
                        }else{
                            Serial.println(F(""));
                        }
                    } else {
                        Serial.println(F("NULL"));
                        printerVariables.overridestage = 999;
                    }
                }
                Changed = true;
            }
        }

        if (Changed == true){
            printerConfig.inactivityStartms = millis();  //restart idle timer
            if (printerConfig.debuging){
                Serial.println(F("Change from mqtt"));
            }
            updateleds();
        }
    }else{
        Serial.println(F("Deserialize error while parsing mqtt"));
        return;
    }
}


void mqttCallback(char *topic, byte *payload, unsigned int length){
    ParseCallback(topic, (byte *)stream.get_buffer(), stream.current_length());
    stream.flush();
}

void setupMqtt(){
    clientId += String(random(0xffff), HEX);
    Serial.print(F("Setting up MQTT with ip: "));
    Serial.println(printerConfig.printerIP);

    device_topic = String("device/") + printerConfig.serialNumber;
    report_topic = device_topic + String("/report");

    wifiSecureClient.setInsecure();
    mqttClient.setBufferSize(1024); //1024
    mqttClient.setServer(printerConfig.printerIP, 8883);
    mqttClient.setStream(stream);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setSocketTimeout(20);
    Serial.println(F("Finished setting up MQTT"));
    connectMqtt();
}

void mqttloop(){
    if(WiFi.status() != WL_CONNECTED){
        //Abort MQTT connection attempt when no Wifi
        return;
    }
    if (!mqttClient.connected()){
        printerVariables.online = false;
        //Only sent the timer from the first instance of a MQTT disconnect
        if(printerVariables.disconnectMQTTms == 0) {
            printerVariables.disconnectMQTTms = millis();
            //Record last time MQTT dropped connection
            Serial.println(F("MQTT dropped during mqttloop"));
            ParseMQTTState(mqttClient.state());
        }
        delay(500);
        connectMqtt();
        delay(32);
        return;
    }
    else{
        printerVariables.disconnectMQTTms = 0;
    }
    mqttClient.loop();
    delay(10);
}

#endif

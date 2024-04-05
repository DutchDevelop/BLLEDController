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
unsigned long lastMQTTupdate = millis();

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
            //Serial.println(F("Updating LEDs from MQTT connect"));
            //updateleds();
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
    filter["print"]["home_flag"] =  true;
    filter["print"]["lights_report"] =  true;
    filter["print"]["stg_cur"] =  true;
    filter["print"]["print_error"] =  true;
    filter["print"]["wifi_signal"] =  true;
    filter["system"]["command"] =  true;
    filter["system"]["led_mode"] =  true;

    auto deserializeError = deserializeJson(messageobject, payload, length, DeserializationOption::Filter(filter));
    if (!deserializeError){
        if (printerConfig.debuging){
            Serial.println(F("Mqtt message received."));
            Serial.print(F("FreeHeap: "));
            Serial.println(ESP.getFreeHeap());
        }

        bool Changed = false;

        if (messageobject["print"].containsKey("command")){
            if (messageobject["print"]["command"] == "gcode_line"           //gcode_line used a lot during print initialisations - Skip these
            || messageobject["print"]["command"] == "project_prepare"       //1 message per print
            || messageobject["print"]["command"] == "project_file"          //1 message per print
            || messageobject["print"]["command"] == "clean_print_error"     //During error (no info)
            || messageobject["print"]["command"] == "resume"                //After error or pause
            || messageobject["print"]["command"] == "get_accessories"       //After error or pause
            || messageobject["print"]["command"] == "prepare"){             //1 message per print
                return;
            }
        }
        if(messageobject.size() == 0)
        {
            //Null or Filtered message that are not 'Print' or 'System' payload - Ignore these
            return;
        }

        //Output Filtered MQTT message
        if (printerConfig.mqttdebug){
            Serial.print(F("(Filtered) MQTT payload, ["));
            Serial.print(millis());
            Serial.print(F("], "));
            serializeJson(messageobject, Serial);
            Serial.println();
        }

        if(printerConfig.mqttdebug && (printerConfig.maintMode || printerConfig.testcolorEnabled || printerConfig.discoMode || printerConfig.debugwifi)) {
            Serial.print(F("MQTT Message Ignored while in "));
            if (printerConfig.maintMode) Serial.print(F("Maintenance"));
            if (printerConfig.testcolorEnabled) Serial.print(F("Test Color"));
            if (printerConfig.discoMode) Serial.print(F("RGB Cycle"));
            if (printerConfig.debugwifi) Serial.print(F("Wifi Debug"));
            Serial.println(F(" mode"));
            return;
        }

        //Check for Door Status
        if (messageobject["print"].containsKey("home_flag")){
            //https://github.com/greghesp/ha-bambulab/blob/main/custom_components/bambu_lab/pybambu/const.py#L324

            bool doorState = false;
            long homeFlag = 0;
            homeFlag = messageobject["print"]["home_flag"];
            doorState = homeFlag >> 23; //shift left 23 to the Door bit
            doorState = doorState & 1;  // remove any bits above Door bit

            if (printerVariables.doorOpen != doorState){
                printerVariables.doorOpen = doorState;
                
                if (printerConfig.debugingchange)Serial.print(F("MQTT Door "));
                if (printerVariables.doorOpen){
                   printerVariables.lastdoorOpenms  = millis();
                   if (printerConfig.debugingchange) Serial.println(F("Opened"));
                }
                else{
                    if ((millis() - printerVariables.lastdoorClosems) < 6000){
                        printerVariables.doorSwitchTriggered = true;
                    }
                    printerVariables.lastdoorClosems = millis();
                    if (printerConfig.debugingchange) Serial.println(F("Closed"));
                }
                Changed = true;
            }
        }

        //Check BBLP Stage
        if (messageobject["print"].containsKey("stg_cur")){
            if (printerVariables.stage != messageobject["print"]["stg_cur"].as<int>() ){
                printerVariables.stage = messageobject["print"]["stg_cur"];
                
                if (printerConfig.debugingchange || printerConfig.debuging){
                    Serial.print(F("MQTT update - stg_cur now: "));
                    Serial.println(printerVariables.stage);
                }
                Changed = true;
            }
        }

        //Check BBLP GCode State
        if (messageobject["print"].containsKey("gcode_state") && ((millis() - lastMQTTupdate) > 3000)){
            String mqttgcodeState = messageobject["print"]["gcode_state"].as<String>();

            if(mqttgcodeState =="RUNNING" || mqttgcodeState =="PAUSE"){
                    //Never turn off light (due to idle timer) while in this state
                    printerConfig.inactivityStartms = millis();
            }

            // Onchange of gcodeState...
            if(printerVariables.gcodeState != mqttgcodeState){
                if(mqttgcodeState =="FINISH"){
                    printerVariables.finished = true;
                    printerVariables.waitingForDoor = true; 
                    printerConfig.finishStartms = millis();
                    printerConfig.finish_check = true;
                }
                printerVariables.gcodeState = mqttgcodeState;

                if (printerConfig.debugingchange || printerConfig.debuging){
                    Serial.print(F("MQTT update - gcode_state now: "));
                    Serial.println(printerVariables.gcodeState);
                }
                Changed = true;
            }
        }

        //Pause Command - quicker, but Only for user generated pause - error & code pauses don't trigger this.
        if (messageobject["print"].containsKey("command")){
            if (messageobject["print"]["command"] == "pause"){
                lastMQTTupdate = millis();
                Serial.println(F("MQTT update - manual PAUSE"));
                printerVariables.gcodeState = "PAUSE";
                Changed = true;
            }
        }

        //Added a delay so the slower MQTT status message doesn't reverse the "system" commands
        if (messageobject["print"].containsKey("lights_report") && ((millis() - lastMQTTupdate) > 3000)) {
            JsonArray lightsReport = messageobject["print"]["lights_report"];
            for (JsonObject light : lightsReport) {
                if (light["node"] == "chamber_light") {

                    if(printerVariables.printerledstate != (light["mode"] == "on")){
                        printerVariables.printerledstate = (light["mode"] == "on");
                        printerConfig.replicate_update = true;
                        if (printerConfig.debugingchange || printerConfig.debuging){
                            Serial.print(F("MQTT chamber_light now: "));
                            Serial.println(printerVariables.printerledstate);
                        }
                        if(printerVariables.waitingForDoor && printerConfig.finish_check){
                            printerVariables.finished = true;
                        }
                        Changed = true;
                    }
                }
            }
        }
        //System Commands are sent quicker than the push_status
        //Message only sent onChange
        if (messageobject["system"].containsKey("command")) {
            if (messageobject["system"]["command"] == "ledctrl"){
                //Ignore Printer sending attempts to turn light on when already on.
                if(printerVariables.printerledstate != (messageobject["system"]["led_mode"] == "on")){
                    printerVariables.printerledstate = (messageobject["system"]["led_mode"] == "on");
                    printerConfig.replicate_update = true;
                    lastMQTTupdate = millis();
                    if (printerConfig.debugingchange || printerConfig.debuging){
                        Serial.print(F("MQTT led_mode now: "));
                        Serial.println(printerVariables.printerledstate);
                    }
                    if(printerVariables.waitingForDoor && printerConfig.finish_check){
                        printerVariables.finished = true;
                    }

                    Changed = true;
                }
            }
        }

        //Bambu Health Management System (HMS)
        if (messageobject["print"].containsKey("hms")){
            String oldHMSlevel = "";
            oldHMSlevel = printerVariables.parsedHMSlevel;

            printerVariables.hmsstate = false;
            printerVariables.parsedHMSlevel = "";
            for (const auto& hms : messageobject["print"]["hms"].as<JsonArray>()) {
                if (ParseHMSSeverity(hms["code"]) != ""){
                    printerVariables.hmsstate = true;
                    printerVariables.parsedHMSlevel = ParseHMSSeverity(hms["code"]);
                    printerVariables.parsedHMScode = ((uint64_t)hms["attr"] << 32) + (uint64_t)hms["code"];
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
            printerConfig.isIdleOFFActive = false;
            if (printerConfig.debuging){
                Serial.println(F("Change from mqtt"));
            }
            printerConfig.maintMode_update = true;
            printerConfig.discoMode_update = true;
            printerConfig.replicate_update = true;
            printerConfig.testcolor_update = true;

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
    Serial.print(F("Setting up MQTT with Bambu Lab Printer IP address: "));
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
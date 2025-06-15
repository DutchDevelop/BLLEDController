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

unsigned long mqttattempt = (millis() - 3000);
unsigned long lastMQTTupdate = millis();

TaskHandle_t mqttTaskHandle = NULL;
bool mqttTaskRunning = false;
volatile bool mqttConnectInProgress = false;

// With a Default BLLED
// Expected information when viewing MQTT status messages

// gcode_state	stg_cur	    BLLED LED control	    Comments
//------------------------------------------------------------------------
// IDLE	        -1	        White	                Printer just powered on
// RUNNING	    -1	        White	                Printer sent print file
// RUNNING	     2	        White	                PREHEATING BED
// RUNNING	    14	        OFF (for Lidar)	        CLEANING NOZZLE
// RUNNING	     1	        OFF (for Lidar)	        BED LEVELING
// RUNNING	     8	        OFF (for Lidar)	        CALIBRATING EXTRUSION
// RUNNING	     0	        White	                All the printing happens here
// FINISH	    -1	        Green	                After bed is lowered and filament retracted
// FINISH	    -1	        Green	                BLLED logic waits for a door interaction
// FINISH	    -1	        White	                After door interaction
// FINISH	    -1	        OFF                     Inactivity after 30mins

/* void connectMqtt()
{
    if (WiFi.status() != WL_CONNECTED  || WiFi.getMode() != WIFI_MODE_STA)
    {
        // Abort MQTT connection attempt when no Wifi
        return;
    }
    if (strlen(printerConfig.printerIP) == 0 || strlen(printerConfig.accessCode) == 0)
    {
        LogSerial.println(F("[MQTT] Abort connect: printerIP oder accessCode wrong or empty"));
        return;
    }
    if (!mqttClient.connected() && (millis() - mqttattempt) >= 3000)
    {
        tweenToColor(10, 10, 10, 10, 10);
        LogSerial.println(F("Connecting to mqtt..."));
        if (mqttClient.connect(clientId.c_str(), "bblp", printerConfig.accessCode))
        {
            LogSerial.print(F("MQTT connected, subscribing to MQTT Topic:  "));
            LogSerial.println(report_topic);
            mqttClient.subscribe(report_topic.c_str());
            printerVariables.online = true;
            printerVariables.disconnectMQTTms = 0;
            // LogSerial.println(F("Updating LEDs from MQTT connect"));
            // updateleds();
        }
        else
        {
            LogSerial.println(F("Failed to connect with error code: "));
            LogSerial.print(mqttClient.state());
            LogSerial.print(F("  "));
            ParseMQTTState(mqttClient.state());
            if (mqttClient.state() == 5)
            {
                //delay(500);
                tweenToColor(127, 0, 0, 0, 0); // light red, indicating not authorized
                //delay(500);
                mqttattempt = (millis() - 3000);
                //                LogSerial.println(F("Restarting Device"));
                //                delay(1000);
                //                ESP.restart();
            }
        }
    }
} */
void connectMqtt()
{
    if (mqttConnectInProgress) return;

    mqttConnectInProgress = true;

    if (WiFi.status() != WL_CONNECTED || WiFi.getMode() != WIFI_MODE_STA)
    {
        mqttConnectInProgress = false;
        return;
    }

    if (strlen(printerConfig.printerIP) == 0 || strlen(printerConfig.accessCode) == 0)
    {
        Serial.println(F("[MQTT] Abort connect: printerIP oder accessCode wrong or empty"));
        mqttConnectInProgress = false;
        return;
    }

    if (!mqttClient.connected() && (millis() - mqttattempt) >= 3000)
    {
        //tweenToColor(10, 10, 10, 10, 10);
        Serial.println(F("Connecting to mqtt..."));

        if (mqttClient.connect(clientId.c_str(), "bblp", printerConfig.accessCode))
        {
            Serial.print(F("MQTT connected, subscribing to MQTT Topic:  "));
            Serial.println(report_topic);
            mqttClient.subscribe(report_topic.c_str());
            printerVariables.online = true;
            printerVariables.disconnectMQTTms = 0;
        }
        else
        {
            Serial.println(F("Failed to connect with error code: "));
            Serial.print(mqttClient.state());
            Serial.print(F("  "));
            ParseMQTTState(mqttClient.state());

            if (mqttClient.state() == 5)
            {
                tweenToColor(127, 0, 0, 0, 0);
                mqttattempt = millis() - 3000;
            }
        }
    }

    mqttConnectInProgress = false;
}


void mqttTask(void *parameter)
{
    mqttTaskRunning = true;

    while (true)
    {
        if (WiFi.status() != WL_CONNECTED || WiFi.getMode() != WIFI_MODE_STA)
        {
            printerVariables.online = false;
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        if (!mqttClient.connected())
        {
            printerVariables.online = false;

            if (printerVariables.disconnectMQTTms == 0)
            {
                printerVariables.disconnectMQTTms = millis();
                LogSerial.println(F("[MQTT Task] Disconnected"));
                ParseMQTTState(mqttClient.state());
            }

            connectMqtt();
            vTaskDelay(pdMS_TO_TICKS(32));
        }
        else
        {
            printerVariables.disconnectMQTTms = 0;
            mqttClient.loop();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    mqttTaskRunning = false;
    vTaskDelete(NULL);
}

void ParseCallback(char *topic, byte *payload, unsigned int length)
{
    JsonDocument messageobject;
    JsonDocument filter;
    // Rather than showing the entire message to Serial - grabbing only the pertinent bits for BLLED.
    // Device Status

//sniped: implement to get layer num. for hms error, swap back to running state after layer change
/*     "print": {
        "3D": {
            "layer_num": 0,
            "total_layer_num": 191
        } */

    filter["print"]["command"] = true;
    filter["print"]["fail_reason"] = true;
    filter["print"]["gcode_state"] = true;
    filter["print"]["print_gcode_action"] = true;
    filter["print"]["print_real_action"] = true;
    filter["print"]["hms"] = true;
    filter["print"]["home_flag"] = true;
    filter["print"]["lights_report"] = true;
    filter["print"]["stg_cur"] = true;
    filter["print"]["print_error"] = true;
    filter["print"]["wifi_signal"] = true;
    filter["system"]["command"] = true;
    filter["system"]["led_mode"] = true;

    auto deserializeError = deserializeJson(messageobject, payload, length, DeserializationOption::Filter(filter));
    if (!deserializeError)
    {
        if (printerConfig.debuging)
        {
            LogSerial.println(F("Mqtt message received."));
            LogSerial.print(F("FreeHeap: "));
            LogSerial.println(ESP.getFreeHeap());
        }

        bool Changed = false;

        if (!messageobject["print"]["command"].isNull())
        {
            if (messageobject["print"]["command"] == "gcode_line"           // gcode_line used a lot during print initialisations - Skip these
                || messageobject["print"]["command"] == "project_prepare"   // 1 message per print
                || messageobject["print"]["command"] == "project_file"      // 1 message per print
                || messageobject["print"]["command"] == "clean_print_error" // During error (no info)
                || messageobject["print"]["command"] == "resume"            // After error or pause
                || messageobject["print"]["command"] == "get_accessories"   // After error or pause
                || messageobject["print"]["command"] == "prepare"
                || messageobject["print"]["command"] == "extrusion_cali_get")
            { // 1 message per print
                return;
            }
        }
        if (messageobject.size() == 0)
        {
            // Null or Filtered message that are not 'Print' or 'System' payload - Ignore these
            return;
        }

        // Output Filtered MQTT message
        if (printerConfig.mqttdebug)
        {
            LogSerial.print(F("(Filtered) MQTT payload, ["));
            LogSerial.print(millis());
            LogSerial.print(F("], "));
            serializeJson(messageobject, LogSerial);
            LogSerial.println();
        }

        if (printerConfig.mqttdebug && (printerConfig.maintMode || printerConfig.testcolorEnabled || printerConfig.discoMode || printerConfig.debugwifi))
        {
            LogSerial.print(F("MQTT Message Ignored while in "));
            if (printerConfig.maintMode)
                LogSerial.print(F("Maintenance"));
            if (printerConfig.testcolorEnabled)
                LogSerial.print(F("Test Color"));
            if (printerConfig.discoMode)
                LogSerial.print(F("RGB Cycle"));
            if (printerConfig.debugwifi)
                LogSerial.print(F("Wifi Debug"));
            LogSerial.println(F(" mode"));
            return;
        }

        // Check for Door Status
        if (!messageobject["print"]["home_flag"].isNull())
        {
            // https://github.com/greghesp/ha-bambulab/blob/main/custom_components/bambu_lab/pybambu/const.py#L324

            bool doorState = false;
            long homeFlag = 0;
            homeFlag = messageobject["print"]["home_flag"];
            doorState = bitRead(homeFlag, 23);

            if (printerVariables.doorOpen != doorState)
            {
                printerVariables.doorOpen = doorState;

                if (printerConfig.debugingchange)
                    LogSerial.print(F("MQTT Door "));
                if (printerVariables.doorOpen)
                {
                    printerVariables.lastdoorOpenms = millis();
                    if (printerConfig.debugingchange)
                        LogSerial.println(F("Opened"));
                }
                else
                {
                    if ((millis() - printerVariables.lastdoorClosems) < 6000)
                    {
                        printerVariables.doorSwitchTriggered = true;
                    }
                    printerVariables.lastdoorClosems = millis();
                    if (printerConfig.debugingchange)
                        LogSerial.println(F("Closed"));
                }
                Changed = true;
            }
        }

        // Check BBLP Stage
        if (!messageobject["print"]["stg_cur"].isNull())
        {
            if (printerVariables.stage != messageobject["print"]["stg_cur"].as<int>())
            {
                printerVariables.stage = messageobject["print"]["stg_cur"];

                if (printerConfig.debugingchange || printerConfig.debuging)
                {
                    LogSerial.print(F("MQTT update - stg_cur now: "));
                    LogSerial.println(printerVariables.stage);
                }
                Changed = true;
            }
        }

        // Check BBLP GCode State
        if (!messageobject["print"]["gcode_state"].isNull() && ((millis() - lastMQTTupdate) > 3000))
        {
            String mqttgcodeState = messageobject["print"]["gcode_state"].as<String>();

            if (mqttgcodeState == "RUNNING" || mqttgcodeState == "PAUSE")
            {
                // Never turn off light (due to idle timer) while in this state
                printerConfig.inactivityStartms = millis();
            }

            // Onchange of gcodeState...
            if (printerVariables.gcodeState != mqttgcodeState)
            {
                if (mqttgcodeState == "FINISH")
                {
                    printerVariables.finished = true;
                    printerVariables.waitingForDoor = true;
                    printerConfig.finishStartms = millis();
                    printerConfig.finish_check = true;
                }
                printerVariables.gcodeState = mqttgcodeState;

                if (printerConfig.debugingchange || printerConfig.debuging)
                {
                    LogSerial.print(F("MQTT update - gcode_state now: "));
                    LogSerial.println(printerVariables.gcodeState);
                }
                Changed = true;
            }
        }

        // Pause Command - quicker, but Only for user generated pause - error & code pauses don't trigger this.
        if (!messageobject["print"]["command"].isNull())
        {
            if (messageobject["print"]["command"] == "pause")
            {
                lastMQTTupdate = millis();
                LogSerial.println(F("MQTT update - manual PAUSE"));
                printerVariables.gcodeState = "PAUSE";
                Changed = true;
            }
        }

        // Added a delay so the slower MQTT status message doesn't reverse the "system" commands
        if (!messageobject["print"]["lights_report"].isNull() && ((millis() - lastMQTTupdate) > 3000))
        {
            JsonArray lightsReport = messageobject["print"]["lights_report"];
            for (JsonObject light : lightsReport)
            {
                if (light["node"] == "chamber_light")
                {

                    if (printerVariables.printerledstate != (light["mode"] == "on"))
                    {
                        printerVariables.printerledstate = (light["mode"] == "on");
                        printerConfig.replicate_update = true;
                        if (printerConfig.debugingchange || printerConfig.debuging)
                        {
                            LogSerial.print(F("MQTT chamber_light now: "));
                            LogSerial.println(printerVariables.printerledstate);
                        }
                        if (printerVariables.waitingForDoor && printerConfig.finish_check)
                        {
                            printerVariables.finished = true;
                        }
                        Changed = true;
                    }
                }
            }
        }
        // System Commands are sent quicker than the push_status
        // Message only sent onChange
        if (!messageobject["system"]["command"].isNull())
        {
            if (messageobject["system"]["command"] == "ledctrl")
            {
                // Ignore Printer sending attempts to turn light on when already on.
                if (printerVariables.printerledstate != (messageobject["system"]["led_mode"] == "on"))
                {
                    printerVariables.printerledstate = (messageobject["system"]["led_mode"] == "on");
                    printerConfig.replicate_update = true;
                    lastMQTTupdate = millis();
                    if (printerConfig.debugingchange || printerConfig.debuging)
                    {
                        LogSerial.print(F("MQTT led_mode now: "));
                        LogSerial.println(printerVariables.printerledstate);
                    }
                    if (printerVariables.waitingForDoor && printerConfig.finish_check)
                    {
                        printerVariables.finished = true;
                    }

                    Changed = true;
                }
            }
        }

        // Bambu Health Management System (HMS)
        if (!messageobject["print"]["hms"].isNull())
        {
            String oldHMSlevel = "";
            oldHMSlevel = printerVariables.parsedHMSlevel;

            printerVariables.hmsstate = false;
            printerVariables.parsedHMSlevel = "";
            for (const auto &hms : messageobject["print"]["hms"].as<JsonArray>())
            {
                if (ParseHMSSeverity(hms["code"]) != "")
                {
                    printerVariables.hmsstate = true;
                    printerVariables.parsedHMSlevel = ParseHMSSeverity(hms["code"]);
                    printerVariables.parsedHMScode = ((uint64_t)hms["attr"] << 32) + (uint64_t)hms["code"];
                }
            }
            if (oldHMSlevel != printerVariables.parsedHMSlevel)
            {

                if (printerVariables.parsedHMScode == 0x0C0003000003000B)
                    printerVariables.overridestage = 10;
                if (printerVariables.parsedHMScode == 0x0300120000020001)
                    printerVariables.overridestage = 17;
                if (printerVariables.parsedHMScode == 0x0700200000030001)
                    printerVariables.overridestage = 6;
                if (printerVariables.parsedHMScode == 0x0300020000010001)
                    printerVariables.overridestage = 20;
                if (printerVariables.parsedHMScode == 0x0300010000010007)
                    printerVariables.overridestage = 21;

                if (printerConfig.debuging || printerConfig.debugingchange)
                {
                    LogSerial.print(F("MQTT update - parsedHMSlevel now: "));
                    if (printerVariables.parsedHMSlevel.length() > 0)
                    {
                        LogSerial.print(printerVariables.parsedHMSlevel);
                        LogSerial.print(F("      Error Code: "));
                        // LogSerial.println(F("https://wiki.bambulab.com/en/x1/troubleshooting/how-to-enter-the-specific-code-page"));
                        int chunk1 = (printerVariables.parsedHMScode >> 48);
                        int chunk2 = (printerVariables.parsedHMScode >> 32) & 0xFFFF;
                        int chunk3 = (printerVariables.parsedHMScode >> 16) & 0xFFFF;
                        int chunk4 = printerVariables.parsedHMScode & 0xFFFF;
                        char strHMScode[20];
                        sprintf(strHMScode, "%04X_%04X_%04X_%04X", chunk1, chunk2, chunk3, chunk4);
                        LogSerial.print(strHMScode);
                        if (printerVariables.overridestage != printerVariables.stage)
                        {
                            LogSerial.println(F(" **"));
                        }
                        else
                        {
                            LogSerial.println(F(""));
                        }
                    }
                    else
                    {
                        LogSerial.println(F("NULL"));
                        printerVariables.overridestage = 999;
                    }
                }
                Changed = true;
            }
        }

        if (Changed == true)
        {
            printerConfig.inactivityStartms = millis(); // restart idle timer
            printerConfig.isIdleOFFActive = false;
            if (printerConfig.debuging)
            {
                LogSerial.println(F("Change from mqtt"));
            }
            printerConfig.maintMode_update = true;
            printerConfig.discoMode_update = true;
            printerConfig.replicate_update = true;
            printerConfig.testcolor_update = true;

            updateleds();
        }
    }
    else
    {
        LogSerial.println(F("Deserialize error while parsing mqtt"));
        return;
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    ParseCallback(topic, (byte *)stream.get_buffer(), stream.current_length());
    stream.flush();
}

void setupMqtt()
{
    clientId += String(random(0xffff), HEX);
    LogSerial.print(F("Setting up MQTT with Bambu Lab Printer IP address: "));
    LogSerial.println(printerConfig.printerIP);

    device_topic = String("device/") + printerConfig.serialNumber;
    report_topic = device_topic + String("/report");

    wifiSecureClient.setInsecure();
    wifiSecureClient.setTimeout(10);
    mqttClient.setSocketTimeout(10);
    mqttClient.setBufferSize(1024);
    mqttClient.setServer(printerConfig.printerIP, 8883);
    mqttClient.setStream(stream);
    mqttClient.setCallback(mqttCallback);

    LogSerial.println(F("Finished setting up MQTT"));

    if (mqttTaskHandle == NULL)
    {
        BaseType_t result;

        #if CONFIG_FREERTOS_UNICORE
            result = xTaskCreate(
                mqttTask,
                "mqttTask",
                6144,
                NULL,
                1,
                &mqttTaskHandle
            );
        #else
            result = xTaskCreatePinnedToCore(
                mqttTask,
                "mqttTask",
                6144,
                NULL,
                1,
                &mqttTaskHandle,
                1 // Core 1 (App Core)
            );
        #endif

        if (result == pdPASS)
        {
            LogSerial.println(F("MQTT task successfully started"));
        }
        else
        {
            LogSerial.println(F("Failed to create MQTT task!"));
        }
    }
}


void mqttloop()
{
    if (WiFi.status() != WL_CONNECTED || WiFi.getMode() != WIFI_MODE_STA)
    {
        // Abort MQTT connection attempt when no Wifi
        return;
    }
    if (!mqttClient.connected())
    {
        printerVariables.online = false;
        // Only sent the timer from the first instance of a MQTT disconnect
        if (printerVariables.disconnectMQTTms == 0)
        {
            printerVariables.disconnectMQTTms = millis();
            // Record last time MQTT dropped connection
            LogSerial.println(F("MQTT dropped during mqttloop"));
            ParseMQTTState(mqttClient.state());
        }
        //delay(500);
        connectMqtt();
        delay(32);
        return;
    }
    else
    {
        printerVariables.disconnectMQTTms = 0;
    }
    mqttClient.loop();
    delay(10);
}

#endif

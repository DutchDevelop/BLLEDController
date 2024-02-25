#include <Arduino.h>
#include "web-server.h"
#include "mqttmanager.h"
#include "filesystem.h"
#include "types.h"
#include "leds.h"
#include "serialmanager.h"
#include "wifi-manager.h"

void setup(){
    Serial.begin(115200);
    delay(100);
    Serial.println(F("Initializing"));
    Serial.println(ESP.getFreeHeap());
    setupLeds();
    tweenToColor(255,255,255,255,255,500);
    delay(2000);
    tweenToColor(255,0,0,0,0,500);
    setupFileSystem();
    loadFileSystem();
    delay(2000);
    tweenToColor(255,165,0,0,0,500); 
    setupSerial();

    if (strlen(globalVariables.SSID) == 0 || strlen(globalVariables.APPW) == 0) {
        Serial.println(F("SSID or password is missing. Please configure both by going to: https://dutchdevelop.com/blled-configuration-setup/"));
        tweenToColor(255,0,255,0,0,500); 
        return;
    }

    if (!connectToWifi()){
        return;
    };
    delay(2000);
    tweenToColor(0,0,255,0,0,500); 
    setupWebserver();

    delay(2000);

    tweenToColor(0,0,0,0,0,500); 
    setupMqtt();

    Serial.println(F("BLLED Controller started"));

    globalVariables.started = true;
}

void loop(){
    serialLoop();
    if (globalVariables.started == true){
        mqttloop();
        webserverloop();
        ledsloop();
        if (WiFi.status() != WL_CONNECTED){
            Serial.println(F("Lost Connection, Reconnecting"));
            WiFi.disconnect();
            WiFi.reconnect();
        }
    }
}
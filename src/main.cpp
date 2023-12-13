#include <Arduino.h>
#include "wifi-manager.h"
#include "web-server.h"
#include "mqttmanager.h"
#include "filesystem.h"
#include "types.h"
#include "leds.h"
#include "serialmanager.h"

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
    
    delay(2000);
    tweenToColor(0,255,0,0,0,500); 
    setupSerial();
    if (!setupWifi()){
        tweenToColor(255,0,255,0,0,500); 
        return;
    };
    delay(2000);
    tweenToColor(0,0,255,0,0,500); 
    setupWebserver();

    delay(2000);

    tweenToColor(0,0,0,0,0,500); 
    setupMqtt();
}

void loop(){
    serialLoop();
    if (WiFi.status() == WL_CONNECTED){
        mqttloop();
        webserverloop();
        ledsloop();
        
        //TODO: Get free heap to check for more headroom.
    }
}
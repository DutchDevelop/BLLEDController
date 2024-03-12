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
    tweenToColor(255,255,255,255,255,500); //ALL LEDS ON
    delay(1000);

    tweenToColor(255,0,0,0,0,500); //RED
    setupFileSystem();
    loadFileSystem();
    delay(1000);

    tweenToColor(255,165,0,0,0,500); //YELLOW
    setupSerial();

    if (strlen(globalVariables.SSID) == 0 || strlen(globalVariables.APPW) == 0) {
        Serial.println(F("SSID or password is missing. Please configure both by going to: https://dutchdevelop.com/blled-configuration-setup/"));
        tweenToColor(255,0,255,0,0,500); //PINK
        return;
    }

    delay(1000);
    
    if(!connectToWifi()){
        Serial.println(F("Wifi Connect Attempt Failed on Boot"));
    }
    
    

    tweenToColor(0,0,255,0,0,500); //BLUE
    setupWebserver();
    delay(1000);

    tweenToColor(34,224,238,0,0,500); //CYAN
    setupMqtt();
    delay(1000);

    Serial.println(F("BLLED Controller started"));
    Serial.println(F(""));

    globalVariables.started = true;
}

void loop(){
    serialLoop();
    if (printerVariables.overrideLEDstate == true){
        webserverloop();
        ledsloop();
    }
    else if (globalVariables.started == true){
        mqttloop();
        webserverloop();
        ledsloop();
        
        if (WiFi.status() != WL_CONNECTED){
            Serial.println(F("Lost Connection, Reconnecting"));
            WiFi.disconnect();
            delay(10);
            WiFi.reconnect();
        }
        else{
            //Monitor Wifi Strength
            long wifiNow = WiFi.RSSI();
            if (wifiNow > (printerVariables.lastRSSI + 9)  || wifiNow < (printerVariables.lastRSSI - 9))
            {
                //Wifi strength changed by +/- 10dBm
                if (printerConfig.debugingchange){
                    Serial.print(F("Wifi Strength now "));
                    Serial.println(wifiNow);
                }
                printerVariables.lastRSSI = wifiNow;
            }
        }
    }
}

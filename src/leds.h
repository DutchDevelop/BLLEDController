#ifndef _LED
#define _LED

#include <Arduino.h>
#include "mqttparsingutility.h"

const int redPin = 19;
const int greenPin = 18;
const int bluePin = 21;
const int warmPin = 22;
const int coldPin = 23;

int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;
int currentWarm = 0;
int currentCold = 0;

void tweenToColor(int targetRed, int targetGreen, int targetBlue, int targetWarm, int targetCold, int duration) {

    float brightness = (float)printerConfig.brightness/100.0;

    int brightenedRed = round(targetRed * brightness);
    int brightenedGreen = round(targetGreen * brightness);
    int brightenedBlue = round(targetBlue * brightness);
    int brightenedWarm = round(targetWarm * brightness);
    int brightenedCold = round(targetCold * brightness);

    if (brightenedRed == currentRed && brightenedGreen == currentGreen && brightenedBlue == currentBlue && brightenedWarm == currentWarm && brightenedCold == currentCold){
        if (printerConfig.debuging){
            Serial.println(F("LEDS Trying to change to the same color."));
        };
        return; // already at that color
    }
    float stepTime = (float)duration / 255.0;
    int redStep = (brightenedRed - currentRed) / 255;
    int greenStep = (brightenedGreen - currentGreen) / 255;
    int blueStep = (brightenedBlue - currentBlue) / 255;
    int warmStep = (brightenedWarm - currentWarm) / 255;
    int coldStep = (brightenedCold - currentCold) / 255;

     for (int i = 0; i < 256; i++) {
        currentRed += redStep;
        currentGreen += greenStep;
        currentBlue += blueStep;
        currentWarm += warmStep;
        currentCold += coldStep;

        analogWrite(redPin, currentRed);
        analogWrite(greenPin, currentGreen);
        analogWrite(bluePin, currentBlue);
        analogWrite(warmPin, currentWarm);
        analogWrite(coldPin, currentCold);
        delay(stepTime);
    }

    currentRed = brightenedRed;
    currentGreen = brightenedGreen;
    currentBlue = brightenedBlue;
    currentWarm = brightenedWarm;
    currentCold = brightenedCold;

    analogWrite(redPin, currentRed);
    analogWrite(greenPin, currentGreen);
    analogWrite(bluePin, currentBlue);
    analogWrite(warmPin, currentWarm);
    analogWrite(coldPin, currentCold);
    
    //Serial.println(F("Leds: "));
    //Serial.println(currentRed);
    //Serial.println(currentGreen);
    //Serial.println(currentBlue);
    //Serial.println(currentWarm);
    //Serial.println(currentCold);
}

float hue = 0.0;

void RGBCycle() {
    if (printerConfig.discoMode == false) {
        //Skip changing colors if RGB Mode not enabled
        return;
    }
    if (printerVariables.online == false) {
        analogWrite(redPin, 0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
        analogWrite(warmPin, 0);
        analogWrite(coldPin, 0);
        return;
    }
    hue += 0.1;
    if (hue > 360.0) {
        hue = 0.0;
    }

    currentRed = cos(hue * 3.14 / 180.0) * 255;
    currentGreen = cos((hue + 120.0) * 3.14 / 180.0) * 255;
    currentBlue = cos((hue + 240.0) * 3.14 / 180.0) * 255;
    currentWarm = 0;
    currentCold = 0;

    analogWrite(redPin, currentRed);
    analogWrite(greenPin, currentGreen);
    analogWrite(bluePin, currentBlue);
    analogWrite(warmPin, currentWarm);
    analogWrite(coldPin, currentCold);
}

void debugState(){
    Serial.print(printerVariables.stage);
    Serial.print(F(" "));
    Serial.print(printerVariables.previous_stage);
    Serial.print(F(" "));
    Serial.print(printerVariables.gcodeState);
    Serial.print(F(" "));
    Serial.print(printerVariables.previous_gcodeState);
    Serial.print(F(" "));
    Serial.print(printerVariables.finishstartms);
    Serial.print(F(" "));    
    Serial.print(printerVariables.idleStartms);
    Serial.print(F(" "));    
    Serial.println(printerVariables.finished);    
}

void updateleds(){
    int LEDTranistion_Duration = 500;

    if (printerConfig.debuging == true){
        Serial.println(F("Updating leds"));

        Serial.println(printerVariables.stage);
        Serial.println(printerVariables.gcodeState);
        Serial.println(printerVariables.ledstate);
        Serial.println(printerVariables.hmsstate);
        Serial.println(printerVariables.parsedHMS);
    }
    
    if (printerConfig.debugwifi == true){  //Force LED to show WIFI Strength (enabled via Webpage)
        //<=-50 dBm Green, <= -60 dBm LightGreen, <= -70 dBm Yellow, <= -80 dBm Orange, >80 Red
        if (WiFi.status() == WL_CONNECTED){
            long wifiNow = WiFi.RSSI();
            if (wifiNow >= -50) tweenToColor(0,255,0,0,0,LEDTranistion_Duration); //GREEN
            else if (wifiNow >= -60) tweenToColor(128,255,0,0,0,LEDTranistion_Duration); //LIGHTGREEN
            else if (wifiNow >= -70) tweenToColor(255,255,0,0,0,LEDTranistion_Duration); //YELLOW
            else if (wifiNow >= -80) tweenToColor(255,128,0,0,0,LEDTranistion_Duration); //ORANGE
            else if (wifiNow < -80) tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            else tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
            return;
        }
    }

    if (printerVariables.overrideLEDstate == true){  //Force LED ON regardless of state (via webpage)
        int r_LED = 0;
        int g_LED = 0;
        int b_LED = 0;
        int cw_LED = 0;
        int ww_LED = 0;
        if(printerConfig.overrideRed == true){ r_LED = 255;}
        if(printerConfig.overrideGreen == true){ g_LED = 255;}
        if(printerConfig.overrideBlue == true){ b_LED = 255;}
        if(printerConfig.overrideColdWhite == true){ cw_LED = 255;}
        if(printerConfig.overrideWarmWhite == true){ ww_LED = 255;}

        tweenToColor(r_LED,g_LED,b_LED,cw_LED,ww_LED,LEDTranistion_Duration); //Variable Test Color
        if (printerConfig.debuging){
            Serial.println(F("LED Test Override ON, Turning Leds On"));
        };
        return;
    }    


    //TOGGLE LIGHTS
    //If door is closed twice in 6 seconds, it will flip the state of the lights
    if (printerVariables.doorSwitchenabled == true){
        if (printerConfig.debugingchange){
            Serial.print(F("Door closed twice within 6 seconds - Toggling lights to "));
        }
        if(currentWarm == 0 && currentCold == 0)
        {
            tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
            //if (printerConfig.debuging || printerConfig.debugingchange) 
            Serial.println(F("ON"));
        }
        else
        {
            tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
            //if (printerConfig.debuging || printerConfig.debugingchange) 
            Serial.println(F("OFF"));
        }
        printerVariables.doorSwitchenabled = false;
    }

    //OFF
    if (printerConfig.discoMode == true){
        //Skip all the following code that sets specific colors if RGB Mode is on - done in RGBCycle()
        return;
    }

    if (printerVariables.online == false && (millis() - printerVariables.disconnectMQTTms) >= 5000){ //printer offline and MQTT disconnect more than 5 seconds.
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Printer offline, Turning Leds OFF"));
        };
        return;
    }

    if (printerVariables.ledstate == false && printerConfig.replicatestate == true){ // replicate printer behaviour
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("LED Replication, Turning Leds OFF"));
        };
        return;
    }

     if (printerVariables.stage == 14 && printerVariables.previous_stage != printerVariables.stage){ //Cleaning nozzle
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        printerVariables.previous_stage = printerVariables.stage;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 14, CLEANING NOZZLE, Turning Leds OFF"));
        };
        return;
    }

    if (printerVariables.stage == 1 && printerVariables.previous_stage != printerVariables.stage){ //Auto Bed Leveling
        printerVariables.previous_stage = printerVariables.stage;
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 1, BED LEVELING, Turning Leds OFF"));
        };
        return;
    }

    if (printerVariables.stage == 8 && printerVariables.previous_stage != printerVariables.stage){ //Calibrating Extrusion
        printerVariables.previous_stage = printerVariables.stage;
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 8, CALIBRATING EXTRUSION, Turning Leds OFF"));
        };
        return;
    }

    if (printerVariables.stage == 9 && printerVariables.previous_stage != printerVariables.stage){ //Scaning surface
        printerVariables.previous_stage = printerVariables.stage;
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 9, SCANNING BED SURFACE, Turning Leds OFF"));
        };
        return;
    }

    if (printerVariables.stage == 10 && printerVariables.previous_stage != printerVariables.stage){ //Inspecting First Layer
        printerVariables.previous_stage = printerVariables.stage;
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 10, FIRST LAYER INSPECTION, Turning Leds OFF"));
        };
        return;
    }

    if (printerVariables.stage == 12 && printerVariables.previous_stage != printerVariables.stage){ //Calibrating  MicroLidar
        printerVariables.previous_stage = printerVariables.stage;
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 12, CALIBRATING MICRO LIDAR, Turning Leds OFF"));
        };
        return;
    }
    
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) && printerVariables.finishstartms == 0 
    && (millis() - printerVariables.idleStartms) > 1800000 && printerVariables.previous_gcodeState == "TIMEOUT"){ // 30 min Idle Timeout
        printerVariables.previous_gcodeState = printerVariables.gcodeState;
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Idle Timeout over 30min, Turning Leds OFF"));
        };
        return;
    }
    
    //RED
    if (printerConfig.errordetection == true){ // allow errordetection to turn ledstrip red
        if (printerVariables.parsedHMS == "Serious"){
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("HMS Severity, SERIOUS PROBLEM, Turning Leds RED"));
            };
            return;
        };

        if (printerVariables.parsedHMS == "Fatal"){
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("HMS Severity, FATAL PROBLEM, Turning Leds RED"));
            };
            return;
        };

        if (printerVariables.stage == 6 && printerVariables.previous_stage != printerVariables.stage){ //Fillament runout
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            printerVariables.previous_stage = printerVariables.stage;
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 17, FILAMENT RUNOUT, Turning Leds RED"));
            };
            return;
        };

        if (printerVariables.stage == 17 && printerVariables.previous_stage != printerVariables.stage){ //Front Cover Removed
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 17, FRONT COVER REMOVED, Turning Leds RED"));
            };
            return;
        };

        if (printerVariables.stage == 20 && printerVariables.previous_stage != printerVariables.stage){ //Nozzle Temp fail
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 20, NOZZLE TEMP FAIL, Turning Leds RED"));
            };
            return;
        };

        if (printerVariables.stage == 21 && printerVariables.previous_stage != printerVariables.stage){ //Bed Temp Fail
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 21, BED TEMP FAIL, Turning Leds RED"));
            };
            return;
        };
    };

    //GREEN
    //Sets to green when print finishes
    //Stays green until 5mins has passed AND door is opened
    if (printerVariables.gcodeState == "FINISH" && printerVariables.previous_gcodeState != printerVariables.gcodeState 
    && printerConfig.finishindication == true && printerVariables.previous_gcodeState != "TIMEOUT"){
        tweenToColor(0,255,0,0,0,LEDTranistion_Duration); //GREEN
        printerVariables.previous_gcodeState = printerVariables.gcodeState;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Finished print, Turning Leds GREEN"));
        };
        return;
    }

    //BLUE 
     if ((printerVariables.stage == 6 || printerVariables.stage == 30) || printerVariables.gcodeState == "PAUSE" 
     && printerVariables.previous_gcodeState != printerVariables.gcodeState){// Pause
        tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
        printerVariables.previous_gcodeState = printerVariables.gcodeState;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage "));
            Serial.print(printerVariables.stage);
            Serial.println(F(", gcodeState PAUSE, Turning Leds BLUE"));
        };
        return;
    }
    if (printerVariables.stage == 34 && printerVariables.previous_stage != printerVariables.stage){ //First Layer Error PAUSED
        tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 34, FIRST LAYER ERROR, PAUSED Turning Leds BLUE"));
        };
        return;
    }
    if (printerVariables.stage == 35 && printerVariables.previous_stage != printerVariables.stage){ //Nozzle Clog PAUSED
        tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 35, NOZZLE CLOG, PAUSED Turning Leds BLUE"));
        };
        return;
    }

    //ON
    if (printerVariables.stage == 2 && printerVariables.previous_stage != printerVariables.stage){ //Offline
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.previous_stage = printerVariables.stage;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 2, PREHEATING BED, Turning Leds WHITE"));
        };
        return;
    }

    if (printerVariables.stage == 0  && printerVariables.gcodeState == "RUNNING" && printerVariables.previous_stage != printerVariables.stage ){ //Printing or Resume after Pausing
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.previous_gcodeState = printerVariables.gcodeState;
        printerVariables.previous_stage = printerVariables.stage;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage 0, PRINTING - gcodeState RUNNING, Turning Leds WHITE"));
        };
        return;
    }

     
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) && printerVariables.finishstartms == 0 
    && (millis() - printerVariables.idleStartms < 1800000) && printerVariables.previous_stage != printerVariables.stage
    && printerVariables.previous_gcodeState != "TIMEOUT"){ // Idle 
        //for IDLE - P1 uses 255, X1 uses -1
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.previous_gcodeState = "TIMEOUT";
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage "));
            Serial.print(printerVariables.stage);
            Serial.println(F(", IDLE Turning Leds WHITE"));
        };
        return;
    }

    if (printerVariables.stage == -2 && printerVariables.previous_stage != printerVariables.stage){ //Offline
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.previous_stage = printerVariables.stage;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage -2, Turning Leds WHITE"));
        };
        return;
    }

    if (printerVariables.initalisedLEDs == false) { //Initial Boot
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.initalisedLEDs = true;
        printerVariables.idleStartms = millis();
        printerVariables.finishstartms = 0;
        Serial.println(F("Initial BLLED bootup - Turning Leds WHITE"));
        return;
    }
}


void setupLeds() {
    Serial.println(F("Updating from setupleds"));
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(coldPin, OUTPUT);
    pinMode(warmPin, OUTPUT);
}

void ledsloop(){
    RGBCycle();

    // Turn off GREEN if... HAS a finished and Door OPENED or CLOSED in last 5 secs 
    // Can't use Boolean finished as printer defaults back to IDLE via MQTT
    if((printerVariables.finishstartms > 0  && ((millis() - printerVariables.lastdoorClosems) < 6000 || (millis() - printerVariables.lastdoorOpenms) < 6000))){
        tweenToColor(0,0,0,255,255,500); //WHITE
        Serial.println(F("Updating from finishloop after Door interaction - Starting IDLE timer"));
        printerVariables.finishstartms = 0;
        printerVariables.previous_gcodeState = "TIMEOUT";
        printerVariables.idleStartms = millis();
        updateleds();
    }
    if((millis() - printerVariables.idleStartms) > 1800000)
    {
        //30mins idle timer kicks in and will turn off the LEDS.
        //Opening or Closing the Door will turn it back on and restart the timer.
        updateleds();
    }
    delay(10);
}

#endif

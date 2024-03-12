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

void updateleds(){
    
    // Adding to top so it can be changed in one location
    int LEDTranistion_Duration = 500; 

    if (printerConfig.debuging == true){
        Serial.println(F("Updating leds"));

        Serial.println(printerVariables.stage);
        Serial.println(printerVariables.gcodeState);
        Serial.println(printerVariables.ledstate);
        Serial.println(printerVariables.hmsstate);
        Serial.println(printerVariables.parsedHMS);
    }
    
    //Use LED to show WIFI Strength (enabled via Setup Webpage)
    if (printerConfig.debugwifi == true){  
        //<=-50 dBm Green, <= -60 dBm LightGreen, <= -70 dBm Yellow, <= -80 dBm Orange, >80 Red
        if (WiFi.status() == WL_CONNECTED){
            long wifiNow = WiFi.RSSI();
            if (printerConfig.debugingchange){
                Serial.print(F("WiFi Stregth Visialisation, turning LEDs On: "));
                Serial.println(wifiNow);
            }
            if (wifiNow >= -50) tweenToColor(0,255,0,0,0,LEDTranistion_Duration); //GREEN
            else if (wifiNow >= -60) tweenToColor(128,255,0,0,0,LEDTranistion_Duration); //LIGHTGREEN
            else if (wifiNow >= -70) tweenToColor(255,255,0,0,0,LEDTranistion_Duration); //YELLOW
            else if (wifiNow >= -80) tweenToColor(255,128,0,0,0,LEDTranistion_Duration); //ORANGE
            else if (wifiNow < -80) tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            else tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
            return;
        }
    }

    //Override Selected LED ON regardless of state (via webpage)
    if (printerVariables.overrideLEDstate == true){  
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
        if (printerConfig.debugingchange){
            Serial.print(F("LED Test Override ON, Turning Leds Custom On: "));
            Serial.print(r_LED);
            Serial.print(F(" "));
            Serial.print(g_LED);
            Serial.print(F(" "));
            Serial.print(b_LED);
            Serial.print(F(" "));
            Serial.print(cw_LED);
            Serial.print(F(" "));
            Serial.print(ww_LED);
            Serial.print(F(" Brightness: "));
            Serial.println(printerConfig.brightness);
        };
        return;
    }    


    //TOGGLE LIGHTS via DOOR
    //If door is closed twice in 6 seconds, it will flip the state of the lights
    if (printerVariables.doorSwitchenabled == true){
        if (printerConfig.debugingchange){
            Serial.print(F("Door closed twice within 6 seconds - Toggling lights to "));
        }
        if(currentWarm == 0 && currentCold == 0)
        {
            tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
            if (printerConfig.debuging || printerConfig.debugingchange) {
                Serial.println(F("ON"));
            }
        }
        else
        {
            tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
            //Force into idle state - note will change back immediately if there is an update of any sort
            printerVariables.idleLightsOff = true;
            printerVariables.idleStartms = -printerVariables.idleLightoffTimeOut;
            if (printerConfig.debuging || printerConfig.debugingchange) {
                Serial.println(F("OFF"));
            }
        }
        printerVariables.doorSwitchenabled = false;
    }

    //OFF -- OFF -- OFF -- OFF

    if (printerConfig.discoMode == true){
        //Skip all the following code that sets specific colors if RGB Mode is on - done in RGBCycle()
        return;
    }

    //printer offline and MQTT disconnect more than 5 seconds.
    if (printerVariables.online == false && (millis() - printerVariables.disconnectMQTTms) >= 5000){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Printer offline, Turning Leds OFF"));
        };
        return;
    }

    // replicate printer behaviour
    if (printerVariables.ledstate == false && printerConfig.replicatestate == true){
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("LED Replication, Turning Leds OFF"));
        };
        return;
    }

    //Cleaning nozzle
    if (printerVariables.stage == 14){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 14, CLEANING NOZZLE, Turning Leds OFF"));
        };
        return;
    }

    //Auto Bed Leveling
    if (printerVariables.stage == 1){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 1, BED LEVELING, Turning Leds OFF"));
        };
        return;
    }

    //Calibrating Extrusion
    if (printerVariables.stage == 8){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 8, CALIBRATING EXTRUSION, Turning Leds OFF"));
        };
        return;
    }

    //Scaning surface
    if (printerVariables.stage == 9){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 9, SCANNING BED SURFACE, Turning Leds OFF"));
        };
        return;
    }

    //Inspecting First Layer
    if (printerVariables.stage == 10){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 10, FIRST LAYER INSPECTION, Turning Leds OFF"));
        };
        return;
    }

    //Calibrating  MicroLidar
    if (printerVariables.stage == 12){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 12, CALIBRATING MICRO LIDAR, Turning Leds OFF"));
        };
        return;
    }
    
    // 30 min Idle Timeout
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) 
    && printerVariables.finishstartms == 0 && (millis() - printerVariables.idleStartms) > printerVariables.idleLightoffTimeOut && printerVariables.idleLightsOff == false){ 
        tweenToColor(0,0,0,0,0,LEDTranistion_Duration); //OFF
        printerVariables.idleLightsOff = true;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Idle Timeout over 30min, Turning Leds OFF"));
        };
        return;
    }
    
    //RED -- RED -- RED -- RED

    // allow errordetection to turn ledstrip red
    if (printerConfig.errordetection == true){ 

        //SERIOUS HMS state
        if (printerVariables.parsedHMS == "Serious"){
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("HMS Severity, SERIOUS PROBLEM, Turning Leds RED"));
            };
            return;
        };

        //FATAL HMS state
        if (printerVariables.parsedHMS == "Fatal"){
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("HMS Severity, FATAL PROBLEM, Turning Leds RED"));
            };
            return;
        };

        //Fillament runout
        if (printerVariables.stage == 6){ 
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 17, FILAMENT RUNOUT, Turning Leds RED"));
            };
            return;
        };

        //Front Cover Removed
        if (printerVariables.stage == 17){ 
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 17, FRONT COVER REMOVED, Turning Leds RED"));
            };
            return;
        };

        //Nozzle Temp fail
        if (printerVariables.stage == 20){ 
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 20, NOZZLE TEMP FAIL, Turning Leds RED"));
            };
            return;
        };

        //Bed Temp Fail
        if (printerVariables.stage == 21){ 
            tweenToColor(255,0,0,0,0,LEDTranistion_Duration); //RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.println(F("Stage 21, BED TEMP FAIL, Turning Leds RED"));
            };
            return;
        };
    };

    //BLUE -- BLUE -- BLUE -- BLUE
    
    // Pause (by user or via Gcode)
     if ((printerVariables.stage == 6 || printerVariables.stage == 30) || printerVariables.gcodeState == "PAUSE"){
        tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage "));
            Serial.print(printerVariables.stage);
            Serial.println(F(", gcodeState PAUSE, Turning Leds BLUE"));
        };
        return;
    }

    //First Layer Error PAUSED
    if (printerVariables.stage == 34){ 
        tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 34, FIRST LAYER ERROR, PAUSED Turning Leds BLUE"));
        };
        return;
    }

    //Nozzle Clog PAUSED
    if (printerVariables.stage == 35){ 
        tweenToColor(0,0,255,0,0,LEDTranistion_Duration); //BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 35, NOZZLE CLOG, PAUSED Turning Leds BLUE"));
        };
        return;
    }

    //ON -- ON -- ON -- ON 

    //Initial Boot
    if (printerVariables.initalisedLEDs == false) {     
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.initalisedLEDs = true;
        printerVariables.idleStartms = millis();
        printerVariables.finishstartms = 0;
        printerVariables.lastdoorClosems = millis();
        Serial.println(F("Initial BLLED bootup - Turning Leds WHITE, Start IDLE timer"));
        return;
    }

    //Preheating Bed
    if (printerVariables.stage == 2){ 
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.idleStartms = 0;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 2, PREHEATING BED, Turning Leds WHITE"));
        };
        return;
    }

    //Printing or Resume after Pausing
    if (printerVariables.stage == 0  && printerVariables.gcodeState == "RUNNING"){ 
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        printerVariables.idleStartms = 0;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 0, PRINTING - gcodeState RUNNING, Turning Leds WHITE"));
        };
        return;
    }

    //for IDLE - P1 uses 255, X1 uses -1
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) 
    && printerVariables.finishstartms == 0  && (millis() - printerVariables.idleStartms < printerVariables.idleLightoffTimeOut)){ // Idle 
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage "));
            Serial.print(printerVariables.stage);
            Serial.print(F(", IDLE Turning Leds WHITE. Brightness: "));
            Serial.println(printerConfig.brightness);
        };
        return;
    }

    //Offline (not sure what gives this value)
    if (printerVariables.stage == -2){ 
        tweenToColor(0,0,0,255,255,LEDTranistion_Duration); //WHITE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage -2, Turning Leds WHITE"));
        };
        return;
    }

    //GREEN -- GREEN -- GREEN -- GREEN

    //Sets to green when print finishes
    if (printerVariables.finished == true && printerVariables.gcodeState == "FINISH" && printerConfig.finishindication == true){
        tweenToColor(0,255,0,0,0,LEDTranistion_Duration); //GREEN
        printerVariables.finished = false;
        //Finish timer continues until interaction
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Finished print, Turning Leds GREEN"));
        };
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

    // Turn off GREEN if... finished and Door OPENED or CLOSED in last 5 secs 
    // Can't use Boolean finished as printer defaults back to IDLE via MQTT
    if((printerVariables.finishstartms > 0  && ((millis() - printerVariables.lastdoorClosems) < 6000 || (millis() - printerVariables.lastdoorOpenms) < 6000))){
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Updating from finishloop after Door interaction - Starting IDLE timer"));
        };
        printerVariables.finishstartms = 0;
        printerVariables.idleStartms = millis();
        updateleds();
    }

    //Need an trigger action to run updateleds() so lights turn off  
    //There is no change in the printer STATE, monitoring the timer and triggering when over a threshhold
    if((millis() - printerVariables.idleStartms) > printerVariables.idleLightoffTimeOut && printerVariables.finished == false && printerVariables.idleLightsOff == false)
    {
        //Opening or Closing the Door will turn LEDs back on and restart the timer.
        updateleds();
    }
    delay(10);
}

#endif

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

struct COLOR {
    short r;
    short g;
    short b;
};

COLOR hex2rgb(String hex) {
    COLOR color;
    long hexcolor;

    if(hex.charAt(0) == '#') {
        hex.remove(0,1);
    }
    while(hex.length() != 6) {
        hex += "0";
    }
    hexcolor = strtol(hex.c_str(), NULL, 16);
    color.r = (hexcolor & 0xFF0000) >> 16;
    color.g = (hexcolor & 0x00FF00) >> 8;
    color.b =  (hexcolor & 0x0000FF);
    return color;
}

void tweenToColor(int targetRed, int targetGreen, int targetBlue, int targetWarm, int targetCold, int duration = 500) {

    float brightness = (float)printerConfig.brightness/100.0;

    int brightenedRed = round(targetRed * brightness);
    int brightenedGreen = round(targetGreen * brightness);
    int brightenedBlue = round(targetBlue * brightness);
    int brightenedWarm = round(targetWarm * brightness);
    int brightenedCold = round(targetCold * brightness);

    if (brightenedRed == currentRed && brightenedGreen == currentGreen && brightenedBlue == currentBlue && brightenedWarm == currentWarm && brightenedCold == currentCold){
        // Already set to the requested color
        if (printerConfig.debuging){
            Serial.print(F("LEDS already at color: ("));
            Serial.print(currentRed);
            Serial.print(F(", "));
            Serial.print(currentGreen);
            Serial.print(F(", "));
            Serial.print(currentBlue);
            Serial.print(F(", "));
            Serial.print(currentWarm);
            Serial.print(F(", "));
            Serial.print(currentCold);
            Serial.print(F(" Brightness: "));
            Serial.print(printerConfig.brightness);
            Serial.println(F(")"));
        };
        return;
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
}

//Helper functions to allow changes colors by using a string or integer hex code
//Example:  tweenToColor("#33FD67")
void tweenToColor(String strTargetColor) {
    COLOR targetcolor;
    targetcolor = hex2rgb(strTargetColor);
    tweenToColor(targetcolor.r, targetcolor.g, targetcolor.b, 0, 0);
}
//Example:  tweenToColor(0xFFACA5)
void tweenToColor(int hexValue) {
    COLOR targetcolor;
    targetcolor.r = ((hexValue >> 16) & 0xFF) / 255.0;
    targetcolor.g = ((hexValue >> 8) & 0xFF) / 255.0;
    targetcolor.b = ((hexValue) & 0xFF) / 255.0;
    tweenToColor(targetcolor.r, targetcolor.g, targetcolor.b, 0, 0);
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

    //Adjust to set brightness level
    float brightness = (float)printerConfig.brightness/100.0;

    int brightenedRed = round(currentRed * brightness);
    int brightenedGreen = round(currentGreen * brightness);
    int brightenedBlue = round(currentBlue * brightness);
    int brightenedWarm = round(currentWarm * brightness);
    int brightenedCold = round(currentCold * brightness);

    analogWrite(redPin, currentRed);
    analogWrite(greenPin, currentGreen);
    analogWrite(bluePin, currentBlue);
    analogWrite(warmPin, currentWarm);
    analogWrite(coldPin, currentCold);
}



void idleStatus(){
    if (printerConfig.debuging || printerConfig.debugingchange){
        Serial.print(F("Stage "));
        Serial.print(printerVariables.stage);
        Serial.print(F(", IDLE Turning LEDs to WHITE. Brightness: "));
        Serial.println(printerConfig.brightness);
    };
}



void updateleds(){
    //Maintenance Mode - White lights on regardless of printer power, WiFi or MQTT connection 
    //priortised over Wifi Strength Display or Custom TEST color
    if (printerConfig.maintMode && printerConfig.updateMaintenance){  
        tweenToColor(0,0,0,255,255); //WHITE
        printerConfig.updateMaintenance = false;
        if (printerConfig.debugingchange){
            Serial.println(F("Maintenance Mode, turning LEDs to WHITE"));
        };
        return;
    }

    //Use LED to show WIFI Strength (enabled via Setup Webpage, priortised over Custom TEST color)
    if (printerConfig.debugwifi == true && !printerConfig.maintMode){  
        //<=-50 dBm Green, <= -60 dBm LightGreen, <= -70 dBm Yellow, <= -80 dBm Orange, >80 Red
        if (WiFi.status() == WL_CONNECTED){
            long wifiNow = WiFi.RSSI();
            if (printerConfig.debugingchange){
                Serial.print(F("WiFi Strength Visialisation, display LEDs for: "));
                Serial.println(wifiNow);
            }
            if (wifiNow >= -50) tweenToColor(0,255,0,0,0); //GREEN
            else if (wifiNow >= -60) tweenToColor(128,255,0,0,0); //LIGHTGREEN
            else if (wifiNow >= -70) tweenToColor(255,255,0,0,0); //YELLOW
            else if (wifiNow >= -80) tweenToColor(255,128,0,0,0); //ORANGE
            else if (wifiNow < -80) tweenToColor(255,0,0,0,0); //RED
            else tweenToColor(0,0,255,0,0); //BLUE
        };
        return;
    }

    //TEST Color Enabled - LED ON regardless of printer state
    if (printerVariables.testcolorEnabled && !printerConfig.maintMode && !printerConfig.debugwifi){
        COLOR customColor = hex2rgb(printerConfig.testRGB);

        tweenToColor(customColor.r,customColor.g,customColor.b,printerConfig.testwarmwhite,printerConfig.testcoldwhite); //Variable Test Color
        if (printerConfig.debugingchange){
            Serial.print(F("LED Test ON, Changing LEDs to selected color: "));
            Serial.print(customColor.r);
            Serial.print(F(" "));
            Serial.print(customColor.g);
            Serial.print(F(" "));
            Serial.print(customColor.b);
            Serial.print(F(" "));
            Serial.print(printerConfig.testwarmwhite);
            Serial.print(F(" "));
            Serial.print(printerConfig.testcoldwhite);
            Serial.print(F(" Brightness: "));
            Serial.println(printerConfig.brightness);
        };
        printerConfig.updateTestLEDS = false;
        return;
    }    

    if(printerVariables.testcolorEnabled || printerConfig.maintMode || printerConfig.debugwifi){
        //Skip trying to set a color as it's in one of the override states
        return;
    }

    //From here the BBLP status sets the colors
    if (printerConfig.debuging == true){
        Serial.println(F("Updating LEDs"));

        Serial.println(printerVariables.stage);
        Serial.println(printerVariables.gcodeState);
        Serial.println(printerVariables.printerledstate);
        Serial.println(printerVariables.hmsstate);
        Serial.println(printerVariables.parsedHMS);
    }

    //TOGGLE LIGHTS via DOOR
    //If door is closed twice in 6 seconds, it will flip the state of the lights
    if (printerVariables.doorSwitchenabled == true){
        if (printerConfig.debugingchange){
            Serial.print(F("Door closed twice within 6 seconds - Toggling LEDs to "));
        }
        if(currentWarm == 0 && currentCold == 0)
        {
            tweenToColor(0,0,0,255,255); //WHITE
            if (printerConfig.debuging || printerConfig.debugingchange) {
                Serial.println(F("ON"));
            }
        }
        else
        {
            tweenToColor(0,0,0,0,0); //OFF
            //Shortcut to idle state - note: light will go back on immediately if there is an MQTT change of any sort
            printerVariables.inactivityLightsOff = true;
            printerVariables.inactivityStartms = millis()-printerConfig.inactivityTimeOut;
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
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Printer offline, Turning LEDs OFF"));
        };
        return;
    }

    // replicate printer behaviour
    if (printerVariables.printerledstate == false && printerConfig.replicatestate == true){
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("LED Replication, Turning LEDs OFF"));
        };
        return;
    }

    //Cleaning nozzle
    if (printerVariables.stage == 14 && printerConfig.lidarLightsOff){ 
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 14, CLEANING NOZZLE, Turning LEDs OFF"));
        };
        return;
    }

    //Auto Bed Leveling
    if (printerVariables.stage == 1 && printerConfig.lidarLightsOff){ 
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 1, BED LEVELING, Turning LEDs OFF"));
        };
        return;
    }

    //Calibrating Extrusion
    if (printerVariables.stage == 8 && printerConfig.lidarLightsOff){ 
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 8, CALIBRATING EXTRUSION, Turning LEDs OFF"));
        };
        return;
    }

    //Scaning surface
    if (printerVariables.stage == 9 && printerConfig.lidarLightsOff){ 
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 9, SCANNING BED SURFACE, Turning LEDs OFF"));
        };
        return;
    }

    //Inspecting First Layer
    if ((printerVariables.stage == 10 && printerConfig.lidarLightsOff)
    ||  (printerVariables.inspectingFirstLayer && printerConfig.lidarLightsOff)){ 
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 10 / HMS 0C00, FIRST LAYER INSPECTION, Turning LEDs OFF"));
        };
        return;
    }

    //Calibrating  MicroLidar
    if (printerVariables.stage == 12 && printerConfig.lidarLightsOff){ 
        tweenToColor(0,0,0,0,0); //OFF
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 12, CALIBRATING MICRO LIDAR, Turning LEDs OFF"));
        };
        return;
    }

    // Idle Timeout (Has to be enabled)
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) 
    && !printerVariables.waitingForDoor 
    && (millis() - printerVariables.inactivityStartms) > printerConfig.inactivityTimeOut 
    && printerVariables.inactivityLightsOff == false
    && printerConfig.inactivityEnabled){ 
        tweenToColor(0,0,0,0,0); //OFF
        printerVariables.inactivityLightsOff = true;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Idle Timeout ["));
            Serial.print((int)(printerConfig.inactivityTimeOut / 60000));
            Serial.println(F(" mins] - Turning LEDs OFF"));
        };
        return;
    }
    
    //RED -- RED -- RED -- RED

    // allow errordetection to turn ledstrip red
    if (printerConfig.errordetection == true){ 

        //SERIOUS HMS state
        if (printerVariables.parsedHMS == "Serious"){
            tweenToColor(printerConfig.hmsSeriousRGB); //Customisable - Default is RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("HMS Severity, SERIOUS PROBLEM, Turning LEDs to "));
                Serial.println(printerConfig.hmsSeriousRGB);
            };
            return;
        };

        //FATAL HMS state
        if (printerVariables.parsedHMS == "Fatal"){
            tweenToColor(printerConfig.hmsFatalRGB); //Customisable - Default is RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("HMS Severity, FATAL PROBLEM, Turning LEDs to "));
                Serial.println(printerConfig.hmsFatalRGB);
            };
            return;
        };

        //Fillament runout
        if (printerVariables.stage == 6){ 
            tweenToColor(printerConfig.filamentRunoutRGB); //Customisable - Default is RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("Stage 17, FILAMENT RUNOUT, Turning LEDs to "));
                Serial.println(printerConfig.filamentRunoutRGB);
            };
            return;
        };

        //Front Cover Removed
        if (printerVariables.stage == 17){ 
            tweenToColor(printerConfig.frontCoverRGB); //Customisable - Default is RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("Stage 17, FRONT COVER REMOVED, Turning LEDs to "));
                Serial.println(printerConfig.frontCoverRGB);
            };
            return;
        };

        //Nozzle Temp fail
        if (printerVariables.stage == 20){ 
            tweenToColor(printerConfig.nozzleTempRGB); //Customisable - Default is RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("Stage 20, NOZZLE TEMP FAIL, Turning LEDs to "));
                Serial.println(printerConfig.nozzleTempRGB);
            };
            return;
        };

        //Bed Temp Fail
        if (printerVariables.stage == 21){ 
            tweenToColor(printerConfig.bedTempRGB); //Customisable - Default is RED
            if (printerConfig.debuging || printerConfig.debugingchange){
                Serial.print(F("Stage 21, BED TEMP FAIL, Turning LEDs to "));
                Serial.println(printerConfig.bedTempRGB);
            };
            return;
        };
    };

    //BLUE -- BLUE -- BLUE -- BLUE
    
    // Pause (by user or via Gcode)
     if ((printerVariables.stage == 6 || printerVariables.stage == 30) || printerVariables.gcodeState == "PAUSE"){
        tweenToColor(printerConfig.pauseRGB); //Customisable - Default is BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage "));
            Serial.print(printerVariables.stage);
            Serial.print(F(", gcodeState PAUSE, Turning LEDs to "));
            Serial.println(printerConfig.pauseRGB);
        };
        return;
    }

    //First Layer Error PAUSED
    if (printerVariables.stage == 34){ 
        tweenToColor(printerConfig.firstlayerRGB); //Customisable - Default is BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage 34, FIRST LAYER ERROR, PAUSED Turning LEDs to "));
            Serial.println(printerConfig.firstlayerRGB);
        };
        return;
    }

    //Nozzle Clog PAUSED
    if (printerVariables.stage == 35){ 
        tweenToColor(printerConfig.nozzleclogRGB); //Customisable - Default is BLUE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage 35, NOZZLE CLOG, PAUSED Turning LEDs to "));
            Serial.println(printerConfig.nozzleclogRGB);
        };
        return;
    }

    //ON -- ON -- ON -- ON 

    //Initial Boot
    if (printerVariables.initalisedLEDs == false) {     
        tweenToColor(0,0,0,255,255); //WHITE
        printerVariables.initalisedLEDs = true;
        printerVariables.inactivityStartms = millis();
        printerVariables.waitingForDoor = false;
        printerVariables.lastdoorClosems = millis();
        Serial.println(F("Initial BLLED bootup - Turning LEDs to WHITE, Start IDLE timer"));
        return;
    }

    //Preheating Bed
    if (printerVariables.stage == 2){ 
        tweenToColor(0,0,0,255,255); //WHITE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Stage 2, PREHEATING BED, Turning LEDs to WHITE"));
        };
        return;
    }

    //Printing or Resume after Pausing
    if (printerVariables.stage == 0  && printerVariables.gcodeState == "RUNNING"){ 
        tweenToColor(0,0,0,255,255); //WHITE
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Stage 0, PRINTING - gcodeState RUNNING, Turning LEDs to WHITE. Brightness: "));
            Serial.println(printerConfig.brightness);
        };
        return;
    }

    //for IDLE - P1 uses 255, X1 uses -1
    //This covers both IDLE & FINISH 
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) 
    && printerVariables.waitingForDoor == false  
    && (millis() - printerVariables.inactivityStartms < printerConfig.inactivityTimeOut)){ 
        tweenToColor(0,0,0,255,255); //WHITE
        idleStatus();
        return;
    }

    //User Cancelled Print
    if (printerVariables.gcodeState == "FAILED"){ 
        tweenToColor(0,0,0,255,255); //WHITE
        idleStatus();
        return;
    }
    //Print file just sent
    if (printerVariables.gcodeState == "PREPARE"){ 
        tweenToColor(0,0,0,255,255); //WHITE
        idleStatus();
        return;
    }

    //Offline
    if (printerVariables.gcodeState == "OFFLINE" || printerVariables.stage == -2){ 
        tweenToColor(0,0,0,255,255); //WHITE
        idleStatus();
        return;
    }

    //GREEN -- GREEN -- GREEN -- GREEN

    //Sets to green when print finishes AND user wants Finish Indication enabled
    if (printerVariables.finished == true && printerConfig.finishindication == true){
        tweenToColor(printerConfig.finishRGB); //Customisable - Default is GREEN
        printerVariables.finished = false;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Finished print, Turning LEDs to "));
            Serial.println(printerConfig.finishRGB);
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

    // Turn off GREEN if... finished and Door OPENED or CLOSED in last 5 secs AND user wants Finish Indication enabled
    // Can't use Boolean finished as printer defaults back to IDLE via MQTT
    if((printerVariables.waitingForDoor && printerConfig.finishindication == true
    && ((millis() - printerVariables.lastdoorClosems) < 6000 || (millis() - printerVariables.lastdoorOpenms) < 6000))){
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Updating from finishloop after Door interaction - Starting IDLE timer"));
        };
        printerVariables.waitingForDoor = false;
        printerVariables.inactivityStartms = millis();
        updateleds();
    }

    //Need an trigger action to run updateleds() so lights turn off  
    //There is no change in the printer STATE, just monitoring the timer and triggering when over a threshhold
    if(printerConfig.inactivityEnabled && (millis() - printerVariables.inactivityStartms) > printerConfig.inactivityTimeOut 
    && printerVariables.finished == false && printerVariables.inactivityLightsOff == false)
    {
        //Opening or Closing the Door will turn LEDs back on and restart the timer.
        updateleds();
    }
    delay(10);
}

#endif

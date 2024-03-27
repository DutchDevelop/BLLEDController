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

unsigned long lastUpdatems = 0;
unsigned long oldms = 0;

COLOR hex2rgb(String hex, short ww_value = 0, short cw_value = 0) {
    COLOR color;
    long hexcolor;
    strcpy(color.RGBhex, hex.c_str());
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
    color.ww = ww_value;
    color.cw = cw_value;
    
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
void tweenToColor(COLOR targetcolor) {
    tweenToColor(targetcolor.r, targetcolor.g, targetcolor.b, targetcolor.ww, targetcolor.cw);
}
//Example:  tweenToColor("#33FD67")
void tweenToColor(String strTargetColor, short ww_value = 0, short cw_value = 0) {
    COLOR targetcolor;
    targetcolor = hex2rgb(strTargetColor, ww_value, cw_value);
    tweenToColor(targetcolor.r, targetcolor.g, targetcolor.b, targetcolor.ww, targetcolor.cw);
}
//Example:  tweenToColor(0xFFACA5)
void tweenToColor(int hexValue, short ww_value = 0, short cw_value = 0) {
    COLOR targetcolor;
    targetcolor.r = ((hexValue >> 16) & 0xFF) / 255.0;
    targetcolor.g = ((hexValue >> 8) & 0xFF) / 255.0;
    targetcolor.b = ((hexValue) & 0xFF) / 255.0;
    targetcolor.ww = ww_value;
    targetcolor.cw = cw_value;
    tweenToColor(targetcolor.r, targetcolor.g, targetcolor.b, targetcolor.ww, targetcolor.cw);
}

float hue = 0.0;

void RGBCycle() {
    if (printerConfig.discoMode == false) {
        //Skip changing colors if RGB Mode not enabled
        printerConfig.discoMode_update = true;
        return;
    }
    if(printerConfig.discoMode_update){
        printerConfig.discoMode_update = false;
        if (printerConfig.debugingchange){
            Serial.print(F("["));
            Serial.print(millis());
            Serial.print(F("]"));
            Serial.println(F(" ** RGB Cycle Mode **"));
        };
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

    //Need to add code so it adjusts to set brightness level

    analogWrite(redPin, currentRed);
    analogWrite(greenPin, currentGreen);
    analogWrite(bluePin, currentBlue);
    analogWrite(warmPin, currentWarm);
    analogWrite(coldPin, currentCold);
}

void printLogs(String Desc, COLOR thisColor){
    if (printerConfig.debuging || printerConfig.debugingchange){
        Serial.print(Desc);
        Serial.print(F(" - Turning LEDs to: "));
        if((thisColor.r + thisColor.g + thisColor.b + thisColor.ww + thisColor.cw) == 0){
            Serial.println(F(" OFF"));
            return;
        }
        Serial.print(F(" r:"));
        Serial.print(thisColor.r);
        Serial.print(F(" g:"));
        Serial.print(thisColor.g);
        Serial.print(F(" b:"));
        Serial.print(thisColor.b);
        Serial.print(F(" ww:"));
        Serial.print(thisColor.ww);
        Serial.print(F(" cw:"));
        Serial.print(thisColor.cw);
        Serial.print(F(" Brightness: "));
        Serial.println(printerConfig.brightness);
    };
}
void printLogs(String Desc, short r, short g, short b, short ww, short cw){
    COLOR tempColor;
    tempColor.r = r;
    tempColor.g = g;
    tempColor.b = b;
    tempColor.ww = ww;
    tempColor.cw = cw;
    printLogs(Desc, tempColor);
}
void updateleds(){
    //Maintenance Mode - White lights on regardless of printer power, WiFi or MQTT connection 
    //priortised over Wifi Strength Display or Custom TEST color
    if (printerConfig.maintMode && printerConfig.maintMode_update){  
        tweenToColor(0,0,0,255,255); //WHITE
        printerConfig.maintMode_update = false;
        printLogs("Maintenance Mode", 0,0,0,255,255);
        Serial.print(F("["));
        Serial.print(millis());
        Serial.print(F("]"));
        Serial.println(F(" ** Maintenance Mode **"));
        return;
    }

    //Use LED to show WIFI Strength (enabled via Setup Webpage)
    if (printerConfig.debugwifi){  
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
    if (printerConfig.testcolorEnabled && printerConfig.testcolor_update){
        tweenToColor(printerConfig.testColor); //Variable Test Color
        printLogs("LED Test ON", printerConfig.testColor);
        Serial.print(F("["));
        Serial.print(millis());
        Serial.print(F("]"));
        Serial.println(F(" ** Test Color Mode **"));
        printerConfig.testcolor_update = false;
        return;
    }

    //From here the BBLP status sets the colors
    if (printerConfig.debuging == true){
        Serial.println(F("Updating LEDs"));

        Serial.println(printerVariables.stage);
        Serial.println(printerVariables.gcodeState);
        Serial.println(printerVariables.printerledstate);
        Serial.println(printerVariables.hmsstate);
        Serial.println(printerVariables.parsedHMSlevel);
    }

    //Initial Boot
    if (printerVariables.initalisedLEDs == false) {     
        printerVariables.initalisedLEDs = true; //Run once per boot
        printerConfig.inactivityStartms = millis();  //restart idle timer
        printerConfig.isIdleOFFActive = false;
        printerVariables.waitingForDoor = false;
        printerConfig.finish_check = false;
        printerVariables.lastdoorClosems = millis();
        Serial.println(F("Initial Boot"));
        return;
    }

    if(printerConfig.testcolorEnabled || printerConfig.maintMode || printerConfig.debugwifi || printerConfig.discoMode){
        //Skip trying to set a color as it's in one of the override states
        return;
    }

    //TOGGLE LIGHTS via DOOR
    //If door is closed twice in 6 seconds, it will flip the state of the lights
    if (printerVariables.doorSwitchTriggered == true){
        if (printerConfig.debugingchange){
            Serial.print(F("Door closed twice within 6 seconds - Toggling LEDs to "));
        }
        if(currentWarm == 0 && currentCold == 0)
        {
            tweenToColor(0,0,0,255,255); //WHITE
            if (printerConfig.debuging || printerConfig.debugingchange) {
                Serial.println(F("ON"));
            }
            printerConfig.isIdleOFFActive = false;
        }
        else
        {
            tweenToColor(0,0,0,0,0); //OFF
            //Shortcut to idle state - note: light will go back on immediately if there is an MQTT change of any sort
            printerConfig.isIdleOFFActive = true;
            printerConfig.inactivityStartms = (millis()-printerConfig.inactivityTimeOut);
            if (printerConfig.debuging || printerConfig.debugingchange) {
                Serial.println(F("OFF"));
            }
        }
        printerVariables.doorSwitchTriggered = false;
        return;
    }
    
    //RED -- RED -- RED -- RED

    // allow errordetection to turn ledstrip red
    if (printerConfig.errordetection == true){ 

        //Fillament runout
        if (printerVariables.stage == 6 || printerVariables.overridestage == 6){ 
            tweenToColor(printerConfig.filamentRunoutRGB); //Customisable - Default is RED
            printLogs("Stage 6, FILAMENT RUNOUT", printerConfig.filamentRunoutRGB);
            return;
        };

        //Front Cover Removed
        if (printerVariables.stage == 17 || printerVariables.overridestage == 17){ 
            tweenToColor(printerConfig.frontCoverRGB); //Customisable - Default is RED
            printLogs("Stage 17, FRONT COVER REMOVED", printerConfig.frontCoverRGB);
            return;
        };

        //Nozzle Temp fail
        if (printerVariables.stage == 20 || printerVariables.overridestage == 20){ 
            tweenToColor(printerConfig.nozzleTempRGB); //Customisable - Default is RED
            printLogs("Stage 20, NOZZLE TEMP FAIL", printerConfig.nozzleTempRGB);
            return;
        };

        //Bed Temp Fail
        if (printerVariables.stage == 21 || printerVariables.overridestage == 21){ 
            tweenToColor(printerConfig.bedTempRGB); //Customisable - Default is RED
            printLogs("Stage 21, BED TEMP FAIL", printerConfig.bedTempRGB);
            return;
        };
        //SERIOUS HMS state
        if (printerVariables.parsedHMSlevel == "Serious"){
            tweenToColor(printerConfig.hmsSeriousRGB); //Customisable - Default is RED
            Serial.print(F("HMS SERIOUS Severity      "));
            Serial.print(F("Error Code: "));
            Serial.printf("%016llX\n", printerVariables.parsedHMScode);
            printLogs("PROBLEM", printerConfig.hmsSeriousRGB);
            return;
        };

        //FATAL HMS state
        if (printerVariables.parsedHMSlevel == "Fatal"){
            tweenToColor(printerConfig.hmsFatalRGB); //Customisable - Default is RED
            Serial.print(F("HMS FATAL Severity      "));
            Serial.print(F("Error Code: "));
            Serial.printf("%016llX\n", printerVariables.parsedHMScode);
            printLogs("PROBLEM", printerConfig.hmsFatalRGB);
            return;
        };
    };

    //BLUE -- BLUE -- BLUE -- BLUE
    
    // Pause (by user or via Gcode)
     if ((printerVariables.stage == 16 || printerVariables.stage == 30) || printerVariables.gcodeState == "PAUSE"){
        tweenToColor(printerConfig.pauseRGB); //Customisable - Default is BLUE
        printLogs("Stage 16, gcodeState pause, PAUSED", printerConfig.pauseRGB);
        return;
    }

    //First Layer Error PAUSED
    if (printerVariables.stage == 34){ 
        tweenToColor(printerConfig.firstlayerRGB); //Customisable - Default is BLUE
        printLogs("Stage 34, FIRST LAYER ERROR, PAUSED", printerConfig.firstlayerRGB);
        return;
    }

    //Nozzle Clog PAUSED
    if (printerVariables.stage == 35){ 
        tweenToColor(printerConfig.nozzleclogRGB); //Customisable - Default is BLUE
        printLogs("Stage 35, NOZZLE CLOG, PAUSED", printerConfig.nozzleclogRGB);
        return;
    }

    //OFF -- OFF -- OFF -- OFF

    //printer offline and MQTT disconnect more than 5 seconds.
    if (printerVariables.online == false && (millis() - printerVariables.disconnectMQTTms) >= 5000){ 
        tweenToColor(0,0,0,0,0); //OFF
        printLogs("Printer offline", 0,0,0,0,0);
        return;
    }

    // replicate printer behaviour OFF
    if (printerConfig.replicatestate && printerConfig.replicate_update && printerVariables.printerledstate == false){
        tweenToColor(0,0,0,0,0); //OFF
        printLogs("LED Replication OFF", 0,0,0,0,0);
        printerConfig.replicate_update = false;
        return;
    }

    //Cleaning nozzle
    if (printerVariables.stage == 14){ 
        tweenToColor(printerConfig.stage14Color); //Customisable - Default is OFF
        printLogs("Stage 14, CLEANING NOZZLE", printerConfig.stage14Color);
        return;
    }

    //Auto Bed Leveling
    if (printerVariables.stage == 1){ 
        tweenToColor(printerConfig.stage1Color); //Customisable - Default is OFF
        printLogs("Stage 1, BED LEVELING", printerConfig.stage1Color);
        return;
    }

    //Calibrating Extrusion
    if (printerVariables.stage == 8){ 
        tweenToColor(printerConfig.stage8Color); //Customisable - Default is OFF
        printLogs("Stage 8, CALIBRATING EXTRUSION", printerConfig.stage8Color);
        return;
    }

    //Scaning surface
    if (printerVariables.stage == 9){ 
        tweenToColor(printerConfig.stage9Color); //Customisable - Default is OFF
        printLogs("Stage 9, SCANNING BED SURFACE", printerConfig.stage9Color);
        return;
    }

    //Inspecting First Layer
    if (printerVariables.stage == 10 || printerVariables.overridestage == 10){ 
        tweenToColor(printerConfig.stage10Color); //Customisable - Default is OFF
        printLogs("Stage 10 / HMS 0C00, FIRST LAYER INSPECTION", printerConfig.stage10Color);
        return;
    }

    //Calibrating  MicroLidar
    if (printerVariables.stage == 12){ 
        tweenToColor(printerConfig.stage10Color); 
        printLogs("Stage 12, CALIBRATING MICRO LIDAR", printerConfig.stage10Color);
        return;
    }

    // Idle Timeout (Has to be enabled)
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) 
    && !((printerConfig.finishExit && printerVariables.waitingForDoor) || (printerConfig.finishExit == false && ((millis() - printerConfig.finishStartms) < printerConfig.finishTimeOut)))
    && (millis() - printerConfig.inactivityStartms) > printerConfig.inactivityTimeOut 
    && printerConfig.isIdleOFFActive == false
    && printerConfig.inactivityEnabled){ 
        tweenToColor(0,0,0,0,0); //OFF
        printerConfig.isIdleOFFActive = true;
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.print(F("Idle Timeout ["));
            Serial.print((int)(printerConfig.inactivityTimeOut / 60000));
            Serial.println(F(" mins] - Turning LEDs OFF"));
        };
        return;
    }

    //ON -- ON -- ON -- ON 

    //Preheating Bed
    if (printerVariables.stage == 2){ 
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("Stage 2, PREHEATING BED", printerConfig.runningColor);
        return;
    }

    //Printing or Resume after Pausing
    if (printerVariables.stage == 0  && printerVariables.gcodeState == "RUNNING"){ 
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("Stage 0, PRINTING - gcodeState RUNNING", printerConfig.runningColor);
        return;
    }

    //for IDLE - P1 uses 255, X1 uses -1
    if ((printerVariables.stage == -1 || printerVariables.stage == 255) 
    && !((printerConfig.finishExit && printerVariables.waitingForDoor) || (printerConfig.finishExit == false && ((millis() - printerConfig.finishStartms) < printerConfig.finishTimeOut)))
    && (millis() - printerConfig.inactivityStartms < printerConfig.inactivityTimeOut)){ 
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("Stage -1/255, IDLE", printerConfig.runningColor);
        return;
    }

    //User Cancelled Print
    if (printerVariables.gcodeState == "FAILED"){ 
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("Stage -1/255, FAILED", printerConfig.runningColor);
        return;
    }
    //Print file just sent
    if (printerVariables.gcodeState == "PREPARE"){ 
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("Stage -1/255, PREPARE", printerConfig.runningColor);
        return;
    }

    //Homing ToolHead
    if (printerVariables.stage == 13){ 
        //No color change assigned
        Serial.println(F("STAGE 13, HOMING TOOL HEAD"));
        return;
    }

    //Offline
    if (printerVariables.gcodeState == "OFFLINE" || printerVariables.stage == -2){ 
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("Stage -1/255, OFFLINE", printerConfig.runningColor);
        return;
    }

    //GREEN -- GREEN -- GREEN -- GREEN

    //Sets to green when print finishes AND user wants Finish Indication enabled
    if (printerVariables.finished == true && printerConfig.finishindication == true){
        tweenToColor(printerConfig.finishColor); //Customisable - Default is GREEN
        printLogs("Finished print", printerConfig.finishColor);
        printerVariables.finished = false;
        return;
    }

    // replicate printer behaviour ON
    if (printerConfig.replicatestate && printerConfig.replicate_update && printerVariables.printerledstate
    && !((printerConfig.finishExit && printerVariables.waitingForDoor) || (printerConfig.finishExit == false && ((millis() - printerConfig.finishStartms) < printerConfig.finishTimeOut)))){
        tweenToColor(printerConfig.runningColor); //Customisable - Default is WHITE
        printLogs("LED Replication ON", printerConfig.runningColor);
        printerConfig.replicate_update = false;
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
    if((millis() - lastUpdatems) > 30000 && (printerConfig.maintMode || printerConfig.testcolorEnabled || printerConfig.discoMode || printerConfig.debugwifi)) {
        Serial.print(F("["));
        Serial.print(millis());
        Serial.print(F("]"));
        if(printerConfig.maintMode) Serial.println(F(" Maintenance Mode - next update in 30 seconds"));
        if(printerConfig.testcolorEnabled) Serial.println(F(" Test Color - next update in 30 seconds"));
        if(printerConfig.discoMode) Serial.println(F(" RGB Cycle Mode - next update in 30 seconds"));
        if(printerConfig.debugwifi) Serial.println(F(" Wifi Debug Mode - next update in 30 seconds"));
        lastUpdatems = millis();
    }

    // Turn off GREEN if... finished and Door OPENED or CLOSED in last 5 secs AND user wants Finish Indication enabled
    // Can't use Boolean finished as printer defaults back to IDLE via MQTT
    if((printerVariables.waitingForDoor && printerConfig.finishindication && printerConfig.finishExit
    && ((millis() - printerVariables.lastdoorClosems) < 6000 || (millis() - printerVariables.lastdoorOpenms) < 6000))){
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Updating from finishloop after Door interaction - Starting IDLE timer"));
        }
        printerVariables.waitingForDoor = false;
        printerConfig.inactivityStartms = millis();
        printerConfig.isIdleOFFActive = false;
        updateleds();
    }

    if((printerConfig.finish_check && printerConfig.finishindication && printerConfig.finishExit == false
    && ((millis() - printerConfig.finishStartms) > printerConfig.finishTimeOut))){
        if (printerConfig.debuging || printerConfig.debugingchange){
            Serial.println(F("Updating from finishloop after Finish timer expired - Starting IDLE timer"));
        }
        printerConfig.finish_check = false;
        printerConfig.inactivityStartms = millis();
        printerConfig.isIdleOFFActive = false;
        updateleds();
    }

    //Need an trigger action to run updateleds() so lights turn off  
    //There is no change in the printer STATE, just monitoring the timer and triggering when over a threshhold
    if(printerConfig.inactivityEnabled 
    && (millis() - printerConfig.inactivityStartms) > printerConfig.inactivityTimeOut 
    && printerVariables.finished == false && printerConfig.isIdleOFFActive == false)
    {
        //Opening or Closing the Door will turn LEDs back on and restart the timer.
        updateleds();
    }

    delay(10);
}

#endif
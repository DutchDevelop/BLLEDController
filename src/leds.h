#ifndef _LED
#define _LED

#include <Arduino.h>
#include "mqttparsingutility.h"

#if defined(ESP8266)
    const int redPin = 5;
    const int greenPin = 14;
    const int bluePin = 4;
    const int warmPin = 2;
    const int coldPin = 0;
#elif defined(ESP32)
    const int redPin = 19;
    const int greenPin = 18;
    const int bluePin = 21;
    const int warmPin = 22;
    const int coldPin = 23;
#endif

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
    if (printerConfig.turbo == false) {
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
    if (printerConfig.debuging){
        Serial.println(F("Updating leds"));

        Serial.println(printerVariables.stage);
        Serial.println(printerVariables.gcodeState);
        Serial.println(printerVariables.ledstate);
        Serial.println(printerVariables.hmsstate);
        Serial.println(printerVariables.parsedHMS);
    }
    //OFF

    if (printerConfig.turbo == true){
        return;
    }

    if (printerVariables.online == false){ //printer offline
        tweenToColor(0,0,0,0,0,500); //OFF
        if (printerConfig.debuging){
            Serial.println(F("Printer offline, Turning Leds off"));
        };
        return;
    }

    if (printerVariables.ledstate == false && printerConfig.replicatestate == true){ // replicate printer behaviour
        tweenToColor(0,0,0,0,0,500); //OFF
        if (printerConfig.debuging){
            Serial.println(F("Chamber light off, Turning Leds off"));
        };
        return;
    }

     if (printerVariables.stage == 14){ //Cleaning nozzle
        tweenToColor(0,0,0,0,0,500); //OFF
        if (printerConfig.debuging){
            Serial.println(F("Cleaning nozzle, Turning Leds off"));
        };
        return;
    }

    if (printerVariables.stage == 9){ //Scaning surface
        tweenToColor(0,0,0,0,0,500); //OFF
        if (printerConfig.debuging){
            Serial.println(F("Scanning Surface, Turning Leds off"));
        };
        return;
    }
    
     //RED

    if (printerConfig.errordetection == true){ // allow errordetection to turn ledstrip red
        if (printerVariables.parsedHMS == "Serious"){
            tweenToColor(255,0,0,0,0,500); //RED
            if (printerConfig.debuging){
                Serial.println(F("Serious problem, Turning Leds red"));
            };
            return;
        }

        if (printerVariables.parsedHMS == "Fatal"){
            tweenToColor(255,0,0,0,0,500); //RED
            if (printerConfig.debuging){
                Serial.println(F("Fatal problem, Turning Leds red"));
            };
            return;
        }

        if (printerVariables.stage == 6){ //Fillament runout
            tweenToColor(255,0,0,0,0,500); //RED
            if (printerConfig.debuging){
                Serial.println(F("Fillament runout, Turning Leds red"));
            };
            return;
        }

        if (printerVariables.stage == 17){ //Front Cover Removed
            tweenToColor(255,0,0,0,0,500); //RED
            if (printerConfig.debuging){
                Serial.println(F("Front Cover Removed, Turning Leds red"));
            };
            return;
        }

        if (printerVariables.stage == 20){ //Nozzle Temp fail
            tweenToColor(255,0,0,0,0,500); //RED
            if (printerConfig.debuging){
                Serial.println(F("Nozzle Temp fail, Turning Leds red"));
            };
            return;
        }

        if (printerVariables.stage == 21){ //Bed Temp Fail
            tweenToColor(255,0,0,0,0,500); //RED
            if (printerConfig.debuging){
                Serial.println(F("Bed Temp fail, Turning Leds red"));
            };
            return;
        }
    };

    //GREEN

    if ((millis() - printerVariables.finishstartms) <= 300000 && printerVariables.gcodeState == "FINISH"){
        tweenToColor(0,255,0,0,0,500); //ON
        if (printerConfig.debuging){
            Serial.println(F("Finished print, Turning Leds green"));
            Serial.println(F("Leds should stay on for: "));
            Serial.print((millis() - printerVariables.finishstartms));
            Serial.print(F(" MS"));
        };
        return;
    }

    //ON

    if (printerVariables.stage == 0){ //Printing
        tweenToColor(0,0,0,255,255,500); //ON
        if (printerConfig.debuging){
            Serial.println(F("Printing, Turning Leds On"));
        };
        return;
    }

    if (printerVariables.stage == -1){ // Idle
        tweenToColor(0,0,0,255,255,500); //ON
        if (printerConfig.debuging){
            Serial.println(F("Idle, Turning Leds On"));
        };
        return;
    }

    if (printerVariables.stage == -2){ //Offline
        tweenToColor(0,0,0,255,255,500); //ON
        if (printerConfig.debuging){
            Serial.println(F("Stage -2, Turning Leds On"));
        };
        return;
    }
}

void setupLeds() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(coldPin, OUTPUT);
    pinMode(warmPin, OUTPUT);
    updateleds();
}

void ledsloop(){
    RGBCycle();
    if((millis() - printerVariables.finishstartms) >= 300000 && printerVariables.gcodeState == "FINISH"){
        printerVariables.gcodeState == "IDLE";
        updateleds();
    }
}

#endif
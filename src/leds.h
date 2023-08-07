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
    const int bluePin = 5;
    const int warmPin = 5;
    const int coldPin = 5;
#endif

int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;
int currentWarm = 0;
int currentCold = 0;

void tweenToColor(int targetRed, int targetGreen, int targetBlue, int targetWarm, int targetCold, int duration) {
    if (targetRed == currentRed && targetGreen == currentGreen && targetBlue == currentBlue && targetWarm == currentWarm && targetCold == currentCold){
        return; // already at that color
    }
    float stepTime = (float)duration / 255.0;
    int redStep = (targetRed - currentRed) / 255;
    int greenStep = (targetGreen - currentGreen) / 255;
    int blueStep = (targetBlue - currentBlue) / 255;
    int warmStep = (targetWarm - currentWarm) / 255;
    int coldStep = (targetCold - currentCold) / 255;

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

    currentRed = targetRed;
    currentGreen = targetGreen;
    currentBlue = targetBlue;
    currentWarm = targetWarm;
    currentCold = targetCold;

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

void updateleds(){

    Serial.println(F("Updating leds"));

    Serial.println(printerVariables.stage);
    Serial.println(printerVariables.gcodeState);
    Serial.println(printerVariables.ledstate);
    Serial.println(printerVariables.hmsstate);
    Serial.println(printerVariables.parsedHMS);

    //OFF

    if (printerVariables.online == false){ //printer offline
        tweenToColor(0,0,0,0,0,500); //OFF
        return;
    }
    if (printerVariables.ledstate == false && printerConfig.replicatestate == true){ // replicate printer behaviour
        tweenToColor(0,0,0,0,0,500); //OFF
        return;
    }

     if (printerVariables.stage == 14){ //Cleaning nozzle
        tweenToColor(0,0,0,0,0,500); //OFF
        return;
    }

    if (printerVariables.stage == 9){ //Scaning surface
        tweenToColor(0,0,0,0,0,500); //OFF
        return;
    }
    
     //RED

    if (printerConfig.errordetection == true){ // allow errordetection to turn ledstrip red
        if (printerVariables.parsedHMS == "Serious"){
            tweenToColor(255,0,0,0,0,500); //RED
            return;
        }

        if (printerVariables.parsedHMS == "Fetal"){
            tweenToColor(255,0,0,0,0,500); //RED
            return;
        }

        if (printerVariables.stage == 6){ //Fillament runout
            tweenToColor(255,0,0,0,0,500); //RED
            return;
        }

        if (printerVariables.stage == 17){ //Front Cover Removed
            tweenToColor(255,0,0,0,0,500); //RED
            return;
        }

        if (printerVariables.stage == 20){ //Nozzle Temp fail
            tweenToColor(255,0,0,0,0,500); //RED
            return;
        }

        if (printerVariables.stage == 21){ //Bed Temp Fail
            tweenToColor(255,0,0,0,0,500); //RED
            return;
        }
    };

    //GREEN

    if ((millis() - printerVariables.finishstartms) <= 300000 && printerVariables.gcodeState == "FINISH"){
        tweenToColor(0,255,0,0,0,500); //ON
        return;
    }

    //ON

    if (printerVariables.stage == 0){ //Printing
        tweenToColor(0,0,0,255,255,500); //ON
        return;
    }

    if (printerVariables.stage == -1){ // Idle
        tweenToColor(0,0,0,255,255,500); //ON
        return;
    }

    if (printerVariables.stage == -2){ //Offline
        tweenToColor(0,0,0,255,255,500); //ON
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

#endif
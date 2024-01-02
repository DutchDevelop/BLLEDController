#ifndef _BLLEDSERIAL_MANAGER
#define _BLLEDSERIAL_MANAGER

#include <Arduino.h>
#include <ArduinoJson.h> 
#include <AESLib.h> // Include the AES library

#include "filesystem.h"
#include "types.h"

AESLib aesLib;

// Generate a random key and IV for demonstration purposes
// WARNING: In production, use a secure way to generate and store keys
byte aes_key[] = {/* 16, 24 or 32 byte key */};
byte aes_iv[] = {/* 16 byte IV */};

// Modify the encrypt function
String encrypt(String data) {
    char encryptedData[64];
    // Assuming AES 128-bit encryption
    aesLib.encrypt64((byte*)data.c_str(), data.length(), encryptedData, aes_key, 128, aes_iv);
    return String(encryptedData);
}

// Modify the decrypt function
String decrypt(String data) {
    char decryptedData[64];
    // Assuming AES 128-bit decryption
    aesLib.decrypt64((char*)data.c_str(), data.length(), (byte*)decryptedData, aes_key, 128, aes_iv);
    return String(decryptedData);
}

void setupSerial(){
   while (!Serial);
}

void serialLoop(){
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        DynamicJsonDocument doc(256);
        deserializeJson(doc, input);
        if (doc.containsKey("ssid") && doc.containsKey("pass")) {
            // Encrypt the data before storing
            String encryptedSSID = encrypt(doc["ssid"].as<String>());
            String encryptedPASS = encrypt(doc["pass"].as<String>());
            String encryptedIP = encrypt(doc["printerip"].as<String>());
            String encryptedCode = encrypt(doc["printercode"].as<String>());
            String encryptedSerial = encrypt(doc["printerserial"].as<String>());

            strcpy(globalVariables.SSID, encryptedSSID.c_str());
            strcpy(globalVariables.APPW, encryptedPASS.c_str());

            strcpy(printerConfig.printerIP, encryptedIP.c_str());
            strcpy(printerConfig.accessCode, encryptedCode.c_str());
            strcpy(printerConfig.serialNumber, encryptedSerial.c_str());

            saveFileSystem();
            ESP.restart();
        }
    };
}

#endif

#ifndef _BLLEDSSDP
#define _BLLEDSSDP

#include <Arduino.h>
#include <WiFi.h>
#include <ESP32SSDP.h>
#include <ArduinoJson.h> 

// this code makes the blled discoverable for the xtouch project.

void start_ssdp(){
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setDeviceType("urn:schemas-upnp-org:device:DimmableLight:1");
    SSDP.setName("BLLED Controller");
    SSDP.setSerialNumber(WiFi.macAddress().c_str());
    SSDP.setURL("/");
    SSDP.setModelName("BLLED ESP32");
    SSDP.setModelNumber("1.0");
    SSDP.setManufacturer("DutchDeveloper");
    SSDP.setManufacturerURL("https://dutchdevelop.com");
    SSDP.begin();
}

#endif
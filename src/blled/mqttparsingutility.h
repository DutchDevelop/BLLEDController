#ifndef _MQTTPARSERUTILITY
#define _MQTTPARSERUTILITY

#include "types.h"

String ParseHMSSeverity(int code){ // Provided by WolfWithSword
    int parsedcode (code>>16);
    switch (parsedcode){
        case 1:
            return F("Fatal");
        case 2:
            return F("Serious");
        case 3:
            return F("Common");
        case 4:
            return F("Info");
        default:;
    }
    return "";
}

void ParseMQTTState(int code){
    switch (code)
    {
    case -4: // MQTT_CONNECTION_TIMEOUT
        LogSerial.println(F("MQTT TIMEOUT"));
        break;
    case -3: // MQTT_CONNECTION_LOST
        LogSerial.println(F("MQTT CONNECTION_LOST"));
        break;
    case -2: // MQTT_CONNECT_FAILED
        LogSerial.println(F("MQTT CONNECT_FAILED"));
        break;
    case -1: // MQTT_DISCONNECTED
        LogSerial.println(F("MQTT DISCONNECTED"));
        break;
    case 0:  // MQTT_CONNECTED
        LogSerial.println(F("MQTT CONNECTED"));
        break;
    case 1:  // MQTT_CONNECT_BAD_PROTOCOL
        LogSerial.println(F("MQTT BAD PROTOCOL"));
        break;
    case 2:  // MQTT_CONNECT_BAD_CLIENT_ID
        LogSerial.println(F("MQTT BAD CLIENT ID"));
        break;
    case 3:  // MQTT_CONNECT_UNAVAILABLE
        LogSerial.println(F("MQTT UNAVAILABLE"));
        break;
    case 4:  // MQTT_CONNECT_BAD_CREDENTIALS
        LogSerial.println(F("MQTT BAD CREDENTIALS"));
        break;
    case 5: // MQTT UNAUTHORIZED
        LogSerial.println(F("MQTT UNAUTHORIZED"));
        break;
    }
}


#endif
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
        LogSerial.println(F("[MQTT] Timeout (-4)"));
        break;
    case -3: // MQTT_CONNECTION_LOST
        LogSerial.println(F("[MQTT] Connection Lost (-3)"));
        break;
    case -2: // MQTT_CONNECT_FAILED
        LogSerial.println(F("[MQTT] Connection Failed (-2)"));
        break;
    case -1: // MQTT_DISCONNECTED
        LogSerial.println(F("[MQTT] Disconnected (-1)"));
        break;
    case 0:  // MQTT_CONNECTED
        LogSerial.println(F("[MQTT] Connected (0)"));
        break;
    case 1:  // MQTT_CONNECT_BAD_PROTOCOL
        LogSerial.println(F("[MQTT] Bad protocol (1)"));
        break;
    case 2:  // MQTT_CONNECT_BAD_CLIENT_ID
        LogSerial.println(F("[MQTT] Bad Client ID (2)"));
        break;
    case 3:  // MQTT_CONNECT_UNAVAILABLE
        LogSerial.println(F("[MQTT] Unavailable (3)"));
        break;
    case 4:  // MQTT_CONNECT_BAD_CREDENTIALS
        LogSerial.println(F("[MQTT] Bad Credentials (4)"));
        break;
    case 5: // MQTT UNAUTHORIZED
        LogSerial.println(F("[MQTT] Unauthorized (5)"));
        break;
    }
}


#endif
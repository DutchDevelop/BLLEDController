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


#endif
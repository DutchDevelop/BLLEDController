#ifndef _TYPES
#define _TYPES

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct PrinterVaraiblesStruct{
        String parsedHMS = "";
        String gcodeState = "IDLE";
        int stage = 0;
        bool ledstate = true;
        bool hmsstate = false;
        bool online = false;
        bool finished = false;
        unsigned long finishstartms;

    } PrinterVariables;

    PrinterVariables printerVariables;

    typedef struct GlobalVariablesStruct{
        char SSID[32];
        char APPW[32];
        String FWVersion = "STABLE 30.12.23";
    } GlobalVariables;

    GlobalVariables globalVariables;

    typedef struct PrinterConfigStruct
    {
        int brightness = 100;
        bool replicatestate = true;
        bool errordetection = true;
        bool finishindication = true;
        bool debuging = true;
        bool turbo = false;
        char printerIP[16];
        char accessCode[9];
        char serialNumber[16];
        //char webpagePassword[8];
    } PrinterConfig;

    PrinterConfig printerConfig;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

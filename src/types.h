#ifndef _TYPES
#define _TYPES

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct PrinterVaraiblesStruct{
        String parsedHMS = "";
        String gcodeState = "FINISH";
        int stage = 0;
        long lastRSSI = 0;
        bool overrideLEDstate = false;
        bool ledstate = true;
        bool hmsstate = false;
        bool online = false;
        bool finished = false;
        bool doorOpen = false;
        bool idleLightsOff = false;
        int idleLightoffTimeOut = 1800000; // 300000 = 5mins   1800000  = 30 mins
        unsigned long finishstartms = 0;
        unsigned long idleStartms = 0;
        unsigned long disconnectMQTTms = 0;
        unsigned long lastdoorClosems = 0;
        unsigned long lastdoorOpenms = 0;
        bool doorSwitchenabled = false;
        bool initalisedLEDs = false;

    } PrinterVariables;

    PrinterVariables printerVariables;

    typedef struct GlobalVariablesStruct{
        char SSID[32];
        char APPW[32];
        String FWVersion = "EXPERIMENTAL 12.3.24";
        String Host = "BLLED";
        bool started = false;
    } GlobalVariables;


    GlobalVariables globalVariables;

    typedef struct PrinterConfigStruct
    {
        int brightness = 100;
        bool replicatestate = true;
        bool errordetection = true;
        bool finishindication = true;
        bool mqttdebug = false;

        char BSSID[18]; // Nominate the specific AP to connect to (Useful when you have multiple accesspoints with same name)

        bool debuging = false;
        bool debugingchange = true;
        bool debugwifi = false;

        bool overrideRed = false;
        bool overrideGreen = false;
        bool overrideBlue = false;
        bool overrideWarmWhite = false;
        bool overrideColdWhite = false;
        
        bool discoMode = false;
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

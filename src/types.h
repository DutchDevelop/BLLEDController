#ifndef _TYPES
#define _TYPES

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct PrinterVaraiblesStruct{
        String parsedHMS = "";
        int parsedHMSattrib = 0;
        String gcodeState = "FINISH";           //Initialised to Finish so the logic doesn't 
                                                //assume a Print has just finished and needs 
                                                //to wait for a door interaction to continue
        int stage = 0;
        bool testcolorEnabled = false;
        bool printerledstate = true;
        bool hmsstate = false;
        bool online = false;
        bool finished = false;
        bool initalisedLEDs = false;
        bool inspectingFirstLayer = false;
        //Time since
        unsigned long disconnectMQTTms = 0;        
        //InactivityLightsOff Timer and status
        bool inactivityLightsOff = false;       // Are the lights out due to inactivity Timeout?
        unsigned long inactivityStartms = 0;    // Time the inactivity countdown is measured from
        
        //Door Monitoring
        bool doorOpen = false;                  // Current State of Door
        bool doorSwitchenabled = false;         // Has door been closed twice within 6 seconds?
        bool waitingForDoor = false;            // Are we waiting for the door to be actuated?
        unsigned long lastdoorClosems = 0;      // Last time door was opened
        unsigned long lastdoorOpenms = 0;       // Last time door was closed
        

    } PrinterVariables;

    PrinterVariables printerVariables;

    typedef struct GlobalVariablesStruct{
        char SSID[32];
        char APPW[32];
        String FWVersion = "EXPERIMENTAL 16.3.24";
        String Host = "BLLED";
        bool started = false;
    } GlobalVariables;

    GlobalVariables globalVariables;

    typedef struct PrinterConfigStruct
    {
        char printerIP[16];             //BBLP IP Address - used for MQTT reports
        char accessCode[9];             //BBLP Access Code - used for MQTT reports
        char serialNumber[16];          //BBLP Serial Number - used for MQTT reports

        char BSSID[18];                 //Nominated AP to connect to (Useful if multiple accesspoints with same name)
        int brightness = 100;           //Brightness of LEDS
        bool rescanWiFiNetwork = false; //Scans available WiFi networks for strongest signal
        // BLLED Settings
        bool replicatestate = true;     //LED will be on if the BBPL Light is on
        bool errordetection = true;     //Utilises Error Colors when BBLP give an error
        bool finishindication = true;   //Utilises Finish Color when print ends successfully
        bool lidarLightsOff = true;     //For X1C owners - option to turn off if you have a P1P with no Lidar.
        bool inactivityEnabled = true;
        int inactivityTimeOut = 1800000;// 1800000 = 30 mins
        bool discoMode = false;         //Cycles through RGB colors slowly for 'pretty' timelapse movie
        bool maintMode = false;         //White lights on, even if printer is unpowered
        bool updateMaintenance = false; //When updateleds() is run, should the Maintenance LEDS be set?
        // Debugging
        bool debuging = false;          //Debugging for all interactions through functions
        bool debugingchange = true;     //Default debugging level - to shows onChange
        bool mqttdebug = false;         //Writes each packet from BBLP to the serial log
        bool debugwifi = false;         //Changes LED to a color range that represents WiFi signal Strength        
        // Test - Fixed LED Colors
        char testRGB[8];                //Test Color (RGB only)
        int testwarmwhite = 100;        //Testing just the Warm White LED line
        int testcoldwhite = 100;        //Testing just the Cold White LED line
        bool updateTestLEDS = false;    //When updateleds() is run, should the TEST LEDS be set?
        // Customise LED Colors
        char wifiRGB[8];                //These need to be 8 char: '#' + 123456 + end char '/0'
        char finishRGB[8];
        char pauseRGB[8];
        char firstlayerRGB[8];
        char nozzleclogRGB[8];
        char hmsSeriousRGB[8];
        char hmsFatalRGB[8];
        char filamentRunoutRGB[8];
        char frontCoverRGB[8];
        char nozzleTempRGB[8];
        char bedTempRGB[8];

        //char webpagePassword[8];
    } PrinterConfig;

    PrinterConfig printerConfig;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

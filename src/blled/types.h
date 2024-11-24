#ifndef _TYPES
#define _TYPES

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct COLORStruct {
        short r;
        short g;
        short b;
        short ww;
        short cw;
        char RGBhex[8];
    } COLOR;


    typedef struct PrinterVaraiblesStruct{
        String parsedHMSlevel = "";
        uint64_t parsedHMScode = 0;            //8 bytes per code stored
        String gcodeState = "FINISH";           //Initialised to Finish so the logic doesn't 
                                                //assume a Print has just finished and needs 
                                                //to wait for a door interaction to continue
        int stage = 0;
        int overridestage = 999;
        bool printerledstate = true;
        bool hmsstate = false;
        bool online = false;
        bool finished = false;
        bool initalisedLEDs = false;
        //Time since
        unsigned long disconnectMQTTms = 0;        

        //PrinterType
        bool isP1Printer = false;               //Is this a P1 Printer without lidar or door switch
        //Door Monitoring
        bool useDoorSwtich = true;              //DoorSwitch to be used for Actions?
        bool doorOpen = false;                  // Current State of Door
        bool doorSwitchTriggered = false;       // Has door been closed twice within 6 seconds?
        bool waitingForDoor = false;            // Are we waiting for the door to be actuated?
        unsigned long lastdoorClosems = 0;      // Last time door was opened
        unsigned long lastdoorOpenms = 0;       // Last time door was closed
    } PrinterVariables;

    PrinterVariables printerVariables;

    typedef struct GlobalVariablesStruct{
        char SSID[32];
        char APPW[63];
        String FWVersion = "Stable 16.5.24";
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
        int brightness = 20;            //Brightness of LEDS - Default to 20% in case user use LED's that draw too much power for their PS
        bool rescanWiFiNetwork = false; //Scans available WiFi networks for strongest signal
        // LED Behaviour (Choose One)
        bool maintMode = false;         //White lights on, even if printer is unpowered
        bool maintMode_update = true;
        bool discoMode = false;         //Cycles through RGB colors slowly for 'pretty' timelapse movie
        bool discoMode_update = true;
        bool replicatestate = true;     //LED will be on if the BBPL Light is on
        bool replicate_update = true;     //LED will be on if the BBPL Light is on
        COLOR runningColor;             //Running Color (Default if no issues)
        bool testcolorEnabled = false;
        bool testcolor_update= true;    //When updateleds() is run, should the TEST LEDS be set?
        COLOR testColor;                //Test Color
        bool debugwifi = false;         //Changes LED to a color range that represents WiFi signal Strength        
        // Options
        bool finishindication = true;   //Enable / Disable
        COLOR finishColor;              //Set Finish Color
        bool finishExit = true;         //True = use Door / False = use Timer
        bool finish_check = false;    //When updateleds() is run, should the TEST LEDS be set?
        unsigned long finishStartms = 0;    // Time the finish countdown is measured from
        int finishTimeOut = 600000;     //300000 = 5 mins
        //Inactivity Timout
        bool inactivityEnabled = true;
        bool isIdleOFFActive = false;       // Are the lights out due to inactivity Timeout?
        unsigned long inactivityStartms = 0;    // Time the inactivity countdown is measured from
        int inactivityTimeOut = 3600000;  // 1800000 = 30mins / 600000 = 10mins / 60000 = 1mins 
        // Debugging
        bool debuging = false;          //Debugging for all interactions through functions
        bool debugingchange = true;     //Default debugging level - to shows onChange
        bool mqttdebug = false;         //Writes each packet from BBLP to the serial log
        //Custom Colors for events using lidar
        COLOR stage14Color;
        COLOR stage1Color;
        COLOR stage8Color;
        COLOR stage9Color;
        COLOR stage10Color;
        // Customise LED Colors
        bool errordetection = true;     //Utilises Error Colors when BBLP give an error
        COLOR wifiRGB;                  
        COLOR pauseRGB;
        COLOR firstlayerRGB;
        COLOR nozzleclogRGB;
        COLOR hmsSeriousRGB;
        COLOR hmsFatalRGB;
        COLOR filamentRunoutRGB;
        COLOR frontCoverRGB;
        COLOR nozzleTempRGB;
        COLOR bedTempRGB;

        //char webpagePassword[8];
    } PrinterConfig;

    PrinterConfig printerConfig;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
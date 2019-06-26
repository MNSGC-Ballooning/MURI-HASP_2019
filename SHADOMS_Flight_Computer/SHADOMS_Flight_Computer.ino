//SHADOMS Payload Flight Computer
//Version 1.2
//Payload 2019-01

/*This code operates the Teensy 3.5/3.6 Microcontroller on the 2019 HASP flight.
The goal of this flight is to collect data from the Plantower PMS-5003, the Alphasense OPC N3,
and the LOAC-R. Then, this data will be compared to examine how each particle detector operates
in the conditions of the upper atmosphere. This examination will be utilized to determine counter
effectiveness and calibration needs for future MURI research.

The code operates the particle counter power, writes the data of the Plantower to disk,
regulates temperature using active heating, runs a Coperinicus GPS, and controls uplink
and downlink with the main HASP gondola.

In this setup, both the Alphasense and the LOAC are running in a standalone mode where they 
record their own data.*/

//University of Minnesota Twin Cities
//Written in June 2019

//Version History
//Version 1.2
/*Passed initial unit tests for thermal control, GPS. This version also updates
the plantower logging system (returning to software serial), updates the LED pins, and fixes various bugs
and unclean code. The system log and the plantower log have also been collapsed.*/

//Version 1.1
/*Updated the serial port system to impliment hardware serial connections in place of software serial.
This will allow for the system to interface with multiple serial systems simaltaneously. This version also
implemented the serial interface with the HASP gondala and established meanings for the LEDs.*/

//Version 1.0
/*This is the initial code after the first stage of debugging.*/





//---ESTABLISH PREREQUISITE DEFINITIONS---\\





//Libraries
  #include <SD.h>                   //Writing to SD card
  #include <OneWire.h>              //Dallas data control
  #include <DallasTemperature.h>    //Dallas temperature control
  #include <TinyGPS++.h>            //GPS control
  #include <LatchRelay.h>           //Relay control
  #include <SoftwareSerial.h>       //Software serial communication

//Pin Definitions
  #define sdLED 23                  //LED pin which blinks to indicates data logging to the SD*****
  #define fixLED 22                 //LED pin which blinks to indicate GPS fix*****
  #define stateLED 21               //LED pin which blinks to indicate an active data recording status******
  #define wireBus1 29               //Temperature sensor 1 pin - Internal Ambient
  #define wireBus2 30               //Temperature sensor 2 pin - External Ambient
  #define wireBus3 31               //Temperature sensor 3 pin - OPCs
  #define heater_ON 24              //Heater relay pins
  #define heater_OFF 25             //^^^
  #define alphaOPC_ON 5             //Alphasense OPC relay pins
  #define alphaOPC_OFF 6            //^^^
  #define planOPC_ON 26             //Plantower OPC relay pins
  #define planOPC_OFF 27            //^^^
  #define LOAC_ON 7                 //LOAC OPC power relay pins
  #define LOAC_OFF 8                //^^^
  #define LS_PD 35                  //LOAC state shutdown transistor
  
//Serial Pins
/*  
  #define HASP_RX 0                 //HASP Recieve Pin                SERIAL 1
  #define HASP_TX 1                 //HASP Transmission Pin
  #define GPS_RX 9                  //GPS Recieve Pin                 SERIAL 2
  #define GPS_TX 10                 //GPS Transmission Pin
*/
  #define PMS_RX 34                 //PMS Recieve Pin                 SERIAL 5/SOFTWARE SERIAL
  #define PMS_TX 33                 //PMS Transmission Pin (Unused) 

//Constant Definitions
  #define UPDATE_RATE 2300          //These definitions are the rates of the individual portions of the
  #define PLAN_RATE 2300            //systemUpdate function.
  #define DWN_BYTES 51              //Number of downlink bytes + 1 (the +1 makes it work)
  #define COLD 280.0                //Minimum acceptable temperature of the OPC
  #define HOT 290.0                 //Maximum acceptable temperature of the OPC
  #define KELVIN 273.15             //Number to convert Celcius to Kelvin

//Relay Definitions
  LatchRelay heater(heater_ON, heater_OFF);             //Define heater relay object
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  LatchRelay LOAC(LOAC_ON, LOAC_OFF);                   //Define LOAC OPC power relay object
  bool dataCollection = false;
  
//Active Heating Definitions
  bool coldOPC = false;

//Temperature Sensor Definitions
  OneWire oneWire1(wireBus1);                           //Temperature sensor 1 data interface
  OneWire oneWire2(wireBus2);                           //Temperature sensor 2 data interface
  OneWire oneWire3(wireBus3);                           //Temperature sensor 3 data interface
  DallasTemperature temperature1(&oneWire1);            //Temperature sensor 1
  DallasTemperature temperature2(&oneWire2);            //Temperature sensor 2
  DallasTemperature temperature3(&oneWire3);            //Temperature sensor 3
  float t1;                                             //Temperature 1
  float t2;                                             //Temperature 2
  float t3;                                             //Temperature 3
  
//Serial Definitions
  //HASP Uplink and downlink data
  String flightState = "";
  String OPCState = "";
  byte packet[DWN_BYTES] = {0};                         //50 char/bytes in the string (54 with checksum)
  
//Data Log Definitions
  //Log Plantower, temperature, GPS, HASP data
  const int chipSelect = BUILTIN_SDCARD;                //Access on board micro-SD
  File fLog;                                            //This part of the code establishes the file and
  String data;                                          //sets up the CSV format
  String Fname = "";                                    //This is the name of the CSV file
  String header = "";                                   //This is the head of the CSV file
  bool sdLogging = false;                               //This variable is used to indicate when logging occurs
  
//Plantower Definitions
  SoftwareSerial pmsSerial(PMS_RX, PMS_TX);             //Sets object for software serial connection
  String dataLog;                                       //Used for data logging
  int nhits=1;                                          //Used to count successful data transmissions    
  int ntot=1;                                           //Used to count total attempted transmissions
  bool goodLog = false;                                 //Used to check if data is actually being logged
  int badLog = 0;                                       //Counts the number of failed log attempts
  //String filename = "ptLog.csv";                      //File name that data wil be written to
  //File ptLog;                                         //File that data is written to 
                  
  struct pms5003data {
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
  } planData;                                           //This struct will organize the plantower bins into seperate parts of the data
  
//GPS Definitions
  TinyGPSPlus GPS;                                      //GPS object definition
  bool inFlight = false;                                //Bool that determines if the payload is in flight. Used with FlightCheck function
  unsigned long flightStart = 0;                        //Time passed since inFlight became true
  byte FlightCheckCounter = 0;                          //If this reaches 5, then inFLight should be set to true
  
  unsigned long lastGPS = 0;                             //Time in seconds since the last GPS update
  unsigned long GPSstartTime = 0;                        //When the GPS starts, time in seconds of last GPS update
  uint8_t days = 0;                                      //If we're flying overnight this serves as a coutner for time keeping
 
  String GPSdata = "";                                  //Initializes data string that prints GPS data to the SD card
  String faillatitude = "0.00";                         //Printed latitude if GPS does not have a fix or any data
  String faillongitude = "0.000000";                    //Printed longitude if GPS does not have a fix or any data
  String failalt = "0.000000";                          //Printed altitude if GPS does not have a fix or any 

//LED Definitions
  bool fixLight = false;                                //These booleans are for the light activation and deactivation logic  
  bool sdLight = false;
  bool stateLight = false;

//Updater Definitions                                   //These values help to regulate the speed of the updater function
  unsigned long lastCycle = 0;
  unsigned long planCycle = 0;
  
  



//---INITIALIZE CODE---\\                              





void setup() {                                       
//Relay Initialization- sets up all relays, sets them all to an open state.
  heater.init(0);
  alphaOPC.init(0);
  planOPC.init(0);
  LOAC.init(0);

//State Pin Shutdown Initialization
  pinMode(LS_PD, OUTPUT);                                              //This pin will allow for a transistor gate to be
                                                                       //opened, triggering LOAC shutdown.

//LED Initialization- sets all LED pins to output mode
  pinMode(sdLED, OUTPUT);
  pinMode(fixLED, OUTPUT);
  pinMode(stateLED, OUTPUT);

//Active Heating Initialization- starts temperature sensors
  temperature1.begin();
  temperature2.begin();
  temperature3.begin();
  
//Serial Initialization
  Serial1.begin(1200);                                                 //Initializes HASP serial port at 1200 baud.      
  Serial2.begin(4800);                                                 //Initializes serial port for GPS communication
  //Serial5.begin(9600);                                               //Initializes serial port for Plantower
  pmsSerial.begin(9600);                                               //Alternate serial for Plantower

//Data Log Initialization
  Serial.print("Initializing SD card...");                             //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization Failed!");
  }
  Serial.println("Initialization done.");                              //This "for" loop checks to see if there are any previous files on
  for (int i = 0; i < 100; i++) {                                      //the SD card already with the generated name
    
    Fname = String("fLog" + String(i/10) + String(i%10) + ".csv");     //has new name for file to make sure file name is not already used
    if (!SD.exists(Fname.c_str())){                                    //does not run if same SD name exists
      break; 
    }
  }

  Serial.println("System Log created: " + Fname);                 
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  header = "ntot,millis,3,5,10,25,50,100,time,GPS lat,GPS long,GPS alt,T Outside,T Inside,T OPC,OPC State";       
  fLog.println(header);                                                           //Set up temp log format and header
  fLog.close();
  Serial.println("System Log header added");

//Plantower Initialization                                                        //This is currently unused, as the log files have been
/*                                                                                //collapsed into a single file.
  Serial.println("Hello, there.");                                            
  Serial.println();
  Serial.println("Setting up Plantower OPC...");
  Serial.println();                                                    

  Serial.print("Initializing SD card...");
  // Check if card is present/initalized: 
  if (!SD.begin()){
  Serial.println("card initialization FAILED - something is wrong...");           //Card not present or initialization failed
  return;                                                                         //Don't do anything more                                         
  }
  
  Serial.println("card initialization PASSED");                                   //Initialization successful

  // Initialize file:
  ptLog = SD.open(filename.c_str(), FILE_WRITE);                                  //Open file
  
  if (ptLog) {
    Serial.println( filename + " opened...");
    ptLog.close();
    Serial.println("File initialized... begin data logging!");
  }
  else {
    Serial.println("error opening file");
    return;
  } */
}






//---ACTIVE CODE---\\





void loop() {
  systemUpdate();                                                                 //This function will update the full loop
}

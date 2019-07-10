//SHADOMS Payload Flight Computer Test
//Libraries
  #include <SD.h>                   //Writing to SD card
  #include <OneWire.h>              //Dallas data control
  #include <DallasTemperature.h>    //Dallas temperature control
  #include <TinyGPS++.h>            //GPS control
  #include <LatchRelay.h>           //Relay control

//Pin Definitions
  #define sdLED 23                  //LED pin which blinks to indicates data logging to the SD
  #define fixLED 22                 //LED pin which blinks to indicate GPS fix
  #define stateLED 21               //LED pin which blinks to indicate an active data recording status
  #define wireBus1 29               //Temperature sensor 1 pin - External Ambient
  #define wireBus2 30               //Temperature sensor 2 pin - OPC
  #define wireBus3 31               //Temperature sensor 3 pin - PCB
  #define heater_ON 24              //Heater relay pins
  #define heater_OFF 25             //^^^
  #define alphaOPC_ON 5             //Alphasense OPC relay pins
  #define alphaOPC_OFF 6            //^^^
  #define planOPC_ON 26             //Plantower OPC relay pins
  #define planOPC_OFF 27            //^^^
  #define LOAC_ON 7                 //LOAC OPC power relay pins
  #define LOAC_OFF 8                //^^^
  #define LS_PD 35                  //LOAC state shutdown transistor
  #define LOAC_FAN 32               //LOAC fan pin
  
//Serial Pins
/*  
  #define HASP_RX 0                 //HASP Recieve Pin                SERIAL 1
  #define HASP_TX 1                 //HASP Transmission Pin
  #define GPS_RX 9                  //GPS Recieve Pin                 SERIAL 2
  #define GPS_TX 10                 //GPS Transmission Pin
  #define PMS_RX 34                 //PMS Recieve Pin                 SERIAL 5
  #define PMS_TX 33                 //PMS Transmission Pin (Unused) 
*/
 
//Constant Definitions
  #define LOG_RATE 7000             //These definitions are the rates of the individual portions of the
  #define UPDATE_RATE 1             //systemUpdate function.
  #define FIXLED_LOOP 15000         //FixLED cycles every 15 seconds
  #define FIXLED_RATE 500           //FixLED is held on for 0.5 seconds
  #define NOFIXLED_RATE 3000        //When there is no GPS fix, FixLED cycles is held on for 3 seconds
  #define DWN_BYTES 54              //Number of downlink bytes + 1 (the +1 makes it work)
  #define COLD 280.0                //Minimum acceptable temperature of the OPC for active heating
  #define HOT 290.0                 //Maximum acceptable temperature of the OPC for active cooling
  #define KELVIN 273.15             //Number to convert Celcius to Kelvin
  //ADD TO MAIN
  #define MIN_FANTEMP 300.0         //Minimum acceptable temperature of the OPC for active cooling
  #define MAX_TEMP 314.5            //Maximum operating temperature of the OPC
  #define MIN_TEMP 244.5            //Minimum operating temperature of the OPC
  #define OVERRIDE_TIME 1500000     //Time of the override command


//Relay Definitions
  LatchRelay heater(heater_ON, heater_OFF);             //Define heater relay object
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  LatchRelay LOAC(LOAC_ON, LOAC_OFF);                   //Define LOAC OPC power relay object
  bool dataCollection = false;
  
//Thermal Control Definitions
  bool coldOPC = false;                                 //cold and hot trigger active thermal control
  //ADD TO MAIN
  bool hotOPC = false;
  bool danger = false;                                  //danger runs an emergency shutdown of the OPC system
  bool overRide = false;                                //manual system override mechanism
  unsigned long overrideTimer = 0;
  char fanState = '0';                                  //Defines the stae of the LOAC fan. "0" means off, "1" means on
 
//Temperature Sensor Definitions
  OneWire oneWire1(wireBus1);                           //Temperature sensor 1 data interface
  OneWire oneWire2(wireBus2);                           //Temperature sensor 2 data interface
  OneWire oneWire3(wireBus3);                           //Temperature sensor 3 data interface
  DallasTemperature temperature1(&oneWire1);            //Temperature sensor 1
  DallasTemperature temperature2(&oneWire2);            //Temperature sensor 2
  DallasTemperature temperature3(&oneWire3);            //Temperature sensor 3
  float t1;                                             //Temperature 1 (outside)
  float t2;                                             //Temperature 2 (OPC)
  float t3;                                             //Temperature 3 (PCB)
  //String HS = "0";                                    //Heater Pad status (1 = on)
  
//Serial Definitions (HASP Communication)
  String flightState = "";
  String OPCState = "";
  String dataPacket = "";
  byte packet[DWN_BYTES] = {0};                         //50 char/bytes in the string (54 with checksum)
  
//Data Log Definitions
  const int chipSelect = BUILTIN_SDCARD;                //Access on board micro-SD
  File fLog;                                            //This part of the code establishes the file and
  String data;                                          //sets up the CSV format
  String Fname = "";                                    //This is the name of the CSV file
  String header = "";                                   //This is the head of the CSV file
  bool sdLogging = false;                               //This variable is used to indicate when logging occurs
  
//Plantower Definitions
  String dataLog = "";                                  //Used for data logging
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
    uint16_t particles_03um, particles_05um, particles_10um;
    uint16_t particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
  } planData;                                           //This struct will organize the plantower bins into usable data
  
//GPS Definitions
  TinyGPSPlus GPS;                                      //GPS object definition

//FlightChecker() variables
  bool inFlight = false;                                //Bool that determines if the payload is in flight. Used with FlightCheck function
  unsigned long flightStart = 0;                        //Time passed since inFlight became true
  byte FlightCheckCounter = 0;                          //If this reaches 5, then inFLight should be set to true

//Variables for UpdateGPS()
  unsigned long lastGPS = 0;                            //Time in seconds since the last GPS update
  unsigned long GPSstartTime = 0;                       //When the GPS starts, time in seconds of last GPS update
  uint8_t days = 0;                                     //If we're flying overnight this serves as a coutner for time keeping

//timers and other variables for fixLED
  unsigned long fixLED_loop_timer = 0;                  //timer to ensure that the fixLED will do "something" every 10 second loop
  bool fixLEDon = false;                                //indicates if the fixLED is on or not
  bool fixLEDshort = false;                             //indicates what LED flash sequence to follow
 
//strings that populate GPS data strings
  String GPSdata = "";                                  //Initializes data string that prints GPS data to the SD card
  String flightstring = "False";                        //Indicates if inFlight is active or not
  String faillatitude = "0.00";                         //Printed latitude if GPS does not have a fix or any data
  String faillongitude = "0.000000";                    //Printed longitude if GPS does not have a fix or any data
  String failalt = "0.000000";                          //Printed altitude if GPS does not have a fix or any 

//LED Definitions
  bool fixLight = false;                                //These booleans are for the light activation and deactivation logic  
  bool stateLight = false;

//Updater Definitions                                   //These values help to regulate the speed of the updater function
  unsigned long lastCycle = 0;
  unsigned long planCycle = 0;
  
  bool powerTest = false;
  bool resetTest = false;
  bool testEnd = false;
  char heatState = '0';



//---ACTIVE CODE---\\                              



void setup() {                                       
  Serial.begin(9600);
 // while (!Serial) ;
  Serial.println("USB Serial initialized.");
  systemInit();                                         //This will initalize the system
    digitalWrite(fixLED, HIGH);
    digitalWrite(sdLED, HIGH);
    digitalWrite(stateLED, HIGH); 
    delay (1000);
    digitalWrite(fixLED, LOW);
    digitalWrite(sdLED, LOW);
    digitalWrite(stateLED, LOW); 
    delay(1000);
    digitalWrite(fixLED, HIGH);
    digitalWrite(sdLED, HIGH);
    digitalWrite(stateLED, HIGH); 
    delay (1000);
    digitalWrite(fixLED, LOW);
    digitalWrite(sdLED, LOW);
    digitalWrite(stateLED, LOW); 
    activeMode();
}

void loop() {
  if (!testEnd) systemUpdate();                         //This function will update the full loop
  /*
    if ((millis()>=45000)&&(!powerTest)){
    Serial.println("START FULL POWER TEST");
    powerTest = true;
    heater.setState(1);
    digitalWrite(fixLED, HIGH);
    digitalWrite(sdLED, HIGH); m
    digitalWrite(stateLED, HIGH); 
    delay(20000);
    heater.setState(0);
    digitalWrite(fixLED, LOW);
    digitalWrite(sdLED, LOW);
    digitalWrite(stateLED, LOW); 
    Serial.println("END FULL POWER TEST");  
  }
/*
  if ((millis()>=50000)&&(!resetTest)){
    Serial.println("START RESET TEST");
    resetTest = true;
    systemReset();
    Serial.println("END RESET TEST");
  }
  */
  if (millis()>=1800000){  //28800000
    standbyMode(); 
    Serial.println("30 minute test completed, or test manually terminated. Safe to remove power.");
    testEnd = true;
    digitalWrite(stateLED, LOW);
  }
}

//HASP SHADOMS Payload Flight Computer
//Version 1.0

/*This code operates the Teensy 3.6 Microcontroller on the 2019 HASP flight.
The goal of this flight is to collect data from the Plantower PMS-5003, the Alphasense OPC N3,
and the LOAC-R. Then, this data will be compared to examine how each particle detector operates
in the conditions of the upper atmosphere. This examination will be utilized to determine counter
effectiveness and calibration needs for future MURI research.

The code operates the particle counter power, writes the data of the Plantower to disk,
regulates temperature using active heating, runs a Coperinicus GPS, and controls uplink
and downlink with the main HASP gondola.

In this setup, both the Alphasense and the LOAC are running in a standalone mode where they 
record their own data.*/

//Last Modified: Nathan Pharis, 5/31/19 10:48
//Last Modification: Updated Pseudocode and OPC commands





/////ESTABLISH PREREQUISITE DEFINITIONS\\\\\





//Libraries
  #include <SD.h>                   //Writing to SD card
  #include <SPI.h>                  //Serial connections
  #include <OneWire.h>              //Dallas data control
  #include <DallasTemperature.h>    //Dallas temperature control
  #include <TinyGPS++.h>            //GPS control
  #include <LatchRelay.h>           //Relay control

//Pin Definitions
  #define chipSelect #              //SD card pin
  #define powerLED #                //LED pin which blinks to indicates power*****
  #define fixLED #                  //LED pin which blinks to indicate GPS fix*****
  #define stateLED #                //LED pin which blinks to indicate an active data recording status******
  #define wireBus1 #                //Temperature sensor 1 pin
  #define wireBus2 #                //Temperature sensor 2 pin
  #define wireBus3 #                //Temperature sensor 3 pin
  #define heater_ON #               //Heater relay pins
  #define heater_OFF #              //^^^
  #define alphaOPC_ON #             //Alphasense OPC relay pins
  #define alphaOPC_OFF #            //^^^
  #define planOPC_ON #              //Plantower OPC relay pins
  #define planOPC_OFF #             //^^^
  #define LOAC_ON #                 //LOAC OPC power relay pins
  #define LOAC_OFF #                //^^^
  #define stateLOAC_ON #            //LOAC OPC state relay pins
  #define stateLOAC_OFF #           //^^^
  
//Relay Definitions
  LatchRelay heater(heater_ON, heater_OFF);             //Define heater relay object
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  LatchRelay LOAC(LOAC_ON, LOAC_OFF);                   //Define LOAC OPC power relay object
  LatchRelay stateLOAC(stateLOAC_ON,stateLOAC_OFF);     //Define LOAC OPC state relay object
  
//Active Heating Definitions
  float cold =  280.0;              //Minimum acceptable temperature of the OPC
  float hot = 290.0;                //Maximum acceptable temperature of the OPC
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
  
//Data Log Definitions
  //Log Plantower, temperature, GPS, HASP data
  File flightLog;
  String data;
  String header = "Time, HASP pressure, HASP temp, HASP GPS, GPS, Temperature outside, T inside, T OPC";
  bool SDcard = true;

//GPS Definitions
  TinyGPSPlus GPS;                  //GPS object definition
  bool inFlight = false;
  unsigned long flightStart;

//LED Definitions
  //Establish patterns for different events





/////INITIALIZE CODE\\\\\





void setup() {
//Relay Initialization- sets up all relays, sets them all to an open state.
  heater.init(0);
  alphaOPC.init(0);
  planOPC.init(0);
  LOAC.init(0);
  stateLOAC.init(0);

//LED Initialization- sets all LED pins to output mode
  pinMode(powerLED, OUTPUT);
  pinMode(fixLED, OUTPUT);
  pinMode(stateLED, OUTPUT);

//Active Heating Initialization- starts temperature sensors
  temperature1.begin();
  temperature2.begin();
  temperature3.begin();
  
//Serial Initialization
  //HASP Stuff

//Data Log Initialization
  pinMode(chipSelect, OUTPUT);
  //Other stuff too

//GPS Initialization
  //Copernicus stuff

}





/////ACTIVE CODE\\\\\





void loop() {
//Update Serial
//Update Sensors
//Update Data Log
}

//THESE NEED TO BE SEPARATE FUNCTIONS, MAYBE ONE OF THE NEW LIBRARIES?

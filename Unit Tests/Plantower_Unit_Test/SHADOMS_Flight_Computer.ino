//HASP SHADOMS Payload Flight Computer
//Version 1.1

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

//Written in June 2019





//---ESTABLISH PREREQUISITE DEFINITIONS---\\





//Libraries
  #include <SD.h>                   //Writing to SD card
  #include <SPI.h>                  //Serial connections

//Pin Definitions
  #define sdLED 22                  //LED pin which blinks to indicates SD*****
  #define fixLED 21                 //LED pin which blinks to indicate GPS fix*****
  #define stateLED 20               //LED pin which blinks to indicate an active data recording status******
  #define alphaOPC_ON 5             //Alphasense OPC relay pins
  #define alphaOPC_OFF 6            //^^^
  #define planOPC_ON 26             //Plantower OPC relay pins
  #define planOPC_OFF 27            //^^^
  #define PMS_RX 34                 //PMS Recieve Pin                 SERIAL 5
  #define PMS_TX 33                 //PMS Transmission Pin

//Constant Definitions
  #define UPDATE_RATE 1000                               //These definitions are the rates of the individual portions of the

//Relay Definitions
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  bool dataCollection = false;
  
//Serial Definitions
  //HASP Uplink and downlink data
  String flightState = "";
  String OPCState = "";
  byte packet[32] = {0};
  
//Data Log Definitions
  //Log Plantower, temperature, GPS, HASP data
  const int chipSelect = BUILTIN_SDCARD;                //Access on board micro-SD
  File fLog;                                            //This part of the code establishes the file and
  String data;                                          //sets up the CSV format.
  String Fname = "";
  String header = "Time, GPS, T outside, T inside, T OPC";
  bool SDcard = true;

//Plantower Definitions
  String dataLog;                                        // used for data logging
  int nhits=1;                                           // used to count successful data transmissions    
  int ntot=1;                                            // used to count total attempted transmissions
  String filename = "ptLog.csv";                         // file name that data wil be written to
  File ptLog;                                            // file that data is written to 
  //SoftwareSerial pmsSerial(PMS_RX,PMS_TX);             //serial comms software                         
  struct pms5003data {
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
  };
  pms5003data planData;                            //This struct will organize the plantower bins into seperate parts of the data
  
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
  alphaOPC.init(0);
  planOPC.init(0);

//LED Initialization- sets all LED pins to output mode
  pinMode(sdLED, OUTPUT);
  pinMode(fixLED, OUTPUT);
  pinMode(stateLED, OUTPUT);

//Serial Initialization
  Serial5.begin(9600);                                                 //Initializes serial port for Plantower

//Data Log Initialization
  while(!Serial){                                                      //Wait for serial port to connect
    ;
  }

  Serial.print("Initializing SD card...");                             //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization Failed!");
    SDcard = false;
  }
  SDcard = true;
  Serial.println("Initialization done.");                              //This "for" loop checks to see if there are any previous files on
  for (int i = 0; i < 100; i++) {                                      //the SD card already with the generated name
    
    Fname = String("fLog" + String(i/10) + String(i%10) + ".csv");     //has new name for file to make sure file name is not already used
    if (!SD.exists(Fname.c_str())){                                    //does not run if same SD name exists
      break; 
    }
  }
  
  Serial.println("Temperature Logger created: " + Fname);                 
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  String FHeader = "time, T Outside,T Inside,T OPC";                        //temperature headers with time
  fLog.println(FHeader);                                                    //Set up temp log format and header
  fLog.close();
  Serial.println("Temp Logger header added");

//Plantower Initialization
  Serial.println("Hello, there.");                                            
  Serial.println();
  Serial.println("Setting up Plantower OPC...");
  Serial.println();                                                    

  Serial.print("Initializing SD card...");
  // Check if card is present/initalized: 
  if (!SD.begin()){
  Serial.println("card initialization FAILED - something is wrong...");       //Card not present or initialization failed
  return; // dont do anything more                                         
  }
  
  Serial.println("card initialization PASSED");                                //Initialization successful

  // Initialize file:
  ptLog = SD.open(filename.c_str(), FILE_WRITE);                               // open file
  
  if (ptLog) {
    Serial.println( filename + " opened...");
    ptLog.close();
    Serial.println("File initialized... begin data logging!");
  }
  else {
    Serial.println("error opening file");
    return;
  }



//---ACTIVE CODE---\\





void loop() {
  systemUpdate();
}

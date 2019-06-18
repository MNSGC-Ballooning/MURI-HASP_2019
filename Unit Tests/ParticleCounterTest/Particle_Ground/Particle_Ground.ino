//Particle Counter Unit Test
/*This code, written for Teensy 3.5/3.6, will run a Plantower PMS-5003, an Alphasense OPC-N3,
and a LOAC-R. The program will run the particle counters and collect data for 20 minutes.*/



//DEFINTIONS\\



//Libraries
  #include <SD.h>                   //Writing to SD card
  #include <LatchRelay.h>           //Relay control

//Define Pinouts
  #define alphaOPC_ON 5             //Alphasense OPC relay pins
  #define alphaOPC_OFF 6            //^^^
  #define planOPC_ON 26             //Plantower OPC relay pins
  #define planOPC_OFF 27            //^^^
  #define LOAC_ON 7                 //LOAC OPC power relay pins
  #define LOAC_OFF 8                //^^^
 //#define LS_ON #                  //Powerdown for LOAC State
 //#define LS_OFF #                 //End powerdown for LOAC State
  #define PLAN_RATE 2300            //Plantower data collection rate
  #define TEST_TIME 1200000         //Test time for 20 minutes

//Relay Object Definitions
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  LatchRelay LOAC(LOAC_ON, LOAC_OFF);                   //Define LOAC OPC power relay object
  //LatchRelay loacState(LS_ON, LS_OFF)                 //Define LOAC State shutdown relay object

//Plantower Defintions
  const int chipSelect = BUILTIN_SDCARD;                 //Access on board micro-SD
  String dataLog;                                        // used for data logging
  int nhits=1;                                           // used to count successful data transmissions    
  int ntot=1;                                            // used to count total attempted transmissions
  String filename = "ptLog.csv";                         // file name that data wil be written to
  File ptLog;                                            // file that data is written to 
  unsigned long planCycle = 0;
  bool SDcard = false;
                  
  struct pms5003data {
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
  };
  pms5003data planData;                                  //This struct will organize the plantower bins into seperate parts of the data



//SETUP\\



void setup() {
//Relay Initialization- sets up all relays, sets them all to an open state.
  alphaOPC.init(1);
  planOPC.init(1);
  LOAC.init(1);
  //loacState.init(0);

//Serial Initialization
  Serial5.begin(9600);                                                 //Initializes serial port for Plantower

//Data Log
    while(!Serial){                                                      //Wait for serial port to connect
    ;
  }

  Serial.print("Initializing SD card...");                             //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization Failed!");
    SDcard = false;
  }
  SDcard = true;
  Serial.println("Initialization done.");

  Serial.println("Hello, there.");                                            
  Serial.println();
  Serial.println("Setting up Plantower OPC...");
  Serial.println();                                                    

  Serial.print("Initializing SD card...");
  // Check if card is present/initalized: 
  if (!SD.begin()){
  Serial.println("card initialization FAILED - something is wrong...");        //Card not present or initialization failed
  return;                                                                      //Don't do anything more                                         
  }
  
  Serial.println("card initialization PASSED");                                //Initialization successful

  // Initialize file:
  ptLog = SD.open(filename.c_str(), FILE_WRITE);                               //Open file
  
  if (ptLog) {
    Serial.println( filename + " opened...");
    ptLog.close();
    Serial.println("File initialized... begin data logging!");
  }
  else {
    Serial.println("error opening file");
    return;
  }
}



//System Operation\\



void loop() {
  if (millis() - planCycle >= PLAN_RATE) {                                    //This regulates the plantower loop to every 2.3 seconds.
    planCycle=millis();

    pmsUpdate();                                                              //This will update the plantower data log.
  }

  if (millis() >= TEST_TIME) {
   alphaOPC.setState(0);
   planOPC.setState(0);
   //loacState.setState(1);                                                     //To turn off the LOAC, the recording state has to be
  // delay(12000);                                                              //shut down before it can be powered down. 
   //loacState.setState(0);
   LOAC.setState(0);            
   Serial.println("System shutdown complete. Test over.");   
   Serial.println("It is safe to turn off the device.");            
  }

}

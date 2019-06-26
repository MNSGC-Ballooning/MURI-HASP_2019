//Particle Counter Unit Test
/*This code, written for Teensy 3.5/3.6, will run a Plantower PMS-5003, an Alphasense OPC-N3,
and a LOAC-R. The program will run the particle counters and collect data for 20 minutes.*/



//DEFINTIONS\\



//Libraries
  #include <SD.h>                   //Writing to SD card
  #include <LatchRelay.h>           //Relay control
  #include <SoftwareSerial.h>       //Software serial communication

//Define Pinouts
  #define alphaOPC_ON 5             //Alphasense OPC relay pins
  #define alphaOPC_OFF 6            //^^^
  #define planOPC_ON 26             //Plantower OPC relay pins
  #define planOPC_OFF 27            //^^^
  #define LOAC_ON 7                 //LOAC OPC power relay pins
  #define LOAC_OFF 8                //^^^
  #define LS_PD 35                  //LOAC state shutdown transistor
 // #define PMS_RX 34                 //PMS Recieve Pin                 SERIAL 5/SOFTWARE SERIAL
 // #define PMS_TX 33                 //PMS Transmission Pin (Unused) 
  #define PLAN_RATE 100            //Plantower data collection rate
  #define TEST_TIME 1200000         //Test time for 20 minutes
  #define LED1 21
  #define LED2 22
  #define LED3 23

//Relay Object Definitions
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  LatchRelay LOAC(LOAC_ON, LOAC_OFF);                   //Define LOAC OPC power relay object
  bool systemoff = false;

//Plantower Defintions
  //SoftwareSerial pmsSerial(PMS_RX, PMS_TX);             //Sets object for software serial connection
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

//Serial Initialization
  Serial.begin(9600);
  while (!Serial) ;
  Serial.println("serial has begun.");
  //pmsSerial.begin(9600);                                               //Alternate serial for Plantower
  Serial5.begin(9600);                                                 //Initializes serial port for Plantower

//Pin activation
  pinMode(LS_PD, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

//Data Log

  Serial.println("Initializing SD card...");                             //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization Failed!");
  }
  Serial.println("Initialization done.");

  Serial.println("Hello, there.");                                            
  Serial.println();
  Serial.println("Setting up Plantower OPC...");
  Serial.println();                                                    

  Serial.println("Initializing SD card...");
  // Check if card is present/initalized: 
  //if (!SD.begin(chipSelect)){
 // Serial.println("card initialization FAILED - something is wrong...");        //Card not present or initialization failed
 // return;                                                                      //Don't do anything more                                         
 // }
  
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
  if ((millis() - planCycle >= PLAN_RATE)&&(!systemoff)) {                   //This regulates the plantower loop to every 2.3 seconds.
    digitalWrite(LED1, HIGH);
    planCycle=millis();
    Serial.println("data update."); 
    pmsUpdate();                                                              //This will update the plantower data log.
    digitalWrite(LED1, LOW);
  }

  if ((millis() >= TEST_TIME)&&(!systemoff)) {
   digitalWrite(LED3, HIGH);
   alphaOPC.setState(0);
   planOPC.setState(0);
   digitalWrite(LS_PD, HIGH);                                           //To turn off the LOAC, the recording state has to be
   delay(12000);                                                        //shut down before it can be powered down. 
   digitalWrite(LS_PD, LOW);
   LOAC.setState(0);            
   Serial.println("System shutdown complete. Test over.");   
   Serial.println("It is safe to turn off the device.");       
   digitalWrite(LED3, LOW);   
   systemoff = true;

  if (systemoff)
  {
   digitalWrite(LED2, HIGH);
   delay(750);
   digitalWrite(LED2, LOW);
   delay(750);
  }
     
  }

}

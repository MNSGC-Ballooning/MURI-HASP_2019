// Arduino code for Plantower (PT) opc operation.                                   //NOTE: PLease just use the prewritten library called OPCSensor in the OPC Library folder
//                                                                                  //NOTE: on MNSGC github. If you need access, contact Phari009@umn.edu or any member of the team.
// Summary: 
//  -- Prepares data for logging  
//  -- Bin numbers correspond to:
//     - bin 1 --> Particles > 0.3um / 0.1L air
//     - bin 2 --> Particles > 0.5um / 0.1L air
//     - bin 3 --> Particles > 1.0um / 0.1L air
//     - bin 4 --> Particles > 2.5um / 0.1L air
//     - bin 5 --> Particles > 5.0um / 0.1L air
//     - bin 6 --> Particles > 10.0um / 0.1L air
//
//=====================================


////////// BEGIN CODE //////////

void OPCInit(){                                                                    //This function initializes the OPC systems
  alphaOPC.init(0);                                                                //Sets all relays to an open state
  planOPC.init(0);
  LOAC.init(0);     
  pinMode(LS_PD, OUTPUT);                                                          //This pin will allow for a transistor gate to be opened, triggering LOAC shutdown.
  pinMode(stateLED, OUTPUT);                                                       //Initializes state LED signal
  Serial5.begin(9600);                                                             //Initializes serial port for Plantower
  while (!Serial5) ;                                                               //Waits for Serial 5 to connect
}

void pmsUpdate() {
    dataLog = "";                                                                  //Log sample number, in flight time
    dataLog += ntot;
    dataLog += ",";
    dataLog += flightTime();                                                       //In flight time from Flight_Timer 
    dataLog += ",";
    
  if (readPMSdata(&Serial5)) {
    dataLog += planData.particles_03um;                                            //If data is in the buffer, log it
    dataLog += ",";
    dataLog += planData.particles_05um;
    dataLog += ",";
    dataLog += planData.particles_10um;
    dataLog += ",";
    dataLog += planData.particles_25um;
    dataLog += ",";
    dataLog += planData.particles_50um;
    dataLog += ",";
    dataLog += planData.particles_100um;
    
    nhits=nhits+1;                                                                 //Log sample number, in flight time
    
    ntot = ntot+1;                                                                 //Total samples

    goodLog = true;                                                                //If data is successfully collected, note the state;
    badLog = 0;

  } else {
    badLog++;                                                                      //If there are five consecutive bad logs, not the state;
    if (badLog >= 5){
      goodLog = false;
      dataLog += '%' + ',' + 'Q' + ',' + '=' + ',' + '!' + ',' + '@' + ',' + '$';
    }
  }
/*
// write data
    fLog = SD.open(filename.c_str(), FILE_WRITE);                                  //Open file

 if (fLog) {
    //Serial.println("tempLog.csv opened...");                                     //File open successfully 
    fLog.println(dataLog);
    fLog.close();
  }
  else {
    Serial.println("error opening file");                                          //File open failed
    return;
  }
  
// print data log to serial monitor
    Serial.println(dataLog); */
}

////////// USER DEFINED FUNCTIONS //////////

boolean readPMSdata(Stream *s) {                      
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];                                                              //This loads the data into a buffer;
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  for (uint8_t i=0; i<30; i++) {                                                   //Get checksum ready
    sum += buffer[i];
  }
 
 /*//debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
*/
  
  uint16_t buffer_u16[15];                                                         //The data comes in endian'd, this solves it so it works on all platforms
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  memcpy((void *)&planData, (void *)buffer_u16, 30);                               //Put it into a nice struct :)
 
  if (sum != planData.checksum) {
//    Serial.println("Checksum failure");
    return false;
  }
  
  else {
  return true;                                                                     //Success!
  }
}

//Unused Initializations
/*
void PlanInit(){                                                                   //Unused independent Plantower initialization. The log files have been collapsed into a single file.
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
  } 
}
*/

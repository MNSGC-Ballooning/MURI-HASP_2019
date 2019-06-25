// Arduino code for Plantower (PT) opc operation.
//
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

/// begin loop
void pmsUpdate() {
//  digitalWrite(led,HIGH);                               //which LED is this??
//  FT = millis();

// log sample number, in flight time
    dataLog = "";
    dataLog += ntot;
    dataLog += ",";
    dataLog += flightTime();                              //in flight time from Flight_Timer 
    dataLog += ",";
    
  if (readPMSdata(&Serial5)) {

// if data is receieved, log it
    dataLog += planData.particles_03um;
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
    dataLog += ",";
    
// samples of data collected
    nhits=nhits+1;
    
// total samples
    ntot = ntot+1;

// write data
    ptLog = SD.open(filename.c_str(), FILE_WRITE);     //Open file

 if (ptLog) {
    //Serial.println("tempLog.csv opened...");         //File open successfully 
    ptLog.println(dataLog);
    ptLog.close();
  }
  else {
    Serial.println("error opening file");              //File open failed
    return;
  }
  
// print data log to serial monitor
    Serial.println(dataLog); 
  }

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
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
 /*//debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
*/
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&planData, (void *)buffer_u16, 30);
 
  if (sum != planData.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}

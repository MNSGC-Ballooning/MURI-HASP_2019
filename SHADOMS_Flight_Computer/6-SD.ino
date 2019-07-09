//Data Logging
void dataLogInit(){                                                      //Initializes the data log
  pinMode(sdLED, OUTPUT);                                                //Initializes SD LED
//  Serial.print("Initializing SD card...");                             //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {
//    Serial.println("Initialization Failed!");
  }
//  Serial.println("Initialization done.");                              //This "for" loop checks to see if there are any previous files on
  for (int i = 0; i < 100; i++) {                                        //the SD card already with the generated name
    
    Fname = String("fLog" + String(i/10) + String(i%10) + ".csv");       //has new name for file to make sure file name is not already used
    if (!SD.exists(Fname.c_str())){                                      //does not run if same SD name exists
      break; 
    }
  }

//  Serial.println("System Log created: " + Fname);                 
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  header = "ntot,millis,3,5,10,25,50,100,time,GPS lat,GPS long,";
  header += "GPS alt,T Outside,T OPC,T PCB,flight State,OPC State,heat State";       
  fLog.println(header);                                                  //Set up temp log format and header
  fLog.close();
// Serial.println("System Log header added");
}

void writeSensorsSD(){
  digitalWrite(sdLED, HIGH);
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  
  if(inFlight && planOPC.getState() == true)                             //the next two booleans typecast the booleans
  {                                                                      //with space for additional logic
    flightState = '1';                                                  
  }
  else
  {
    flightState = '0';
    dataLog = ",-,-,-,-,-,-,-,";
  }
  if(dataCollection && goodLog)
  {
    OPCState = '1';
  }
  else if (danger && !overRide)
  {
    OPCState = '2';
  }
  else
  {
    OPCState = '0';
  }
  
  data = dataLog+','+logTime()+','+printGPS()+','+String(t1)+','+String(t2);
  data +=','+String(t3)+','+flightState+','+OPCState+','+heatState;
 // Serial.println(data);
  fLog.println(data);                                                    //PMS and Sensor data log

 // Serial.println("Data line was added");
  fLog.close();
  digitalWrite(sdLED, LOW);
}

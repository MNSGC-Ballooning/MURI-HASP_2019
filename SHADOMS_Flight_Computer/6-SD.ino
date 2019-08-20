//Data Logging
void dataLogInit(){                                                      //Initializes the data log
  pinMode(sdLED, OUTPUT);                                                //Initializes SD LED
//  Serial.print("Initializing SD card...");                             //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {}                                          //This pauses the program until an SD card is input.                 NOTE: Bold, and foolish.
                                                                         //This "for" loop checks to see if there are any previous files on   NOTE: Nothing should stop the code.
  for (int i = 0; i < 100; i++) {                                        //the SD card already with the generated name                        NOTE: Add redundancy instead. Two
                                                                         //                                                                   NOTE: logs would be better.
    Fname = String("fLog" + String(i/10) + String(i%10) + ".csv");       //has new name for file to make sure file name is not already used
    if (!SD.exists(Fname.c_str())){                                      //does not run if same SD name exists
      break; 
    }
  }
               
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  header = "ntot,millis,3,5,10,25,50,100,time,date,real time,GPS lat,GPS long,GPS alt,";
  header +="T Outside,T OPC,T PCB,flight State,OPC State,heat State,fan State,danger";       
  fLog.println(header);                                                  //Set up temp log format and header
  fLog.close();
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
  
  data = dataLog+','+logTime()+','+getdate()+','+gettime()+','+printGPS()+','+String(t1)+','+String(t2);            //NOTE: The most issues seemed to happen here. Log everything,
  data +=','+String(t3)+','+flightState+','+OPCState+','+heatState+','+fanState+','+danger;                         //NOTE: and test it rigourously.

  fLog.println(data);                                                    //PMS and Sensor data log

  fLog.close();
  digitalWrite(sdLED, LOW);
}

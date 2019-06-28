//System Operation

//System Initialization
void systemInit(){                                      
  thermalInit();                                        //These functions run the systems initializations
  Serial.println("thermal initialized.");
  GPSInit();
  Serial.println("GPS initialized.");
  OPCInit();
  Serial.println("OPC initialized.");
  dataLogInit();
  Serial.println("Data Log initialized.");
  serialInit();
  Serial.println("Serial initialized.");
  Serial.println("System initialized.");
}

//System updater
void systemUpdate(){                                         
   if (millis() - planCycle >= UPDATE_RATE) {           //This regulates the plantower loop to every 2.3 seconds.
    planCycle=millis();
  Serial.println("UPDATE CYCLE");
    readPMSdata(&Serial5);                              //This will update the Plantower data 
   // updateGPS();                                        //This will update the GPS data
   // updateTemp();                                       //Update Temp
  }

 // fixLEDupdater();                                      //Runs the fixLED through loops depending on if it has a GPS fix or not 
  
  
  if (millis() - lastCycle >= LOG_RATE) {               //This regulates the loop, so that it runs consistently
    lastCycle=millis();                                 //at 1Hz. lastCycle gives the previous time, and
    Serial.println("LOG CYCLE");                                                   //millis gives the current time. Both are in ms.

    updateTemp();
    //GPS Flight Check
     flightCheck();                                     //The flight check variable will identify when the payload is in flight.
  
    //Data Log Update
    pmsUpdate();
     writeSensorsSD();

    //Serial Update
  //   Data_Downlink();                                   //Send down packet of data
  //   Read_Uplink_Command();                             //Check if command is sent then follow command

    //LED update
     if (dataCollection && !stateLight) {               //If the OPCs are powered and the light is not active,
      digitalWrite(stateLED, HIGH);                     //this will activate the light.
      stateLight = true;
     } else if (!dataCollection && stateLight){         //If the OPCs are not powered and the light is active,
      digitalWrite(stateLED, LOW);                      //this will deactivate the light.
      stateLight = false;
     }    
  }
}

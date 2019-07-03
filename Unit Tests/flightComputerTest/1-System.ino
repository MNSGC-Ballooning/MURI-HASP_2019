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
    readPMSdata(&Serial5);                              //This will update the Plantower data
  }

   if (millis() - fixLED_loop_timer >= FIXLED_LOOP) {
    Serial.println("New Fix LED loop.");
    fixLEDupdater();                                    //Runs the fixLED through loops depending on if it has a GPS fix or not 
  }

  if (fixLEDon) {                                     //Timer determines when to turn the fixLED off depending on boolean set in fixLEDupdater()
    if (fixLEDshort && (millis() - fixLED_loop_timer >= FIXLED_RATE)) {
      fixLEDturnoff();
    }
    else if (!fixLEDshort && (millis() - fixLED_loop_timer >= NOFIXLED_RATE)) {
      fixLEDturnoff();
    }
  }
  
  if (millis() - lastCycle >= LOG_RATE) {               //This regulates the loop, so that it runs consistently
    lastCycle=millis();                                 //at 1Hz. lastCycle gives the previous time, and
    //Serial.println("LOG CYCLE");                                                   //millis gives the current time. Both are in ms.

    updateGPS();                                        //This will update the GPS data
    updateTemp();                                       //Update Temp

    //GPS Flight Check
     flightCheck();                                     //The flight check variable will identify when the payload is in flight.
  
    //Data Log Update
     pmsUpdate();
     writeSensorsSD();

    //Serial Update
     Data_Downlink();                                   //Send down packet of data
     Read_Uplink_Command();                             //Check if command is sent then follow command

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

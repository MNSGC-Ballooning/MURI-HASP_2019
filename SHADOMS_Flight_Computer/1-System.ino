//System Operation

//System Initialization
void systemInit(){                                      
  thermalInit();                                        //These functions run the systems initializations
  GPSInit();
  OPCInit();
  dataLogInit();
  serialInit();
}

//System updater
void systemUpdate(){                                         
//Update Loop  
  if (millis() - planCycle >= UPDATE_RATE) {            //This regulates the rapid update loop every millisecond
    planCycle=millis();

    readPMSdata(&Serial5);
  }

//Fix Loop
 if (millis() - fixLED_loop_timer >= FIXLED_LOOP) {
    fixLEDupdater();                                    //Runs the fixLED through loops depending on if it has a GPS fix or not 
  }

  if (fixLEDon) {                                       //Timer determines when to turn the fixLED off depending on boolean set in fixLEDupdater()
    if (fixLEDshort && (millis() - fixLED_loop_timer >= FIXLED_RATE)) {
      fixLEDturnoff();
    }
    else if (!fixLEDshort && (millis() - fixLED_loop_timer >= NOFIXLED_RATE)) {
      fixLEDturnoff();
    }
  }
  
//Logging Loop  
  if (millis() - lastCycle >= LOG_RATE) {               //This regulates the loop, so that it runs consistently
    lastCycle=millis();                                 //every 7 seconds. lastCycle gives the previous time, and
                                                        //millis gives the current time. Both are in ms.
    //Temperature Update
     updateTemp();                                      //Update Temp
   
    //GPS Flight Check
     updateGPS();                                       //This will update the GPS data
     flightCheck();                                     //The flight check variable will identify when the payload is in flight.
  
    //Data Log Update
      pmsUpdate();                                      //This will update the Plantower data 
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

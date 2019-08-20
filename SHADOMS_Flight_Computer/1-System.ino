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
    updateGPS();
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
     ThermalControl();                                  //This will check temperatures of sensors and determine if any modes need to be activated
   
    //GPS Flight Check
     updateGPS();                                       //This will update the GPS data
     flightCheck();                                     //The flight check variable will identify when the payload is in flight.
  
    //Data Log Update
     pmsUpdate();                                       //This will update the Plantower data 
     writeSensorsSD();                                  //This will log the data to the SD card

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

///// Command Functions /////
void systemReset(){                                     //This will reset the system                   NOTE: There is nothing fancy here. It Turns the system off and back on.
  standbyMode();
  heater.setState(0);
  delay (1000);
  activeMode();
}


void activeMode(){                                      //This will activate all of the particle detectors.
  LOAC.setState(1);
  alphaOPC.setState(1);
  planOPC.setState(1);
  dataCollection = true;                                //This will enable storage of data from the particle counters

  if (!inFlight) {
    inFlight = true;                                    //If the "in flight" boolean has not been triggered, this will begin the flight.
    flightStart = millis();
  }
  
  if ((inFlight)&&(danger)) {                           //If active mode is called while the system is active, then the system will run
    overRide = true;                                    //an automatic override of the automatic shutdown for a given time.
    overrideTimer = millis();                           //                                            NOTE: I should have just added more commands, rather than making existing
  }                                                     //                                            NOTE: commands do more than one thing. Adding commands and downlink is
}                                                       //                                            NOTE: easy and can be done up until the end of integration.

void standbyMode(){                                     //This will shut down all of the particle detectors.
  dataCollection = false;
  alphaOPC.setState(0);
  planOPC.setState(0);
  digitalWrite(LS_PD, HIGH);                            //To turn off the LOAC, the recording state has to be
  delay(12000);                                         //shut down before it can be powered down. 
  digitalWrite(LS_PD, LOW);
  LOAC.setState(0); 
      
  if (overRide) {
    overRide = false;                                   //A manual shutdown will end the override state.
  }
}

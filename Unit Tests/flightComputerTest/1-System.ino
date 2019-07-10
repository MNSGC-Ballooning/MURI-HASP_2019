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

    //Update
    updateGPS();                                        //This will update the GPS data
    updateTemp();                                       //This will update the temperature
    pmsUpdate();                                        //This will write the plantower data string
    
    //Thermal Systems
    ThermalControl();                                   //This will ensure that the system has not reached a dangerous temperature

    //GPS Flight Check
    flightCheck();                                      //The flight check variable will identify when the payload is in flight.
  
    //Data Log Update
    writeSensorsSD();                                   //This will update the PMS data log and then write the data to an SD

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
void systemReset(){                                                      //This will reset the system
  standbyMode();
  Serial.println("Shutdown successful.");
  heater.setState(0);
  delay(1000);
  activeMode();
  Serial.println("Activation successful.");
}

void activeMode(){                                                      //This will activate all of the particle detectors.
  LOAC.setState(1);
  alphaOPC.setState(1);
  planOPC.setState(1);
  dataCollection = true;                                                //This will enable storage of data from the particle counters
  if (!inFlight) inFlight = true;                                       //If the "in flight" boolean has not been triggered, this will begin the flight.
  //ADD MAIN
  if ((inFlight)&&(danger)) {                                           //If active mode is called while the system is active, then the system will run
    overRide = true;                                                    //an automatic override of the automatic shutdown for a given time.
    overrideTimer = millis();
    Serial.println("Override mode active");
  }
  
  testEnd = false;
  Serial.println("System Active.");
}

void standbyMode(){                                                     //This will shut down all of the particle detectors.
  dataCollection = false;
  alphaOPC.setState(0);
  planOPC.setState(0);
  digitalWrite(LS_PD, HIGH);                                           //To turn off the LOAC, the recording state has to be
  delay(12000);                                                        //shut down before it can be powered down. 
  digitalWrite(LS_PD, LOW);
  LOAC.setState(0);     
  if (overRide){
    overRide = false;                                      //A manual shutdown will end the override state.
    Serial.println("Override mode inactive");
  }
}

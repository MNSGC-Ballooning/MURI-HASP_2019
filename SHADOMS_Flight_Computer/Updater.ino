//Loop Updater
 

void systemUpdate(){                                   
   if (millis() - planCycle >= PLAN_RATE) {             //This regulates the plantower loop to every 2.3 seconds.
    planCycle=millis();

    pmsUpdate();                                        //This will update the plantower data log string.
  }

  fixLEDupdater();                                      //Runs the fixLED through loops depending on if it has a GPS fix or not 
  
  
  if (millis() - lastCycle >= UPDATE_RATE) {            //This regulates the loop, so that it runs consistently
    lastCycle=millis();                                 //at 1Hz. lastCycle gives the previous time, and
                                                        //millis gives the current time. Both are in ms.
    //Temperature Update
    temperature1.requestTemperatures();                 //Request temperatures from the sensors.
    temperature2.requestTemperatures();
    temperature3.requestTemperatures();
    t1 = temperature1.getTempCByIndex(0) + KELVIN;      //Pull the temperatures from the objects,
    t2 = temperature2.getTempCByIndex(0) + KELVIN;      //and converts those temperatures from degrees Celcius
    t3 = temperature3.getTempCByIndex(0) + KELVIN;      //to Kelvin.

    activeHeat();

    //GPS Update
    updateGPS();                                        //Internal GPS clock checks to see if it has rolled over from
                                                        //one day to another and accounts for it if so.

    //For logging GPS data, need to use SDprintGPS(), which is a void function that prints GPS data to SD as a string
    //The print() function in SDprintGPS() can be removed if need be, and the function can serve to just change the
    //data string, which could be printed in a different function if that makes things easier.
  
     flightCheck();                                     //The flight check variable will identify when the payload is in flight.
  
     //Data Log Update

     writeSensorsSD();

     //Serial Update

     Data_Downlink();                                   //Send down packet of data
     Read_Uplink_Command();                             //Check if command is sent then follow command

     //LED updates

     if (dataCollection && !stateLight) {               //If the OPCs are powered and the light is not active,
      digitalWrite(stateLED, HIGH);                     //this will activate the light.
      stateLight = true;
     } else if (!dataCollection && stateLight){         //If the OPCs are not powered and the light is active,
      digitalWrite(stateLED, LOW);                      //this will deactivate the light.
      stateLight = false;
     }

     
  }
}

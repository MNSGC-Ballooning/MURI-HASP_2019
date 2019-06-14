//Loop Updater

void systemUpdate(){                                    //This regulates the loop, so that it runs consistently
  if (millis() - lastCycle >= UPDATE_RATE) {            //at 1Hz. lastCycle gives the previous time, and
    lastCycle=millis();                                 //millis gives the current time. Both are in ms.

    //Temperature Update
    temperature1.requestTemperatures();                 //Request temperatures from the sensors.
    temperature2.requestTemperatures();
    temperature3.requestTemperatures();
    t1 = temperature1.getTempCByIndex(0) + 273.15;      //Pull the temperatures from the objects,
    t2 = temperature2.getTempCByIndex(0) + 273.15;      //and converts those temperatures from degrees Celcius
    t3 = temperature3.getTempCByIndex(0) + 273.15;      //to Kelvin.

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

     if (sdLogging && !sdLight) {                       //If the data is being logged and the light is not active,
      digitalWrite(sdLED, HIGH);                        //this will activate the light.
      sdLight = true;
     } else if (!sdLogging && sdLight){                 //If data is not being logged and the light is active,
      digitalWrite(sdLED, LOW);                         //this will deactivate the light.
      sdLight = false;
     }  

     if (GPS.Fix && !fixLight) {                        //If the GPS has a fix and the light is not active, this
      digitalWrite(fixLED, HIGH);                       //will activate the light.
      fixLight = true;
     } else if (!GPS.Fix && fixLight){                  //If the GPS does not have a fix and the light is active,
      digitalWrite(fixLED, LOW);                        //this will deactivate the light.
      fixLight = false;
     }
  }

  if (millis() - planCycle >= PLAN_RATE) {             //This regulates the plantower loop to every 2.3 seconds.
    planCycle=millis();

    pmsUpdate();                                       //This will update the plantower data log.
  }
}

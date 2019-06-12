//Loop Updater

void systemUpdate(){                                    //This regulates the loop, so that it runs consistently
  if (millis() - lastCycle >= UPDATE_RATE) {            //at 1Hz. lastCycle gives the previous time, and
    lastCycle=millis();                                 //millis gives the current time. Both are in ms.


     //Data Log Update

     writeSensorsSD();

     //Serial Update

     Data_Downlink(); // send down packet of data
     Read_Uplink_Command(); // check if command is sent then follow command

     //LED updates

     if (dataCollection && !stateLight) {               //If the OPCs are powered and the light is not active,
      digitalWrite(stateLED, HIGH);                     //this will activate the light.
      stateLight = true;
     } else if (!dataCollection && stateLight){         //If the OPCs are not powered and the light is active,
      digitalWrite(stateLED, LOW);                      //this will deactivate the light.
      stateLight = false;
     }

     if (SDcard && !sdLight) {                          //If the data is being logged and the light is not active,
      digitalWrite(sdLED, HIGH);                       //this will activate the light.
      sdLight = true;
     } else if (!SDcard && sdLight){                    //If data is not being logged and the light is active,
      digitalWrite(sdLED, LOW);                        //this will deactivate the light.
      sdLight = false;
     }  
  }

  if (millis() - planCycle >= PLAN_RATE) {             //This regulates the plantower loop to every 2.3 seconds.
    planCycle=millis();

    pmsUpdate();
  }
}

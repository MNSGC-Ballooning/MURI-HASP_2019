//Loop Updater

//Update Serial
//Update Temperature Sensors
//Update GPS
//Update Data Log

void systemUpdate(){
  static unsigned long lastCycle = 0;                   //This regulates the loop, so that it runs consistently
  if (millis() - lastCycle >= 1000) {                   //at 1Hz. lastCycle gives the previous time, and
    lastCycle=millis();                                 //millis gives the current time. Both are in ms.

    //Serial Update
   
    //Temperature Update
    temperature1.requestTemperatures();                 //Lines 14-16 request temperatures from the sensors.
    temperature2.requestTemperatures();
    temperature3.requestTemperatures();
    t1 = temperature1.getTempCByIndex(0) + 273.15;      //Lines 17-19 pull the temperatures from the objects,
    t2 = temperature2.getTempCByIndex(0) + 273.15;      //and converts those temperatures from degrees Celcius
    t3 = temperature3.getTempCByIndex(0) + 273.15;      //to Kelvin.

    activeHeat();
    
     //GPS Update

     //PJ ADD UPDATE GPS FUNCTION
  
     if (GPSfix && !fixLight) {                         //If the GPS has a fix and the light is not active, this
      digitalWrite(fixLED, HIGH);                       //will activate the light.
      fixLight = true;
     } else if (!GPSfix && fixLight){                   //If the GPS does not have a fix and the light is active,
      digitalWrite(fixLED, LOW);                        //this will deactivate the light.
      fixLight = false;
     }

     //Data Log Update



     if (dataCollection && !stateLight) {               //If the OPCs are powered and the light is not active,
      digitalWrite(stateLED, HIGH);                     //this will activate the light.
      stateLight = true;
     } else if (!dataCollection && stateLight){         //If the OPCs are not powered and the light is active,
      digitalWrite(stateLED, LOW);                      //this will deactivate the light.
      stateLight = false;
     }

     if (SDlog && !sdLight) {                          //If the data is being logged and the light is not active,
      digitalWrite(sdLED, HIGH);                       //this will activate the light.
      sdLight = true;
     } else if (!SDlog && sdLight){                    //If data is not being logged and the light is active,
      digitalWrite(sdLED, LOW);                        //this will deactivate the light.
      sdLight = false;
     }  
  }
}

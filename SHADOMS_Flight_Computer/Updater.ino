//Loop Updater

//Update Serial
//Update Temperature Sensors
//Update GPS
//Update Data Log

void systemUpdate(){
  static unsigned long lastCycle = 0;
  if (millis() - lastCycle >= 1000) {
    lastCycle=millis();
   
    //Temperature Update
    temperature1.requestTemperatures();
    temperature2.requestTemperatures();
    temperature3.requestTemperatures();
    t1 = temperature1.getTempCByIndex(0) + 273.15;
    t2 = temperature2.getTempCByIndex(0) + 273.15;
    t3 = temperature3.getTempCByIndex(0) + 273.15; 
  }
}

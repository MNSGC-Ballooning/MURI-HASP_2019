

//function to handle both retrieval of data from GPS module and sensors, as well as recording it on the SD card
void updateSensors() {
  static unsigned long prevTime = 0;
  if(millis() - prevTime >= 1000){
    prevTime = millis();
    adxl.readAccel(&x,&y,&z);
    //Request temp values
    sensor1.requestTemperatures();    //Temperature sensor 1 (internal)
    sensor2.requestTemperatures();    //Temperature sensor 2 (external)
    sensor3.requestTemperatures();    //Temperature sensor 3 (OPC's)

    //Get temp values
    t1 = sensor1.getTempCByIndex(0) + 273.15;   //temperature 1 value
    t2 = sensor2.getTempCByIndex(0) + 273.15;   //temperature 2 value
    t3 = sensor3.getTempCByIndex(0) + 273.15;   //temperature 3 value
   
    String data = "";
    openFlightlog();
    if(GPS.Fix && GPS.altitude.feet()!=0) {
      data += (flightTimeStr() + "," + String(GPS.location.lat(), 6) + "," + String(GPS.location.lng(), 6) + ",");
      data += ((String(GPS.altitude.feet())) + ",");    //convert meters to feet for datalogging
      data += (String(GPS.date.month()) + "/" + String(GPS.date.day()) + "/" + String(GPS.date.year()) + ",");
      data += (String(GPS.time.hour()) + ":" + String(GPS.time.minute()) + ":" + String(GPS.time.second()) + ",");
      data += "fix,";
    }
    else{
    data += (flightTimeStr() + ",0.000000,0.000000,0.00,01/01/2000,00:00:00,No Fix,");
    
    }
    data += (String(x) + "," + String(y) + "," + String(z) + ","); 
    data += (String(t1) + "," +String(t2) + "," + String(t3) + "," + String(t4) + ",");
    data += (String(batHeatRelay.getRelayStatus()) + "," + String(opcHeatRelay.getRelayStatus()) + ",");
    data += (String(kpa) + ",");
    data += (String(ms_temp)+ ",");
    data += (String(ms_pressure) + ",");

    Serial.println(data);
    Flog.println(data);
    closeFlightlog();
    writeEvents();
  }
  
}

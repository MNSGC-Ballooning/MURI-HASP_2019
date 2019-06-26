//Data Logging

void writeSensorsSD(){
  sdLogging = true;
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  data = dataLog;                                   //PMS data log
  data += "Time=" + logTime() + ',' + "GPS=" + SDprintGPS() + ',' + "T Outside=" + String(t1) +',' + "T Inside=" + String(t2) +',' + "T OPC=" + String(t3) + "," + "OPC State=" + String(OPCState);
  Serial.println(data);
  fLog.println(data);                               //PMS and Sensor data log

  Serial.println("Data line was added");
  fLog.close();
  sdLogging = false;
}

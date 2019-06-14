//Data Logging

void writeSensorsSD(){
  sdLogging = true;
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  data = "Time=" + logTime() + ',' + "GPS=" + SDprintGPS() + ',' + "T Outside=" + String(t1) +',' + "T Inside=" + String(t2) +',' + "T OPC=" + String(t3);
  Serial.println(data);
  fLog.println(data);

  Serial.println("Data line was added");
  fLog.close();
  sdLogging = false;
}

//Data Logging


void writeSensorsSD(){
  fLog = SD.open(Fname.c_str(), FILE_WRITE);
  data = "T Outside=" + String(t1) +',' + "T Inside=" + String(t2) +',' + "T OPC=" + String(t3) + ',' + "Time" + logTime() + ',';
  Serial.println(data);
  tLog.println(data);

  Serial.println("Data line was added");
  fLog.close();
}
void loop() {
  writeSensorsSD();
}

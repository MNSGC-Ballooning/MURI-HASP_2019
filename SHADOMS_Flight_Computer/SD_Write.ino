//Data Logging

//Setting up SD card file stuff//
File tLog;
// NOTE the SD wont work if the file name is
// longer than 8 characters. 
// Also the for loop further down adds more characters 
// so be careful
const int chipSelect = BUILTIN_SDCARD;
String data;
String Fname = "";
boolean SDcard = true;

void setup() {
  while(!Serial){ //Wait for serial port to connect
    ;
  }

  Serial.print("Initializing SD card...");   //Tells us if the SD card faled to open:
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization Failed!");
    SDcard = false;
  }
  SDcard = true;
  Serial.println("Initialization done."); //This "for" loop checks to see if there are any previous files on
  for (int i = 0; i < 100; i++) {         //the SD card already with the generated name
    
    Fname = String("tLog" + String(i/10) + String(i%10) + ".csv");  //has new name for file to make sure file name is not already used
    if (!SD.exists(Fname.c_str())){   //does not run if same SD name exists
      break; 
    }
  }
  
  Serial.println("Temperature Logger created: " + Fname);  
  tLog = SD.open(Fname.c_str(), FILE_WRITE);
  String FHeader = "T Outside,T Inside,T OPC";    //temperature headers
  tLog.println(FHeader);//Set up temp log format and header
  tLog.close();
  Serial.println("Temp Logger header added");
  
}
void writeSensorsSD(){
  tLog = SD.open(Fname.c_str(), FILE_WRITE);
  data = "T Outside=" + String(t1) +',' + "T Inside=" + String(t2) +',' + "T OPC=" + String(t3) + ',';
  Serial.println(data);
  tLog.println(data);
  Serial.println("Data line was added");
  tLog.close();
}
void loop() {
  writeSensorsSD();
}

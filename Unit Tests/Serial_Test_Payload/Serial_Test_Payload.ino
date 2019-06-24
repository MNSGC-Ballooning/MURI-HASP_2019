// code to run on payload for serial test


#define C1LED 21
#define C2LED 22
#define C3LED 23
#define DWN_BYTES 51

byte packet[DWN_BYTES] = {0};

void setup()
{
  pinMode(C1LED,OUTPUT);
  pinMode(C2LED,OUTPUT);
  pinMode(C3LED,OUTPUT);

  Serial1.begin(1200);
  delay(100);
}

void loop()
{
  ///// Data Downlink ////
  byte checksum_byte_1 = 'U';
  byte checksum_byte_2 = 'M';
  byte checksum_byte_3 = '@';
  byte checksum_byte_4 = '@';
  
  String timestamp = "02:38:35";
  float lat = 38.456;
  String latSign;
  latSign = "0"; // if positive, sign flag = 0
  String s1 = String(lat); // covert to string
  while(s1.length() < 5)
  {
    s1 = "0"+s1; //if less than 5 char, left fill with 0
  }
  
  float lng = 103.213;
  String lngSign;
  lngSign = "1"; // if negative, sign flag = 1
  String s2 = String(lng); //convert to string
  while(s2.length() < 6)
  {
    s1 = "0"+s1; //if less than 6 char, left fill with 0
  }
  
  float alt = 104320.878;
  String s3 = String(alt); //convert to string
  while(s3.length() < 9)
  {
    s1 = "0"+s1; //if less than 9 char, left fill with 0
  }
  
  String s4 = "273.45"; 
  String s5 = "300.45";
  String s6 = "420.69";   
  String flightState = "0";
  String OPCState = "0";
  
  // combine strings into 1 long one
  String dataPacket = timestamp + s1 + s2 + s3 + s4 + s5 + s6 + latSign + lngSign + flightState + OPCState;
  dataPacket.getBytes(packet, DWN_BYTES);       //convert string to bytes (should be 50 bytes)
  
  // send the data packet string
  Serial.write(checksum_byte_1); // start with checksums 1 & 2
  Serial.write(checksum_byte_2);
  for (int i = 0; i<50; i++) // send the bytes in the data string
  {
    Serial1.write(packet[i]);
  }
  Serial.write(checksum_byte_3); // checksums 3 & 4
  Serial.write(checksum_byte_4);

  delay(1500);

  ///// Uplink Command /////
  uint8_t command_byte = 0;                     // byte for the command
  uint8_t ID_byte = 0;                          // byte for ID and checksum byte

  if(Serial1.available() > 1) //************************** this may need to change to just !=0
  {
      ID_byte = Serial1.read();                 //read first byte to ID variable
      command_byte = Serial1.read();            // read second byte to command variable
  }

  if(ID_byte == 0x1C)                           
  {
    switch(command_byte)                        // this may need to be switched to an if statement
    {
      case 0xAA:                                
        digitalWrite(C1LED,HIGH);
        delay(500);
        digitalWrite(C1LED,LOW);
        break;
   
      case 0xBB:                             
        digitalWrite(C2LED,HIGH);
        delay(500);
        digitalWrite(C2LED,LOW);                          
        break;

      case 0xCC:
        digitalWrite(C3LED,HIGH);
        delay(500);
        digitalWrite(C3LED,LOW);
        break;
        
    }
  }
}

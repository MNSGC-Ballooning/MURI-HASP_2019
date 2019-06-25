// code to run on payload for serial test


#define C1LED 21
#define C2LED 22
#define C3LED 23
#define DWN_BYTES 51

byte packet[DWN_BYTES] = {0};

  byte checksum_byte_1 = 'U';
  byte checksum_byte_2 = 'M';
  byte checksum_byte_3 = '@';
  byte checksum_byte_4 = '@';
  
  String timestamp = "02:38:35";
  float lat = 38.456;
  String latSign = "0"; // if positive, sign flag = 0
  String s1 = String(lat); // covert to string
  float lng = 103.213;
  String lngSign = "1"; // if negative, sign flag = 1
  String s2 = String(lng); //convert to string
  float alt = 104320.878;
  String s3 = String(alt); //convert to string
  String s4 = "273.45"; 
  String s5 = "300.45";
  String s6 = "420.69";   
  String flightState = "0";
  String OPCState = "0";
  
  // combine strings into 1 long one
  String dataPacket = timestamp + s1 + s2 + s3 + s4 + s5 + s6 + latSign + lngSign + flightState + OPCState;

void setup()
{
  pinMode(C1LED,OUTPUT);
  pinMode(C2LED,OUTPUT);
  pinMode(C3LED,OUTPUT);

  //Serial1.begin(1200);
  delay(100);
  Serial1.begin(1200);

  digitalWrite(C1LED,HIGH);
  digitalWrite(C2LED,HIGH);
  digitalWrite(C3LED,HIGH);
  delay(2000);
  digitalWrite(C1LED,LOW);
  digitalWrite(C2LED,LOW);
  digitalWrite(C3LED,LOW);
}

void loop()
{
  dataPacket.getBytes(packet, DWN_BYTES);       //convert string to bytes (should be 50 bytes)
  
  // send the data packet string
  Serial1.write(checksum_byte_1); // start with checksums 1 & 2
  Serial.print(checksum_byte_1);
  Serial1.write(checksum_byte_2);
  Serial.print(checksum_byte_2);
  for (int i = 0; i<50; i++) // send the bytes in the data string
  {
    Serial1.write(packet[i]);
    Serial.print(packet[i]);
  }
  Serial1.write(checksum_byte_3); // checksums 3 & 4
  Serial.print(checksum_byte_3);
  Serial1.write(checksum_byte_4);
  Serial.println(checksum_byte_4);
  //digitalWrite(C1LED,HIGH);
  delay(3000);
  //digitalWrite(C1LED,LOW);

  ///// Uplink Command /////
  uint8_t command_byte = 0;                     // byte for the command
  uint8_t ID_byte = 0;                          // byte for ID and checksum byte

 if(Serial1.available()) //************************** this may need to change to just !=0
  {
      digitalWrite(C3LED,HIGH);
      ID_byte = Serial1.read();                 //read first byte to ID variable
      command_byte = Serial1.read();            // read second byte to command variable
      Serial.print(ID_byte);
      Serial.print(command_byte);
      delay(500);
      digitalWrite(C3LED,LOW);
  }

  if(ID_byte == 28)                           
  {
    if(command_byte == 170)
    {
      digitalWrite(C1LED,HIGH);
      delay(1000);
      digitalWrite(C1LED,LOW);
    }
    else if(command_byte == 187)
    {
      digitalWrite(C2LED,HIGH);
      delay(500);
      digitalWrite(C2LED,LOW);                          
    }
    else if(command_byte == 204)
    {
      digitalWrite(C3LED,HIGH);
      delay(500);
      digitalWrite(C3LED,LOW);
    }
  }
  delay(500);
}

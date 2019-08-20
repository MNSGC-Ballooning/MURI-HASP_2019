//Serial Uplink and Downlink                                              NOTE: You can easily test the serial using CoolTerm (1200 baud, 8bit, 1 stop, no parity),
//                                                                        NOTE: a USB to RS232 connector, and an RS232 to UART connector. The USB cable must have a
//HEX COMMAND VALUES                                                      NOTE: FTDI chip in it to successfully do the conversion. Do this for uplink and downlink.
/*
1C AA - System reset    //First byte has to be ID and checkbyte (1C) second byte is reset command (AA)
1C BB - OPC Activation  //First byte has to be ID and checkbyte (1C) second byte is activation command (BB)
1C CC - OPC Shutdown    //First byte has to be ID and checkbyte (1C) second byte is shutdown command (CC)

To send packets of data, we might need to establish a buffer, and then fill that buffer with bytes, and then send the buffer?  NOTE: We didn't, but it would've been nice.
*/

void serialInit(){
  Serial1.begin(1200);                                                   //Initializes HASP serial port at 1200 baud
}

///// Data Downlink /////
void Data_Downlink()
{ 
  //Send 4 check bytes- 2 at the beginning and 2 at the end
  byte checksum_byte_1 = 'U';                                            //beginning check byte
  byte checksum_byte_2 = 'M';                                            //beginning check byte
  byte checksum_byte_3 = '@';                                            //end check byte
  byte checksum_byte_4 = '@';                                            //end check byte

  String timestamp = logTimeSerial();                                    //HH:MM:SS timestamp (8 char string)

  float lat = float(GPS.location.lat());                                 //get data in float
  String latSign;
  if(lat < 0)
  {
    latSign = "1";                                                       //if negative, sign flag = 1
    lat = lat*(-1);                                                      //remove "-"
  }
  else
  {
    latSign = "0";                                                       //if positive, sign flag = 0
  }
  String s1 = String(lat);                                               //covert to string
  while(s1.length() < 5)
  {
    s1 = "0"+s1;                                                         //if less than 5 char, left fill with 0
  }
  
  float lng = float(GPS.location.lng());                                 //get data in float
  String lngSign;
  if(lng < 0)
  {
    lngSign = "1";                                                       //if negative, sign flag = 1
    lng = lng*(-1);                                                      //remove "-"
  }
  else
  {
    lngSign = "0";                                                       //if positive, sign flag = 0
  }
  String s2 = String(lng);                                               //convert to string
  while(s2.length() < 6)
  {
    s2 = "0"+s2;                                                         //if less than 6 char, left fill with 0
  }
  
  float alt = float(GPS.altitude.feet());
  String s3 = String(alt);                                               //convert to string
  while(s3.length() < 9)
  {
    s3 = "0"+s3;                                                         //if less than 9 char, left fill with 0
  }

  String s4 = String(t1);                                                //6 char string
  String s5 = String(t2);                                                //6 char string
  String s6 = String(t3);                                                //6 char string   

  String planSample = planData.particles_05um;
  while(planSample.length()<6)
  {
    planSample = "#" + planSample;
  }

  dataPacket = timestamp + s1 + s2 + s3 + s4 + s5;                       //Combine strings into 1 long one
  dataPacket += s6 + planSample;                                         //add in sample of 2nd bin on plantower
  dataPacket += latSign + lngSign + flightState + OPCState;

  //dataPacket = timestamp + " t1:" + s4 + "t2:" + s5 + "t3:" + s6 + "heater: " + HS + " OPCstate:" + OPCState; //This is 56 bytes
  // string for thermal test
  
  dataPacket.getBytes(packet, DWN_BYTES);                                //convert string to bytes (should be 53 bytes)

  //Send the data packet string
  Serial1.write(checksum_byte_1);                                        //start with check bytes 1 & 2
  Serial1.write(checksum_byte_2);
  for (int i = 0; i<(DWN_BYTES-1); i++)                                  //send the bytes in the data string
  {
    Serial1.write(packet[i]);
  }
  Serial1.write(checksum_byte_3);                                        //checksums 3 & 4
  Serial1.write(checksum_byte_4);
}

/////Uplink Command/////
void Read_Uplink_Command()
{
  byte command_byte = 0;                                                //byte for the command
  byte ID_byte = 0;                                                     //byte for ID and checksum byte
  byte bin = 0;                                                         //Spare bytes

  while(Serial1.available()>6)
  {
      bin = Serial1.read();                                             //NOTE: YOU MUST PARSE THE EXTRA BYTES! We simply discarded them, which works well.
      bin = Serial1.read();
      ID_byte = Serial1.read();                                         //read first byte to ID variable
      command_byte = Serial1.read();                                    //read second byte to command variable
      bin = Serial1.read();
      bin = Serial1.read();
      bin = Serial1.read();
  }

  if(ID_byte == 0x1C)                                                   //check to see id check byte is correct, if not, command is ignored
  {
    if(command_byte == 0xAA)                                            //System reset command
    {
      systemReset(); 
    }
    else if(command_byte == 0xBB)                                       //OPC activation command
    {
      activeMode();
    }
    else if(command_byte == 0xCC)                                       //OPC shutdown command
    {
      standbyMode();  
    }   
  }
}

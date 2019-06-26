//Serial Uplink and Downlink

//HEX COMMAND VALUES
/*
1C AA - System reset    // first byte has to be ID and checksum (1C) second byte is reset command (AA)
1C BB - OPC Activation  // first byte has to be ID and checksum (1C) second byte is activation command (BB)
1C CC - OPC Shutdown    // first byte has to be ID and checksum (1C) second byte is shutdown command (CC)

To send packets of data, we might need to establish a buffer, and then fill that buffer with bytes, and then send the buffer?
*/

///// Data Downlink /////
void Data_Downlink()
{ 
  // send a 2 byte checksum at the beginning and at the end
  byte checksum_byte_1 = 'U';                                            //beginning check byte
  byte checksum_byte_2 = 'M';                                            //beginning check byte
  byte checksum_byte_3 = '@';                                            //end check byte
  byte checksum_byte_4 = '@';                                            //end check byte


  // assign timestamp
  String timestamp = logTime();                                          //HH:MM:SS timestamp (8 char string)

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
    s1 = "0"+s1;                                                         //if less than 6 char, left fill with 0
  }
  
  float alt = float(GPS.altitude.feet());
  String s3 = String(alt);                                               //convert to string
  while(s3.length() < 9)
  {
    s1 = "0"+s1;                                                         //if less than 9 char, left fill with 0
  }

  String s4 = String(t1);                                                //6 char string
  String s5 = String(t2);                                                //6 char string
  String s6 = String(t3);                                                //6 char string   
  
  if(inFlight)                                                           //the next two booleans typecast the booleans
  {                                                                      //with space for additional logic
    flightState = "1";                                                  
  }
  else
  {
    flightState = "0";
  }
  if(dataCollection && goodLog)
  {
    OPCState = "1";
  }
  else
  {
    OPCState = "0";
  }

  // combine strings into 1 long one
  String dataPacket = timestamp + s1 + s2 + s3 + s4 + s5 + s6 + latSign + lngSign + flightState + OPCState;
  dataPacket.getBytes(packet, DWN_BYTES);                                //convert string to bytes (should be 50 bytes)

  // send the data packet string

  Serial.write(checksum_byte_1);                                         //start with check bytes 1 & 2
  Serial.write(checksum_byte_2);
  for (int i = 0; i<50; i++)                                             //send the bytes in the data string
  {
    Serial1.write(packet[i]);
  }
  Serial.write(checksum_byte_3);                                         //checksums 3 & 4
  Serial.write(checksum_byte_4);


}

///// Uplink Command /////
void Read_Uplink_Command()
{
  uint8_t command_byte = 0;                                              //byte for the command
  uint8_t ID_byte = 0;                                                   //byte for ID and checksum byte


  while(Serial1.available() > 1)
  {
      ID_byte = Serial1.read();                                          //read first byte to ID variable
      command_byte = Serial1.read();                                     //read second byte to command variable
  }

  if(ID_byte == 0x1C)                                                    //check to see id check byte is correct, if not, command is ignored
  {
    if(command_byte == 0xAA)                 // system reset command
    {
      systemReset(); 
    }
    else if(command_byte == 0xBB)            // OPC activation command
    {
      activeMode();
    }
    else if(command_byte == 0xCC)            // OPC shutdown command
    {
      standbyMode();  
    }   
  }
}


///// Command Functions /////
void systemReset(){                                                      //This will reset the system
  standbyMode();
  heater.setState(0);
  activeMode();
}

void activeMode(){                                                      //This will activate all of the particle detectors.
  LOAC.setState(1);
  alphaOPC.setState(1);
  planOPC.setState(1);
  dataCollection = true;                                                //This will enable storage of data from the particle counters
  if (!inFlight) inFlight = true;                                       //If the "in flight" boolean has not been triggered, this will begin the flight.
}

void standbyMode(){                                                     //This will shut down all of the particle detectors.
  dataCollection = false;
  alphaOPC.setState(0);
  planOPC.setState(0);
  digitalWrite(LS_PD, HIGH);                                           //To turn off the LOAC, the recording state has to be
  delay(12000);                                                        //shut down before it can be powered down. 
  digitalWrite(LS_PD, LOW);
  LOAC.setState(0);                           
}

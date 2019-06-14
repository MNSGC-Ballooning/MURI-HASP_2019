//HASP STUFF

/* HEX COMMAND VALUES
1C AA - System reset    // first byte has to be ID and checksum (1C) second byte is reset command (AA)
1C BB - OPC Activation  // first byte has to be ID and checksum (1C) second byte is activation command (BB)
1C CC - OPC Shutdown    // first byte has to be ID and checksum (1C) second byte is shutdown command (CC)

//To send packets of data, we might need to establish a buffer, and then fill that buffer with bytes, and then send the buffer?
 */

///// Data Downlink /////
void Data_Downlink()
{ 
  // send a 2 byte checksum at the beginning and at the endv
  uint8_t checksum_byte_1 = 'U';                                            // beginning checksum .......... pretty sure i can set these as characters. but maybe not
  uint8_t checksum_byte_2 = 'M';                                            // beginning checksum
  uint8_t checksum_byte_3 = '@';                                            // end checksum
  uint8_t checksum_byte_4 = '@';                                            // end checksum

// send all data as one long string so it is all at once? i dont know best way to do this

  // assign first checksums
  String check1 = String(checksum_byte_1);                                  //initial checksum 1
  String check2 = String(checksum_byte_2);                                  //initial checksum 2

  // assign timestamp
  String timestamp = logTime();                                             // HH:MM:SS timestamp 

  // assign data
  String GPS = getlat() + getlong() + getalt();                             // need to know what these variable should be
  String Temp = String(t1) + String(t2) + String(t3); 
  
  if(inFlight)
  {
    flightState = "True";
  }
  else
  {
    flightState = "False";
  }
  if(dataCollection)
  {
    OPCState = "True";
  }
  else
  {
    OPCState = "False";
  }
  
  // assign end checksums
  String check3 = String(checksum_byte_3);
  String check4 = String(checksum_byte_4);

  // combine strings into 1 long one
  String dataPacket = check1 + check2 + timestamp + GPS + Temp + flightState + OPCState + check3 + check4;
  dataPacket.getBytes(packet, DWN_BYTES);       //convert string to bytes

  // send the data packet string
  for (int i = 0; i<32; i++) {
  Serial1.write(packet[i]);
  }
}

///// Uplink Command /////
void Read_Uplink_Command()
{
  uint8_t command_byte = 0;                     // byte for the command
  uint8_t ID_byte = 0;                          // byte for ID and checksum byte

  while(Serial1.available() > 1)
  {
      ID_byte = Serial1.read();                 //read first byte to ID variable
      command_byte = Serial1.read();            // read second byte to command variable
  }

  if(ID_byte == 0x1C)                           //check to see id checksum is correct, if not, command is ignored
  {
    switch(command_byte)
    {
      case 0xAA:                                // case for startup command
        activeMode();                           // not sure how to implement this...
        break;
   
      case 0xBB:                                // case for shutdown command
        standbyMode();                          // again, not sure how to implement this...
        break;

      case 0xCC:
        systemReset();
        break;
        
    }
  }
}

///// Command Functions /////
void systemReset(){                             //This will reset the system
  standbyMode();
  heater.setState(0);
  activeMode();
}

void activeMode(){                             //This will activate all of the particle detectors.
  LOAC.setState(1);
  alphaOPC.setState(1);
  planOPC.setState(1);
  dataCollection = true;
}

void standbyMode(){                           //This will shut down all of the particle detectors.
  LOAC.setState(0);                           //To turn off the LOAC, the recording state has to be
  alphaOPC.setState(0);                       //shut down before it can be powered down. This will be
  planOPC.setState(0);                        //done (somehow) in the hardware.
  dataCollection = false;
}

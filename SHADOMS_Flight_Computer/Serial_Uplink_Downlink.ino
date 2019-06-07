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
  uint8_t checksum_byte_1 = U; // beginning checksum .......... pretty sure i can set these as characters. but maybe not
  uint8_t checksum_byte_2 = M; // beginning checksum
  uint8_t checksum_byte_3 = @; // end checksum
  uint8_t checksum_byte_4 = @; // end checksum

  // send all data as one long string so it is all at once? i dont know best way to do this

  // assign first checksums
  String check1 = String(checksum_byte_1); //initial checksum 1
  String check2 = String(checksum_byte_2); //initial checksum 2

  // assign timestamp
  String timestamp = logTime; // HH:MM:SS timestamp 

  // assign data
  String GPS = "Alt: " + String(alt) + " Lat: " + String(latitude) + " Long: " + String(longitude); // need to know what these variable should be
  String Temp = "t1: " + String(t1) + " t2: " + String(t2) + " t3: " + String(t3); 
  if(Inflight)
  {
    String flightState = "True";
  }
  else
  {
    String flightState = "False";
  }
  if(dataCollection)
  {
    String OPCState = "True";
  }
  else
  {
    String OPCState = "False";
  }
  
  // assign end checksums
  String check3 = String(checksum_byte_3);
  String check4 = String(checksum_byte_4);

  // combine strings into 1 long one
  String Data_packet = check1 + check2 + " " + timestamp + " " + GPS + " " + Temp + " " + flightState + " "  OPCState + " " + check3 + check4;

  // send the data packet string
  Serial1.write(Data_packet);
  
}

///// Uplink Command /////
void Read_Uplink_Command()
{
  uint8_t command_byte; // byte for the command
  uint8_t ID_byte; // byte for ID and checksum byte

  while(Serial1.available() > 0)
  {
      ID_byte = Serial1.read();  //read first byte to ID variable
      command_byte = Serial1.read(); // read second byte to command variable
  }

  if(ID_byte == 0x1C) //check to see id checksum is correct, if not, command is ignored
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
void systemReset(){                      //This will reset the system
  standbyMode();
  heater.setState(0);
  //Close data log
  //Open new data log
  activeMode();
}

void activeMode(){                       //This will activate all of the particle detectors.
  LOAC.setState(1);
  alphaOPC.setState(1);
  planOPC.setState(1);
}

void standbyMode(){                     //This will shut down all of the particle detectors.
  stateLOAC.setState(1);                //To turn off the LOAC, the recording state has to be
  delay (12000);                        //shut down before it can be powered down.
  stateLOAC.setState(0);
  LOAC.setState(0);
  alphaOPC.setState(0);
  planOPC.setState(0);
}

//HASP STUFF

/* HEX COMMAND VALUES
0C C0 - System reset
00 AA - OPC Activation
BB 00 - OPC Shutdown

//To send packets of data, we might need to establish a buffer, and then fill that buffer with bytes, and then send the buffer?
 */

//// Data Downlink ////

void Data_Downlink(String Sensor_Data, int mission_time) // sensor data is the stuff we want to send (i.e. pressure,temp,etc.) 
{
  // pull sensor data (should all be strings)
  temp = Sensor_Data[0];
  GPS = Sensor_Data[1];
  LOAC_state = Sensor_Data[2];
  Alphasense_state = Sensor_Data[3];
  Plantower_state = Sensor_Data[4];
  
  // send a 2 byte checksum at the beginning and at the endv
  // not entirely sure if this is how to go about sending characters
  uint8_t checksum_byte_1 = U; // beginning checksum
  uint8_t checksum_byte_2 = M; // beginning checksum

  uint8_t checksum_byte_3 = @; // end checksum
  uint8_t checksum_byte_4 = @; // end checksum

  // Send first checksums
  Serial1.write(String(checksum_byte_1));
  Serial1.write(String(checksum_byte_2));

  // send mission timestamp
  Serial.write(String(mission_time)); // mission time could just be like an instanteous reading from millis() or something

  // send data
  Serial1.write("Pressure: " + pressure + " Temperature: " + temp + " LOAC: " + LOAC_state + " Alphasense: " + Alphasense_state + " Plantower: " + Plantower_state);
  // not sure if it works in this format... I hope it does

  // send 2 byte checksum at the end
  Serial1.write(String(checksum_byte_3));
  Serial1.write(String(checksum_byte_4));
}


//// Uplink Commands ////

void Read_Uplink_Command()
{
  uint8_t full_command_string[7];               //incoming command array. 1 spot for each byte

  while(Serial1.available() > 0)
  {
    for(int i=0;i<7;i++)
    {
      full_command_string[i] = Serial1.read();  //read a byte into the command array
    }
  }

  uint8_t ID_checksum = full_command_string[2]; // byte with payload ID (1) and checksum (going with C in this case)
  uint8_t command = full_command_string[3];     // byte with command

  if(ID_checksum == 0x1C)                       //check to see id checksum is correct, if not, command is ignored
  {
    switch(command)
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

/*
void uplinkCommand(String COMMAND) {     //This function will analyze the command from the
  if (COMMAND==HEX0) {
    systemReset();
  } else if (COMMAND==HEX1) {            //HASP uplink and utilize the command to update the
    activeMode();                        //the state of the payload.
  } else if (COMMAND==HEX2) {
    standbyMode();
  }
}
*/
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

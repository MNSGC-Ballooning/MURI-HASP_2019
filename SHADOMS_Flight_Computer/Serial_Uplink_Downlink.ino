//HASP STUFF

/* HEX COMMAND VALUES
00 00 - System reset
00 01 - OPC Activation
00 10 - OPC Shutdown
 */

void uplinkCommand(String COMMAND) {     //This function will analyze the command from the
  if (COMMAND==HEX0) {
    systemReset();
  } else if (COMMAND==HEX1) {                   //HASP uplink and utilize the command to update the
    activeMode();                        //the state of the payload.
  } else if (COMMAND==HEX2) {
    standbyMode();
  }
}

void systemReset(){
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
  stateLOAC.setState(1);
  delay (12000);
  stateLOAC.setState(0);
  LOAC.setState(0);
  alphaOPC.setState(0);
  planOPC.setState(0);
}

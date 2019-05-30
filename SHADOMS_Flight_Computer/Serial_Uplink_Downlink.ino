//HASP STUFF


void uplinkCommand(String COMMAND) {     //This function will analyze the command from the
  if (COMMAND=="ON") {                   //HASP uplink and utilize the command to update the
    activeMode();                        //the state of the payload.
  } else if (COMMAND=="OFF") {
    standbyMode();
  }
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

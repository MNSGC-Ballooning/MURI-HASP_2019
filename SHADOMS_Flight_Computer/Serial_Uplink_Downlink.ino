//HASP STUFF

void activeMode(UPLINK COMMAND){         //System startup- to active data collection mode
  LOAC.setState(1);
  alphaOPC.setState(1);
  planOPC.setState(1);
}

void standbyMode(UPLINK COMMAND){       //System Shutdown- to standby mode
  stateLOAC.setState(1);
  delay (12000);
  stateLOAC.setState(0);
  LOAC.setState(0);
  alphaOPC.setState(0);
  planOPC.setState(0);
}

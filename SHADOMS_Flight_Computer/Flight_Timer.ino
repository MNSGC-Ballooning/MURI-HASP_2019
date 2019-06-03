//Flight Timer

unsigned long poweredTime() {
  return millis();
}

unsigned long flightTime() {
  while (!inFlight) {
    return 0;
  }
  return (poweredTime()-flightStart);
}


String logTime() {
  String logString = "";
  unsigned long timer;
  
  if (!inFlight) {
    timer = poweredTime()/1000;
    logString = "Flight has not yet begun. Time since power on: ";
  } else {
    timer = flightTime()/1000;
  }
  
    logString += (String(timer/3600) + ':');
    timer=timer%3600;
    logString += (String(timer/600) + ':');
    timer=timer%600;
    logString += (String(timer/60) + ':');
    timer=timer%60;
    logString += (String(timer/10) + String(timer%10));
  return logString;
}

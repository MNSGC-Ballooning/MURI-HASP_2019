//Flight Timer

unsigned long poweredTime() {
  return millis();                                                      //The millis command counts the time in milliseconds since the system has been powered on.
}

unsigned long flightTime() {                                            //This function will track time of actual flight. The inFlight boolean will be triggered by
  if (!inFlight) {                                                   //the GPS. flightStart will be set by the same function. This will allow for a flight counter
    return 0;                                                           //to stamp data from the actual flight. This will also be used to start the particle counters.
  }
  else {
    return (poweredTime()-flightStart);
  }
}

String logTime() {                                                      //This function will establish the time for the data log in HH:MM:SS:MSS format. MSS is milliseconds.
  String logString = "";                                                
  unsigned long timer;
  
  if (!inFlight) {                                                      //If the flight has not started, the logging function will track the powered time. That way, if the
    timer = poweredTime();                                              //flight time fails to start, the time is still logged in a usable way. Else, the timer will be based
    logString = "Flight has not yet begun. Time since power on: ";      //on the flight time.
  } else {
    timer = flightTime();
  }
  
  if ((timer/3600000)<10){                                              //This if statement, and the proceeding if statements, will make sure that the times are always
    logString += '0';                                                   //two chars in width.
  }
    logString += (String(timer/3600000) + ':');                         //This will parse the time by hours, minutes, seconds, and parts of a second and add the value to the
    timer=timer%3600000;                                                //timer log. Then, these times will be returned.

  if ((timer/60000)<10){                                                
    logString += '0';                                                  
  }
    logString += (String(timer/60000) + ':');
    timer=timer%60000;

  if ((timer/1000)<10){                                                
    logString += '0';                                                  
  }
    logString += (String(timer/1000) + ':');
    timer=timer%1000;
    if ((timer/10)<100){                                                
    logString += '0'; 
    if (timer/10<10) {
      logString += '0';                                                 
    }
   }
  logString += String(timer);
    
  return logString;
}

String logTimeSerial() {                                                //This function will establish the time for the data log in HH:MM:SS format.
  String logString = "";                                                
  unsigned long timer;
  
  if (!inFlight) {                                                      //If the flight has not started, the logging function will track the powered time. That way, if the
    timer = poweredTime();                                              //flight time fails to start, the time is still logged in a usable way. Else, the timer will be based
                                                                        //on the flight time. In the Serial log time function, times will not be explicitly indicated.
  } else {
    timer = flightTime();
  }
  
  if ((timer/3600000)<10){                                              //This if statement, and the proceeding if statements, will make sure that the times are always
    logString += '0';                                                   //two chars in width.
  }
    logString += (String(timer/3600000) + ':');                         //This will parse the time by hours, minutes, seconds, and parts of a second and add the value to the
    timer=timer%3600000;                                                //timer log. Then, these times will be returned.

  if ((timer/60000)<10){                                                
    logString += '0';                                                  
  }
    logString += (String(timer/60000) + ':');
    timer=timer%60000;

  if ((timer/1000)<10){                                                
    logString += '0';                                                  
  }
    logString += (String(timer/1000));
    
  return logString;
}

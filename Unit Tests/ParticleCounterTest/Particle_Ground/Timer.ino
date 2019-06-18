//Flight Timer

unsigned long poweredTime() {
  return millis();                                                      //The millis command counts the time in milliseconds since the system has been powered on.
}

String logTime() {                                                      //This function will establish the time for the data log in HH:MM:SS:DS format. DS is decimal seconds.
  String logString = "";                                                
  unsigned long timer;
                                                     
    timer = poweredTime()/1000;                                       
  
  if ((timer/3600)<10){                                                 //This if statement, and the proceeding if statements, will make sure that the times are always
    logString += '0';                                                   //two chars in width.
  }
    logString += (String(timer/3600) + ':');                            //This will parse the time by hours, minutes, seconds, and parts of a second and add the value to the
    timer=timer%3600;                                                   //timer log. Then, these times will be returned.

  if ((timer/600)<10){                                                
    logString += '0';                                                  
  }
    logString += (String(timer/600) + ':');
    timer=timer%600;

  if ((timer/60)<10){                                                
    logString += '0';                                                  
  }
    logString += (String(timer/60) + ':');
    timer=timer%60;
    logString += (String(timer/10) + String(timer%10));
    
  return logString;
}

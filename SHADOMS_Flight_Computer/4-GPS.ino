//GPS Operation

void GPSInit(){                                                                   //Initializes GPS systems
   pinMode(fixLED, OUTPUT);                                                       //Initializes fix LED
   Serial2.begin(4800);                                                           //Initializes serial port for GPS communication
//   while (!Serial2) ;                                                           //Waits for Serial 2 to connect
}

String getlat() {                                                                 //Function that returns latitude as a string.
  String latitude = "";
  latitude = String(GPS.location.lat(), 6);
  return latitude;
}

String getlong() {                                                                //Function that returns longitude as a string.
  String longitude = "";
  longitude = String(GPS.location.lng(), 6);
  return longitude;
}

String getalt() {                                                                 //Function that returns altitude as a string.
  String alt = "";
  alt = String(GPS.altitude.feet());
  return alt;
}

void flightCheck() {                                                              //Function that repeatedly checks if Payload is in flight (above the in flight altitude).
  if (GPS.Fix && (GPS.altitude.feet() >= INFLIGHT_START_ALT)  && (GPS.location.lng() != 0)) {    
    FlightCheckCounter++;                                                         //If three conditions are met, FlightCheckComputer gets a hit.
    if ((FlightCheckCounter >= 5)&&(!inFlight)) {                                 //5 FlightCheckCounter hits in a row needed to set inFlight to true
      inFlight = true;                                                            //Bool that indicates if the payload is in flight.
      flightStart = millis();                                                     //flightStart holds the time when inFlight becomes true since the batteries for the payload
      activeMode();                                                               //are powered on.
    }
  }
  else {
    FlightCheckCounter = 0;                                                       //If the three conditions aren't met, FlightCheckCounter loses all of its hits.
  }
}

String printGPS() {                                                               //Function that takes GPS data and prints it to the SD card
  if (GPS.Fix && GPS.altitude.feet() != 0) {                                      //If GPS has a fix with good data (may need more parameters to define "good data", GPS data is printed
    GPSdata = getlat() + ", " + getlong() + ", " + getalt();
  } else {                                                                        //If GPS has bad data or doesn't have a fix, zeros are printed for all three variables
    GPSdata = faillatitude + ", " + faillongitude + ", " + failalt;   
  }

  return GPSdata;
}

void fixLEDupdater() {
  fixLED_loop_timer = millis();                                                   //This will check for fixes and use the fix LED to indicate status.
    
  if (GPS.Fix && GPS.altitude.feet() != 0) {
    digitalWrite(fixLED, HIGH);
    fixLEDon = true;
    fixLEDshort = true;
  }
  else {
    digitalWrite(fixLED, HIGH);
    fixLEDon = true;
    fixLEDshort = false;
  }
}

void fixLEDturnoff() {
  digitalWrite(fixLED, LOW);
  fixLEDon = false; 
}

void updateGPS() {                                                                //Function that updates GPS every second and accounts for
  static bool firstFix = false;                                                   //clock rollover at midnight (23:59:59 to 00:00:00)
  while (Serial2.available() > 0) {
    GPS.encode(Serial2.read());
    }
  if (GPS.altitude.isUpdated() || GPS.location.isUpdated()) {
    if (!firstFix && GPS.Fix && (GPS.altitude.feet() != 0)) {     
      GPSstartTime = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
      firstFix = true;                                                            //Time in second of GPS clock will now be compared to GPS Start Time
      }
    }
  if (getGPStime() > lastGPS) {    //if it's been more than a second
        lastGPS = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
    }
}
   
unsigned int getGPStime() {
  return (GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second());
}

int getLastGPS() {                                                                //returns time in seconds between last successful fix and initial fix.
  static bool newDay  = false;                                                    //Used to match with altitude data variable in case we're flying late at
  if (!newDay && lastGPS < GPSstartTime) {                                        //night (clock rollover).
    days++;
    newDay = true;
  }
  else if (newDay && lastGPS > GPSstartTime)
    newDay = false;
  return days * 86400 + lastGPS;

  
/*     The following three functions are here in the unlikely case that we want to implement them later
String getdate() {
  String datetime = "";
  datetime = String(GPS.date.month()) + "/" + String(GPS.date.day()) + "/" + String(GPS.date.year());
  return datetime;
}

String gettime() {
  String timestamp = "";
  timestamp = String(GPS.time.hour()) + ":" + String(GPS.time.minute()) + ":" + String(GPS.time.second());
  return timestamp; 
}

String getsat() {
  String sat = "";
  sat = String(GPS.satellites.value());
  return sat;
}
*/
}

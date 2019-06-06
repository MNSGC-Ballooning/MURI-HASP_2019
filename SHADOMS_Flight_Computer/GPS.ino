//GPS Operation


String getlat() {
  String latitude = "";
  latitude = String(GPS.location.lat(), 6);
  return latitude;
}

String getlong() {
  String longitude = "":
  longitude = String(GPS.location.lng(), 6);
  return longitude;
}

String getalt() {
  String alt = "";
  alt = String(GPS.altitude.feet());
  return alt;
}

void FlightCheck() { //Function that repeatedly checks if Payload is in flight.
  if (GPS.Fix && (GPS.altitude.feet() > 5000)  && (GPS.location.lng != 0)) {
    FlightCheckCounter++;
    if (FlightCheckCounter >= 5) { //5 FlightCheckCounter hits in a row needed to set inFlight to true
      inFlight = true; 
      flightStart = millis();
    }
  }
  else {
    FlightCheckCounter = 0;
  }

}


void updateGPS() {                    //Function that updates GPS every second and accounts for
  static bool firstFix = false;       //clock rollover at midnight (23:59:59 to 00:00:00)
  while (GPS_serial.available() > 0) {
    GPS.encode(GPS_serial.read());
    }
  if (GPS.altitude.isUpdated() || GPS.location.isUpdated()) {
    if (!firstFix && GPS.Fix) {    
      GPSstartTime = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
      firstFix = true;
      }
    }
  if (getGPStime() > lastGPS) {    //if it's been more than a second
        lastGPS = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
    }
}
   
int getGPStime() {
  return (GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second());
}

int getLastGPS() { 
  //returns time in seconds between last successful fix and initial fix. Used to match with altitude data
  static bool newDay  = false;           //variable in case we're flying late at night (clock rollover)
  if (!newDay && lastGPS < GPSstartTime) {
    days++;
    newDay = true;
  }
  else if (newDay && lastGPS > GPSstartTime)
    newDay = false;
  return days * 86400 + lastGPS;
}

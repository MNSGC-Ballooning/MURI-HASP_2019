#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>


//Place for definitions//
//#define redLED 3
//#define greenLED 4
#define GPS_RX 9
#define GPS_TX 10
#define chipSelect 10


TinyGPSPlus GPS;
SoftwareSerial gps_serial(9,10);


File datalog;                     //File object for datalogging
char filename[] = "COPGPS00.csv"; //Template for file name to save data
bool SDactive = false;            //used to check for SD card before attempting to log


//functions to manage the GPS in updateGPS().
unsigned long lastGPS = 0;
unsigned long GPSstartTime = 0;    //when the GPS starts, time in seconds of last GPS update.
uint8_t days = 0;                  //if we're flying overnight
unsigned long timer = 0;           //timer to ensure GPS prints data once every second
unsigned long fixtimer = 0;        //timer to ensure that if the GPS has a fix, and LED flashes once every 10 seconds
unsigned long LEDlocktimer = 0;    //timer to ensure that the green LED that goes on if there is a GPS lock stays on for a specified amount of time
bool fixLED = false;               //bool that communicates if the green LED is on or not

//GPS Operation

String getlat() {                             //Function that returns latitude as a string.
  String latitude = "";
  latitude = String(GPS.location.lat(), 6);
  return latitude;
}

String getlong() {                            //Function that returns longitude as a string.
  String longitude = "";
  longitude = String(GPS.location.lng(), 6);
  return longitude;
}

String getalt() {                             //Function that returns altitude as a string.
  String alt = "";
  alt = String(GPS.altitude.feet());
  return alt;
}

void FlightCheck() {                                                            //Function that repeatedly checks if Payload is in flight.
  if (GPS.Fix && (GPS.altitude.feet() > 5000)  && (GPS.location.lng() != 0)) {    //HASP vehicle takeoff altitude is at roughly 4000 feet.
    FlightCheckCounter++;                                                       //If three conditions are met, FlightCheckComputer gets a hit.
    if (FlightCheckCounter >= 5) {                                              //5 FlightCheckCounter hits in a row needed to set inFlight to true
      inFlight = true;                                                          //Bool that indicates if the payload is in flight.
      flightStart = millis();                                                   //flightStart holds the time when inFlight becomes true since the batteries for the payload are powered on
    }
  }
  else {
    FlightCheckCounter = 0;                                                     //If the three conditions aren't met, FlightCheckCounter loses all of its hits.
  }
}

String SDprintGPS() {                                                            //Function that takes GPS data and prints it to the SD card

  if (GPS.Fix && GPS.altitude.feet() != 0) {                                    //If GPS has a fix with good data (may need more parameters to define "good data", GPS data is printed
    GPSdata = getlat() + ", " + getlong() + ", " + getalt();
  }
  else {                                                                        //If GPS has bad data or doesn't have a fix, zeros are printed for all three variables
    GPSdata = faillatitude + ", " + faillongitude + ", " + failalt;    
  }

  return GPSdata;
}



void updateGPS() {                                                              //Function that updates GPS every second and accounts for
  static bool firstFix = false;                                                 //clock rollover at midnight (23:59:59 to 00:00:00)
  while (Serial2.available() > 0) {
    GPS.encode(Serial2.read());
    }
  if (GPS.altitude.isUpdated() || GPS.location.isUpdated()) {
    if (!firstFix && GPS.Fix) {     
      GPSstartTime = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
      firstFix = true;                                                          //Time in second of GPS clock will now be compared to GPS Start Time
      }
    }
  if (getGPStime() > lastGPS) {    //if it's been more than a second
        lastGPS = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
    }
}
   
unsigned int getGPStime() {
  return (GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second());
}

int getLastGPS() { 
                                                                     //returns time in seconds between last successful fix and initial fix. Used to match with altitude data
  static bool newDay  = false;                                       //variable in case we're flying late at night (clock rollover)
  if (!newDay && lastGPS < GPSstartTime) {
    days++;
    newDay = true;
  }
  else if (newDay && lastGPS > GPSstartTime)
    newDay = false;
  return days * 86400 + lastGPS;
}






void setup() {

}

void loop() {

}
}

#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>


//Place for definitions//
#define stateLED 21
#define fixLED 22
#define sdLED 23
#define GPS_RX 9
#define GPS_TX 10
const int chipSelect = BUILTIN_SDCARD; //On board SD card for teensy


TinyGPSPlus GPS;
SoftwareSerial gps_serial(9,10);



File datalog;                     //File object for datalogging
char filename[] = "COPGPS00.csv"; //Template for file name to save data
bool SDactive = false;            //used to check for SD card before attempting to log


//functions to manage the GPS in updateGPS().
unsigned long lastGPS = 0;
unsigned long GPSstartTime = 0;    //when the GPS starts, time in seconds of last GPS update.
uint8_t days = 0;                  //if we're flying overnight

//timers for SD and LEDs
unsigned long timer = 0;           //timer to ensure GPS prints data once every second
unsigned long fixtimer = 0;        //timer to ensure that if the GPS has a fix, and LED flashes once every 10 seconds
unsigned long LEDlocktimer = 0;    //timer to ensure that the green LED that goes on if there is a GPS lock stays on for a specified amount of time
bool fixLEDbool = false;           //indicates if the fixLED is on or not

//strings that populate GPS data strings
String GPSdata = "";                                  //Initializes data string that prints GPS data to the SD card
String data = "";                                     //Initializes string that prints data to SD
String flightstring = "False";                        //Indicates if inFlight is active or not
String faildate = "00/00/2000";                       //Printed date if GPS does not have a fix or any data
String failtime = "00:00:00";                         //Printed time if GPS does not have a fix or any data                    
String faillatitude = "0.00";                         //Printed latitude if GPS does not have a fix or any data
String faillongitude = "0.000000";                    //Printed longitude if GPS does not have a fix or any data
String failalt = "0.000000";                          //Printed altitude if GPS does not have a fix or any 

//flightchecker() variables
bool inFlight = false;                                //Bool that determines if the payload is in flight. Used with FlightCheck function
unsigned long flightStart = 0;                        //Time passed since inFlight became true
byte FlightCheckCounter = 0;                          //If this reaches 5, then inFLight should be set to true


//GPS Operation

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


String getsat() {
  String sat = "";
  sat = String(GPS.satellites.value());
  return sat;
}

void FlightCheck() {                                                              //Function that repeatedly checks if Payload is in flight.
  if (GPS.Fix && (GPS.altitude.feet() > 5000)  && (GPS.location.lng() != 0)) {    //HASP vehicle takeoff altitude is at roughly 4000 feet.
    FlightCheckCounter++;                                                         //If three conditions are met, FlightCheckComputer gets a hit.
    if (FlightCheckCounter >= 5) {                                                //5 FlightCheckCounter hits in a row needed to set inFlight to true
      inFlight = true;                                                            //Bool that indicates if the payload is in flight.
      flightStart = millis();                                                     //flightStart holds the time when inFlight becomes true since the batteries for the payload are powered on
      flightstring = "True";
    }
  }
  else {
    FlightCheckCounter = 0;                                                       //If the three conditions aren't met, FlightCheckCounter loses all of its hits.
  }
}

void writeSensorsSD(){
  datalog = SD.open(filename, FILE_WRITE);
  data = SDprintGPS();
  datalog.println(data);

  Serial.println(data);
  datalog.close();
}


//***SD will only print good data if the inFlight condtion is satisfied. This can easily be fixed by deleting that condition requirement.***

String SDprintGPS() {                                                //Function that takes GPS data and prints it to the SD card
  if (GPS.Fix && GPS.altitude.feet() != 0) {                         //If GPS has a fix with good data (may need more parameters to define "good data", GPS data is printed
    GPSdata = getdate() + ", " + gettime() + ", " + getlat() + ", " + getlong() + ", " + getalt() + ", " + getsat() + ", " + flightstring;

  }
  else {                                                                         //If GPS has bad data or doesn't have a fix, zeros are printed for all three variables
    GPSdata = faildate + ", " + failtime  + ", " + faillatitude + ", " + faillongitude + ", " + failalt + ", " + getsat() + ", " + flightstring;    
  }

  return GPSdata;
}



void updateGPS() {                                                              //Function that updates GPS every second and accounts for
  static bool firstFix = false;                                                 //clock rollover at midnight (23:59:59 to 00:00:00)
  while (gps_serial.available() > 0) {                                         //*check to see if this is the correct serial
    GPS.encode(gps_serial.read());
    }
  if (GPS.altitude.isUpdated() || GPS.location.isUpdated()) {
    if (!firstFix && GPS.Fix) {     
      GPSstartTime = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
      firstFix = true;                                                          //Time in second of GPS clock will now be compared to GPS Start Time
      }
    }
  if (getGPStime() > lastGPS) {    //if it's been more than a second
        lastGPS = GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second();
    }   }

   
unsigned int getGPStime() {
  return (GPS.time.hour() * 3600 + GPS.time.minute() * 60 + GPS.time.second());   
}


int getLastGPS() {                                                   //returns time in seconds between last successful fix and initial fix. Used to match with altitude data
  static bool newDay  = false;                                       //variable in case we're flying late at night (clock rollover)
  if (!newDay && lastGPS < GPSstartTime) {
    days++;
    newDay = true;
  }
  else if (newDay && lastGPS > GPSstartTime)
    newDay = false;
  return days * 86400 + lastGPS;    }






void setup() {
  Serial.begin(9600);
  gps_serial.begin(4800);             //Initializes serial port for GPS communication

  pinMode(sdLED, OUTPUT);
  pinMode(fixLED, OUTPUT);
  pinMode(stateLED, OUTPUT);

  Serial.print("Initializing SD card...");
  
  if(!SD.begin(chipSelect)) {                               //attempt to start SD communication
    Serial.println("Card failed, or not present");          //print out error if failed; remind user to check card
    for (byte i = 0; i < 25; i++) {                         //also flash error LED rapidly for 5 seconds, then leave it on
      digitalWrite(sdLED, HIGH);
      delay(100);
      digitalWrite(sdLED, LOW);
      delay(100);
    }
    digitalWrite(sdLED, HIGH);
  }
  
  else {                                                    //if successful, attempt to create file
    Serial.println("Card initialized.\nCreating File...");
    for (byte i = 0; i < 100; i++) {                        //can create up to 100 files with similar names, but numbered differently
      filename[6] = '0' + i/10;
      filename[7] = '0' + i%10;
      if (!SD.exists(filename)) {                           //if a given filename doesn't exist, it's availables
        datalog = SD.open(filename, FILE_WRITE);            //create file with that name
        SDactive = true;                                    //activate SD logging since file creation was successful
        Serial.println("Logging to: " + String(filename));  //Tell user which file contains the data for this run of the program
        break;                                              //Exit the for loop now that we have a file
      }
    }
  }
    
  if (!SDactive) {
      Serial.println("No available file names; clear SD card to enable logging");
      for (byte i = 0; i < 10; i++) {                        //flash LED more slowly if error is too many files (unlikely to happen)
        digitalWrite(sdLED, HIGH);
        delay(250);
        digitalWrite(sdLED, LOW);
        delay(250);
      }
      digitalWrite(sdLED, HIGH);
    }

  String header = "GPS Date, GPS time, Latitude, Longitude, Altitude(ft), Number of Satellites, inFlight Status";  //setup data format, and print it to monitor and SD card
  Serial.println(header);
  if (SDactive) {
    datalog.println(header);
    datalog.close();
  }
  
}



void loop() {
  
  if (millis() - fixtimer > 10000 && GPS.Fix && GPS.altitude.feet() != 0) {
    fixtimer = millis(); //both fixtimer and LEDlocktimer set every 10 seconds
    LEDlocktimer = millis();
    digitalWrite(fixLED, HIGH);
    fixLEDbool = true; 
    }
  
  updateGPS();    
  FlightCheck();

  if (millis() - timer > 1000) {
    timer = millis();  //every second data is logged to the serial monitor and SD card

    writeSensorsSD(); //data printed to serial
    
    if (SDactive) {
      digitalWrite(sdLED, HIGH); //if SD is active, then the red LED flases everytime data is logged
      datalog = SD.open(filename, FILE_WRITE);
      SDprintGPS(); //data printed to SD
      datalog.close();
      digitalWrite(sdLED, LOW);
      }
   }

  if (fixLEDbool && (millis() - LEDlocktimer > 1000)) { //when the GPS has a fix, this ensures that the
    digitalWrite(fixLED, LOW);                    //green LED is on for a full second every 10 seconds
    fixLEDbool = false;  
   }
 }

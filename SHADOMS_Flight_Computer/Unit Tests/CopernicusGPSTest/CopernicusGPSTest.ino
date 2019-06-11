#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//pin definitions
  #define GPS_RX 7                  //GPS Recieve Pin         SERIAL 2
  #define GPS_TX 8                  //GPS Transmission Pin
  #define chipSelect 10             //Port for writing to SD card


 TinyGPSPlus GPS;
 SoftwareSerial gps_serial (7,8);
 File CapLog;                       




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

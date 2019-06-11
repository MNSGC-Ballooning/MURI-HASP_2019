// Serial Communication Unit Test

//Libraries
#include <SoftwareSerial.h>       //Software serial comms

#define HASP_RX 0                 //HASP Recieve Pin                SERIAL 1
#define HASP_TX 1                 //HASP Transmission Pin

// variables
String data; // data string from payload
uint8_t com; // second command bit to send (AA,BB,CC) (170,187,204)

void setup()
{
  //Serial Definitions
  //HASP Uplink and downlink data
  String flightState = "";
  String OPCState = "";
  byte packet[32] = {0};
  
  //Serial Initialization
  Serial1.begin(1200); // line to payload
  Serial.begin(9600); // line to computer
}

void loop()
{
  // read data downlink from payload
  if( Serial1.available() > 0)
  {
    data = Serial1.readString();
    Serial.println(data);
  }

  // send commands to payload
  Serial.print("Enter command 1: "); // probably can enter as hexidecimal, i.e. AA, but may need to enter as decimal
  while( Serial.available() == 0 ){}
  com = String(Serial.read(),HEX);
  Serial1.write(com); // writes a string, Serial Comm code probably will need to do some string comparing to check which command it is
}

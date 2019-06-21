//Active Heating

//Libraries
  #include <LatchRelay.h>           //Relay Control
  #include <SPI.h>                  //Serial connections
  #include <OneWire.h>              //Dallas data control
  #include <DallasTemperature.h>    //Dallas temperature control

//Pin Definitions
  #define wireBus1 29               //Temperature sensor 1 pin - Internal Ambient
  #define wireBus2 30               //Temperature sensor 2 pin - External Ambient
  #define wireBus3 31               //Temperature sensor 3 pin - OPCs
  #define heater_ON 24              //Heater relay pins
  #define heater_OFF 25             //^^^
  #define LED_1 20                  //Temp light 1
  #define LED_2 21                  //Temp light 2
  #define LED_3 22                  //Temp light 3

//Constants
  #define COLD 280.0                //Minimum acceptable temperature of the OPC
  #define HOT 290.0                 //Maximum acceptable temperature of the OPC
  #define KELVIN 273.15             //Number to convert Celcius to Kelvin

//Active Heating Definitions
  LatchRelay heater(heater_ON, heater_OFF);             //Define heater relay object
  bool coldOPC = false;

//Temp Definitions 
  OneWire oneWire1(wireBus1);                           //Temperature sensor 1 data interface
  OneWire oneWire2(wireBus2);                           //Temperature sensor 2 data interface
  OneWire oneWire3(wireBus3);                           //Temperature sensor 3 data interface
  DallasTemperature temperature1(&oneWire1);            //Temperature sensor 1
  DallasTemperature temperature2(&oneWire2);            //Temperature sensor 2
  DallasTemperature temperature3(&oneWire3);            //Temperature sensor 3
  float t1;                                             //Temperature 1
  float t2;                                             //Temperature 2
  float t3;                                             //Temperature 3

void setup() {
//Relay Initialization- sets up all relays, sets them all to an open state.
  heater.init(0);

//LED Initialization
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  
}

void loop() {
  temperature1.requestTemperatures();
  temperature2.requestTemperatures();
  temperature3.requestTemperatures(); 

  t1 = temperature1.getTempCByIndex(0) + KELVIN;
  t2 = temperature2.getTempCByIndex(0) + KELVIN;
  t3 = temperature3.getTempCByIndex(0) + KELVIN;

  activeHeat();

  Serial.println("t1 = " + String(t1));
  Serial.println("t2 = " + String(t2));
  Serial.println("t3 = " + String(t3)); 

for (int i = 0; i<2; i++){
  if (t1 > 280) 
    digitalWrite(LED_1, HIGH); 
  if (t2 > 280)
    digitalWrite(LED_2, HIGH); 
  if (t3 > 280) 
    digitalWrite(LED_3, HIGH); 
  
   delay (750);
   
   if (t1 > 280) 
     digitalWrite(LED_1, LOW); 
   if (t2 > 280) 
     digitalWrite(LED_2, LOW); 
   if (t3 > 280) 
     digitalWrite(LED_3, LOW); 
  
   delay(750);
}
}

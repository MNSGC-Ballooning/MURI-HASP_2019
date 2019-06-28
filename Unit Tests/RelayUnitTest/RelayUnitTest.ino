// Relay Unit Test
// Garrett Ailts

#include <LatchRelay.h>

///////Pin Definitions/////////
  #define heater_ON 24              //Heater relay pins
  #define heater_OFF 25             //^^^
  #define alphaOPC_ON 5             //Alphasense OPC relay pins
  #define alphaOPC_OFF 6            //^^^
  #define planOPC_ON 26             //Plantower OPC relay pins
  #define planOPC_OFF 27            //^^^
  #define LOAC_ON 7                 //LOAC OPC power relay pins
  #define LOAC_OFF 8                //^^^

////////Relay Objects//////////
  LatchRelay heater(heater_ON, heater_OFF);             //Define heater relay object
  LatchRelay alphaOPC(alphaOPC_ON, alphaOPC_OFF);       //Define Alphasense OPC relay object
  LatchRelay planOPC(planOPC_ON, planOPC_OFF);          //Define Plantower OPC relay object
  LatchRelay LOAC(LOAC_ON, LOAC_OFF);                   //Define LOAC OPC power relay object

void setup() {
  alphaOPC.init(0);                       //Sets all relays to an open state
  planOPC.init(0);
  LOAC.init(0); 
  heater.init(0);

}

void loop() {
  alphaOPC.setState(false);
  delay(1000);
  planOPC.setState(false);
  delay(1000);
  LOAC.setState(false);
  delay(1000); 
  heater.setState(false);
  delay(1000);
}

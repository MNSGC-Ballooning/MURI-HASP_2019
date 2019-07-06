//Active Heating Operation
void thermalInit(){                                   //This function initializes the thermal monitoring and control system.
      heater.init(0);                                 //Heater relay initialization
      temperature1.begin();                           //Temperature sensor initialization
      temperature2.begin();
      temperature3.begin();
}

void updateTemp(){
    temperature1.requestTemperatures();               //Request temperatures from the sensors.
    temperature2.requestTemperatures();
    temperature3.requestTemperatures();
    t1 = temperature1.getTempCByIndex(0) + KELVIN;    //Pull the temperatures from the objects,
    t2 = temperature2.getTempCByIndex(0) + KELVIN;    //and converts those temperatures from degrees Celcius
    t3 = temperature3.getTempCByIndex(0) + KELVIN;    //to Kelvin.
}

void activeHeat(){
  if ((150 < t2) && (t2 < COLD)) {                    //Check if the temperature of the OPCs exceed the minimum
    coldOPC=true;                                     //or maximum heating temperatures.
  } else if (t2 > HOT) {
    coldOPC=false;
  }

  if (coldOPC && heater.getState()==0) {              //If the OPCs are cold, the heating mesh will activate until
    heater.setState(1);                               //the OPCs are no longer cold.
    heatState = '1';
  } else if (!coldOPC && heater.getState()==1) {      //ERROR HERE IF STATES ARE NOT 1 AND 0
    heater.setState(0);
    heatState = '0';
  }
}

//ADD TO MAIN
void activeCool(){
  if (LOAC.getState()==1) {                           //Only goes through if LOAC is on
    if (MIN_FANTEMP <= t3) {                          //Set hotOPC to true if t3 is above 300K
    hotOPC=true;
    }
    else if (t3 < MIN_FANTEMP) {                      //Set hotOPC to false if t3 is below 300K
    hotOPC=false;
    }
  }

  if (hotOPC && fanState == '0') {                    //Turns the fan on if hotOPC is true and the
    digitalWrite(LOAC_FAN, HIGH);                     //fan is off
    fanState = '1'; 
  }
  else if (!hotOPC && fanState == '1') {              //Turns the fan off if hotOPC is false and the
    digitalWrite(LOAC_FAN, LOW);                      //fan is on
    fanState = '0';
  }
}

void ThermalControl(){
  activeHeat();                                       //Checks for the system temperature and runs active controls
//  activeCool();
  
  if ((overrideTimer != 0) && (millis()-overrideTimer >= OVERRIDE_TIME))  {
    overRide = false;                                 //if the system override is completed, this will end the override state
    overrideTimer = 0;
  }

  if (((t3 >= MAX_TEMP)||(t3 <= MIN_TEMP))&&(!overRide)&&(!danger)) {
    danger = true;                                    //if the system reaches a dangerous temperature and it has not yet recognized it
    standbyMode();                                    //and there is no override, then the system will shutdown the particle counters.
  } 
  
   if (((t3<MAX_TEMP)&&(t3>MIN_TEMP))&&(danger)) {
    danger = false;                                   //if the system returns to a safe operating temperature, then the warning will end
    activeMode();                                     //and the system will return to normal operation.
  }
}

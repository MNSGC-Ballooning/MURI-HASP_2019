//Active Heating Operation
void thermalInit(){                                   //This function initializes the thermal monitoring and control system.
      heater.init(0);                                 //Heater relay initialization
      temperature1.begin();                           //Temperature sensor initialization
      temperature2.begin();
      temperature3.begin();
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
  
}

void autoShutdown(){
  if ((overrideTimer != 0) && (millis()-overrideTimer >= OVERRIDE_TIME))  {
    overRide = false;
    overrideTimer = 0;
  }

  if (((t3 >= MAX_TEMP)||(t3 <= MIN_TEMP))&&(!overRide)&&(!danger)) {
    danger = true;
    standbyMode();
  } 
  
   if (((t3<MAX_TEMP)&&(t3>MIN_TEMP))&&(danger)) {
    danger = false;
    activeMode();
  }
}

void updateTemp(){
    temperature1.requestTemperatures();                 //Request temperatures from the sensors.
    temperature2.requestTemperatures();
    temperature3.requestTemperatures();
    t1 = temperature1.getTempCByIndex(0) + KELVIN;      //Pull the temperatures from the objects,
    t2 = temperature2.getTempCByIndex(0) + KELVIN;      //and converts those temperatures from degrees Celcius
    t3 = temperature3.getTempCByIndex(0) + KELVIN;      //to Kelvin.

    activeHeat();
}

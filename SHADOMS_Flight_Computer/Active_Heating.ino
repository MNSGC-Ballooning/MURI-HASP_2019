//Active Heating Operation
void activeHeat(){
  if (150 < t1 < cold) {            //Check if the temperature of the OPCs exceed the minimum
    coldOPC=true;                   //or maximum heating temperatures.
  } else if (t1 > hot) {
    coldOPC=false;
  }

  if (coldOPC && heater.getState()==0) {              //If the OPCs are cold, the heating mesh will activate until
    heater.setState(1);                               //the OPCs are no longer cold.
  } else if (!coldOPC && heater.getState()==1) {      //ERROR HERE IF STATES ARE NOT 1 AND 0
    heater.setState(0);
  }
}

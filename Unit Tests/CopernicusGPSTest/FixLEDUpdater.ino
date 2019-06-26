void fixLEDupdater() {
  
  if (millis() - fixLED_loop_timer >= FIXLED_LOOP) {    //FixLED cycle updates every 20 seconds
    fixLED_loop_timer = millis();
    fixLED_length_timer = millis();
    Serial.println("New loop cycle");
    if (GPS.Fix && GPS.altitude.feet() != 0 ) {         //This will send loop down the "Fix" path
      GPSfix = true;
      satnum = GPS.satellites.value();                  //Number of times "Fix" path will blink equals number of satellites that have a lock
    }
    else {                                              //This will send loop down the "No Fix" path
      GPSfix = false;
      nofix_blink_counter = 3;                          //"No Fix" path will be a series of 3 blinks
    }
  }

  if ((millis() - fixLED_length_timer >= FIXLED_RATE) && GPSfix) {        //"Fix" Path
    fixLED_length_timer = millis();                     //Resets LED length timer
    if (!fixLEDon && satnum > 0) {                      //Only occurs if number of blinks occured is less than the number of satellites locked
      digitalWrite(fixLED, HIGH);     
      fixLEDon = true;                                  //LED is now on
      satnum--;                                         //satnum variable is decreased
    }
    else {
      digitalWrite(fixLED, LOW);                        
      fixLEDon = false;                                 //LED is now off
    }
  }

  if ((millis() - fixLED_length_timer >= NOFIXLED_RATE) && !GPSfix) {     //"No Fix" Path
    fixLED_length_timer = millis();                     //Resets LED length timer
    if (!fixLEDon && nofix_blink_counter > 0) {         //Only occurs if number of blinks occurred is less than 3
      digitalWrite(fixLED, HIGH);
      fixLEDon = true;                                  //LED is now on
      nofix_blink_counter--;                            //nofix blink counter is decreased
    }
    else {
      digitalWrite(fixLED, LOW);
      fixLEDon = false;                                 //LED is now off
    }
  }
}

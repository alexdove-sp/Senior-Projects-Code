
void normOP( ) {
  digitalWrite(rdLed, LOW);
  isActive( );
  chkSemi_Dormant( );
  chkGPS5( );
}


void isActive( ) {
  //DISPLAY ACTIVE MODE
  if (active_Mode) {
    Blynk.setProperty(V11, "offLabel", "ACTIVE");
    Blynk.setProperty(V11, "offBackColor", BLYNK_GREEN);
    Serial.println("ACTIVE");
    active_Mode = false;
    //blynk = Blynk.connected( );
  }
}

void chkSemi_Dormant( ) {

  if (timer_chk(pt4, sdormant_tm))//Timer to enter semi_dormant function
  {
    norm_op = false;
    semi_dormant = true;
    flag10 = false;//////
    sleep = true;
    digitalWrite(blLed, LOW);
    //pt2 = millis( );//reset time till hibernation
    pt1 = millis( );//Start flip_time1 timer
  }

}

void chkGPS5( ) {
  boolean found;
  if (timer_chk(pt3, flip_time2))//Chk GPS Every 5seconds
  {
    flag11 = !flag11;
    digitalWrite(blLed, flag11);
    pt3 = millis( );//reset flip

  }


  //DISPLAY LOST STATUS
  if (tracker % 25 == 0)
    reportLostStatus( );

  //EVERY 5 SECONDS CHECK GPS FOR UP TO 5 SECONDS
  if (flag11) {
    while (ss.available( ) > 0) {
      if (gps.encode(ss.read( ))) {
        found = reportCurrentInfo( );
        if (!found) { //BOOLEAN LOGIC FLIPPED HERE
          tracker = 1;
          flip_time2 = ex_it;
          break;
        }
        else
          tracker++;
        break;
      }
    }//end while
  }//end if flag11
  else
    flip_time2 = CLK_SEC * 3;

}//end chk gps

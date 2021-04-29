
void semi_Dormant( ) {
  boolean report;
  if (timer_chk(pt1, flip_time1)) {
    flag10 = !flag10;//flip flag depending on flip_time1 value
    pt1 = millis( );//reset flip interval
    Serial.print("Semi-Dormant Flip"); Serial.println(clock_count);
  }
  //WAKEUP ROUTINE
  if (flag10) {
    while (!sleep) {
      WiFi.persistent(false);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pass);
      delay(100);
      ss.listen( );
      sleep = true;
      flip_time1 = CLK_SEC * 30;
    }

    if (WiFi.status( ) == WL_CONNECTED) {
      if (!blynk) {
        Blynk.config(auth);
        Blynk.connect( );
      }
      while (blynk && !wynk) {
        Blynk.setProperty(V10, "color", BLYNK_GREEN);
        Blynk.setProperty(V11, "offLabel", "ACTIVE");
        Blynk.setProperty(V11, "offBackColor", BLYNK_GREEN);

        digitalWrite(rdLed, HIGH);

        pt1 = millis( );//reset flip interval
        wynk = true;//exit while loop
      }//end while

      encod = false;

      while (blynk && !encod && clock_count % 5 == 0) {
        while (ss.available( ) > 0) {
          if (gps.encode(ss.read( ))) {
            report = reportCurrentInfo( );//changed wynk to report 1/25
            if (!report) {
              //flip_time1 = ex_it;
              //wynk = true;//next tim around enter while loop until isLost( )// commented out 1/25
              encod = true;//exit while loop
              tracker = 1;
              Serial.println("SD - display");//Serial.println(flip_time1);
            }
            else {
              tracker++;
              encod = true;//exit while loop
              if (isLost(pt1, flip_time1) && tracker > 50) {
                reportLostStatus( );
                Serial.println("Lossst SS loop");
                flip_time1 = ex_it;
              }
            }
          }//end if gps.encode
        }//end while(ss)
        
        encod = true;
        
      }//end while(blynk && !encode
    }//end if wifi


    else {     //if !wifi
      if (isLost(pt1, flip_time1)) {
        reportLostStatus( );
        Serial.println("WiFi neveron & Lost");
        flip_time1 = ex_it;
      }
      //sleep = false;//added 2/7/21
    }//end else
  }//end if flag10



  //SLEEP ROUTINE
  else {
    while (sleep) {
      Blynk.setProperty(V11, "offBackColor", BLYNK_DARKBLUE);
      Blynk.setProperty(V11, "offLabel", "LITE-SLEEP");
      digitalWrite(rdLed, LOW);
      delay(200);
      WiFi.mode(WIFI_OFF);
      WiFi.disconnect( );
      wynk = false;
      sleep = false;
    }

    flip_time1 = CLK_SEC * 30;//
  }

}//end semi_dormant activity

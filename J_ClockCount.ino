

//CLOCK COUNT
void clck( ) {
  if (timer_chk(pt5, CLK_SEC)) {
    blynk = Blynk.connected( );//every second check blynk connection
    clock_count++;
    Serial.print(clock_count); Serial.print("..."); Serial.print(" Trkr: "); Serial.println(tracker);
    Serial.print("WiFi "); Serial.print(WiFi.status( ));
    Serial.print("blynk "); Serial.println(blynk);
    //delay(300);//just for debugging
    pt5 = millis( );
  } 
}

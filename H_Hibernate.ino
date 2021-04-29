
void  chckHibernation( ) {

  //CHECK FOR HYBERANATION FUNCTION TIME
  if (timer_chk(pt2, hybrn8_time)) {
    Blynk.setProperty(V11, "offLabel", "ZZZZZZZ...");///////////////1/13
    Blynk.setProperty(V11, "offBackColor", BLYNK_RED);/////////////1/13
    delay(500);
    Blynk.virtualWrite(V5, LOW);
    Blynk.setProperty(V5, "offLabel", "NO GPS");
    Blynk.setProperty(V5, "offBackColor", BLYNK_RED);
    Serial.println("By");
    Serial.end( );
    delay(500);
    hibernate = true;
    semi_dormant = false;
    

  }
  if (hibernate) {
    //digitalWrite(pwr, HIGH);//Turn off device
    //delay(MIN);
    ESP.deepSleep(0); //  external wake up
    
  }
}



void monitor_batt( ) {
  if (timer_chk(pt6, flip_time6)) {
    //internalVoltage = ESP.getVcc()/1000;
    flip_time6 = CLK_SEC * 10;
    float correctedStartValue = float(ESP.getVcc() / 1000.0) - MIN_VOLT;
    int percentage = (correctedStartValue * 100.0) / range;
    Blynk.virtualWrite(V9, percentage);
    Serial.print("Battery: "); Serial.println(percentage);
    Serial.print("Voltage: "); Serial.println(correctedStartValue + MIN_VOLT);
    pt6 = millis( );
  }
}



/*
  void monitor_batt( ){
   if(timer_chk(pt6, flip_time6)){
    pt6 = millis( );
    battery =  getBattery( );
    flip_time6 = CLK_SEC * 10;
    Blynk.virtualWrite(V9, battery);
    Serial.printf("Battery = %.2fV", battery);
  }
  }
  float getBattery( ){
  int vlt = analogRead(BATTPin);//
  return vlt * BITStoVOLT;// (3.3V/1023bits)*1.92258(node mcu scaler)  = .006202
  }
*/

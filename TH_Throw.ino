



boolean isThrowing( ) {
  //Check and see if two buttons are depressed and gpsModule is currently tracking
  //tracker < 10 signifies device has a gps fix
  if (tracker < 10 && find_throwPin && trackPin) {
    throwing = true;//this needs to be set false at end of throwing routine
    return true;
  }
  else
    return false;
}

void throwDisc( ) {
  bool report_throwing = true;
  //Perform this while loop once
  while (!throw_color) {
    while (report_throwing) {
      //Blynk.setProperty(V8, "onBackColor", BLYNK_DARKBLUE);
      Blynk.setProperty(V8, "onLabel", "THROW");
      Blynk.virtualWrite(V5, LOW);
      digitalWrite(blLed, HIGH);
      delay(200);
      norm_op = false;
      semi_dormant = false;
      wifii = true;
      report_throwing = false;
    }  
    //from here disconnect wifi do this once only
    while (wifii) {
      Blynk.setProperty(V11, "offBackColor", BLYNK_DARKBLUE);
      Blynk.setProperty(V11, "offLabel", "THROW");
      digitalWrite(rdLed, LOW);
      delay(200);
      WiFi.mode(WIFI_OFF);
      WiFi.disconnect( );
      Serial.print("lat "); Serial.print(latStart, 3); Serial.print("lng "); Serial.print(lngStart, 3);
      Serial.print("alt "); Serial.print(altStart, 3); Serial.print("spd "); Serial.println(grnd_spdStart, 3);
      pt10 = millis( );// reset pt10 for disc flight check
      //blynk = false;
      wifii = false;
    }
    throw_color = true;//break out of throw color loop
  }

  //Here: throw_color = true, norm_op = false, semi_dormant = false, throwing = true
  //Here it is accumulating throwing data
  int count = 0;
  while (ss.available( ) > 0 && count < 11) {
    if (gps.encode(ss.read( ))) {
      latFin = gps.location.lat( );
      lngFin = gps.location.lng( );
      //altFin = gps.altitude.feet( );
      grnd_spdFin = gps.speed.mph( );
      if (grnd_spdFin > max_grndSpd)
        max_grndSpd = grnd_spdFin;
      Serial.print("LAT:  ");
      Serial.println(latFin, 9);
      Serial.print("LONG: ");
      Serial.println(lngFin, 9);
      Serial.print("MAX: ");
      Serial.println(max_grndSpd);
      count = 11;
      tracker = 1;
    }
    else {
      tracker++;
      Serial.println("...");
      count++;
    }
  }//end while(ss.available)
  

  if (timer_chk(pt10, flip_time10))//After 20 seconds flight speed check?
  {
    if (max_grndSpd <= grnd_spdStart + 4.0) {
      Serial.println("NO THROW!");
      WIFIconnect( );
      Blynk.virtualWrite(V4, 0);//maxgrnd spd error
      Blynk.virtualWrite(V3, 0.00);//max distance error
      //Blynk.virtualWrite(V4, "label", "DISTANCE");
      //Blynk.setProperty(V3, "label", "");
    }
  }
  if (tracker % 200 == 0) {
    Serial.println("GPS ERROR!");//Lost the fix if tracker gets to 10000?????
    WIFIconnect( );
    Blynk.virtualWrite(V4, 0);//maxgrnd spd error
   //Blynk.virtualWrite(V4, "label", "ERROR: GPS-LOST");
    Blynk.virtualWrite(V3, 0.00);//max distance error
    //Blynk.setProperty(V3, "label", "ERROR: GPS-LOST");
  }
  if (grnd_spdFin <= 1.0 && max_grndSpd > grnd_spdStart + 4.0) {
    WIFIconnect( );
    Serial.print("latf "); Serial.print(latFin, 3); Serial.print("lngf "); Serial.print(lngFin, 3); 
    Serial.print("altf "); Serial.println(altFin, 3);//Serial.print("max "); Serial.println(max_grndSpd, 3);
    Serial.print("Distance: "); Serial.print(distance(latStart, lngStart, latFin, lngFin));Serial.println("ft");
    Serial.print("Max Gspd: "); Serial.print(max_grndSpd, 2); Serial.println("mph");
    delay(10000);
    Blynk.virtualWrite(V4, max_grndSpd);
    //Blynk.virtualWrite(V4,"label", "Mph");
    Blynk.virtualWrite(V3, distance(latStart, lngStart, latFin, lngFin));
    //Blynk.setProperty(V3, "label", "Ft");
  }
  
}//end throwDisc()



double distance(double lat1, double lng1, double lat2, double lng2) {
  double distanceKm = (gps.distanceBetween(lat2, lng2, lat1, lng1))/1000.0;
  //Not 100% this accounts for change in altitude.
  double distanceFt = (distanceKm * 1000.0) * 3.28; //1000m/Km * 3.28ft/m
  return distanceFt;
}

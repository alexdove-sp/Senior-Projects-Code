

bool reportCurrentInfo( ) {
  if (gps.location.isUpdated( )) {
    latStart = (gps.location.lat());     //Storing the Lat. and Lon.
    lngStart = (gps.location.lng());
    grnd_spdStart = (gps.speed.mph( ));
    altStart = (gps.altitude.feet( )); //Storing Altitude.
    max_grndSpd = grnd_spdStart;
    Serial.print("LAT:  ");
    Serial.println(latStart, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(lngStart, 6);
    Serial.print("SPEED: ");
    Serial.println(grnd_spdStart);
    
    Blynk.virtualWrite(V5, LOW);
    Blynk.setProperty(V5, "offLabel", "TRACKING");
    Blynk.setProperty(V5, "offBackColor", BLYNK_DARKBLUE);
    myMap.location(move_index, latStart, lngStart, "Disc_Location");
    
    return false;
  }

  Serial.println("...");
  return true;
}
void reportLostStatus( ) {
  Serial.println("Lost");
  Blynk.virtualWrite(V1, String("Lost SIG"));
  Blynk.virtualWrite(V2, String("Lost SIG"));
  Blynk.virtualWrite(V5, LOW);
  Blynk.setProperty(V5, "offLabel", "NO GPS");
  Blynk.setProperty(V5, "offBackColor", BLYNK_RED);
  //delay(100);
}

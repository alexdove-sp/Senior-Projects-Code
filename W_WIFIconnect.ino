//This will be a blocking function and code will not progress passed this functions completion
void WIFIconnect( ) {
  boolean wifi = false;
  unsigned long int pt7 = millis( );
  int flip_time7 = CLK_SEC * 10;
  int count = 0;
  Serial.println("reconnect");
  Serial.print("Count: "); Serial.println(count);

  while (!wifi) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    boolean blynking = false;
    while (!blynking) {
      if (timer_chk(pt7, flip_time7)) {
        blynking = true;
        pt7 = millis( );
        count = count + 10;
        Serial.print("-"); Serial.println(count);
      }
      //put a timer here in 20 seconds if hasn't connected make blinking true and mmake wifi false
      if (WiFi.status( ) == WL_CONNECTED) {
        while (!blynk) {
          Blynk.config(auth);
          Blynk.connect( );
          Serial.print("-");
          delay(1000);
          if(Blynk.connected( )){
            blynk = true;
            blynking = true;
            wifi = true;
          }
        }
      }
      else {
        Serial.print(".");
        delay(1000);
      }
    }
  }

  norm_op = true;
  semi_dormant = false;
  active_Mode = true;
  pt2 = pt4 = millis( );
  clock_count = 0;

  //When connecting while using the throw function
  if (throwing == true) {
    throwing = false;
    throw_color = false;
    Blynk.setProperty(V8, "onLabel", "FIND");
    Blynk.virtualWrite(V8, HIGH);
    digitalWrite(blLed, LOW);
    delay(300);
  }

}

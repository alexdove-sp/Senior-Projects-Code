
boolean timer_chk(unsigned long int pt, unsigned int ft ) {
  return millis( ) - pt >= ft;
}


boolean isLost(unsigned long int pvt, unsigned int fliptm) {
  return millis( ) - pvt >= fliptm - 50;
}

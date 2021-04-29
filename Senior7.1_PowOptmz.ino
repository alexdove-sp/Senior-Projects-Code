//Current version has reverted back to Hard/ON and Soft/OFF  2/5/21
//Running batt_monitor Heins
//Throw function updated & Wifi reconnect

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG //WARNING WARNING SLOWS DOWN HARDWARE PROCESSING UP TO 10X.
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
ADC_MODE(ADC_VCC);


#define MIN 60000
#define CLK_SEC 1000
#define GPSBAUD 9600
#define RXPIN 0//GPIO2 = D4(Tx of Gps)
#define TXPIN 2//GPIO0 = D3(Rx of Gps)
#define MAX_VOLT 3.1//Max operating voltage of NodeMCU Esp8266
#define MIN_VOLT 2.5//Min opertaing voltage of NodeMCU Esp8266
//#define BATTPin A0//Analog input

//GPIO VARIABLES
static const int rdLed = 5;//GPIO5 = D1
static const int blLed = 4;//GPIO4 = D2
int find_throwPin;//Connected to V8
int trackPin;//Connected to V5
int pwrPin;//Connected to V11
static const int niteLed = 12;//GPIO12 = D6
static const int buzzer = 14;//GPIO14 = D5
static const int pwr = 13;//Transistor pin base GPIO13 = D7
static const int pwrBkUp = 15;//GPIO15 = D8

//TIMING VARIABLES
unsigned long int pt1;//previous time in semidormant
volatile unsigned long int pt2;//hibernation previous time
unsigned long int pt3;//chck gps every 5seconds in normOp
volatile unsigned long int pt4;//chk semi_dormant previous time. Called from normOP
unsigned long int pt5;//clock count previous time 1 seond
unsigned long int pt6;//battery monitoring previous time
unsigned long int pt10;//timer for throwing function


int ex_it = 1;
volatile int clock_count;
unsigned int flip_time1;
unsigned int flip_time2;
unsigned int flip_time6;
unsigned int flip_time10;
unsigned int hybrn8_time;
unsigned int sdormant_tm;

//TIMING FLAGS
volatile boolean semi_dormant;
volatile boolean norm_op;
boolean flag10;
boolean flag11;
boolean hibernate;


//DATA/ FLOW-CONTROL
long int tracker;
boolean sleep;
boolean blynk;
boolean wynk;
boolean encod;
boolean active_Mode;
boolean throw_color;
boolean throwing;
boolean wifii;
const float range = MAX_VOLT - MIN_VOLT;
//float battery;
//const float BITStoVOLT = .00323;// (3.3v/1023bits)*1.92258(NodeMcu scaler)=.006202
unsigned int move_index = 1;


//THROWING PARAMETERS
double latStart, lngStart, altStart, grnd_spdStart;
double latFin, lngFin, altFin, grnd_spdFin;
double max_grndSpd;

//WIFI VARIABLES
WiFiSleepType_t LITE_SLEEP = WIFI_LIGHT_SLEEP;//
WiFiSleepType_t MOD_SLEEP = WIFI_MODEM_SLEEP;//

//WIFI/BLYNK NETWORK AUTHORIZATION
char auth[] = "CmalslP0bAFuEOdN5QmlCebVFwkPp2eT";                     //Your Project authentication key
char ssid[] = "Cooper"/*"NETGEAR14"*/;                         // Name of your network (HotSpot or Router name)
char pass[] = "8b60020e646d"/*"bravecar489"*/;                         // Corresponding Password


//BLYNK COLOR CODES:
#define BLYNK_GREEN "#23C48E"
#define BLYNK_BLUE "#04C0F8"
#define BLYNK_YELLOW "#ED9D00"
#define BLYNK_RED "#DE283E"
#define BLYNK_DARKBLUE "#5F7CD8"
#define BLYNK_WHITE "#FFFFFF"
#define BLYNK_BLACK "000000"

//VARIOUS OBJECTS
TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
SoftwareSerial ss(RXPIN, TXPIN);  // The serial connection to the GPS device
BlynkTimer timer;

//PROTOTYPES
boolean timer_chk(unsigned long int, unsigned int);//Prototype
boolean isLost(unsigned long int, unsigned int);
void normOP( );
void semi_Dormant( );
bool reportCurrentInfo( );
void reportLostStatus( );
void isActive( );
void chkSemi_Dormant( );
void chkGPS5( );
void monitor_batt( );
float getBattery( );
void clck( );
boolean isThrowing( );
void throwDisc( );
void WIFIconnect( );
double distance(double, double, double, double);

//BLYNK-VIRTUAL WRITE/ READ FUNCTIONS:
BLYNK_WRITE(V11) {
  int pwrPin = param.asInt( );
  if (pwrPin == 1)
    digitalWrite(pwr, LOW);
  else
    digitalWrite(pwr, HIGH);
  Serial.print("pwrPin "); Serial.println(pwrPin);
}
/*
BLYNK_WRITE(V9) {
  float battery = param.asFloat( );
  Serial.print("Batt "); Serial.println(battery);
}
*/
BLYNK_WRITE(V8) {
  find_throwPin = param.asInt( );
  Serial.print("Find/Throw "); Serial.println(find_throwPin);
}

BLYNK_WRITE(V5) {
  trackPin = param.asInt( );
  Serial.print("Track button "); Serial.println(trackPin);
}

BLYNK_CONNECTED( ) {
  Blynk.syncVirtual(V11);
  //Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V5);
}


//INTERRUPTS
void ICACHE_RAM_ATTR ISR_find( ) {
  semi_dormant = false;
  norm_op = true;
  active_Mode = true;
  pt4 = pt2 = millis( );
}

void setup()
{
  pinMode(pwr, OUTPUT);//GPIO13 = D7
  digitalWrite(pwr, HIGH);//Signal to Relay/Mosfet....Turns off device when high
  pinMode(pwrBkUp, OUTPUT);//GPIO15 = D8
  digitalWrite(pwrBkUp, HIGH);//Power transistor bkup

  pinMode(rdLed, OUTPUT);//GPIO5 =D1
  pinMode(blLed, OUTPUT);//GPIO4 =D2
  pinMode(niteLed, OUTPUT);//GPIO 12 = D6
  pinMode(buzzer, OUTPUT);//GPIO14 = D5

  //pinMode(BATTPin, INPUT);//Analog input

  digitalWrite(buzzer, LOW);//PIN NORMALLY LOW
  digitalWrite(niteLed, LOW);//PIN NORMALLY LOW


  Serial.begin(115200);
  ss.begin(GPSBAUD);//Begin comms with gps module
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  WiFi.persistent(false);
  Blynk.begin(auth, ssid, pass);

  //SEMI_DORMANT: CONFIG
  semi_dormant = false;//Do not enter semi_dormant rountine
  pt1 = 0;//Previous time in semi_dormant
  sdormant_tm = MIN * 3; //Time from norm_op -> semi_dormant op
  flip_time1 = CLK_SEC * 30;

  //DEEP SLEEP: CONFIG
  hibernate = false;//Do not enter hibernate rountine
  pt2 = 0;
  hybrn8_time = MIN * 5;//Time from norm_op -> hibernate op

  //NORMAL OPERATING: CONFIG
  norm_op = true;
  active_Mode = true;
  Blynk.virtualWrite(V8, LOW);
  Blynk.setProperty(V8, "onLabel", "FIND");
  find_throwPin = 0;
  pt3 = 0;//Gps timer
  pt4 = 0;//semi_dormant timer
  pt5 = 0;//clck timer
  flip_time2 = CLK_SEC * 5;//Flippin blLed
  flag11 = false;
  tracker = 1;

  //THROWING FUNCT
  throw_color = false;//First time entering throw_disc function will illuminate Find/Throw button darkblue and with text " Throw"
  throwing = false;//Start up as assuming not throwing
  pt10 = 0;
  flip_time10 = CLK_SEC * 20;

  //BATTERY TIMER SETUP
  flip_time6 = 1;//Call immediately after which update every 5 minutes
  pt6 = 0;//Cooper defined this as reference time

  //CLOCK COUNTER
  clock_count = 0;
  blynk = true;

  //INTERRUPTS CALLS
  //attachInterrupt(find_throwPin, ISR_find, CHANGE);//Blynk V8
  attachInterrupt(digitalPinToInterrupt(buzzer), ISR_find, CHANGE);
  attachInterrupt(digitalPinToInterrupt(niteLed), ISR_find, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pwr), ISR_find, CHANGE);
}


void loop() {

  //MONITOR WIFI. 1-19
  if (!blynk && !semi_dormant && !throwing) { //not semi_dormant because in semi_dormant it is cycling through its own wifi cycle.
    //!throwing because in throwing mode it will reconnect via another avenue.
    WIFIconnect( );//blocking code won't do anything else but try to reconnect.
  }

  //CLOCK FUNCT.
  clck( );

  //POLLING FOR DOUBLE OFF FUNCTION
  if (digitalRead(niteLed))
    digitalWrite(pwrBkUp, LOW);//Energize relay
  else
    digitalWrite(pwrBkUp, HIGH);



  //KEEP CONNECTION IN NORM OPERATION
  if (find_throwPin || digitalRead(buzzer) == HIGH || digitalRead(niteLed) == HIGH && !throwing) {
    pt4 = pt2 = millis( );//Never allow semi_dormant state when find button is pushed.
    norm_op = true;
    semi_dormant = false;
    clock_count = 0;
  }

  //MONITOR BATTERY
  monitor_batt( );

  //THROW FUNCTION
  if (throwing || isThrowing( )) {
    throwDisc( );
  }

  //NORMAL OPERATION
  if (norm_op && blynk) {
    normOP( );
  }

  //PERFORM SEMI_DORMANT FUNCTION EVERY 15SEC-25SEC
  if (semi_dormant) {//Semi_dormant only flips to true if time expires in Normal operation
    semi_Dormant( );
  }

  //CHECK HIBERNATION TIME
  if (flag10 && blynk) {  //flag10 is true when not in LiteSleep in semi_dormant
    chckHibernation( );
  }

  if (blynk) {
    Blynk.run();
  }

}//end loop

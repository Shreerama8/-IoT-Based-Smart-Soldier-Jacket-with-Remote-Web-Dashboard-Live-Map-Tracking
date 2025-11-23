/******************** Libraries ********************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "dht.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/******************** OLED Setup ********************/
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

/******************** DHT11 Setup ********************/
#define dht_apin A1
dht DHT1;

/******************** GPS Setup ********************/
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

/******************** RF24 Wireless Setup ********************/
RF24 radio(9, 10);  // CE, CSN
const byte address[6] = "10101";

/******************** Base Station Coordinates ********************/
const double HOME_LAT = 14.6155;
const double HOME_LNG = 74.8353;

/******************** Global Variables ********************/
double Distance_To_Base;
double GPSlat = 0;
double GPSlng = 0;
double GPSalt = 0;

int beat;
int temp1;
int hum;
int bullet = 0;

float val[8];

int l = 0;
int disp_cnt = 0;

/******************** Setup ********************/
void setup() {

  ss.begin(GPSBaud);
  Serial.begin(9600);
  Serial.println("Soldier Health Monitoring & Navigation System");

  /*************** OLED Initialization ***************/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  String intro[] = {"  Smart", "  Health", "Monitoring", "   And", "  Bullet", " Detection", "  Jacket"};

  for (int i = 0; i < 7; i++) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(intro[i]);
    display.display();
    delay(700);
  }

  pinMode(A0, INPUT);   // Heartbeat sensor
  pinMode(dht_apin, INPUT);
  pinMode(A2, INPUT);   // Bullet sensor
  pinMode(5, OUTPUT);   // Heating coil relay

  /*************** RF24 Initialization ***************/
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  delay(1200);
}

/******************** Loop ********************/
void loop() {

  /*************** Heartbeat Sensor ***************/
  beat = analogRead(A0);
  beat = map(beat, 0, 1023, 0, 120);
  if (beat >= 100) beat = 0;

  Serial.print("Heart Beat: ");
  Serial.println(beat);

  /*************** DHT11 Sensor ***************/
  DHT1.read11(dht_apin);

  hum = DHT1.humidity;
  temp1 = DHT1.temperature;

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%   ");

  Serial.print("Temperature: ");
  Serial.print(temp1);
  Serial.println("C");

  /*************** GPS Data ***************/
  smartDelay(500);

  GPSlat = gps.location.lat();
  GPSlng = gps.location.lng();
  GPSalt = gps.altitude.feet();

  // Default values if GPS fails
  if (GPSlat == 0) GPSlat = 14.6111;
  if (GPSlng == 0) GPSlng = 74.8393;
  if (GPSalt == 0) GPSalt = 710.45;

  Distance_To_Base = (unsigned long)TinyGPSPlus::distanceBetween(GPSlat, GPSlng, HOME_LAT, HOME_LNG);

  Serial.print("GPS Lat: ");
  Serial.println(GPSlat, 4);

  Serial.print("GPS Lon: ");
  Serial.println(GPSlng, 4);

  Serial.print("GPS Alt: ");
  Serial.println(GPSalt);

  /*************** Bullet Detection ***************/
  bullet = analogRead(A2);
  bullet = map(bullet, 0, 1023, 0, 30000);

  Serial.print("Bullet Value: ");
  Serial.println(bullet);

  if (bullet <= 29900) {
    Serial.println("⚠ SOLDIER HIT BY BULLET!");
  }

  /*************** Send Wireless Data ***************/
  sendmessage();
  delay(1000);

  /*************** Heating Coil Logic ***************/
  if (temp1 >= 27)
    digitalWrite(5, LOW);
  else
    digitalWrite(5, HIGH);

  /*************** OLED Display ***************/
  oled_disp();

  disp_cnt++;

  delay(500);

  if (bullet <= 29900) {
    display.setTextSize(1);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("   Signal Sent!!");
    display.println("   Help Arriving");
    display.display();
    delay(800);
  }
}

/******************** Send Data via RF24 ********************/
void sendmessage() {
  val[0] = beat;
  val[1] = temp1;
  val[2] = hum;
  val[3] = bullet;
  val[4] = GPSlat * 100;
  val[5] = GPSlng * 100;
  val[6] = GPSalt * 100;
  val[7] = Distance_To_Base * 100;

  radio.write(&val, sizeof(val));
}

/******************** Smart Delay for GPS ********************/
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

/******************** OLED Display Function ********************/
void oled_disp() {

  if (disp_cnt == 1) {
    String b_pt = " " + String(beat) + " BPM";

    display.setTextSize(2);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(" Pulse:");
    display.println(b_pt);
    display.display();
    delay(600);
  }

  if (disp_cnt == 2) {
    String t_pt = " " + String(temp1) + " C";

    display.setTextSize(2);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(" Temp:");
    display.println(t_pt);
    display.display();

    disp_cnt = 0;
    delay(600);
  }
}

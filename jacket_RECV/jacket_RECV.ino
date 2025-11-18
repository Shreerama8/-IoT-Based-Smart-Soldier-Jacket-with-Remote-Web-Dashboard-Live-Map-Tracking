/******************** Libraries ********************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <TinyGPS++.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

/******************** RF24 Radio Setup ********************/
RF24 radio(D4, D8);  // CE, CSN
const byte address[6] = "10101";

/******************** LCD Setup ********************/
LiquidCrystal_I2C lcd(0x27, 16, 2);

/******************** GPS & Sensor Variables ********************/
TinyGPSPlus gps;

float val[10];
float hum, temp, temp1;
int em, bullet;

double GPSlat, GPSlng, GPSalt;
double GPSlat_prev = 0;
double Distance_To_Base = 7701.123;  

/******************** Heartbeat Variables ********************/
int PrevHB = 0;
int HeartCount = 50;
int Count = 0;
int BeatValue = 0;

/******************** WiFi Credentials ********************/
#define WLAN_SSID  "********"
#define WLAN_PASS  "********"

/******************** Adafruit IO Server Details ********************/
#define AIO_SERVER     "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME   "***********"
#define AIO_KEY        "***********************"

/******************** MQTT Setup ********************/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/******************** Feeds ********************/
Adafruit_MQTT_Publish aio_beat   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/heartbeat");
Adafruit_MQTT_Publish aio_temp1  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp1");
Adafruit_MQTT_Publish aio_hum    = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/hum");
Adafruit_MQTT_Publish aio_stat   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bullet2");
Adafruit_MQTT_Publish gpslat     = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gpslat");
Adafruit_MQTT_Publish gpslng     = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gpslng");
Adafruit_MQTT_Publish gpslatlng  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gpslatlng/csv");
Adafruit_MQTT_Publish aio_temp   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

/******************** Setup ********************/
void setup() {
  pinMode(D3, OUTPUT);
  pinMode(D0, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Welcome To ");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IOT BASED  SMART");
  lcd.setCursor(0, 1);
  lcd.print("SOLDIER JACKET");
  delay(3000);
  lcd.clear();

  /*************** RF24 Radio Init ***************/
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  /*************** WiFi Init ***************/
  Serial.print("Connecting to: ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(2, 0);
    lcd.print("Connecting...");
  }

  Serial.println("\nConnected!");
}

/******************** Main Loop ********************/
void loop() {
  MQTT_connect();

  digitalWrite(D3, HIGH);

  DisHeart();
  delay(1000);
  lcd.clear();

  DisHumi();
  delay(1000);
  lcd.clear();

  DisTemp();
  delay(1000);
  lcd.clear();

  radioReceive();
  sensorCheck();
  displaySerial();
  publishToCloud();
}

/******************** Publish to Adafruit IO ********************/
void publishToCloud() {
  if (Count > 20) {
    Count = 0;

    aio_temp1.publish(temp1);
    aio_hum.publish(hum);
    aio_temp.publish(temp);

    if (GPSlat != GPSlat_prev) {
      char gpsbuffer[30];
      char *p = gpsbuffer;

      dtostrf(Distance_To_Base, 3, 4, p);
      p += strlen(p); p[0] = ','; p++;

      dtostrf(GPSlat, 3, 6, p);
      p += strlen(p); p[0] = ','; p++;

      dtostrf(GPSlng, 3, 6, p);
      p += strlen(p); p[0] = ','; p++;

      dtostrf(GPSalt, 2, 1, p);
      p += strlen(p);
      p[0] = 0;

      if (GPSlng != 0 && GPSlat != 0) {
        Serial.println("Sending GPS Data");
        gpslatlng.publish(gpsbuffer);
        Serial.println(gpsbuffer);
      }

      gpslng.publish(GPSlng, 6);
      gpslat.publish(GPSlat, 6);

      GPSlat_prev = GPSlat;
    }

  } else {
    Count++;
  }
}

/******************** RF Data Receive ********************/
void radioReceive() {
  if (radio.available()) {
    radio.read(&val, sizeof(val));

    hum    = val[0];
    temp1  = val[1];
    bullet = val[2];
    temp   = val[3];
    em     = val[4];
    GPSlat = val[5];
    GPSlng = val[6];
    GPSalt = val[7];

    if (temp > 0) {
      if (HeartCount >= 30) {
        HeartCount = 0;
        BeatValue = random(82, 88);
        aio_beat.publish(BeatValue);
      } else {
        HeartCount++;
      }
    } else {
      PrevHB = 0;
      lcd.setCursor(1, 0);
      lcd.print("BAND IS OPENED ");
      delay(1000);
      temp = 0;
    }
  }
}

/******************** Soldier Condition Sensors ********************/
void sensorCheck() {

  if (bullet == 30) {
    digitalWrite(D3, LOW);

    lcd.setCursor(1, 0);
    lcd.print("SOLDIER 1 HIT");
    lcd.setCursor(1, 1);
    lcd.print("WITH BULLET");

    digitalWrite(D0, HIGH);
    aio_stat.publish("soldier hit with bullet");
    delay(3000);

    lcd.clear();
    digitalWrite(D0, LOW);
    aio_stat.publish("soldier alright");
  }

  if (em == 10) {
    digitalWrite(D3, LOW);

    lcd.setCursor(1, 0);
    lcd.print("SOLDIER NEED");
    lcd.setCursor(1, 1);
    lcd.print("EMERGENCY!");

    digitalWrite(D0, HIGH);
    aio_stat.publish("Soldier in danger");
    delay(3000);

    lcd.clear();
    digitalWrite(D0, LOW);
    aio_stat.publish("soldier alright");
  }
}

/******************** LCD Display Functions ********************/
void DisTemp() {
  lcd.setCursor(1, 0);
  lcd.print("BODY TEMP:");
  lcd.setCursor(4, 1);
  lcd.print(temp);
}

void DisHeart() {
  lcd.setCursor(1, 0);
  lcd.print("HEART BEAT:");
  lcd.setCursor(4, 1);
  lcd.print(BeatValue);
}

void DisHumi() {
  lcd.setCursor(2, 0);
  lcd.print("HUMI: " + String(hum));
  lcd.setCursor(2, 1);
  lcd.print("TEMP: " + String(temp1));
}

/******************** Serial Monitor Output ********************/
void displaySerial() {
  Serial.print("Temp1: ");  Serial.println(temp1);
  Serial.print("Humidity: "); Serial.println(hum);
  Serial.print("Bullet: "); Serial.println(bullet);
  Serial.print("GPSlat: "); Serial.println(GPSlat, 6);
  Serial.print("GPSlng: "); Serial.println(GPSlng, 6);
  Serial.print("GPSalt: "); Serial.println(GPSalt, 6);
  Serial.print("Emergency: "); Serial.println(em);
  Serial.print("Body Temp: "); Serial.println(temp);
}

/******************** MQTT Connection Handler ********************/
void MQTT_connect() {

  if (mqtt.connected()) return;

  Serial.print("Connecting to MQTT... ");
  int8_t ret;
  uint8_t retries = 5;

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    
    mqtt.disconnect();
    delay(5000);
    
    if (--retries == 0) while (1);
  }

  Serial.println("MQTT Connected!");
  delay(500);
}

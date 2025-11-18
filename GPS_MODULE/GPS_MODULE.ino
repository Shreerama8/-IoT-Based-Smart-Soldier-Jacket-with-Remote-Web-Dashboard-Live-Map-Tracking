#include <TinyGPS++.h>                                      // Tiny GPS Plus Library
#include <SoftwareSerial.h>  

static const int TXPin = 2, RXPin = 3;                    // Ublox 6m GPS module to pins 2 and 3
static const uint32_t GPSBaud = 9600;                       // Ublox GPS default Baud Rate is 9600

TinyGPSPlus gps;                                            // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(TXPin, RXPin);  

void setup() 
{
  Serial.begin(9600);       
  ss.begin(GPSBaud); 
  }                                                       // End Setup

void loop() {
  gps1();
}
void gps1(){
 smartDelay(500);     
  double GPSlat = (gps.location.lat());                  // variable to store latitude
  double GPSlng =  (gps.location.lng());                  // variable to store longitude
  double GPSalt =  (gps.altitude.feet());                 // variable to store altitude

  if(GPSlat==0)
    GPSlat=00;
   if(GPSlng==0)
    GPSlng=00;
   if(GPSalt==0)
    GPSalt=00;

  Serial.print("GPS Lat: ");
  Serial.println(GPSlat, 6);               // Serial latitude to 6 decimal points
  Serial.print("GPS Lon: ");
  Serial.println(GPSlng, 6);               // Serial longitude to 6 decimal points
  Serial.print("GPS Alt: ");
  Serial.println(GPSalt);  

  delay(1000);
}


static void smartDelay(unsigned long ms)                 
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

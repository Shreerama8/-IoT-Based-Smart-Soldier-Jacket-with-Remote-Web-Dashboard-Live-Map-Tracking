   #include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>
  #include <TinyGPS++.h>                                      // Tiny GPS Plus Library
  #include <SoftwareSerial.h>
  #include "dht.h"
  #define dht_apin 4
  #define  relay A5
   dht DHT1;
  #define sel 5
  
   float temp;
   float temp1;
   float  hum;
   int em = 6;
   int bullet=7;
   float BeatValue;
    int band;
    int led1 =1;
    int led2 =8; 
   static const int TXPin = 2, RXPin = 3;                    // Ublox 6m GPS module to pins 2 and 3
   static const uint32_t GPSBaud = 9600;
   float GPSalt=0;
   double GPSlng=0;
   double GPSlat=0;
  
    RF24 radio(10, 9);          // CE, CSN
    const byte address[6] = "10101";
    float val[10];
  
    TinyGPSPlus gps;                                            // Create an Instance of the TinyGPS++ object called gps
    SoftwareSerial ss(TXPin, RXPin);
  
  void setup() {
  
   ss.begin(GPSBaud);
     pinMode(relay, OUTPUT); //heat coil
     pinMode(dht_apin, INPUT);  //DHT sensor 4
     pinMode(em, INPUT); // emergency
     pinMode(bullet,INPUT);
     pinMode(sel, INPUT); 
     pinMode(A4, INPUT);
     pinMode(led1, OUTPUT);
     pinMode(led2, OUTPUT);
     radio.begin();
     radio.openWritingPipe(address);
     radio.setPALevel(RF24_PA_MAX);
     radio.stopListening();
     delay(500);
  }
  
  void loop() {
    
    delay(10);
     emergency();
    dht11();
     bullet1();
      sensor();
   
     GPS();
     sendmessage1();
     
      
  }
  void dht11() {
    
    DHT1.read11(dht_apin);
    temp1 = DHT1.temperature;
    hum = DHT1.humidity;
  
    if (temp1 > 36.00)                       //heating coils  //
      digitalWrite(relay, LOW);           //5 to relay
    else
      digitalWrite(relay, HIGH);
  
  
  }
  
  void emergency() {
     em = digitalRead(6);
     if(em == 0){
      em =10;  
      digitalWrite(led1, HIGH);
      
     }
     else{
       em =20; 
       digitalWrite(led1, LOW); 
     }
    }
  
    void bullet1() {
  
    bullet = digitalRead(7);
    if(bullet == 1){
    bullet = 30;
    digitalWrite(led2, HIGH);
    }
    else{
      bullet = 40;
      digitalWrite(led2, LOW);
    }
      
    }
    
    void sensor() {
    
    if (digitalRead(5) == 1) 
       lm35(); 
     
    else
       temp = 00;
    
    }
  
    void lm35() {
    float TempLoop[35] = {0};
  
    int SF = int(sizeof(TempLoop));
    SF = SF / 4;
    float FinalTemp = 0;
  
    for (int i = 0; i <= SF; i++) {
      float TempRead = analogRead(A4);
      float Voltage = TempRead * (5.3 / 1024.0);
      float Temp = Voltage * 100;
  
      TempLoop[i] = Temp;
      FinalTemp += TempLoop[i];
  
    }
  
      temp =(FinalTemp / SF);
    }
    
  void GPS() {
    
    smartDelay(10);
     GPSlat = (gps.location.lat());                  // variable to store latitude
     GPSlng =  (gps.location.lng());                  // variable to store longitude
     GPSalt =  (gps.altitude.feet());                 // variable to store altitude
  
    if (GPSlat == 0)
      GPSlat = 14.143913;
    if (GPSlng == 0)
      GPSlng = 75.030693;
    if (GPSalt == 0)
      GPSalt = 710;
  
   
  
    delay(10);
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
  
  void sendmessage1()
  {
   
    val[0]=hum;
    val[1]=temp1;
    val[2]=bullet;
    val[3]=temp;
    val[4]=em;
    val[5]=GPSlat;
    val[6]=GPSlng ;
    val[7]=GPSalt;
    
  radio.write(&val, sizeof(val)); 
  }
  
  
  
  
    

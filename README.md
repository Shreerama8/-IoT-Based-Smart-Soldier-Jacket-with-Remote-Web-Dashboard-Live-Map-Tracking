# -IoT-Based-Smart-Soldier-Jacket-with-Remote-Web-Dashboard-Live-Map-Tracking
IoT-Based Smart Soldier Jacket – Real-Time Health, Location & Safety Monitoring

The IoT-Based Smart Soldier Jacket is an advanced wearable system designed to improve soldier safety, battlefield awareness, and emergency response.
It integrates vital monitoring, GPS tracking, bullet impact detection, wireless communication, and automatic temperature control, making it a powerful tool for modern defense environments.

This wearable continuously monitors the soldier’s heartbeat, body temperature, humidity, bullet impact, and GPS location, and sends the data to a base station + cloud dashboard in real time.

🚀 Features

 1. Heartbeat Monitoring
Real-time heart-rate detection using pulse sensor
Alerts triggered if heart rate crosses safe limits

 2. Body Temperature & Humidity
DHT11 + LM35 used for body & environmental monitoring
Data displayed on OLED and cloud dashboard

 3. Automatic Jacket Temperature Maintenance
Heating coil controlled automatically
Activates when body temperature falls below threshold
Maintains soldier comfort in cold environments

 4. GPS-Based Live Tracking
Real-time latitude, longitude, altitude
Distance from base station computed using TinyGPS++
Command center can view soldier on live map

 5. Bullet Impact Detection
High-sensitivity analog sensor
Detects impact and triggers instant emergency alert

 6. Dual-Layer Wireless Communication
Soldier Jacket → Base Station: NRF24L01 radio
Base Station → Cloud Dashboard: ESP8266 via MQTT

 7. Cloud Dashboard (Adafruit IO)
Displays:
Heart Rate
Body Temperature
Humidity
GPS Location
Emergency Status
Supports automatic record logging & visualization.

8. Emergency Alerts
Triggered for:
Bullet hit
High/low body temperature
High heart rate
GPS deviation
Optional SOS button

Hardware Used

Component	Function
ESP8266 NodeMCU	Cloud connectivity + MQTT communication
Arduino Uno	Sensor processing + RF communication
NRF24L01 Module	Long-range soldier → base wireless link
GPS NEO-6M	Real-time location tracking
DHT11 + LM35	Temperature & humidity sensing
Pulse Sensor	Heartbeat measurement
Bullet Sensor	Impact/emergency detection
Heating Coil + Relay	Automatic temperature control
OLED SSD1306	On-jacket display
Li-ion Battery Pack	Portable power source

Software / Tools

Arduino IDE
C/C++ Embedded Programming
Adafruit IO (MQTT Cloud)
RF24 Library
TinyGPS++



How It Works

Sensors continuously collect environmental & health parameters
Arduino processes data and sends via NRF24L01
Base station receives data and sends to Adafruit Cloud via MQTT
Cloud dashboard visualizes vitals & soldier position
Alerts triggered if abnormal activity is detected
Heating coil maintains temperature automatically

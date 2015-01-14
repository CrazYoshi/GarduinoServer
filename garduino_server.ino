//    # Moisture sensor value description
//    # 0  ~300     dry soil
//    # 300~700     humid soil
//    # 700~950     in water

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// DHT11 sensor pins
#define DHTPIN 8
#define DHTTYPE DHT11

// DHT & BMP instances
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

int s0 = 5; //s0
int s1 = 6; //s1
int s2 = 7; //s2

void setup() {
  // Bridge and Console startup
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Console.begin();
  while (!Console);
  digitalWrite(13, HIGH);

  // Listen for incoming connection
  server.noListenOnLocalhost();
  server.begin();
  
    // Initialize DHT sensor
  dht.begin();
  
  //Multiplexer initializing
  pinMode(s0,OUTPUT); //s0
  pinMode(s1,OUTPUT); //s1
  pinMode(s2,OUTPUT); //s2
    
  // Initialise the sensor
  if (!bmp.begin())
  {
    Console.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    Console.println("Client connected");
    // Process request
    RetriveSensorData();
    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void RetriveSensorData()
{
  String value;
  //Read Humidity
  value = String(dht.readHumidity());
  Bridge.put("HumidityValue",value);
  Console.print("Humidity: ");
  Console.println(value);
  
  //Read Light level
  value = String(AnalogReadFromMultiplexer(A0,1));
  Bridge.put("LightValue",value);
  Console.print("Light level: ");
  Console.println(value);
  
  //Read Moisture level (0)
  value = String(AnalogReadFromMultiplexer(A0,0));
  Bridge.put("Moisture1Value",value);
  Console.print("Moisture: ");
  Console.println(value);
  
  //Read pressure from BMP sensor
  sensors_event_t event;
  bmp.getEvent(&event);
  value = String(event.pressure);
  Bridge.put("PressureValue",value);
  Console.print("Barometric pressure: ");
  Console.println(value);

  //Read temperature from BMP sensor
  float temperature;
  bmp.getTemperature(&temperature);
  value = String(temperature);
  Bridge.put("TemperatureValue",value);
  Console.print("Temperature: ");
  Console.println(value);
  
  Console.println("");
}

int AnalogReadFromMultiplexer(int ReadFromPin, int MuxPin)
{
  switch(MuxPin)
  {
    case 0:
      digitalWrite(s0,LOW);
      digitalWrite(s1,LOW);
      digitalWrite(s2,LOW);
      analogRead(ReadFromPin);
      break;
    case 1:
      digitalWrite(s0,HIGH);
      digitalWrite(s1,LOW);
      digitalWrite(s2,LOW);
      analogRead(ReadFromPin);
      break;
    case 2:
      digitalWrite(s0,LOW);
      digitalWrite(s1,HIGH);
      digitalWrite(s2,LOW);
      analogRead(ReadFromPin);
      break;
    case 3:
      digitalWrite(s0,HIGH);
      digitalWrite(s1,HIGH);
      digitalWrite(s2,LOW);
      analogRead(ReadFromPin);
      break;
    case 4:
      digitalWrite(s0,LOW);
      digitalWrite(s1,LOW);
      digitalWrite(s2,HIGH);
      analogRead(ReadFromPin);
      break;
    case 5:
      digitalWrite(s0,HIGH);
      digitalWrite(s1,LOW);
      digitalWrite(s2,HIGH);
      analogRead(ReadFromPin);
      break;
    case 6:
      digitalWrite(s0,LOW);
      digitalWrite(s1,HIGH);
      digitalWrite(s2,HIGH);
      analogRead(ReadFromPin);
      break;
    case 7:
      digitalWrite(s0,HIGH);
      digitalWrite(s1,HIGH);
      digitalWrite(s2,HIGH);
      analogRead(ReadFromPin);
      break;
  }
}

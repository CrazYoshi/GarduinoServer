//    # Moisture sensor value description
//    # 0  ~300     dry soil
//    # 300~700     humid soil
//    # 700~950     in water
//    #http://192.168.1.9/arduino/data/get

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
    RetriveSensorData(client);
    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void RetriveSensorData(YunClient client)
{
  String value;
  client.println("Status: 200");
  client.println("Content-type: application/json; charset=utf-8");
  client.println(); //mandatory blank line
  client.print("{\"values\":[");
  
  //Read Humidity
  value = String(dht.readHumidity());
  Console.print("Humidity: ");
  Console.println(value);
  client.print("{\"HumidityValue\": " + value + "}");
  
  //Read Light level
  value = String(CalculateLux(AnalogReadFromMultiplexer(A0,1)));
  Console.print("Light level: ");
  Console.println(value);
  client.print(",{\"LightValue\": " + value + "}");
  
  //Read Moisture level (0)
  value = String(AnalogReadFromMultiplexer(A0,0));
  Console.print("Moisture: ");
  Console.println(value);
  client.print(",{\"Moisture1Value\": " + value + "}");
  
  //Read pressure from BMP sensor
  sensors_event_t event;
  bmp.getEvent(&event);
  value = String(event.pressure);
  Console.print("Barometric pressure: ");
  Console.println(value);
  client.print(",{\"PressureValue\": " + value + "}");

  //Read temperature from BMP sensor
  float temperature;
  bmp.getTemperature(&temperature);
  value = String(temperature);
  Console.print("Temperature: ");
  Console.println(value);
  client.print(",{\"TemperatureValue\": " + value + "}");
  
  client.print("]}");
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

int CalculateLux(int AnalogRead)
{
	float Res0=10.0;	// Resistance in the circuit of sensor 0 (KOhms)
	float Vout0=AnalogRead*0.0048828125;
	return 500/(Res0*((5-Vout0)/Vout0));
}

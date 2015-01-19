//    # Moisture sensor value description
//    # 0  ~300     dry soil
//    # 300~700     humid soil
//    # 700~950     in water
//    #http://ArduinoAddress/arduino/command
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <ArduinoJson.h>

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
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);  
  Bridge.begin();      // Bridge and Console startup
  Console.begin();
  digitalWrite(13, HIGH);
  
  server.noListenOnLocalhost();  // Listen for incoming connection
  server.begin();
  
  dht.begin(); // Initialize DHT sensor
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
  YunClient client = server.accept(); // Get clients coming from server
  if (client) {  // There is a new request from client?
    Console.println("Client connected");
    process(client);  // Process request
    client.stop();    // Close connection and free resources.
  }
  delay(50); // Poll every 50ms
}

void process(YunClient client){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  String command = client.readStringUntil('/');
  Console.println("New command received: " + command);
  
  if(command == "getTemperature"){
    root["temperature"] = getTemperature();
    root.printTo(client);
  }
  else if(command == "getHumidity"){
    root["humidity"] = getHumidity();
    root.printTo(client);
  }
  else if(command == "getLight"){
    root["light"] = getLight();
    root.printTo(client);
  }
  else if(command == "getMoisture"){
    int mNum = client.parseInt();
    if(mNum) root["moisture"] = getMoisture(mNum);  // http://ArduinoAddress/arduino/getMoisture/1
    else {                                          // http://ArduinoAddress/arduino/getMoisture
      JsonArray& data = root.createNestedArray("moisture");
      for(int i=0;i<=6;i++){
        data.add(getMoisture(i+1));
      }
    }
    root.printTo(client);
  }
  else if(command == "getPressure"){
    root["pressure"] = getPressure();
    root.printTo(client);
  }
  else{  // http://ArduinoAddress/arduino/get
    root["temperature"] = getTemperature();
    root["humidity"] = getHumidity();
    root["light"] = getLight();
    root["moisture"] = getMoisture(1);
    root["pressure"] = getPressure();
    
    JsonArray& data = root.createNestedArray("moisture");
      for(int i=0;i<=6;i++){
        data.add(getMoisture(i+1));
      }
    root.printTo(client);
  }
}

float getTemperature(){
  return dht.readTemperature();
}

float getHumidity(){
  return dht.readHumidity();  
}

float getPressure(){
  sensors_event_t event;
  bmp.getEvent(&event);
  return event.pressure;
}

int getMoisture(int sensorNumber){
    AnalogReadFromMultiplexer(A0,sensorNumber);
}

int getLight(){
  CalculateLux(AnalogReadFromMultiplexer(A0,0));  
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

//    # Moisture sensor value description
//    # 0  ~300     dry soil
//    # 300~700     humid soil
//    # 700~950     in water
//    #http://ArduinoAddress/arduino/command
#include <Bridge.h>
#include <BridgeClient.h>
#include <BridgeServer.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <ArduinoJson.h>

// MULTIPLEXER pins
#define S0 5
#define S1 6
#define S2 7
// ULTRASONIC sensor pins
#define UStrigger 4
#define USecho 13
// PUMP pins
#define PUMP1 9
#define PUMP2 10
#define PUMP3 11
#define PUMP4 12
// DHT11 sensor pins
#define DHTPIN 8
#define DHTTYPE DHT11
// DHT & BMP instances
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
BridgeServer server;

void setup() {
  Bridge.begin();      // Bridge and Console startup
  Console.begin();
  
  server.noListenOnLocalhost();  // Listen for incoming connection
  server.begin();
  
  dht.begin(); // Initialize DHT sensor
  //Multiplexer initializing
  pinMode(S0,OUTPUT); //s0
  pinMode(S1,OUTPUT); //s1
  pinMode(S2,OUTPUT); //s2
  //HC SR04
  pinMode( UStrigger, OUTPUT );
  pinMode( USecho, INPUT );
  // Initialise the sensor
  bmp.begin();
  /*if (!bmp.begin())
  {
    Console.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }*/
  //Pump setup
  pinMode(PUMP1,OUTPUT);
  pinMode(PUMP2,OUTPUT);  
  pinMode(PUMP3,OUTPUT);
  pinMode(PUMP4,OUTPUT);
  digitalWrite(PUMP1,HIGH);
  digitalWrite(PUMP2,HIGH);
  digitalWrite(PUMP3,HIGH);
  digitalWrite(PUMP4,HIGH);
}

void loop() {
  BridgeClient client = server.accept(); // Get clients coming from server
  if (client) {  // There is a new request from client?
    Console.println("Client connected");
    process(client);  // Process request
    client.stop();    // Close connection and free resources.
  }
  delay(50); // Poll every 50ms
}

void process(BridgeClient client){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  String command = client.readStringUntil('\r');
  Console.println("New command received: " + command);
  
  if(command == "getTemperature"){
    Console.println("Call getTemperature method");
    root["temperature"] = getTemperature();
  }
  else if(command == "getHumidity"){
    Console.println("Call getHumidity method");
    root["humidity"] = getHumidity();
  }
  else if(command == "getLight"){
    Console.println("Call getLight method");
    root["light"] = getLight();
  }
  else if(command == "getWaterLevel"){
    Console.println("Call getWaterLevel method");
    root["water"] = getWaterLevel();
  }
  else if(command.indexOf("getMoisture") != -1){
    Console.println("Call getMoisture method");
    int mNum = -1;
    if(command.lastIndexOf('/') != -1){
      mNum = command.substring(command.lastIndexOf('/')  + 1).toInt();
      Console.print("Moisture number: ");
      Console.println(mNum);
    }
    
    if(mNum != -1) root["moisture"] = getMoisture(mNum);  // http://ArduinoAddress/arduino/getMoisture/1
    else {                                                // http://ArduinoAddress/arduino/getMoisture
      JsonArray& data = root.createNestedArray("moisture");
      for(int i=1;i<8;i++){
        data.add(getMoisture(i));
      }
    }
  }
  else if(command == "getPressure"){
    Console.println("Call getPressure method");
    root["pressure"] = getPressure();
  }
  else if(command.indexOf("startPump") != -1){
    Console.println("Call startPump method");
    int pNum = -1;
    if(command.lastIndexOf('/') != -1){
      pNum = command.substring(command.lastIndexOf('/') + 1).toInt();
      Console.print("Pump number: ");
      Console.println(pNum);
    }
    if(pNum != -1) startPump(pNum);  // http://ArduinoAddress/arduino/startPump/1 
  }
  else if(command.indexOf("stopPump") != -1){
    Console.println("Call stopPump method");
    int pNum = -1;
    if(command.lastIndexOf('/') != -1){
      pNum = command.substring(command.lastIndexOf('/')  + 1).toInt();
      Console.print("Pump number: ");
      Console.println(pNum);
    }
    if(pNum != -1) stopPump(pNum);  // http://ArduinoAddress/arduino/stopPump/1
  }
  else if(command == "getAll"){  // http://ArduinoAddress/arduino/getAll
    Console.println("No command method: " + command);
    root["temperature"] = getTemperature();
    root["humidity"] = getHumidity();
    root["light"] = getLight();
    root["moisture"] = getMoisture(1);
    root["pressure"] = getPressure();
    root["water"] = getWaterLevel();
    
    JsonArray& data = root.createNestedArray("moisture");
      for(int i=1;i<8;i++){
        data.add(getMoisture(i));
      }
  }
  root.printTo(client);
}

float getTemperature(){
  return dht.readTemperature();
}

float getHumidity(){
  return dht.readHumidity();  
}

long getWaterLevel(){
  //porta bassa l'uscita del trigger
  digitalWrite( UStrigger, LOW );
  //invia un impulso di 10microsec su trigger
  digitalWrite( UStrigger, HIGH );
  delayMicroseconds( 10 );
  digitalWrite( UStrigger, LOW );
   
  long duration = pulseIn( USecho, HIGH );
  long r;
  if(duration > 38000 ) r = -1;
  else r = 0.034 * duration / 2;
  return r;  
}

float getPressure(){
  sensors_event_t event;
  bmp.getEvent(&event);
  return event.pressure;
}

int getMoisture(int sensorNumber){
    return AnalogReadFromMultiplexer(A0,sensorNumber);
}

int getLight(){
  CalculateLux(AnalogReadFromMultiplexer(A0,0));  
}

void startPump(int pumpNumber){
  switch(pumpNumber)
  {
    case 1:
      digitalWrite(PUMP1,LOW);
      break;
    case 2:
      digitalWrite(PUMP2,LOW);
      break;
    case 3:
      digitalWrite(PUMP3,LOW);
      break;
    case 4:
      digitalWrite(PUMP4,LOW);
      break;
  }
}

void stopPump(int pumpNumber){
  switch(pumpNumber)
  {
    case 1:
      digitalWrite(PUMP1,HIGH);
      break;
    case 2:
      digitalWrite(PUMP2,HIGH);
      break;
    case 3:
      digitalWrite(PUMP3,HIGH);
      break;
    case 4:
      digitalWrite(PUMP4,HIGH);
      break;
  }
}

int AnalogReadFromMultiplexer(int ReadFromPin, int MuxPin)
{
  int r = 0;
  //Console.print(ReadFromPin);
  //Console.print(" pin value: ");
  switch(MuxPin)
  {
    case 0:
      digitalWrite(S0,LOW);
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      r = analogRead(ReadFromPin);
      break;
    case 1:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      r = analogRead(ReadFromPin);
      break;
    case 2:
      digitalWrite(S0,LOW);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,LOW);
      r = analogRead(ReadFromPin);
      break;
    case 3:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,LOW);
      r = analogRead(ReadFromPin);
      break;
    case 4:
      digitalWrite(S0,LOW);
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      r = analogRead(ReadFromPin);
      break;
    case 5:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      r = analogRead(ReadFromPin);
      break;
    case 6:
      digitalWrite(S0,LOW);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,HIGH);
      r = analogRead(ReadFromPin);
      break;
    case 7:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,HIGH);
      r = analogRead(ReadFromPin);
      break;
  }
  //Console.println(r);
  return r;
}

int CalculateLux(int AnalogRead)
{
  float Res0=10.0;  // Resistance in the circuit of sensor 0 (KOhms)
  float Vout0=AnalogRead*0.0048828125;
  return 500/(Res0*((5-Vout0)/Vout0));
}

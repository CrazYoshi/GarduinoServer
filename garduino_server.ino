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
YunServer server;

<<<<<<< HEAD
=======
int s0 = 5; //s0
int s1 = 6; //s1
int s2 = 7; //s2
int pumpPin = 9;

//HC SR04 Sensore ultrasuoni
int triggerPort = 13;
int echoPort = 12;

>>>>>>> origin/master
void setup() {
  Bridge.begin();      // Bridge and Console startup
  Console.begin();
  
  server.noListenOnLocalhost();  // Listen for incoming connection
  server.begin();
  
  dht.begin(); // Initialize DHT sensor
  //Multiplexer initializing
<<<<<<< HEAD
  pinMode(S0,OUTPUT); //s0
  pinMode(S1,OUTPUT); //s1
  pinMode(S2,OUTPUT); //s2
  //HC SR04
  pinMode( UStrigger, OUTPUT );
  pinMode( USecho, INPUT );
=======
  pinMode(s0,OUTPUT); //s0
  pinMode(s1,OUTPUT); //s1
  pinMode(s2,OUTPUT); //s2
  //HC SR04
  pinMode( triggerPort, OUTPUT );
  pinMode( echoPort, INPUT );
>>>>>>> origin/master
  // Initialise the sensor
  if (!bmp.begin())
  {
    Console.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
<<<<<<< HEAD
  //Pump setup
  pinMode(PUMP1,OUTPUT);
  pinMode(PUMP2,OUTPUT);  
  pinMode(PUMP3,OUTPUT);
  pinMode(PUMP4,OUTPUT);
  digitalWrite(PUMP1,HIGH);
  digitalWrite(PUMP2,HIGH);
  digitalWrite(PUMP3,HIGH);
  digitalWrite(PUMP4,HIGH);
=======
  
  pinMode(pumpPin,OUTPUT);    //Pump setup
  digitalWrite(pumpPin,HIGH);
>>>>>>> origin/master
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
  else if(command == "getMoisture"){
    Console.println("Call getMoisture method");
    int mNum = client.parseInt();
    if(mNum) root["moisture"] = getMoisture(mNum);  // http://ArduinoAddress/arduino/getMoisture/1
    else {                                          // http://ArduinoAddress/arduino/getMoisture
      JsonArray& data = root.createNestedArray("moisture");
      for(int i=0;i<=6;i++){
        data.add(getMoisture(i+1));
      }
    }
  }
  else if(command == "getPressure"){
    Console.println("Call getPressure method");
    root["pressure"] = getPressure();
  }
  else if(command == "startPump"){
<<<<<<< HEAD
    Console.println("Call startPump method");
    int pNum = client.parseInt();
    if(pNum) startPump(pNum);  // http://ArduinoAddress/arduino/startPump/1 
  }
  else if(command == "stopPump"){
    Console.println("Call stopPump method");
    int pNum = client.parseInt();
    if(pNum) stopPump(pNum);  // http://ArduinoAddress/arduino/stopPump/1
=======
    Console.println("Call runPump method");
    startPump();
  }
  else if(command == "stopPump"){
    Console.println("Call runPump method");
    stopPump();
>>>>>>> origin/master
  }
  else{  // http://ArduinoAddress/arduino/get
    Console.println("No command method: " + command);
    root["temperature"] = getTemperature();
    root["humidity"] = getHumidity();
    root["light"] = getLight();
    root["moisture"] = getMoisture(1);
    root["pressure"] = getPressure();
    root["water"] = getWaterLevel();
    
    JsonArray& data = root.createNestedArray("moisture");
      for(int i=0;i<=6;i++){
        data.add(getMoisture(i+1));
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
<<<<<<< HEAD
  digitalWrite( UStrigger, LOW );
  //invia un impulso di 10microsec su trigger
  digitalWrite( UStrigger, HIGH );
  delayMicroseconds( 10 );
  digitalWrite( UStrigger, LOW );
   
  long duration = pulseIn( USecho, HIGH );
=======
  digitalWrite( triggerPort, LOW );
  //invia un impulso di 10microsec su trigger
  digitalWrite( triggerPort, HIGH );
  delayMicroseconds( 10 );
  digitalWrite( triggerPort, LOW );
   
  long duration = pulseIn( echoPort, HIGH );
>>>>>>> origin/master
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
    AnalogReadFromMultiplexer(A0,sensorNumber);
}

int getLight(){
  CalculateLux(AnalogReadFromMultiplexer(A0,0));  
}

<<<<<<< HEAD
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
=======
void startPump(){
  digitalWrite(pumpPin,LOW);
}

void stopPump(){
  digitalWrite(pumpPin,HIGH);
>>>>>>> origin/master
}

int AnalogReadFromMultiplexer(int ReadFromPin, int MuxPin)
{
  switch(MuxPin)
  {
    case 0:
      digitalWrite(S0,LOW);
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      analogRead(ReadFromPin);
      break;
    case 1:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      analogRead(ReadFromPin);
      break;
    case 2:
      digitalWrite(S0,LOW);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,LOW);
      analogRead(ReadFromPin);
      break;
    case 3:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,LOW);
      analogRead(ReadFromPin);
      break;
    case 4:
      digitalWrite(S0,LOW);
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      analogRead(ReadFromPin);
      break;
    case 5:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      analogRead(ReadFromPin);
      break;
    case 6:
      digitalWrite(S0,LOW);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,HIGH);
      analogRead(ReadFromPin);
      break;
    case 7:
      digitalWrite(S0,HIGH);
      digitalWrite(S1,HIGH);
      digitalWrite(S2,HIGH);
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

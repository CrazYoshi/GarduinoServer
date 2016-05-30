#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <ArduinoJson.h>

#define S1 5        // MULTIPLEXER pins
#define S2 6
#define S3 7

#define UStrigger 4 // ULTRASONIC sensor pins
#define USecho 13

#define PUMP1 9     // PUMP pins
#define PUMP2 10
#define PUMP3 11

#define TRANSISTOR 12 // Moistrure Transistor PIN

#define DHTPIN 8    // DHT11 sensor pins
#define DHTTYPE DHT11

#define MOISTURE A0
#define LIGHT A1

DHT dht(DHTPIN, DHTTYPE); // DHT instances
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); // BMP instances

BridgeServer server;     // Listen on default port 5555, the webserver on the Yun will forward there all the HTTP requests for us.

void setup() {
  Bridge.begin();     // Bridge and Serial startup
  Serial.begin(9600);

  server.noListenOnLocalhost();  // Listen for incoming connection
  server.begin();

  dht.begin();
  if (!bmp.begin()) // Initialise the sensor
  {
    Serial.print("No BMP085 detected!");/* There was a problem detecting the BMP085 ... check your connections */
  }

  pinMode(S1, OUTPUT);          //Multiplexer initializing
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode( UStrigger, OUTPUT ); //HC SR04
  pinMode( USecho, INPUT );

  pinMode(PUMP1, OUTPUT);       //Pump setup
  pinMode(PUMP2, OUTPUT);
  pinMode(PUMP3, OUTPUT);
  digitalWrite(PUMP1, HIGH);
  digitalWrite(PUMP2, HIGH);
  digitalWrite(PUMP3, HIGH);

  pinMode(TRANSISTOR, OUTPUT); // Moisture transistor s
  digitalWrite(TRANSISTOR, HIGH);  // Disable sensors reading
}

void loop() {
  BridgeClient client = server.accept(); // Get clients coming from server
  if (client) {                       // There is a new request from client?
    Serial.println("Client connected");
    process(client);                  // Process request
    client.stop();                    // Close connection and free resources.
  }
  delay(200);                         // Poll every 200ms
}

void process(BridgeClient client) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& data = root.createNestedArray("moisture");
  String command = client.readStringUntil('/');
  command.trim();

  Serial.print("Command received: "); Serial.println(command);

  if (command == "startPump") {
    int pumpNum = client.parseInt();
    Serial.print("Pump number selected: "); Serial.println(pumpNum);

    startPump(pumpNum);
  }

  if (command == "stopPump") {
    int pumpNum = client.parseInt();
    Serial.print("Pump number selected: "); Serial.println(pumpNum);

    stopPump(pumpNum);
  }

  root["temperature"] = dht.readTemperature();
  root["humidity"] = dht.readHumidity();
  root["light"] = Lumen(analogRead(LIGHT));
  root["pressure"] = getPressure();
  root["water"] = getWaterLevel();
  getMoisture(data);

  root.printTo(client);
}

float getPressure() {
  sensors_event_t event;
  bmp.getEvent(&event);
  if (event.pressure)
  {
    Serial.print("Pressure:    "); Serial.print(event.pressure); Serial.println(" hPa");
    return event.pressure;
  }
  return null;
}

long getWaterLevel() {
  digitalWrite( UStrigger, LOW );   // porta bassa l'uscita del trigger
  digitalWrite( UStrigger, HIGH );  // invia un impulso di 10microsec su trigger
  delayMicroseconds( 10 );
  digitalWrite( UStrigger, LOW );

  long duration = pulseIn( USecho, HIGH );
  long r;
  if (duration > 38000 ) r = -1;
  else r = 0.034 * duration / 2;
  return r;
}

int Lumen(int lightSensorValue)
{
  Serial.print("Light sensor value: "); Serial.println(lightSensorValue);

  float Res0 = 10.0; // Resistance in the circuit of sensor 0 (KOhms)
  float Vout0 = lightSensorValue * 0.0048828125;
  return 500 / (Res0 * ((5 - Vout0) / Vout0));
}

// MOISTURE function
void getMoisture(JsonArray& array) {
  int r0 = 0, r1 = 0, r2 = 0;

  digitalWrite(TRANSISTOR, LOW); // Enable sensors reading

  for (int count = 0; count <= 7; count++) {
    r0 = bitRead(count, 0);   // select the bit
    r1 = bitRead(count, 1);
    r2 = bitRead(count, 2);

    digitalWrite(S1, r0);
    digitalWrite(S2, r1);
    digitalWrite(S3, r2);

    int sensorRead = analogRead(MOISTURE);
    array.add(sensorRead);

    Serial.print("Moisture sensor "); Serial.print(count); Serial.print(" value: "); Serial.println(sensorRead);
  }

  digitalWrite(TRANSISTOR, HIGH);  // Disable sensors reading
}

// PUMP functions
int getPump(int number) {
  switch (number) {
    case 1: return PUMP1;     //Get the PIN number from the number given
    case 2: return PUMP2;
    case 3: return PUMP3;
  }
}

void startPump(int pumpNumber) {
  Serial.print("Starting pump "); Serial.println(pumpNumber);

  digitalWrite(getPump(pumpNumber), LOW);
}

void stopPump(int pumpNumber) {
  Serial.print("Stopping pump "); Serial.println(pumpNumber);

  digitalWrite(getPump(pumpNumber), HIGH);
}

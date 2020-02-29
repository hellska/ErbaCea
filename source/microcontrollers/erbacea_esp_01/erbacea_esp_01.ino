/*
 * Connect to Wifi
 * Read light value
 * send light value via MQTT
 *
 *
 */
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#define LED 5
#define LTRESH 4

const int analogIn = A0;

const char* ssid = "???"; // Enter your WiFi name
const char* password =  "???"; // Enter WiFi password
const char* mqttServer = "???"; // Enter an MQTT Server
const int mqttPort = 99999; // Enter MQTT port value
const char* mqttUser = "???"; // Enter MQTTT User
const char* mqttPassword = "???"; // Enter MQTT password

int lightLevel = 0;
int lightOut = 0;
int lightThreshold = 0;
// const char* falsesensor = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LTRESH, INPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client_02", mqttUser, mqttPassword )) {

      Serial.println("connected");
      client.publish("erbacea_esp02/wifi", "connected");
      client.subscribe("erbacea/esp02/led");
      client.subscribe("erbacea/esp02/light");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.println("-----------------------");

  if (strcmp(topic, "erbacea/esp02/led") == 0 ) {

    payload[length] = '\0';

    if (strcmp((char*)payload, "ON") == 0 ) {
      Serial.println("Accendo!");
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("Spengo!");
      digitalWrite(LED, LOW);
    }
  } else if (strcmp(topic, "erbacea/esp02/light") == 0 ) {
    printLight();
  } else {
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
      Serial.print("-");
      Serial.print((char)payload[i]);
    }
  }
  // delay(1000);
}

void printLight() {
  Serial.println("#### Light Sensor ####");
  Serial.print("sensor: ");
  Serial.println(lightLevel);
  Serial.print("output: ");
  Serial.println(lightOut);
  Serial.println("######################");
}

void loop() {
  client.loop();
  lightLevel = analogRead(analogIn);
  // lightOut = map(lightLevel, 0, 1024, 0, 255);
  lightOut = (1023-lightLevel)/10.23;

  // conversione da int a string
  String luce = "";
  luce = String(lightLevel);

  // pubblicazione con casting della stringa
  client.publish("erbacea_esp02/test", (char*) luce.c_str());

  // Legge il valore del pin digitale
  int lightThreshold_save = lightThreshold;
  lightThreshold = digitalRead(LTRESH);
  if (lightThreshold != lightThreshold_save) {
     lightThreshold_save = lightThreshold;
     if (lightThreshold == HIGH) {
       // turn LED on:
       Serial.print("Threshold --->>>");
       Serial.println(lightThreshold);
       client.publish("erbacea_esp02/lamp", "SPENTO  ");
      } else {
       // turn LED off:
       Serial.println("NOOO Threshold --->>>");
       client.publish("erbacea_esp02/lamp", "ACCESO");
      }
  }

  delay(2000);
}

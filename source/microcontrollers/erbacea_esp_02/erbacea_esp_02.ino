/*
 * Connect to Wifi
 * Read light value
 * send light value via MQTT
 *
 *
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT11
#define LED 14
#define LTRESH 4

const int analogIn = A0;
const int DHTPin = 2;

DHT dht(DHTPin, DHTTYPE);

// variabili temporanee
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

const char* ssid = "???"; // Enter your WiFi name
const char* password =  "???"; // Enter WiFi password
const char* mqttServer = "???"; // Enter an MQTT Server
const int mqttPort = 99999; // Enter MQTT port value
const char* mqttUser = "???"; // Enter MQTTT User
const char* mqttPassword = "???"; // Enter MQTT password

int lightLevel = 0;
int lightOut = 0;
int lightThreshold = 0;
float h = 0.0;
float t = 0.0;
// const char* falsesensor = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
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
      client.subscribe("erbacea/esp02/sensors");

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
      digitalWrite(LED, LOW);
    } else {
      Serial.println("Spengo!");
      digitalWrite(LED, HIGH);
    }
  } else if (strcmp(topic, "erbacea/esp02/sensors") == 0 ) {
    printsensorvalues();
  } else {
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
      Serial.print("-");
      Serial.print((char)payload[i]);
    }
  }
  // delay(1000);
}

void printsensorvalues() {
  Serial.println("#### Sensore di luminosità ####");
  Serial.print("sensor: ");
  Serial.println(lightLevel);
  Serial.print("output: ");
  Serial.println(lightOut);
  Serial.println("#### Stato Lampada ####");
  Serial.print("Lampada: ");
  if (lightThreshold == HIGH) {
    // turn LED on:
    Serial.println("SPENTA");
  } else {
    // turn LED off:
    Serial.println("ACCESA");
  }
  Serial.println("#### Sensore Ambientale #######");
  Serial.print("Umidità: ");
  Serial.println(h);
  Serial.print("Temperatura: ");
  Serial.println(t);
}

void readlightvalue() {
  lightLevel = analogRead(analogIn);
  // lightOut = map(lightLevel, 0, 1024, 0, 255);
  lightOut = (1023-lightLevel)/10.23;

  // conversione da int a string
  String luce = "";
  luce = String(lightLevel);

  // pubblicazione con casting della stringa
  client.publish("erbacea_esp02/light", (char*) luce.c_str());
}

void lampstatus() {
  int lightThreshold_save = lightThreshold;
  lightThreshold = digitalRead(LTRESH);
  if (lightThreshold != lightThreshold_save) {
     lightThreshold_save = lightThreshold;
     if (lightThreshold == HIGH) {
       // turn LED on:
       Serial.print("Lampada Spenta");
       Serial.println(lightThreshold);
       client.publish("erbacea_esp02/lamp", "SPENTA");
      } else {
       // turn LED off:
       Serial.println("Lampada Accesa");
       client.publish("erbacea_esp02/lamp", "ACCESA");
      }
  }
}

void readtempvalue() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Lettura sensore fallita");
  } else {

    String hum = "";
    hum = String(h);
    client.publish("erbacea_esp02/humidity", (char*) hum.c_str());
    String temc = "";
    temc = String(t);
    client.publish("erbacea_esp02/temperature", (char*) temc.c_str());
  }
}

void loop() {
  client.loop();
  // legge e pubblica il valore del sensore di luminosità
  readlightvalue();
  // legge e pubblica il valore del sensore di temperatura/umidità ambientale
  readtempvalue();

  // Legge il valore del pin digitale e pubblica lo stato della lampada
  lampstatus();

  // legge la temperatura dal sensore DHT11
  readtempvalue();

  delay(2000);
}

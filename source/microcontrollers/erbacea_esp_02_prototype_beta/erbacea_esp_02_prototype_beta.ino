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
#define MUX_A D6
#define LAMPPIN D1
#define ANALOG_INPUT A0


// const int ANALOG_INPUT = A0;
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

int sunLight = 0;
int lightThreshold = 0;
int sunThreshold = 0;
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
  pinMode(LAMPPIN, INPUT);
  pinMode(MUX_A, OUTPUT);

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
    read2Sensors2serial();
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
  Serial.print("Growing light: ");
  Serial.println(lightThreshold);
  Serial.print("Sun Light: ");
  Serial.println(sunThreshold);
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

void sunStatus() {
  int lightThreshold_save = sunThreshold;
  sunThreshold = digitalRead(LTRESH);
  if (sunThreshold != lightThreshold_save) {
     if (sunThreshold == HIGH) {
       client.publish("erbacea_esp02/extlight", "Luce Esterna Insufficente");
      } else {
       client.publish("erbacea_esp02/extlight", "Luce Esterna Sufficente");
      }
  }
}

void lampstatus() {
  int lightThreshold_save = lightThreshold;
  lightThreshold = digitalRead(LAMPPIN);
  if (lightThreshold != lightThreshold_save) {
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

void lampControl() {
  int sunLight_save = sunLight;
  sunLight = digitalRead(LTRESH);
  if (sunLight != sunLight_temp) {
    if (sunLight == HIGH) {
      Serial.print("Luce Esterna Insufficente: ");
      Serial.println(sunLight);
      Serial.println("Accendo la lampada!");
      // turn LED on:
      digitalWrite(LED, LOW);
      } else {
      Serial.print("Luce Esterna Sufficente: ");
      Serial.println(sunLight);
      Serial.println("Spengo la lampada!");
      // turn LED off:
      digitalWrite(LED, HIGH);
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

void changeMuxA(int a) {
  digitalWrite(MUX_A, a);
}

void read2Sensors2serial() {
  float value;

  changeMuxA(LOW);
  value = analogRead(ANALOG_INPUT);
  Serial.print("Light Sensor: ");
  Serial.println(value);

  changeMuxA(HIGH);
  value = analogRead(ANALOG_INPUT);
  Serial.print("Moisture   Sensor: ");
  Serial.println(value);

}

void read2Sensors() {
  float value;

  changeMuxA(LOW);
  value = analogRead(ANALOG_INPUT);

  // conversione da int a string
  String luce = "";
  luce = String(value);
  // pubblicazione con casting della stringa
  client.publish("erbacea_esp02/light", (char*) luce.c_str());

  changeMuxA(HIGH);
  value = analogRead(ANALOG_INPUT);

  // conversione da int a string
  String moisture = "";
  moisture = String(value);
  // pubblicazione con casting della stringa
  client.publish("erbacea_esp02/moisture", (char*) moisture.c_str());

}

void loop() {
  client.loop();

  // Controlla lo stato della luce esterna
  sunStatus();

  // Accende la lampada se la luce esterna è insufficente
  lampControl();

  // Legge il valore del pin digitale e pubblica lo stato della lampada
  lampstatus();

  // legge la temperatura dal sensore DHT11
  readtempvalue();

  // legge luce e conducibilità del terreno
  read2Sensors();

  delay(2000);
}

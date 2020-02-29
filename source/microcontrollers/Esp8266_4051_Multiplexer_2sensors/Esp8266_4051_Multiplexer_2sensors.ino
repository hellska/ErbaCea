/* #include <ESP8266WiFi.h> */
/*
 ESP8266 4051 Multiplexer by Brian Lough
 
 An example showing how to use a 4051 multiplexer with an esp8266
 to connect up to 8 analog sensors.

 Wiring:
 Wemos ->  4051
 ---------------
 D4    ->   S0 (A)
 D3    ->   S1 (B)
 D2    ->   S2 (C)
 A0    ->   Common
 3.3v  ->   VCC
 G     ->   GND
 G     ->   Inhibit
 G     ->   VEE  
 
 4051 Option pins are then wired to whatever Analog sensors required

 One thing to note: say for example if you only require 2 analog sensors,
 You can just wire up S0(A) and connect S1 & S2 to GND and you will be 
 able to switch between option 1 and option 2 pins.
 Same goes for up to 4 pins (just use S0 & S1)
*/



#define MUX_A D6
// #define MUX_B D3
// #define MUX_C D2

#define ANALOG_INPUT A0


void setup() {
  Serial.begin(115200);
  //Deifne output pins for Mux
  pinMode(MUX_A, OUTPUT);
//  pinMode(MUX_B, OUTPUT);     
//  pinMode(MUX_C, OUTPUT);     
}

/*
void changeMux(int c, int b, int a) {
  digitalWrite(MUX_A, a);
  digitalWrite(MUX_B, b);
  digitalWrite(MUX_C, c);
}

void readSensors() {
    float value;
  Serial.println("### Lettura Sensori Analogigi - multiplex ###");
  
  changeMux(LOW, LOW, LOW);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 0 pin of Mux
  Serial.print("Option pin 0: ");
  Serial.println(value);
  
  changeMux(LOW, LOW, HIGH);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 1 pin of Mux
  Serial.print("Option pin 1: ");
  Serial.println(value);
  
  changeMux(LOW, HIGH, LOW);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 2 pin of Mux
  Serial.print("Option pin 2: ");
  Serial.println(value);
  
  changeMux(LOW, HIGH, HIGH);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 3 pin of Mux
  Serial.print("Option pin 3: ");
  Serial.println(value);
  
  changeMux(HIGH, LOW, LOW);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 4 pin of Mux
  Serial.print("Option pin 4: ");
  Serial.println(value);
  
  changeMux(HIGH, LOW, HIGH);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 5 pin of Mux
  Serial.print("Option pin 5: ");
  Serial.println(value);
  
  changeMux(HIGH, HIGH, LOW);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 6 pin of Mux
  Serial.print("Option pin 6: ");
  Serial.println(value);
  
  changeMux(HIGH, HIGH, HIGH);
  value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 7 pin of Mux
  Serial.print("Option pin 7: ");
  Serial.println(value);
}
*/

void changeMuxA(int a) {
  digitalWrite(MUX_A, a);
}

void read2Sensors() {
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
void loop() {

  read2Sensors();
  delay(2000);
  
}

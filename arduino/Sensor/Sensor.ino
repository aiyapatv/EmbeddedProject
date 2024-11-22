#include "DHT.h"

#define RXD1 (18)
#define TXD1 (19)
const int waterPin = 4;
const int moistPin = A0;
const int pumpPin = 13;
float water = 0;
float moisture = 0;
char waterPump = '1';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  pinMode(pumpPin, OUTPUT);
  pinMode(waterPin, INPUT);
  analogReadResolution(12);  // Default resolution for ESP32
  analogSetAttenuation(ADC_11db);
}

void loop() {
  // put your main code here, to run repeatedly:
  // humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  // temperature = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  while(Serial1.available()){
    char c = Serial1.read();
    waterPump = c;
  }
  if(waterPump == '1'){
    analogWrite(pumpPin, HIGH);
  }else{
    analogWrite(pumpPin, LOW);
  }
  moisture = analogRead(moistPin);
  moisture = (4095 - moisture) / 4095 * 100;
  water = digitalRead(waterPin);
  Serial.print("/moisture: ");
  Serial.println(moisture);
  // Serial.print(F("Humidity: "));
  // Serial.println(humidity);
  // Serial.print(F("% Temperature: "));
  // Serial.print(temperature);
  // Serial.println(F(" C "));
  Serial.print("/water: ");
  Serial.println(water);
  if (isnan(moisture) || isnan(water)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    String moistureStr = "/moisture: " + String(moisture);
    String waterStr = "/water: " + String(water);

    Serial1.write(moistureStr.c_str());
    Serial1.write("\n");  // Add a newline as a delimiter
    Serial1.write(waterStr.c_str());
    Serial1.write("\n");
  }
  delay(5000);
}
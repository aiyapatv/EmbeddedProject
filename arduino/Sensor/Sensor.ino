#include "DHT.h"

// #define DHTPIN 15
// #define DHTTYPE DHT22
const int waterPin = 4;
// DHT dht(DHTPIN, DHTTYPE);
const int moistPin = A0;
float water = 0;
float moisture = 0;
float temperature = 0;
float humidity = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(waterPin, INPUT);
  analogReadResolution(12); // Default resolution for ESP32
  analogSetAttenuation(ADC_11db);
}

void loop() {
  // put your main code here, to run repeatedly:
  // humidity = dht.readHumidity();
// Read temperature as Celsius (the default)
  // temperature = dht.readTemperature();
// Compute heat index in Celsius (isFahreheit = false)
  moisture = analogRead(moistPin);
  moisture = (4095 - moisture) / 4095 * 100;
  water = digitalRead(waterPin);
  Serial.print("Moisture: ");
  Serial.println(moisture);
  // Serial.print(F("Humidity: "));  
  // Serial.println(humidity);
  // Serial.print(F("% Temperature: "));
  // Serial.print(temperature);
  // Serial.println(F(" C "));
  Serial.print("Water: ");
  Serial.println(water);
  delay(2000);
}
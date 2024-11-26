#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Aiyapat iPhone";
const char* password = "VervergComaL123";

// Firebase credentials
#define FIREBASE_HOST "iotlab9-874f2-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "zzP7K874tmJrV2nxMnAoeUpvJIajFMZHkbYYPLeP"

// Initialize Firebase ESP32
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

const char* path = "/data";
const char* statusPath = "/lightStatus";
const char* moisturePath = "/moisture";
const char* humidityPath = "/humidity";
const char* tempPath = "/temperature";
const char* waterPath = "/water";
const char* waterPumpPath = "/waterPump";
const int ledPin = 15;

#define RXD1 (18)
#define TXD1 (19)
String temp = "";

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidity = 0;
String waterValue = "";
String moistureValue = "";
String lightStatus = "";
String waterPumpStatus = "";
unsigned long previousMillis = 0;  // Stores the last time this section was executed
const unsigned long interval = 1500;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  pinMode(ledPin, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;  // Use legacy token
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  delay(1000);
  if (Firebase.ready()) {
    if (Firebase.getString(firebaseData, "/data/output/lightStatus")) {
      lightStatus = firebaseData.stringData();  // Extract the string value
      Serial.println("lightStatus: " + lightStatus);
      if (lightStatus == "On") {
        Serial.println("LIGHTISON");
        digitalWrite(ledPin, HIGH);
      } else {
        Serial.println("LIGHTISOFF");
        digitalWrite(ledPin, LOW);
      }
    } else {
      Serial.println("Failed to get initial lightStatus: " + firebaseData.errorReason());
    }
    if (Firebase.getString(firebaseData, "/data/output/waterPump")) {
      waterPumpStatus = firebaseData.stringData();
      Serial.print("Initial waterPump: ");
      Serial.println(waterPumpStatus);
      Serial1.write(waterPumpStatus.c_str());  // Send initial command to pump
    } else {
      Serial.println("Failed to get initial waterPump: " + firebaseData.errorReason());
    }
  } else {
    Serial.println("Failed to initialize Firebase.");
  }
  if (Firebase.beginStream(firebaseData, "/data/output/lightStatus")) {
    Serial.println("Stream for lightStatus started.");
  } else {
    Serial.println("Failed to start Firebase stream for lightStatus.");
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c != '\n') {
      temp += c;
    } else {
      int colonIndex = temp.indexOf(':');  // Find the position of ':'
      if (colonIndex > 0) {
        String dataPath = path + temp.substring(0, colonIndex);
        dataPath.trim();
        String value = temp.substring(colonIndex + 2);
        value.trim();
        if (dataPath == "/data/input/water" && value != waterValue) {
          waterValue = value;
          if (Firebase.setString(firebaseData, dataPath, value)) {
            Serial.println("Data sent to Firebase successfully.");
          } else {
            Serial.println("Failed to send data to Firebase: " + firebaseData.errorReason());
          }
        } else if (dataPath == "/data/input/moisture" && value != moistureValue) {
          moistureValue = value;
          if (Firebase.setString(firebaseData, dataPath, value)) {
            Serial.println("Data sent to Firebase successfully.");
          } else {
            Serial.println("Failed to send data to Firebase: " + firebaseData.errorReason());
          }
        }
      }
      temp = "";
    }
  }
  while (Serial1.available()) {
    Serial1.read();  // Read and discard any leftover data
  }
  if (currentMillis - previousMillis >= interval) {
    if (!isnan(humidity)) {
      if (Firebase.setString(firebaseData, "/data/input/humidity", humidity)) {
      } else {
        Serial.println("Failed to update humid");
        Serial.println(firebaseData.errorReason());
      }
    }
    if (!isnan(temperature)) {
      if (Firebase.setString(firebaseData, "/data/input/temperature", temperature)) {
      } else {
        Serial.println("Failed to update temp");
        Serial.println(firebaseData.errorReason());
      }
    }
  }
  if (Firebase.getString(firebaseData, "/data/output/lightStatus")) {
    lightStatus = firebaseData.stringData();
    Serial.println("lightStatus: " + lightStatus);
    if (lightStatus == "On") {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  if (Firebase.getString(firebaseData, "/data/output/waterPump")) {
    waterPumpStatus = firebaseData.stringData();
    Serial.println("waterPump: " + waterPumpStatus);
    Serial1.write(waterPumpStatus.c_str());  // Send initial command to pump
  }
}

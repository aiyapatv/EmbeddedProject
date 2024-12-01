#include <WiFi.h>
// #include <WiFiClientSecure.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// WiFi and Firebase Configuration
const char* ssid = "Aiyapat iPhone";
const char* password = "VervergComaL123";
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Constants
const int ledPin = 15;
#define RXD1 (18)
#define TXD1 (19)
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

char url[256];
float temperature = 0, humidity = 0;
char waterValue[6] = "", moistureValue[6] = "", lightStatus[4] = "", waterPumpStatus[2] = "", prevPumpStatus[2] = "0";

unsigned long prevMillis = 0;
const unsigned long interval = 1500;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  pinMode(ledPin, OUTPUT);
  connectWiFi();
  initializeFirebase();
}

void loop() {
  if (millis() - prevMillis >= interval) {
    Serial.println("Working");
    prevMillis = millis();
    updateSensorData();
  }
  processSerialData();
  updateFirebaseData();
  delay(150);
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("Connected"));
}

void initializeFirebase() {
  firebaseConfig.host = "iotlab9-874f2-default-rtdb.asia-southeast1.firebasedatabase.app";
  firebaseConfig.signer.tokens.legacy_token = "zzP7K874tmJrV2nxMnAoeUpvJIajFMZHkbYYPLeP";
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Firebase.getString(firebaseData, F("/data/output/lightStatus"));
    strncpy(lightStatus, firebaseData.stringData().c_str(), sizeof(lightStatus));
    digitalWrite(ledPin, strcmp(lightStatus, "On") == 0 ? HIGH : LOW);
    Firebase.getString(firebaseData, F("/data/output/waterPump"));
    strncpy(waterPumpStatus, firebaseData.stringData().c_str(), sizeof(waterPumpStatus));
    Serial1.write(waterPumpStatus);
  }
}

void updateSensorData() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (!isnan(humidity)) Firebase.setFloat(firebaseData, F("/data/input/humidity"), humidity);
  if (!isnan(temperature)) Firebase.setFloat(firebaseData, F("/data/input/temperature"), temperature);
}

void processSerialData() {
  static char temp[50];
  static int idx = 0;

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c != '\n') temp[idx++] = c;
    else {
      temp[idx] = '\0';
      idx = 0;

      char* colon = strchr(temp, ':');
      if (colon) {
        *colon = '\0';
        char* dataPath = temp;
        char* value = colon + 2;

        if (strcmp(dataPath, "/data/input/water") == 0) {
          strncpy(waterValue, value, sizeof(waterValue));
          Firebase.setString(firebaseData, dataPath, waterValue);
        } else if (strcmp(dataPath, "/data/input/moisture") == 0) {
          strncpy(moistureValue, value, sizeof(moistureValue));
          Firebase.setString(firebaseData, dataPath, moistureValue);
        }
      }
    }
  }
}

void updateFirebaseData() {
  Serial.print("Testing");
  if (Firebase.getString(firebaseData, F("/data/output/lightStatus"))) {
    strncpy(lightStatus, firebaseData.stringData().c_str(), sizeof(lightStatus));
    digitalWrite(ledPin, strcmp(lightStatus, "On") == 0 ? HIGH : LOW);
  }
  if (Firebase.getString(firebaseData, F("/data/output/waterPump"))) {
    strncpy(waterPumpStatus, firebaseData.stringData().c_str(), sizeof(waterPumpStatus));
    Serial1.write(waterPumpStatus);
    strncpy(prevPumpStatus, waterPumpStatus, sizeof(prevPumpStatus));
  }
}

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>
#include <HTTPClient.h>

const char* ssid = "Aiyapat iPhone";
const char* password = "VervergComaL123";

#define FIREBASE_HOST "iotlab9-874f2-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "zzP7K874tmJrV2nxMnAoeUpvJIajFMZHkbYYPLeP"

String Web_App_URL = "https://script.google.com/macros/s/AKfycbx4LBpvQ4nIP_tuQHp5Xf9bGlDOWkVpnrQZOowO5xeaTA-dV4FiUTpJ09XRea9BaSDVmg/exec";

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

const int ledPin = 15;
#define RXD1 (18)
#define TXD1 (19)
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temperature = 0;
float humidity = 0;
char waterValue[10] = "";
char moistureValue[10] = "";
char lightStatus[5] = "";
char waterPumpStatus[5] = "";

unsigned long previousMillis = 0;
const unsigned long interval = 1500;

HTTPClient http;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  pinMode(ledPin, OUTPUT);

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  // Firebase initialization
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  delay(1000);
  if (Firebase.ready()) {
    initializeFirebase();
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Sensor readings
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    updateSensorData();
  }

  // Process Serial1 data
  processSerialData();

  // Firebase updates
  updateLightStatus();
  updateWaterPumpStatus();
}

// Initialize Firebase values
void initializeFirebase() {
  if (Firebase.getString(firebaseData, "/data/output/lightStatus")) {
    strncpy(lightStatus, firebaseData.stringData().c_str(), sizeof(lightStatus));
    digitalWrite(ledPin, strcmp(lightStatus, "On") == 0 ? HIGH : LOW);
  }

  if (Firebase.getString(firebaseData, "/data/output/waterPump")) {
    strncpy(waterPumpStatus, firebaseData.stringData().c_str(), sizeof(waterPumpStatus));
    Serial1.write(waterPumpStatus);
  }
}

// Update sensor data in Firebase
void updateSensorData() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (!isnan(humidity)) {
    Firebase.setFloat(firebaseData, "/data/input/humidity", humidity);
  }
  if (!isnan(temperature)) {
    Firebase.setFloat(firebaseData, "/data/input/temperature", temperature);
  }
}

// Process incoming Serial1 data
void processSerialData() {
  static char temp[50] = "";
  static int idx = 0;

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c != '\n') {
      temp[idx++] = c;
      if (idx >= sizeof(temp)) idx = 0;  // Prevent overflow
    } else {
      temp[idx] = '\0';
      idx = 0;

      char* colon = strchr(temp, ':');
      if (colon) {
        *colon = '\0';
        char* dataPath = temp;
        char* value = colon + 2;

        if (strcmp(dataPath, "/data/input/water") == 0 && strcmp(waterValue, value) != 0) {
          strncpy(waterValue, value, sizeof(waterValue));
          Firebase.setString(firebaseData, dataPath, waterValue);
        } else if (strcmp(dataPath, "/data/input/moisture") == 0 && strcmp(moistureValue, value) != 0) {
          strncpy(moistureValue, value, sizeof(moistureValue));
          Firebase.setString(firebaseData, dataPath, moistureValue);
        }
      }
    }
  }
}

// Update light status
void updateLightStatus() {
  if (Firebase.getString(firebaseData, "/data/output/lightStatus")) {
    strncpy(lightStatus, firebaseData.stringData().c_str(), sizeof(lightStatus));
    digitalWrite(ledPin, strcmp(lightStatus, "On") == 0 ? HIGH : LOW);
  }
}

// Update water pump status
void updateWaterPumpStatus() {
  if (Firebase.getString(firebaseData, "/data/output/waterPump")) {
    strncpy(waterPumpStatus, firebaseData.stringData().c_str(), sizeof(waterPumpStatus));
    Serial1.write(waterPumpStatus);

    if (strcmp(waterPumpStatus, "1") == 0) {
      sendToWebApp();
    }
  }
}

// Send data to the web app
void sendToWebApp() {
  char url[200];
  snprintf(url, sizeof(url), "%s?sts=write&humidity=%.2f&temperature=%.2f&moisture=%s&waterTank=%s",
           Web_App_URL, humidity, temperature, moistureValue, waterValue);
  http.begin(url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.end();
}

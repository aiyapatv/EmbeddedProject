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

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);

  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set Firebase credentials
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;  // Use legacy token

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);

  // Test Firebase connection
  if (Firebase.ready()) {
    if (Firebase.getString(firebaseData, "/data/output/lightStatus")) {
      lightStatus = firebaseData.stringData();  // Extract the string value
      Serial.println("lightStatus: " + lightStatus);
      if (lightStatus == "On") {
        digitalWrite(ledPin, HIGH);
      } else {
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

  // Fetch data from Firebase
  if (Firebase.beginStream(firebaseData, "/data/output")) {
    Serial.println("Stream started successfully.");
    Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  } else {
    Serial.println("Could not start stream.");
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // Handle stream events
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
  } else {
    String humidityStr = String(humidity);
    String temperatureStr = String(temperature);
    parseAndSendToFirebase("/humidity: " + humidityStr);
    parseAndSendToFirebase("/temperature: " + temperatureStr);
  }

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
        if (dataPath == "/data/water" && value != waterValue) {
          waterValue = value;
          parseAndSendToFirebase(temp);
        } else if (dataPath == "/data/moisture" && value != moistureValue) {
          waterValue = value;
          parseAndSendToFirebase(temp);
        }
      }
      temp = "";
    }
  }
  while (Serial1.available()) {
    Serial1.read();  // Read and discard any leftover data
  }
  if (!Firebase.readStream(firebaseData)) {
    Serial.println("Stream error: ");
    Serial.println(firebaseData.errorReason());
  }
  delay(1000);
}
// Callback function for when the data at the specified path changes
void streamCallback(StreamData data) {
  Serial.println("Stream data received:");
  Serial.println(data.stringData());  // Log the received data for debugging

  // Check if the data type is JSON
  if (data.dataType() == "json") {
    // Parse the JSON object
    FirebaseJson* json = data.jsonObjectPtr();
    FirebaseJsonData jsonData;

    // Extract lightStatus
    if (json->get(jsonData, "lightStatus")) {
      String lightStatus = jsonData.stringValue;
      Serial.println("Updated lightsStatus: " + lightStatus);
      if (lightStatus == "On") {
        digitalWrite(ledPin, HIGH);
        Serial.println("Light turned ON");
      } else if (lightStatus == "Off") {
        digitalWrite(ledPin, LOW);
        Serial.println("Light turned OFF");
      }
    }

    // Extract waterPump
    if (json->get(jsonData, "waterPump")) {
      String waterPumpStatus = jsonData.stringValue;
      Serial.println("Updated waterPump: " + waterPumpStatus);
      Serial1.write(waterPumpStatus.c_str());  // Send the updated status to the pump
    }
  } else {
    Serial.println("Unhandled data type. Expected JSON.");
  }
}

// Callback function for stream timeout
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, reconnecting...");
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    delay(1000);
    if (Firebase.beginStream(firebaseData, "/data/output")) {
      Serial.println("Stream reconnected successfully.");
    } else {
      Serial.print("Failed to reconnect stream. Error: ");
      Serial.println(firebaseData.errorReason());
    }
  }
}

void parseAndSendToFirebase(String message) {
  int colonIndex = message.indexOf(':');
  if (colonIndex > 0) {
    // Extract the dataPath and value
    String dataPath = path + message.substring(0, colonIndex);
    dataPath.trim();
    String value = message.substring(colonIndex + 2);
    value.trim();
    // Log the data being sent
    Serial.print(dataPath);
    Serial.println(value);

    // Send the data to Firebase
    if (Firebase.setString(firebaseData, dataPath, value)) {
      Serial.println("Data sent to Firebase successfully.");
    } else {
      Serial.println("Failed to send data to Firebase: " + firebaseData.errorReason());
    }
  } else {
    Serial.println("Invalid message format. Skipping...");
  }
}
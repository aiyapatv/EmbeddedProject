#include <WiFi.h>
#include <FirebaseESP32.h>

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

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);

  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set Firebase credentials
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH; // Use legacy token

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  
  // Test Firebase connection
  if (Firebase.ready()) {
    Serial.println("Firebase initialized successfully.");
  } else {
    Serial.println("Failed to initialize Firebase.");
  }

  // Fetch data from Firebase
  if (Firebase.beginStream(firebaseData, path)) {
    Serial.println("Stream started successfully.");
    Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  } else {
    Serial.println("Could not start stream.");
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // Handle stream events
  if (Serial.available()) {
    char c = Serial.read();
    Serial1.write(c);
  }
  while (Serial1.available()) {
    char c = Serial1.read();
    if(c != '\n'){
      temp += c;
    }else{
      parseAndSendToFirebase(temp);
      temp = "";
    }
  }
  if (!Firebase.readStream(firebaseData)) {
    Serial.println("Stream error: ");
    Serial.println(firebaseData.errorReason());
  }
}

// Callback function for when the data at the specified path changes
void streamCallback(StreamData data) {
  Serial.println("Stream data received:");

  // Get the path and value that triggered the event
  String changedPath = data.dataPath();
  String value = data.stringData();

  Serial.print("Changed Path: ");
  Serial.println(changedPath);
  Serial.print("Value: ");
  Serial.println(value);

  // Process based on the subpath
  if (changedPath == "/lightStatus") {
    if (value == "On"){
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  } else if (changedPath == "/temperature") {
    Serial.print("Temperature updated: ");
    Serial.println(value);
  } else if (changedPath == "/humidity") {
    Serial.print("Humidity updated: ");
    Serial.println(value);
  } else if (changedPath == "/waterPump") {
    Serial.print("Pump updated: ");
    Serial.println(value);
  } else if (changedPath == "/moisture"){
  } else {
    Serial.println("Unknown path. No action taken.");
  }
}

// Callback function for stream timeout
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, reconnecting...");
    Firebase.beginStream(firebaseData, path);
  }
}

void updateLightStatus(String value) {
  
}

void parseAndSendToFirebase(String message) {
  int colonIndex = message.indexOf(':'); // Find the position of ':'
  if (colonIndex > 0) {
    // Extract the statusPath and status
    String dataPath = path + message.substring(0, colonIndex);
    dataPath.trim();
    String value = message.substring(colonIndex + 1);
    value.trim();

    Serial.println("Parsed Data:");
    Serial.println("Path: " + dataPath);
    Serial.println("Value: " + value);

    // Send to Firebase
    if (Firebase.setString(firebaseData, dataPath, value)) {
      Serial.println("Data sent to Firebase successfully.");
    } else {
      Serial.println("Failed to send data to Firebase: " + firebaseData.errorReason());
    }
  } else {
    Serial.println("Invalid message format. Skipping...");
  }
}
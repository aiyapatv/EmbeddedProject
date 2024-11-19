#include <WiFi.h>
#include <FirebaseESP32.h>

int LEDState = 0;

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

const char* path = "/voiceCommand";
const char* statusPath = "/lightStatus";

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
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
  if (!Firebase.readStream(firebaseData)) {
    Serial.println("Stream error: ");
    Serial.println(firebaseData.errorReason());
  }
}

// Callback function for when the data at the specified path changes
void streamCallback(StreamData data) {
  Serial.println("Stream data received:");
  if (data.dataType() == "string") {
    String voiceCommand = data.stringData();
    Serial.println("Updated voice command: ");
    Serial.println(voiceCommand);
    voiceCommand.toLowerCase();
    if(voiceCommand == "on" || voiceCommand == "on."){
      LEDState = 1;
      updateLightStatus(true);
    }else if(voiceCommand == "off" || voiceCommand == "off."){
      LEDState = 0;
      updateLightStatus(false);
    }
  }
}

// Callback function for stream timeout
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, reconnecting...");
    Firebase.beginStream(firebaseData, path);
  }
}

void updateLightStatus(bool isOn) {
  String status = isOn ? "ON" : "OFF";
  if (Firebase.setString(firebaseData, statusPath, status)) {
    Serial.println("Light status updated successfully.");
  } else {
    Serial.print("Failed to update light status: ");
    Serial.println(firebaseData.errorReason());
  }
}
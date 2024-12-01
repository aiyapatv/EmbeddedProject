#include "DHT.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define RXD1 (18)
#define TXD1 (19)

#define WIFI_SSID "Aiyapat iPhone"
#define WIFI_PASSWORD "VervergComaL123"

// ตั้งค่าเชื่อมต่อ SMTP
#define SMTP_HOST "smtp.gmail.com"             // กำหนด Host SMTP
#define SMTP_PORT 587                          // กำหนด SMTP port 25 หรือ 465 หรือ 587
#define AUTHOR_EMAIL "aiyapatvo@gmail.com"     // กำหนด Email SMTP
#define AUTHOR_PASSWORD "olau dzbt kbdg ectj"  // กำหนดรหัสผ่าน SMTP
#define SENDER_NAME "Water water on the Wall"  // กำหนดชื่อผู้ส่งเมล์

const int waterPin = 4;
const int moistPin = A0;
const int pumpPin = 13;
float water = 0;
float waterPrev = 0;
float moisture = 0;
char waterPump = ' ';
unsigned long previousMillis = 0;  // Stores the last time this section was executed
const unsigned long interval = 5000;

Session_Config config;  // Declare globally
SMTPSession smtp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  pinMode(pumpPin, OUTPUT);
  pinMode(waterPin, INPUT);
  digitalWrite(pumpPin, HIGH);
  analogReadResolution(12);  // Default resolution for ESP32
  analogSetAttenuation(ADC_11db);

  // เริ่มเชื่อมต่อ WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // ตั้งค่าเชื่อมต่อ SMTP
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;

  /* Set the NTP config time */
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 7;
  config.time.day_light_offset = 0;

  smtp.StatusCallback(smtpCallback);
  // เชื่อมต่อเซิร์ฟเวอร์
  if (!smtp.connect(&config)) {
    smtp.debug(1);
    Serial.println("SMTP connect fail");
    return;
  }

  if (!smtp.isLoggedIn()) {
    Serial.println("SMTP login fail");
    return;
  }
}



void loop() {
  // put your main code here, to run repeatedly:
  // humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  // temperature = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  unsigned long currentMillis = millis();
  moisture = analogRead(moistPin);
  moisture = (4095 - moisture) / 4095 * 100;
  water = digitalRead(waterPin);
  while (Serial1.available()) {
    char c = Serial1.read();
    waterPump = c;
  }
  if ((waterPump == '1' || moisture <= 10) && water == 0) {
    digitalWrite(pumpPin, LOW);
  } else {
    digitalWrite(pumpPin, HIGH);
  }
  if (currentMillis - previousMillis >= interval) {
    // Update the previous time
    previousMillis = currentMillis;
    Serial.print("/moisture: ");
    Serial.println(moisture);
    Serial.print("/water: ");
    Serial.println(water);
    if (isnan(moisture) || isnan(water)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      String moistureStr = "/data/input/moisture: " + String(moisture);
      String waterStr = "/data/input/water: " + String(water);

      if (water == 0) {
        waterPrev = 0;
      }
      Serial1.flush();
      Serial1.write(moistureStr.c_str());
      Serial1.write("\n");  // Add a newline as a delimiter
      Serial1.write(waterStr.c_str());
      Serial1.write("\n");
      if (water == 1 && waterPrev == 0) {
        waterPrev = 1;
        Serial.println("Sending Email...");
        SMTP_Message message;
        message.sender.name = SENDER_NAME;                                                                                                                                     // ชื่อผู้ส่ง
        message.sender.email = AUTHOR_EMAIL;                                                                                                                                   // อีเมล์ผู้ส่ง
        message.subject = "Notification: Water is running out!";                                                                                                               // หัวข้อเมล์
        message.text.content = "Your plant is in trouble! It’s running out of water and won’t survive much longer without your care. Please water it soon to save its life!";  // ข้อความในเมล์

        // ตั้งค่าผู้รับ
        message.addRecipient("Pingy", "pingypingy777@gmail.com");  // ชื่อผู้รับ, อีเมล์ผู้รับ
        smtp.debug(1);
        // ส่งอีเมล์
        if (MailClient.sendMail(&smtp, &message)) {
          Serial.println("Send mail successful");
        } else {
          Serial.println("Send mail fail : " + smtp.errorReason());
        }
      }
    }
  }
}

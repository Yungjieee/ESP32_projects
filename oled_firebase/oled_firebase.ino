#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <WebServer.h>

// === OLED Setup ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === WiFi Credentials ===
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

// Firebase credentials
#define API_KEY "api"
#define DATABASE_URL "url"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Firebase helper files
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// === Firebase Path ===
String firebasePath = "/102/oled/message";  // Example path to your message

// === Timing Control ===
unsigned long lastUpdate = 0;
unsigned long interval = 2000;  // 10 seconds

void setup() {
  Serial.begin(115200);

  // OLED Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Connecting WiFi...");
  display.display();

  // WiFi Init
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.display();

  // Firebase Init
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = "yungjielee@gmail.com";
  auth.user.password = "Asdfg12345";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  while (auth.token.uid == "") {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nFirebase Ready!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Firebase Ready");
  display.display();
}

void loop() {

    // Refresh token if needed
  if (!Firebase.ready() && Firebase.isTokenExpired()) {
    Serial.println("Token expired. Restarting Firebase...");
    Firebase.begin(&config, &auth);
    Serial.println("Token refreshed successfully.");
    return;
  }

  // Skip loop if token is not ready
  if (!Firebase.ready()) {
    Serial.println("Skipping loop: Firebase token not ready");
    // Firebase.refreshToken() is void in latest library, no return value to check
    delay(1000);
    return;
  }

  if (millis() - lastUpdate >= interval) {
    lastUpdate = millis();

    if (Firebase.RTDB.getString(&fbdo, firebasePath)) {
      String fetchedText = fbdo.stringData();
      Serial.println("Fetched from Firebase: " + fetchedText);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Firebase Msg:");
      display.println(fetchedText);
      display.display();
    } else {
      String error = fbdo.errorReason();
      Serial.println("Firebase Read Failed: " + error);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Error reading:");
      display.println(error);
      display.display();
    }
  }
}

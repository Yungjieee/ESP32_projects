#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"
#include <time.h>  // For NTP time

// WiFi credentials
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

// Firebase credentials
#define API_KEY "api"
#define DATABASE_URL "url"

// DHT sensor setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LED_PIN 2
int previousLedStatus = -1; 

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Firebase helper files
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // pinMode(RELAY_PIN, OUTPUT);
  // digitalWrite(RELAY_PIN, HIGH);  // OFF initially (for active LOW relay)

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize NTP
  configTime(28800, 0, "pool.ntp.org", "time.nist.gov");  // Malaysia UTC+8
  Serial.println("Waiting for NTP time sync...");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synchronized");

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = "yungjielee@gmail.com";
  auth.user.password = "password";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Authenticating with Firebase...");
  while (auth.token.uid == "") {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nFirebase Ready!");
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
  static unsigned long lastDhtUpdate = 0;
  unsigned long currentMillis = millis();

  // Update DHT data every 10 seconds
  if (currentMillis - lastDhtUpdate >= 10000) {
    lastDhtUpdate = currentMillis;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    time_t now = time(nullptr);  // Get current epoch time

    if (!isnan(temperature) && !isnan(humidity)) {
      Serial.printf("Temp: %.2f°C | Humidity: %.2f%%", temperature, humidity);

      Firebase.RTDB.setFloat(&fbdo, "/101/dht/temperature", temperature);
      Firebase.RTDB.setFloat(&fbdo, "/101/dht/humidity", humidity);
      Firebase.RTDB.setInt(&fbdo, "/101/dht/timestamp", now);
    } else {
      Serial.println("⚠️ DHT sensor read failed.");
      Firebase.RTDB.setString(&fbdo, "/101/errors/dht", "Failed to read from DHT sensor");
    }
  }

  // Get led control value from Firebase (expecting 0 or 1)
  if (Firebase.RTDB.getInt(&fbdo, "/103/led/status")) {
    int ledStatus = fbdo.intData();
    Serial.print("LED command from Firebase: ");
    Serial.println(ledStatus);

    if (ledStatus != previousLedStatus) {
      if (ledStatus == 1) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED turned ON");
      } else {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED turned OFF");
      }

      previousLedStatus = ledStatus;
      // Firebase.RTDB.setInt(&fbdo, "/103/led/status", ledStatus);  //application will set the status, arduino will not set the status
    } else {
      Serial.println("LED status unchanged, no action taken.");
    }
  } else {
    String errorMsg = fbdo.errorReason();
    Serial.print("Failed to read led status: ");
    Serial.println(errorMsg);
    Firebase.RTDB.setString(&fbdo, "/103/errors/led", errorMsg);
  }

}

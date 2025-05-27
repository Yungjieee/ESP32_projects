#include <WiFi.h>
#include "DHT.h"
#include <HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "Alicia";
const char* pass = "";
float hum = 0, temp = 0;
unsigned long sendDataPrevMillis = 0;
int count = 0;
String serverName = "http://iottraining.threelittlecar.com/";

// DHT sensor setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  WiFi.begin(ssid, pass);
  delay(100);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0) {
    count++;
    getDHT();
    sendDataPrevMillis = millis();

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String httpReqStr = serverName + "dht11.php?id=101&temp=" + temp + "&hum=" + hum;
      http.begin(client, httpReqStr.c_str());
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
  }
}


void getDHT() {
  // int chk = DHT.read11(19);
  // Serial.print("Temperature=");
  // Serial.println(DHT.temperature);
  // hum = DHT.humidity;
  // temp = DHT.temperature;
  // Serial.print("Humidity=");
  // Serial.println(DHT.humidity);
  // delay(200);
  
    delay(2000);
    temp = dht.readTemperature();
    hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      Serial.printf("Temp: %.2f°C | Humidity: %.2f%% ", temp, hum);

    } else {
      Serial.println("DHT sensor read failed.");
      
    }

    delay(200);

}
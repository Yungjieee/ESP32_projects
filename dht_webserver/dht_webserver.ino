#include <WebServer.h>
#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 4  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11

String ssid = "DHTServer";
String password = "";
WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

float h = 0.0;
float t = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  server.begin();
  delay(500);
  server.on("/", handle_root);
}

void handle_root() {
  String content = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'><title>DHT Controller</title>";
  content += "</head><body><center><h3>DHT 11 Control System</h3>";
  content += "<br><p>Temperature: " + String(t) + "<br>Humidity: " + String(h) + "<br></p>";
  content += "</center></body></html>";
  server.send(200, "text/html", content);
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  delay(2000);

  h = dht.readHumidity();
  t = dht.readTemperature();

  //Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}
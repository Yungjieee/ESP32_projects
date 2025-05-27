#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// wifi credentials
const char* ssid = "Alicia";
const char* password = "";

// MQTT broker settings
const char* mqttServer = "broker.hivemq.com";  //broker.hivemq.com
const int mqttPort = 1883;
const char* mqttClientId = "ESP_DHT11_YUNGJIE";

//DHT11 settings
#define DHTPIN 4     // GPIO19 (D4) on ESP32
#define DHTTYPE DHT11  // DHT11
DHT dht(DHTPIN, DHTTYPE);

float hum = 0, temp = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);

  // Initialize dht sensor
  dht.begin();

  // connect to wifi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n WiFi connected");

  //setup MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  Serial.print("Connecting to MQTT_");
  while (!mqttClient.connected()) {
    if (mqttClient.connect(mqttClientId)) {
      Serial.println(" connected");
    } else {
      Serial.println(" failed, rc = ");
      Serial.print(mqttClient.state());
      Serial.print(", retrying in 2 seconds");
      delay(2000);
    }
  }
}

void loop() {
  
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  
  //validate reading
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT sensor read failed.");
    
  } else {
    // publish to mqtt topics
    char buf[16];

    snprintf(buf, sizeof(buf), "%.1f", temp);
    mqttClient.publish("esp32_lee/dht11/temperature", buf);

    snprintf(buf, sizeof(buf), "%.1f", hum);
    mqttClient.publish("esp32_lee/dht11/humidity", buf); 

    Serial.printf("\nTemp: %.2f°C | Humidity: %.2f%% ", temp, hum);
  }

  // maintain mqtt connection
  if (!mqttClient.connected()){
    // reconnect logic here if needed
  }
  mqttClient.loop();

  delay(5000); // wait 5 seconds between reads
}

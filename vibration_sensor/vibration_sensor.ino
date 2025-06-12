const int sensorPin = 25;

bool vibrationDetected = false;
unsigned long lastVibrationTime = 0;
const unsigned long vibrationHoldTime = 1000;  // How long to wait before saying "No vibration"

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
}

void loop() {
  bool state = digitalRead(sensorPin);

  if (state == HIGH) {
    if (!vibrationDetected) {
      Serial.println("Detected vibration...");
      vibrationDetected = true;
    }
    lastVibrationTime = millis();
  }

  // If no vibration detected for a while, print "No vibration"
  if (vibrationDetected && millis() - lastVibrationTime > vibrationHoldTime) {
    Serial.println("No vibration detected.");
    vibrationDetected = false;
  }

  delay(10); // Fast loop to catch short pulses
}

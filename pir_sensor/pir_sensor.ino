const int PIR_SENSOR_OUTPUT_PIN = 13;
volatile bool motionDetected = false;
unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 5000; // 5 seconds

void setup() {
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_OUTPUT_PIN), pir, RISING);
  Serial.begin(115200);
  Serial.println("Waiting For Power On Warm Up...");
  delay(20000); // Warm-up delay
  Serial.println("Ready!");
}

void loop() {
  if (motionDetected) {
    Serial.println("Motion detected!");
    motionDetected = false;
    lastMotionTime = millis(); // Update last motion time
  }

  // If it's been more than 5 seconds since last motion, show "No motion"
  if (millis() - lastMotionTime > motionTimeout) {
    Serial.println("No motion detected.");
    lastMotionTime = millis(); // Prevent spamming
  }

  delay(200); // Lower CPU usage
}

void pir() {
  motionDetected = true;
}

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);

// Minimal ESP32 + SEN-11574 with auto-threshold
int PulseSensorPurplePin = 34;  // ADC1 pin
int LED = 2;                    // onboard LED

int Signal;                                    // raw 0..4095
unsigned long lastBeat = 0, refractory = 300;  // ms
int BPM = 0;

// Filters / adaptive stats
float dc = 0.0f;               // slow DC baseline
const float dcAlpha = 0.01f;   // slower -> 0.005..0.02
float band = 0.0f;             // high-passed pulse
float env = 0.0f;              // envelope (EMA of |band|)
const float envAlpha = 0.05f;  // 0.03..0.10
float k = 1.6f;                // threshold factor (raise to reduce false triggers)

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  analogReadResolution(12);
  analogSetPinAttenuation(PulseSensorPurplePin, ADC_11db);

  // Warm-up reads
  for (int i = 0; i < 20; i++) {
    Signal = analogRead(PulseSensorPurplePin);
    delay(5);
  }
  dc = Signal;
}

void loop() {
  // --- read & filter ---
  Signal = analogRead(PulseSensorPurplePin);
  dc += dcAlpha * (Signal - dc);         // track DC
  band = Signal - dc;                    // remove DC
  env += envAlpha * (fabs(band) - env);  // envelope of pulse
  float thr = k * env;                   // dynamic threshold

  // --- detection with refractory ---
  static bool below = true;  // rising-edge arm
  unsigned long now = millis();

  if (below && band > thr && (now - lastBeat) > refractory) {
    // Beat!
    digitalWrite(LED, HIGH);
    if (lastBeat != 0) {
      unsigned long ibi = now - lastBeat;
      BPM = (ibi > 0) ? (60000 / ibi) : 0;
    }
    lastBeat = now;
    below = false;
  }
  if (band < thr * 0.5f) {  // re-arm when well below
    below = true;
    digitalWrite(LED, LOW);
  }

  // --- plot: raw and BPM ---
  // Serial.print(Signal); Serial.print(',');
  // Serial.println(BPM);

  Serial.print(Signal - 1800);  // remove baseline so you can see the pulse oscillation
  Serial.print(",");
  Serial.println(BPM);

  delay(20);  // ~50 Hz sampling (fine for BPM)

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("BPM");
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.println(BPM);
  display.display();
}


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32  // 0.91" OLED
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String inputText = "";

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Hello I am Alicia");
  display.display();
}

void loop() {
  if (Serial.available()) {
    inputText = Serial.readStringUntil('\n');
    inputText.trim();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Received:");
    display.println(inputText);
    display.display();

    Serial.println("Message shown on OLED!");
  }
}

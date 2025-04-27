#include <Adafruit_GFX.h>             // OLED graphics support
#include <Adafruit_SSD1306.h>         // OLED screen driver
// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define I2C_SDA 21
#define I2C_SCL 22
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
   // Initialize OLED display
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true); // Stop if OLED fails
  }
  Serial.println("OLED INIT...");

}

void loop() {
  // put your main code here, to run repeatedly:

}

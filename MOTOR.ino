#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library

const char* API_KEY = "Q5R6WLKSRJAM64O8";
unsigned int CHANNEL_ID = 2440023;

const char *ssid = "GD";
const char *pass = "1234567890";

WiFiClient client;

int motorPin1 = 4;
unsigned long previousMillis = 0;  // Variable to store the last time motor started
unsigned long currentMillis = 0;   // Variable to store the current time
bool delayActive = false;          // Flag to control the delay

#define TFT_CS     15
#define TFT_RST    0  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup() {
  Serial.begin(57600);
  delay(10);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  ThingSpeak.begin(client);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(10, 10);
    tft.print(String(data, 2));

  pinMode(motorPin1, OUTPUT);
}

void loop() {
  // Reading float value from field 4
  float data = ThingSpeak.readFloatField(CHANNEL_ID, 4, API_KEY);
  Serial.println("Field 4 value: " + String(data, 2)); // Displaying with 2 decimal places

  

  currentMillis = millis();  // Update current time

  // Run the motor if data is non-zero and motor is not already running
  if (data > 0) {
    digitalWrite(motorPin1, HIGH);
    previousMillis = millis(); // Record the start time of motor operation
    delayActive = true;        // Set the flag to activate the delay
  }

  // Check if the delay is active and has been running for more than 10 seconds
  if (delayActive && (currentMillis - previousMillis >= 10000)) {
    digitalWrite(motorPin1, LOW); // Stop the motor
    delayActive = false;           // Reset the delay flag
  }

  // Check if the delay is not active and has been running for more than 150 seconds
  if (!delayActive && (currentMillis - previousMillis >= 150000)) {
    delayActive = true;  // Set the flag to activate the delay
    digitalWrite(motorPin1, HIGH); // Start the motor
    previousMillis = millis(); // Update the start time
  }
}
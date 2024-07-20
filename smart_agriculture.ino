#include <ThingSpeak.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

const char* API_KEY = "Q5R6WLKSRJAM64O8";
unsigned int CHANNEL_ID = 2440023;

const char *ssid = "Rajeshwar"; 
const char *pass = "123456789";

#define DHTPIN 0 
#define SENSOR_PIN A0

DHT dht(DHTPIN, DHT11);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();

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
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float m;

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from sensor!");
    return;
  }

  m = 100.00 - ((analogRead(SENSOR_PIN) / 1023.00) * 100.00);

  Serial.print("Soil Moisture (in Percentage) = ");
  Serial.print(m);
  Serial.println("%");

  Serial.printf("Temp: %.2f Humidity: %.2f Moisture: %.2f \n", t, h, m);

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, m);
  ThingSpeak.setField(4, 0); // Assuming you want to send 0 to field 4

  ThingSpeak.writeFields(CHANNEL_ID, API_KEY);

  delay(1000);

  // Reading float value from field 4
  float data = ThingSpeak.readFloatField(CHANNEL_ID, 4, API_KEY);
  Serial.println("Field 4 value: " + String(data));
}

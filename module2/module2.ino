#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>    // Include the graphics library

#define DHTPIN 2          // Set the pin connected to the DHT11 data pin
#define DHTTYPE DHT11     // DHT 11
#define BUTTON_PIN 0      // Change this to your actual button pin

const char* ssid = "TODO";
const char* password = "TODO";
const char* googleScriptUrl = "https://script.google.com/macros/s/TODO/exec";

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();  // Create TFT instance
volatile bool displayInCelsius = false;  // Initialized to false to default to Fahrenheit
unsigned long lastDebounceTime = 0;  // For button debounce
unsigned long debounceDelay = 50;    // the debounce time in milliseconds

void IRAM_ATTR isr() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDebounceTime > debounceDelay) {
    displayInCelsius = !displayInCelsius;
    lastDebounceTime = currentMillis;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 test!");
  dht.begin();

  tft.init();
  tft.setRotation(3); // landscape with cable on left
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  tft.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempts++;
    tft.print(".");
    if(attempts >= 30) { // Give up after 30 attempts
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.println("Failed to connect");
        return;
    }
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.println("WiFi Connected");
  tft.println("SSID: ");
  tft.println(WiFi.SSID());
  tft.println("IP Address: ");
  tft.println(WiFi.localIP());

  Serial.println("WiFi Connected");
  Serial.println("SSID: " + WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr, FALLING);
}

void loop() {
  delay(2000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(!displayInCelsius); // Read temperature, invert displayInCelsius for correct interpretation

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);

  // Adjust the unit display based on displayInCelsius
  String tempUnit = displayInCelsius ? " C" : " F";

  // Display Temperature and Humidity
  tft.print("Temp: ");
  tft.print(temperature);
  tft.println(tempUnit);

  tft.print("Humi: ");
  tft.print(humidity);
  tft.println("%");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(tempUnit);

  if(WiFi.status() == WL_CONNECTED){
    String url = String(googleScriptUrl) + "?temperature=" + String(temperature) + "&humidity=" + String(humidity);

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
      String payload = http.getString();
      Serial.println("Data sent to Google Sheets");
      Serial.println(payload);
    } else {
      Serial.print("Error on sending HTTP Request: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

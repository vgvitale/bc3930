#include "DHT.h"

#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 2  // Set the pin connected to the DHT11 data pin
#define DHTTYPE DHT11 // DHT 11 

const char* ssid = "TODO";
const char* password = "TODO";
const char* googleScriptUrl = "https://script.google.com/macros/s/TODO/exec";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 test!");
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Print the humidity and temperature
  Serial.print("Humidity: "); 
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temperature);
  Serial.println(" *C");

  if(WiFi.status() == WL_CONNECTED){
    String url = String(googleScriptUrl) + "?temperature=" + String(temperature) + "&humidity=" + String(humidity);

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET(); //Send the request
    if (httpCode > 0) { //Check the returning code
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
      String payload = http.getString();   //Get the request response payload
      Serial.println("Data sent to Google Sheets");
      Serial.println(payload);             //Print the response payload
    } else {
      Serial.print("HTTP Request failed, error: ");
      Serial.println(httpCode);
    }
    http.end(); //Free resources
  } else {
    Serial.println("Error in WiFi connection");
  }
}

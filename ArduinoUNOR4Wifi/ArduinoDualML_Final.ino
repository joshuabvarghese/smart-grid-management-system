// Author: Imaar Rathur
// Date: 06/10/2024
// I/O: I2C Data Stream, Sensor Data
// Source: https://www.ti.com/product/INA219, https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf, https://reference.arduino.cc/reference/en/libraries/wifi/wificlient/

#include <Wire.h>
#include <Adafruit_INA219.h>
#include "DHT.h"           // For DHT sensors
#include <WiFi.h>          // For WiFi connectivity
#include <Arduino_JSON.h>  // JSON library for Arduino

#define DHTTYPE DHT11  // DHT 11
#define DHTPIN1 3      // Pin for DHT sensor 1
#define DHTPIN2 4      // Pin for DHT sensor 2

// WiFi credentials
const char* ssid = "HS8MMDOg8inZIZLI";    // WiFi SSID
const char* password = "pKy5fkxfmlVrWQB7";   // WiFi password
const char* server = "192.168.xxx.xxx";  // Raspberry Pi's IP address

WiFiClient client;  // Create a WiFi client

Adafruit_INA219 ina219;
DHT dht1 = DHT(DHTPIN1, DHTTYPE);
DHT dht2 = DHT(DHTPIN2, DHTTYPE);

// Transistor control pins
#define TRANSISTOR1_PIN 5  // Pin for transistor 1
#define TRANSISTOR2_PIN 6  // Pin for transistor 2

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void sendSensorData(String data) {
  if (client.connect(server, 5000)) {  // Connect to Raspberry Pi on port 5000
    Serial.println("Connected to server");

    // HTTP POST request to /sensor endpoint
    client.println("POST /sensor HTTP/1.1");
    client.println("Host: " + String(server));  // Use the actual server address
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);

    // Wait for server response
    while (client.connected()) {
      if (client.available()) {
        String response = client.readString();
        Serial.println("Received response: ");
        Serial.println(response);

        // Parse the JSON response using Arduino_JSON library
        JSONVar responseObject = JSON.parse(response);

        // Find the position of "transistor1_state" and extract its value
        int start1 = response.indexOf("transistor1_state") + strlen("transistor1_state") + 2;
        int end1 = response.indexOf(",", start1);
        String transistor1_value = response.substring(start1, end1);
        int transistor1_state = transistor1_value.toInt();

        // Find the position of "transistor2_state" and extract its value
        int start2 = response.indexOf("transistor2_state") + strlen("transistor2_state") + 2;
        int end2 = response.indexOf("}", start2);
        String transistor2_value = response.substring(start2, end2);
        int transistor2_state = transistor2_value.toInt();
        Serial.println(String(transistor1_state));

        // Control transistors based on the received state
        if (transistor1_state == 1) {
          digitalWrite(TRANSISTOR1_PIN, HIGH);  // Turn transistor 1 ON
        } else {
          digitalWrite(TRANSISTOR1_PIN, LOW);  // Turn transistor 1 OFF
        }

        if (transistor2_state == 1) {
          digitalWrite(TRANSISTOR2_PIN, HIGH);  // Turn transistor 2 ON
        } else {
          digitalWrite(TRANSISTOR2_PIN, LOW);  // Turn transistor 2 OFF
        }

        // Debug output for transistor states
        Serial.print("Transistor 1 State: ");
        Serial.println(transistor1_state ? "ON" : "OFF");
        Serial.print("Transistor 2 State: ");
        Serial.println(transistor2_state ? "ON" : "OFF");
      }
    }
    client.stop();  // Close the connection
  } else {
    Serial.println("Connection failed");
  }
}

void setup(void) {
  Serial.begin(115200);
  setup_wifi();

  dht1.begin();
  dht2.begin();

  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  // Initialize GPIO pins for transistors
  pinMode(TRANSISTOR1_PIN, OUTPUT);
  pinMode(TRANSISTOR2_PIN, OUTPUT);

  // Set initial state to LOW (off)
  digitalWrite(TRANSISTOR1_PIN, LOW);
  digitalWrite(TRANSISTOR2_PIN, LOW);

  Serial.println("Measuring voltage and current...");
}

void loop(void) {
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  float humidity = dht1.readHumidity();
  float temperature = dht2.readTemperature();

  // Create JSON payload
  String payload = "{\"bus_voltage\":" + String(busvoltage) + ",\"shunt_voltage\":" + String(shuntvoltage) + ",\"current_mA\":" + String(current_mA) + ",\"power_mW\":" + String(power_mW) + ",\"humidity\":" + String(humidity) + ",\"temperature\":" + String(temperature) + "}";

  // Send data over WiFi
  sendSensorData(payload);

  // Debug output
  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(shuntvoltage);
  Serial.println(" mV");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.println("");

  delay(1000);  // Wait for 1 second before the next reading
}

// Author: Imaar Rathur
// Date: 24/09/2024
// I/O: I2C Data Stream, Sensor Data
// Source: https://www.ti.com/product/INA219, https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf, https://reference.arduino.cc/reference/en/libraries/wifi/wificlient/

#include <Wire.h>
#include <Adafruit_INA219.h>
#include "DHT.h"

#define DHTTYPE DHT11 // DHT 11
#define DHTPIN1 4
#define DHTPIN2 3

Adafruit_INA219 ina219;
DHT dht1 = DHT(DHTPIN1, DHTTYPE);
DHT dht2 = DHT(DHTPIN2, DHTTYPE);

void setup(void) {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  while (!Serial) {
      // Wait for the serial connection
  }

  if (!ina219.begin()) {
      Serial.println("Failed to find INA219 chip");
      while (1) { delay(10); }
  }

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


  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.println("");

  delay(500);  // Wait for 2 seconds before the next reading
}

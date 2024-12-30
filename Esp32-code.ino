#define BLYNK_TEMPLATE_ID "TMPL3t_H_wknB"
#define BLYNK_TEMPLATE_NAME "EMI AND EMF"
#define BLYNK_AUTH_TOKEN "Wv48PQdl679IFiqP9HdkL5IhpLPGIiDB"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <EEPROM.h>
#include <WebServer.h>

// WiFi Credentials
char ssid[] = "Anandan";         // Replace with your WiFi SSID
char pass[] = "12345671";        // Replace with your WiFi password

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// HTTP Server
WebServer server(8080);

// Hall Effect Sensor
const int sensorPin = 34;   // ADC Pin
const float sensorVoltage = 3.3;
const int sensorResolution = 4095;
const float sensitivity = 0.001;

// Battery Percentage Variables
int batteryPercentage1;
unsigned long previousMillis = 0;
const unsigned long interval = 300000; // 5 minutes in milliseconds

// Variables
float rawValue, sensorOutputVoltage, magneticField;
String ipAddress;
const int port = 8080;

void handleReadings() {
  String json = "{";
  json += "\"sensorVoltage\":" + String(sensorOutputVoltage, 3) + ",";
  json += "\"magneticField\":" + String(magneticField, 3) + ",";
  json += "\"batteryPercentage\":" + String(batteryPercentage1);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("TITANS02 EMI Meter");

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize EEPROM
  EEPROM.begin(512);
  batteryPercentage1 = EEPROM.read(1);
  if (batteryPercentage1 > 100 || batteryPercentage1 < 0) {
    batteryPercentage1 = 100;
  }

  // Get IP Address
  ipAddress = WiFi.localIP().toString();

  // Start HTTP Server
  lcd.clear();
  lcd.print("Loading.............");
  lcd.print("Wait a Minute");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();
  server.handleClient();

  // Read sensor data
  rawValue = analogRead(sensorPin);
  sensorOutputVoltage = (rawValue * sensorVoltage) / sensorResolution;
  magneticField = fabs((sensorOutputVoltage - (sensorVoltage / 2)) / sensitivity);
  const float voltageThreshold = 2.0; // Set the voltage threshold for zero calibration
  if (sensorOutputVoltage < voltageThreshold) {
    sensorOutputVoltage = 0.0; // Set sensor voltage to zero if below the threshold
  }

  // Apply zero calibration
  const float threshold = 150.0; // Set the threshold for zero calibration
  if (magneticField < threshold) {
    magneticField = 0.0; // Set magnetic field to zero if below the threshold
  }

  // Update battery percentage every 15 minutes
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (batteryPercentage1 > 0) {
      batteryPercentage1 -= 1;
      EEPROM.write(0, batteryPercentage1);
      EEPROM.commit();
    }
  }
  float value1 = magneticField / 10;

  // Display on LCD
  lcd.setCursor(0, 0); 
  lcd.print("EMI: ");
  lcd.print(sensorOutputVoltage, 2);
  lcd.print(" V");

  lcd.setCursor(0, 1); 
  lcd.print("Mag Field: ");
  lcd.print(value1, 2);
  lcd.print(" G");

  lcd.setCursor(0, 2);
  lcd.print("Raw ADC: ");
  lcd.print(rawValue);

  lcd.setCursor(0, 3);
  lcd.print("Battery: ");
  lcd.print(batteryPercentage1);
  lcd.print("%");

  // Print to Serial Monitor
  Serial.print("IP: ");
  Serial.println(ipAddress);
  Serial.print("Port: ");
  Serial.println(port);
  Serial.print("EMI (Sensor Voltage): ");
  Serial.println(sensorOutputVoltage, 2);
  Serial.print("Magnetic Field: ");
  Serial.println(value1, 2);
  Serial.print("Battery Percentage: ");
  Serial.println(batteryPercentage1);

  // Send data to Blynk
  Blynk.virtualWrite(V0, sensorOutputVoltage);
  Blynk.virtualWrite(V1, value1);
  Blynk.virtualWrite(V2, batteryPercentage1);

  // Check thresholds and trigger Blynk events
  if (sensorOutputVoltage > 2.8) {
    Blynk.logEvent("high_emi", "High EMI Voltage Detected: " + String(sensorOutputVoltage, 2) + " V");
  }
  
  if (value1 > 80) {
    Blynk.logEvent("high_magnetic_field", "High Magnetic Field Detected: " + String(value1, 2) + " G");
  }

  delay(1000);
}

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize I2C LCD (Address: 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int hallPin = A0;  // Analog pin connected to Hall sensor
float sensorValue = 0;   // Variable to store sensor reading

void setup() {
  lcd.init();          // Initialize LCD
  lcd.backlight();     // Turn on LCD backlight
  Serial.begin(9600);  // Initialize Serial Monitor
  lcd.setCursor(0, 0);
  lcd.print("Reading EMI:");
}

void loop() {
  // Read sensor value (0-1023 for 5V ADC)
  sensorValue = analogRead(hallPin);
  
  // Convert to voltage
  float voltage = sensorValue ;

  // Display on Serial Monitor
  Serial.print("Voltage: ");
  Serial.println(voltage);

  // Display on LCD
  lcd.setCursor(0, 1);
  lcd.print("Voltage: ");
  lcd.print(voltage, 2);  // Print voltage with 2 decimals
  
  delay(500);  // Delay for stability
}
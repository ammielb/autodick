#include <Arduino.h>

// Define the analog pin connected to the voltage divider
const int analogInputPin = A0;

// Constants for voltage measurement
const float referenceVoltage = 5.0; // Arduino Uno's reference voltage

void setup() {
  // Start serial communication
  Serial.begin(9600);
}

void loop() {
  // Read the analog input
  int sensorValue = analogRead(analogInputPin);
  
  // Convert the analog reading to voltage
  float voltage = sensorValue * (referenceVoltage / 1023.0);
  
  // Estimate the remaining capacity in percentage
  int batteryPercentage = map(voltage, 3.0, 4.2, 0, 100); // Assuming battery voltage ranges from 3.0V to 4.2V

  // Print the percentage
  Serial.print("Battery Percentage: ");
  Serial.print(batteryPercentage);
  Serial.println("%");
  
  delay(1000); // Delay for stability
}

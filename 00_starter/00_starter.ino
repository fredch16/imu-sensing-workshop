/*
  GOAL
  Connect to and configure the MPU6050.

  NEW IN THIS CHECKPOINT
  Set up serial, I2C, and the sensor registers.

  UNLOCKED API
  See API.md in this folder.

  SUCCESS LOOKS LIKE
  Serial Monitor prints "MPU6050 is ready".

  THINK ABOUT
  - What does I2C address 0x68 identify?
  - Why do we power the sensor from 3.3 V?
*/
#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDRESS = 0x68;

bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(25, 26, 100000);  // SDA, SCL, 100 kHz
  if (!writeRegister(0x6B, 0x00) ||  // Wake up
      !writeRegister(0x1C, 0x00) ||  // Accelerometer +/-2 g
      !writeRegister(0x1B, 0x00)) {  // Gyroscope +/-250 deg/s
    Serial.println("MPU6050 setup failed. Check wiring.");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("MPU6050 is ready");
}

void loop() {}

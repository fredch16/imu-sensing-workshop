/*
  GOAL
  Read acceleration in g.

  NEW IN THIS CHECKPOINT
  Read six accelerometer bytes and convert raw counts to g.

  SUCCESS LOOKS LIKE
  One axis is near +1 g or -1 g while the sensor is stationary.

  THINK ABOUT
  - Why are there two bytes per axis?
  - What changes when you tilt the sensor?
*/
#include <Arduino.h>
#include <Wire.h>
const uint8_t MPU_ADDRESS = 0x68;

float accelX, accelY, accelZ;

bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
}

bool readAccel() {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);  // First accelerometer register

  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(MPU_ADDRESS, (size_t)6, true) != 6) {
    return false;
  }

  int16_t rawX = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  int16_t rawY = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  int16_t rawZ = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());

  // The +/-2 g range has 16,384 counts per g.
  accelX = rawX / 16384.0f;
  accelY = rawY / 16384.0f;
  accelZ = rawZ / 16384.0f;
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22, 100000);

  if (!writeRegister(0x6B, 0x00) || !writeRegister(0x1C, 0x00)) {
    Serial.println("MPU6050 setup failed");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  if (readAccel()) {
    Serial.print("X: ");
    Serial.print(accelX, 3);
    Serial.print(" g  Y: ");
    Serial.print(accelY, 3);
    Serial.print(" g  Z: ");
    Serial.print(accelZ, 3);
    Serial.println(" g");
  } else {
    Serial.println("Accelerometer read failed");
  }

  delay(100);
}

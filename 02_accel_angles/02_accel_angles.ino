/*
  GOAL
  Calculate roll and pitch from gravity.

  NEW IN THIS CHECKPOINT
  Convert the acceleration vector into tilt angles with atan2().

  SUCCESS LOOKS LIKE
  Roll and pitch follow the sensor's tilt while it is stationary.

  THINK ABOUT
  - Why can gravity not tell us yaw?
  - What happens during fast linear motion?
*/
#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDRESS = 0x68;

float accelX, accelY, accelZ;
float roll, pitch;

bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
}

bool readAccel() {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(MPU_ADDRESS, (size_t)6, true) != 6) {
    return false;
  }

  int16_t rawX = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  int16_t rawY = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  int16_t rawZ = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());

  accelX = rawX / 16384.0f;
  accelY = rawY / 16384.0f;
  accelZ = rawZ / 16384.0f;
  return true;
}

void computeAccelAngles() {
  roll = atan2f(accelY, accelZ) * 180.0f / PI;
  pitch = atan2f(
            -accelX,
            sqrtf(accelY * accelY + accelZ * accelZ)
          ) * 180.0f / PI;
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
    computeAccelAngles();

    Serial.print("Roll: ");
    Serial.print(roll, 1);
    Serial.print(" deg  Pitch: ");
    Serial.print(pitch, 1);
    Serial.println(" deg");
  } else {
    Serial.println("Accelerometer read failed");
  }

  delay(100);
}

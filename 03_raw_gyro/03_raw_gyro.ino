/*
  GOAL
  Measure angular velocity in degrees per second.

  NEW IN THIS CHECKPOINT
  Read the gyroscope and subtract its startup bias.
  Accelerometer readings and tilt calculations remain available.

  UNLOCKED API
  See API.md in this folder.

  SUCCESS LOOKS LIKE
  Stationary rates stay near zero and react when the sensor rotates.

  THINK ABOUT
  - Why must the sensor remain still during calibration?
  - Is an angular rate the same thing as an angle?
*/
#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDRESS = 0x68;
const int CALIBRATION_SAMPLES = 500;

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float gyroBiasX, gyroBiasY, gyroBiasZ;
float accelRoll, accelPitch;

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

bool readGyro() {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x43);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(MPU_ADDRESS, (size_t)6, true) != 6) {
    return false;
  }

  int16_t rawX = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  int16_t rawY = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  int16_t rawZ = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());

  gyroX = rawX / 131.0f;
  gyroY = rawY / 131.0f;
  gyroZ = rawZ / 131.0f;
  return true;
}

void computeAccelAngles() {
  accelRoll = atan2f(accelY, accelZ) * 180.0f / PI;
  accelPitch = atan2f(
                 -accelX,
                 sqrtf(accelY * accelY + accelZ * accelZ)
               ) * 180.0f / PI;
}

bool calibrateGyro() {
  float sumX = 0;
  float sumY = 0;
  float sumZ = 0;

  Serial.println("Keep still: calibrating gyro...");

  for (int sample = 0; sample < CALIBRATION_SAMPLES; sample++) {
    if (!readGyro()) {
      return false;
    }
    sumX += gyroX;
    sumY += gyroY;
    sumZ += gyroZ;
    delay(3);
  }

  gyroBiasX = sumX / CALIBRATION_SAMPLES;
  gyroBiasY = sumY / CALIBRATION_SAMPLES;
  gyroBiasZ = sumZ / CALIBRATION_SAMPLES;
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(25, 26, 100000);

  if (!writeRegister(0x6B, 0x00) ||
      !writeRegister(0x1C, 0x00) ||
      !writeRegister(0x1B, 0x00) ||
      !calibrateGyro()) {
    Serial.println("MPU6050 setup or calibration failed");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  if (readAccel() && readGyro()) {
    computeAccelAngles();

    Serial.print("GX: ");
    Serial.print(gyroX - gyroBiasX, 2);
    Serial.print(" dps  GY: ");
    Serial.print(gyroY - gyroBiasY, 2);
    Serial.print(" dps  GZ: ");
    Serial.print(gyroZ - gyroBiasZ, 2);
    Serial.println(" dps");
  } else {
    Serial.println("Sensor read failed");
  }

  delay(100);
}

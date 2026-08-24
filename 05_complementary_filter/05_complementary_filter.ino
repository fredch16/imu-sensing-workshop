/*
  GOAL
  Combine stable accelerometer tilt with responsive gyro integration.

  NEW IN THIS CHECKPOINT
  Add accelerometer angles and a complementary filter.

  SUCCESS LOOKS LIKE
  Roll and pitch resist drift. Yaw still drifts because it is gyro-only.

  THINK ABOUT
  - What do the filter weights 0.98 and 0.02 control?
  - Why can gravity not correct yaw?
*/
#include <Arduino.h>
#include <Wire.h>
const uint8_t MPU_ADDRESS = 0x68;
const uint32_t SAMPLE_PERIOD_US = 5000;
const float GYRO_WEIGHT = 0.98f;

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float gyroBiasX, gyroBiasY, gyroBiasZ;
float accelRoll, accelPitch;
float roll, pitch, yaw;
float startRoll, startPitch;
uint32_t lastSampleUs;

bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
}

bool readAxes(uint8_t firstRegister, int16_t &x, int16_t &y, int16_t &z) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(firstRegister);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDRESS, (size_t)6, true) != 6) return false;

  x = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  y = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  z = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
  return true;
}

bool readAccel() {
  int16_t rawX, rawY, rawZ;
  if (!readAxes(0x3B, rawX, rawY, rawZ)) return false;
  accelX = rawX / 16384.0f;
  accelY = rawY / 16384.0f;
  accelZ = rawZ / 16384.0f;
  return true;
}

bool readGyro() {
  int16_t rawX, rawY, rawZ;
  if (!readAxes(0x43, rawX, rawY, rawZ)) return false;
  gyroX = rawX / 131.0f;
  gyroY = rawY / 131.0f;
  gyroZ = rawZ / 131.0f;
  return true;
}

void computeAccelAngles() {
  accelRoll = atan2f(accelY, accelZ) * 180.0f / PI;
  accelPitch = atan2f(-accelX, sqrtf(accelY * accelY + accelZ * accelZ)) * 180.0f / PI;
}

float wrapAngle(float angle) {
  while (angle > 180.0f) angle -= 360.0f;
  while (angle < -180.0f) angle += 360.0f;
  return angle;
}

bool calibrateGyro() {
  const int sampleCount = 500;
  float sumX = 0, sumY = 0, sumZ = 0;
  Serial.println("Keep still: calibrating gyro...");

  for (int sample = 0; sample < sampleCount; sample++) {
    if (!readGyro()) return false;
    sumX += gyroX;
    sumY += gyroY;
    sumZ += gyroZ;
    delay(3);
  }

  gyroBiasX = sumX / sampleCount;
  gyroBiasY = sumY / sampleCount;
  gyroBiasZ = sumZ / sampleCount;
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22, 100000);

  if (!writeRegister(0x6B, 0x00) ||
      !writeRegister(0x1C, 0x00) ||
      !writeRegister(0x1B, 0x00) ||
      !calibrateGyro() || !readAccel()) {
    Serial.println("Setup failed");
    while (true) delay(1000);
  }

  computeAccelAngles();
  startRoll = accelRoll;
  startPitch = accelPitch;
  lastSampleUs = micros();
}

void loop() {
  uint32_t now = micros();
  uint32_t elapsedUs = now - lastSampleUs;
  if (elapsedUs < SAMPLE_PERIOD_US) return;
  lastSampleUs = now;

  if (!readAccel() || !readGyro()) {
    Serial.println("Sensor read failed");
    return;
  }

  float dt = elapsedUs * 0.000001f;
  if (dt > 0.1f) return;

  computeAccelAngles();
  float measuredRoll = wrapAngle(accelRoll - startRoll);
  float measuredPitch = wrapAngle(accelPitch - startPitch);

  roll = wrapAngle(GYRO_WEIGHT * (roll + (gyroX - gyroBiasX) * dt) +
                   (1.0f - GYRO_WEIGHT) * measuredRoll);
  pitch = wrapAngle(GYRO_WEIGHT * (pitch + (gyroY - gyroBiasY) * dt) +
                    (1.0f - GYRO_WEIGHT) * measuredPitch);
  yaw = wrapAngle(yaw + (gyroZ - gyroBiasZ) * dt);

  static uint8_t printDivider = 0;
  if (++printDivider >= 20) {
    printDivider = 0;
    Serial.print("Roll: ");
    Serial.print(roll, 1);
    Serial.print("  Pitch: ");
    Serial.print(pitch, 1);
    Serial.print("  Yaw (drifts): ");
    Serial.println(yaw, 1);
  }
}

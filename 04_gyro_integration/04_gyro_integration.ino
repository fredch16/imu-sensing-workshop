/*
  GOAL
  Turn angular velocity into angles using: angle += rate * dt.

  NEW IN THIS CHECKPOINT
  Measure elapsed time and integrate all three gyro axes.

  SUCCESS LOOKS LIKE
  Angles follow rotations but slowly drift while the sensor is stationary.

  THINK ABOUT
  - Where does drift come from?
  - Why do we measure dt for every sample?
*/
#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDRESS = 0x68;
const uint32_t SAMPLE_PERIOD_US = 5000;  // 200 Hz
const int CALIBRATION_SAMPLES = 500;

float gyroX, gyroY, gyroZ;
float gyroBiasX, gyroBiasY, gyroBiasZ;
float roll, pitch, yaw;
uint32_t lastSampleUs;

bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
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

float wrapAngle(float angle) {
  while (angle > 180.0f) {
    angle -= 360.0f;
  }
  while (angle < -180.0f) {
    angle += 360.0f;
  }
  return angle;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22, 100000);

  if (!writeRegister(0x6B, 0x00) ||
      !writeRegister(0x1B, 0x00) ||
      !calibrateGyro()) {
    Serial.println("MPU6050 setup or calibration failed");
    while (true) {
      delay(1000);
    }
  }

  lastSampleUs = micros();
}

void loop() {
  uint32_t now = micros();
  uint32_t elapsedUs = now - lastSampleUs;

  if (elapsedUs < SAMPLE_PERIOD_US) {
    return;
  }
  lastSampleUs = now;

  if (!readGyro()) {
    Serial.println("Gyroscope read failed");
    return;
  }

  float dt = elapsedUs * 0.000001f;
  if (dt > 0.1f) {
    return;
  }

  roll = wrapAngle(roll + (gyroX - gyroBiasX) * dt);
  pitch = wrapAngle(pitch + (gyroY - gyroBiasY) * dt);
  yaw = wrapAngle(yaw + (gyroZ - gyroBiasZ) * dt);

  static uint8_t printDivider = 0;
  if (++printDivider >= 20) {
    printDivider = 0;
    Serial.print("Roll: ");
    Serial.print(roll, 1);
    Serial.print("  Pitch: ");
    Serial.print(pitch, 1);
    Serial.print("  Yaw: ");
    Serial.println(yaw, 1);
  }
}

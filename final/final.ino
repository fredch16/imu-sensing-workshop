#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDRESS = 0x68;
const int SDA_PIN = 25;
const int SCL_PIN = 26;
const uint32_t SAMPLE_PERIOD_US = 5000;  // 200 Hz
const float FILTER_GYRO_WEIGHT = 0.98f;

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

bool readRegisters(uint8_t firstRegister, uint8_t *data, size_t count) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(firstRegister);
  if (Wire.endTransmission(false) != 0) return false;

  size_t received = Wire.requestFrom(MPU_ADDRESS, count, true);
  if (received != count) {
    while (Wire.available()) Wire.read();
    return false;
  }
  for (size_t i = 0; i < count; i++) data[i] = Wire.read();
  return true;
}

int16_t signedWord(const uint8_t *data) {
  return (int16_t)((uint16_t(data[0]) << 8) | data[1]);
}

bool readAccel() {
  uint8_t data[6];
  if (!readRegisters(0x3B, data, sizeof(data))) return false;
  accelX = signedWord(data) / 16384.0f;
  accelY = signedWord(data + 2) / 16384.0f;
  accelZ = signedWord(data + 4) / 16384.0f;
  return true;
}

bool readGyro() {
  uint8_t data[6];
  if (!readRegisters(0x43, data, sizeof(data))) return false;
  gyroX = signedWord(data) / 131.0f;
  gyroY = signedWord(data + 2) / 131.0f;
  gyroZ = signedWord(data + 4) / 131.0f;
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
  Serial.println("Keep the sensor still: calibrating gyro...");
  for (int i = 0; i < sampleCount; i++) {
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

void stopWithError(const char *message) {
  Serial.println(message);
  while (true) delay(1000);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(SDA_PIN, SCL_PIN, 100000);

  if (!writeRegister(0x6B, 0x00) ||  // Wake sensor
      !writeRegister(0x1C, 0x00) ||  // Accelerometer: +/-2 g
      !writeRegister(0x1B, 0x00)) {  // Gyroscope: +/-250 deg/s
    stopWithError("MPU6050 setup failed. Check wiring and address.");
  }
  delay(100);
  if (!calibrateGyro()) stopWithError("Gyro calibration failed.");
  if (!readAccel()) stopWithError("Initial accelerometer read failed.");

  computeAccelAngles();
  startRoll = accelRoll;
  startPitch = accelPitch;
  roll = pitch = yaw = 0;
  lastSampleUs = micros();
}

void loop() {
  uint32_t now = micros();
  uint32_t elapsedUs = now - lastSampleUs;  // Also works across micros() rollover.
  if (elapsedUs < SAMPLE_PERIOD_US) return;
  lastSampleUs = now;

  if (!readAccel() || !readGyro()) {
    Serial.println("# MPU6050 read failed");  // Visualizer ignores non-data lines.
    return;
  }

  float dt = elapsedUs * 0.000001f;
  if (dt > 0.1f) return;  // Do not integrate a pause or serial/USB stall.
  gyroX -= gyroBiasX;
  gyroY -= gyroBiasY;
  gyroZ -= gyroBiasZ;
  computeAccelAngles();

  float measuredRoll = wrapAngle(accelRoll - startRoll);
  float measuredPitch = wrapAngle(accelPitch - startPitch);
  roll = FILTER_GYRO_WEIGHT * (roll + gyroX * dt) +
         (1.0f - FILTER_GYRO_WEIGHT) * measuredRoll;
  pitch = FILTER_GYRO_WEIGHT * (pitch + gyroY * dt) +
          (1.0f - FILTER_GYRO_WEIGHT) * measuredPitch;
  roll = wrapAngle(roll);
  pitch = wrapAngle(pitch);
  yaw = wrapAngle(yaw + gyroZ * dt);  // No magnetometer: yaw will drift.

  static uint8_t printDivider = 0;
  if (++printDivider >= 5) {  // 40 Hz output from the 200 Hz estimator.
    printDivider = 0;
    Serial.print(roll, 3);
    Serial.print('/');
    Serial.print(pitch, 3);
    Serial.print('/');
    Serial.println(yaw, 3);
  }
}

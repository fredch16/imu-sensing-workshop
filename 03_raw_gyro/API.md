# Level 03 API — Read and calibrate the gyro

## Mission

Read angular velocity in degrees per second and remove the sensor’s startup bias. Stationary rates should settle near zero.

Everything from [Level 02](../02_accel_angles/API.md) remains unlocked.

## New sensor data

Gyroscope data begins at register `0x43`. Request six bytes and combine them exactly as you did for acceleration.

At the configured ±250°/s range, convert each signed raw value with:

```cpp
float degreesPerSecond = raw / 131.0f;
```

## Unlocked: counted sampling

```cpp
for (int sample = 0; sample < sampleCount; sample++) {
  // Repeated measurement
}
```

A `for` loop has an initializer, a condition checked before each pass, and an update after each pass. It does not return a value.

## Bias calibration

The bias is the average stationary reading:

```cpp
bias = sum / sampleCount;
correctedRate = measuredRate - bias;
```

Use about `500` samples with `delay(3)` between them. Keep the sensor completely still. A useful signature is:

```cpp
bool calibrateGyro();
```

It takes no parameters and returns `true` after successful calibration or `false` if a sensor read fails.

## Build hint

Add `bool readGyro()` using the same structure as `readAccel()`. Calibrate once in `setup()`, but subtract the saved X/Y/Z biases whenever you display a rate.

Next: [Level 04 API](../04_gyro_integration/API.md)

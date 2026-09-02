# Level 04 API — Integrate angular velocity

## Mission

Turn gyro rates into roll, pitch, and yaw using `angle += rate * dt`. Rotate the sensor and observe that the angles respond but slowly drift.

Keep reading and calculating accelerometer tilt from [Level 02](../02_accel_angles/API.md); it will correct this drift in Level 05. Everything from [Level 03](../03_raw_gyro/API.md) remains unlocked.

## Unlocked: `micros()`

```cpp
uint32_t micros();
```

Returns the number of microseconds since the board started. It takes no parameters. The unsigned value eventually wraps around; unsigned subtraction still gives the correct elapsed time.

```cpp
uint32_t now = micros();
uint32_t elapsedUs = now - lastSampleUs;
lastSampleUs = now;
float dt = elapsedUs * 0.000001f;
```

`dt` is in seconds, which matches gyro rates measured in degrees per second.

## Integration

```cpp
angle = angle + correctedRate * dt;
```

This is a numerical approximation: it assumes the measured rate was roughly constant during `dt`. Small rate errors accumulate, which is why the result drifts.

## Unlocked: angle wrapping

A helper can keep display angles between −180° and +180°:

```cpp
float wrapAngle(float angle);
```

| Item | Meaning |
| --- | --- |
| Parameter | Any angle in degrees |
| Return | Equivalent angle in the range −180° to +180° |

## Build hint

Run the loop at a fixed minimum period, but calculate `dt` from actual elapsed time. Reject unusually large values such as `dt > 0.1f`, which can create a jump after a pause.

Next: [Level 05 API](../05_complementary_filter/API.md)

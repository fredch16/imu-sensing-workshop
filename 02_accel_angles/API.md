# Level 02 API — Turn gravity into angles

## Mission

Convert the acceleration vector into roll and pitch angles in degrees. The angles should follow tilt while the sensor is stationary.

Everything from [Level 01](../01_raw_accel/API.md) remains unlocked.

## Unlocked: `atan2f()`

```cpp
float atan2f(float y, float x);
```

Returns the angle of vector `(x, y)` in radians, in the range −π to +π. Unlike `atan(y / x)`, it preserves the quadrant and works when `x` is zero.

| Parameter | Meaning |
| --- | --- |
| `y` | Vertical component |
| `x` | Horizontal component |

## Unlocked: `sqrtf()` and `PI`

```cpp
float sqrtf(float value);
```

Returns the non-negative square root. `value` must not be negative. Arduino’s `PI` constant is π; multiply radians by `180.0f / PI` to get degrees.

## Tilt equations

```cpp
accelRoll = atan2f(accelY, accelZ) * 180.0f / PI;
accelPitch = atan2f(
  -accelX,
  sqrtf(accelY * accelY + accelZ * accelZ)
) * 180.0f / PI;
```

These equations use gravity as a reference. They cannot determine yaw, and linear acceleration can temporarily disturb them.

## Build hint

Put the equations in `void computeAccelAngles()`. Call it only after `readAccel()` succeeds, then print both results.

Next: [Level 03 API](../03_raw_gyro/API.md)

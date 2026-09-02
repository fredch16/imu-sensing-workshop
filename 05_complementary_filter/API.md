# Level 05 API — Fuse both sensors

## Mission

Combine responsive gyro integration with the accelerometer’s stable gravity reference. Roll and pitch should resist drift; yaw will still drift.

Everything from [Level 04](../04_gyro_integration/API.md) remains unlocked. This level introduces no new Arduino library call: the unlock is a way to combine the measurements you can already produce.

## Unlocked: output reference parameters

```cpp
bool readAxes(uint8_t firstRegister, int16_t &x, int16_t &y, int16_t &z);
```

| Parameter | Meaning |
| --- | --- |
| `firstRegister` | First of the six sensor-data registers |
| `x`, `y`, `z` | Variables that receive the decoded measurements |

Returns `true` after reading all three axes or `false` after an I2C error. The `&` means the function can update the caller’s variables. This helper removes duplicate I2C code from `readAccel()` and `readGyro()`.

## Unlocked: complementary filter

```cpp
filteredAngle = gyroWeight * (oldAngle + correctedRate * dt)
              + (1.0f - gyroWeight) * measuredAccelAngle;
```

| Term | Role |
| --- | --- |
| `gyroWeight` | Trust in short-term gyro motion; use `0.98f` |
| `1.0f - gyroWeight` | Slow correction toward gravity; here `0.02f` |
| `oldAngle + correctedRate * dt` | Responsive but drifting estimate |
| `measuredAccelAngle` | Stable but motion-sensitive reference |

At startup, save the accelerometer roll and pitch and subtract them from later measurements if the starting pose should be zero.

Yaw remains gyro-only because gravity contains no heading information.

## Build hint

Apply the filter independently to roll and pitch. Continue using gyro integration alone for yaw. Wrap each finished angle, not the individual weighted terms.

Next: [Level 06 API](../06_final_serial/API.md)

# Level 01 API — Read acceleration

## Mission

Read the three accelerometer axes and print them in g. When the stationary sensor lies flat, one axis should be close to +1 g or −1 g.

Everything from [Level 00](../00_starter/API.md) remains unlocked.

## Unlocked: I2C reads

### `size_t Wire.requestFrom(uint8_t address, size_t quantity, bool sendStop)`

Requests bytes from an I2C device.

| Parameter | Meaning |
| --- | --- |
| `address` | Device address: `0x68` |
| `quantity` | Number of bytes wanted: `6` for three 16-bit axes |
| `sendStop` | `true` ends the transaction after the read |

Returns the number of bytes actually received. Check that it equals `quantity` before reading.

### `int Wire.read()`

Removes and returns the next received byte as an integer from `0` to `255`. Returns `-1` if no byte is available.

## Unlocked: selecting a register before reading

Tell the sensor which register to start at, but keep the transaction open with `endTransmission(false)`. Then request the bytes:

```cpp
Wire.beginTransmission(0x68);
Wire.write(0x3B);
if (Wire.endTransmission(false) == 0 &&
    Wire.requestFrom(0x68, (size_t)6, true) == 6) {
  // Six bytes are ready.
}
```

Register `0x3B` is the first accelerometer data byte. The following five bytes complete X, Y, and Z.

## Unlocked: combining two bytes

Each measurement is a signed 16-bit number sent high byte first:

```cpp
int16_t raw = (int16_t)((uint16_t(Wire.read()) << 8) | Wire.read());
float accelerationG = raw / 16384.0f;
```

| Operation | Purpose |
| --- | --- |
| `uint16_t(...)` | Prevents sign problems while assembling bytes |
| `<< 8` | Moves the high byte into the upper eight bits |
| `\|` | Joins the low byte |
| `int16_t(...)` | Interprets the result as a signed value |
| `/ 16384.0f` | Converts ±2 g raw counts to g |

## Build hint

Create `bool readAccel()`. Return `false` for either I2C error and `true` only after updating `accelX`, `accelY`, and `accelZ`.

Next: [Level 02 API](../02_accel_angles/API.md)

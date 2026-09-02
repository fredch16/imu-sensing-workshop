# Level 00 API — Connect and configure

## Mission

Start Serial and I2C, wake the MPU6050, configure both sensor ranges, and print `MPU6050 is ready`.

Success means that exact message appears in Serial Monitor at 115200 baud.

## Unlocked: Arduino lifecycle and timing

### `void setup()`

Arduino calls this once after boot or reset. Put initialization here. It has no parameters and returns nothing.

### `void loop()`

Arduino calls this repeatedly after `setup()` finishes. It has no parameters and returns nothing. An empty loop is valid at this level.

### `void delay(uint32_t milliseconds)`

Pauses the program.

| Parameter | Meaning |
| --- | --- |
| `milliseconds` | Time to wait; `500` means half a second |

Return value: none.

## Unlocked: Serial

### `Serial.begin(uint32_t baudRate)`

Starts serial communication. Returns nothing. Use `115200`, and select the same rate in Serial Monitor.

### `Serial.print(value)` / `Serial.println(value)`

Writes a value. `println()` also ends the line. Returns the number of bytes written (`size_t`), which you can normally ignore.

```cpp
Serial.begin(115200);
Serial.println("Starting...");
```

## Unlocked: I2C writes

### `bool Wire.begin(int sda, int scl, uint32_t frequency)`

Starts the I2C controller.

| Parameter | This workshop |
| --- | --- |
| `sda` | `25` |
| `scl` | `26` |
| `frequency` | `100000` Hz |

Returns `true` when initialization succeeds.

### `void Wire.beginTransmission(uint8_t address)`

Starts building a message for one device. `address` is `0x68` for this MPU6050. Returns nothing.

### `size_t Wire.write(uint8_t value)`

Adds one byte to the outgoing message. Returns the number of bytes queued.

### `uint8_t Wire.endTransmission(bool sendStop = true)`

Sends the queued message. `sendStop` controls whether the I2C transaction ends. Returns `0` on success; any nonzero result is an error.

```cpp
Wire.beginTransmission(0x68);
Wire.write(0x6B);  // register
Wire.write(0x00);  // value
bool ok = Wire.endTransmission(true) == 0;
```

## Registers for this level

| Register | Value | Effect |
| --- | --- | --- |
| `0x6B` | `0x00` | Wake the MPU6050 |
| `0x1C` | `0x00` | Accelerometer range: ±2 g |
| `0x1B` | `0x00` | Gyroscope range: ±250°/s |

## Build hint

Make a helper `bool writeRegister(uint8_t reg, uint8_t value)`. Its return value lets `setup()` stop and report an error if any configuration write fails.

Next: [Level 01 API](../01_raw_accel/API.md)

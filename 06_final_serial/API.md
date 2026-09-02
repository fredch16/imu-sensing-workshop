# Level 06 API — Define the data protocol

## Mission

Send orientation to the Processing visualizer as one machine-readable record per line:

```text
roll/pitch/yaw
```

Everything from [Level 05](../05_complementary_filter/API.md) remains unlocked.

## Unlocked: formatted serial output

### `Serial.print(float value, int decimalPlaces)`

Writes a floating-point value using the requested number of digits after the decimal point. Returns the number of bytes written.

### `Serial.print(char separator)`

Writes one character. Use `'/'` between values.

### `Serial.println(float value, int decimalPlaces)`

Writes the final value and terminates the record with a newline. Returns the number of bytes written.

```cpp
Serial.print(roll, 3);
Serial.print('/');
Serial.print(pitch, 3);
Serial.print('/');
Serial.println(yaw, 3);
```

## Protocol contract

| Part | Requirement |
| --- | --- |
| Fields | Exactly three finite numbers |
| Order | Roll, pitch, yaw |
| Separator | `/` |
| Terminator | Newline from `println()` |
| Rate | About 40 records per second |

Diagnostic messages begin with `#`, allowing the receiver to distinguish them from data. The provided visualizer also ignores malformed records.

## Build hint

The sensor loop runs at 200 Hz. Count five successful loop passes before printing to produce about 40 records per second without slowing the filter.

You have now unlocked the complete workshop API. Compare your result with [`final/final.ino`](../final/final.ino), then run the Processing visualizer.

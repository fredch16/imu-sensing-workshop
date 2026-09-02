# ESP32 + MPU6050 Orientation Workshop

This workshop builds a register-level orientation tracker one idea at a time. An ESP32 reads an MPU6050, estimates roll and pitch with a complementary filter, integrates gyro-only yaw, and sends the result to a Processing 3D visualizer.

## Hardware and wiring

- ESP32 Dev Module
- MPU6050 breakout board
- Four jumper wires

```text
MPU6050    ESP32
VCC   ->   3V3
GND   ->   GND
SDA   ->   GPIO 25
SCL   ->   GPIO 26
```

The sketches use MPU6050 address `0x68`, I2C at 100 kHz, accelerometer range +/-2 g, gyro range +/-250 degrees/second, and serial at 115200 baud. Some breakout boards accept 5 V, but this workshop deliberately uses the ESP32's 3.3 V supply.

## Arduino IDE setup

1. Install Arduino IDE 2.x.
2. In Boards Manager, install **esp32 by Espressif Systems**.
3. Open the `.ino` inside the checkpoint folder you need.
4. Select **ESP32 Dev Module**, select its serial port, then upload.
5. When using Serial Monitor, set it to **115200 baud**.

Keep the sensor still and in the intended startup pose during the roughly 1.5-second gyro calibration. That pose becomes zero roll and pitch in the filtered sketches. Axis signs assume the MPU6050 board's printed X/Y/Z axes; reverse a sign in both the matching accelerometer angle and gyro rate if your physical mounting needs a different convention.

## How the workshop works

Build the project one level at a time. Each checkpoint folder contains two things:

- The `.ino` sketch is a working checkpoint you can compare against when you are stuck.
- `API.md` is the documentation unlocked for that level. It gives you the available functions, parameters, return values, and small usage patterns without giving away the complete solution.

Start with the mission in `00_starter/API.md`. Make your own attempt using only the APIs unlocked so far, upload it, and check the success condition. Move on when it works; open the checkpoint sketch only when you need a hint or want to catch up.

## Levels

| Level | Mission | New unlocks |
| --- | --- | --- |
| [`00_starter`](00_starter/API.md) | Wake and configure the sensor | Serial, timing, and I2C writes |
| [`01_raw_accel`](01_raw_accel/API.md) | Read acceleration in g | I2C reads and raw-byte conversion |
| [`02_accel_angles`](02_accel_angles/API.md) | Derive roll/pitch from gravity | `atan2f()`, `sqrtf()`, and `PI` |
| [`03_raw_gyro`](03_raw_gyro/API.md) | Read calibrated angular velocity | Sampling loops and bias calibration |
| [`04_gyro_integration`](04_gyro_integration/API.md) | Integrate rates and observe drift | `micros()` and elapsed-time integration |
| [`05_complementary_filter`](05_complementary_filter/API.md) | Stabilize roll/pitch with gravity | Reference parameters and sensor fusion |
| [`06_final_serial`](06_final_serial/API.md) | Stream data to the visualizer | The serial wire protocol |
| `final` | Polished reference implementation |

Yaw has no absolute correction: the MPU6050 has no magnetometer, so yaw is integrated from gyro Z and will drift over time. Accelerometer roll/pitch can also be disturbed by strong linear acceleration.

## Processing visualizer

1. Install Processing 4 and open `visualizer/visualizer.pde`.
2. Close Arduino Serial Monitor; only one application can normally own the serial port.
3. Run once to see the available ports in the Processing console.
4. If more than one port is listed, set `PORT_HINT` near the top of the sketch to a unique part of the ESP32 port name and run again.
5. Upload `06_final_serial` or `final`, then run the visualizer.

The receiver accepts exactly three finite numbers separated by `/` and terminated by a newline. Startup, error, partial, and malformed lines are ignored safely.

## Troubleshooting

- **Setup/read failed:** recheck power, common ground, SDA/SCL, and that the module uses address `0x68` (AD0 low).
- **Upload or Processing cannot open the port:** close the other program using it.
- **Angles move backward or around another axis:** check board orientation and adjust the documented axis convention.
- **Yaw slowly changes while still:** this is expected gyro drift, not an absolute compass heading.

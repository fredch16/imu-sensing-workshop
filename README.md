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
SDA   ->   GPIO 21
SCL   ->   GPIO 22
```

The sketches use MPU6050 address `0x68`, I2C at 100 kHz, accelerometer range +/-2 g, gyro range +/-250 degrees/second, and serial at 115200 baud. Some breakout boards accept 5 V, but this workshop deliberately uses the ESP32's 3.3 V supply.

## Arduino IDE setup

1. Install Arduino IDE 2.x.
2. In Boards Manager, install **esp32 by Espressif Systems**.
3. Open the `.ino` inside the checkpoint folder you need.
4. Select **ESP32 Dev Module**, select its serial port, then upload.
5. When using Serial Monitor, set it to **115200 baud**.

Keep the sensor still and in the intended startup pose during the roughly 1.5-second gyro calibration. That pose becomes zero roll and pitch in the filtered sketches. Axis signs assume the MPU6050 board's printed X/Y/Z axes; reverse a sign in both the matching accelerometer angle and gyro rate if your physical mounting needs a different convention.

## Checkpoints

| Folder | Result |
| --- | --- |
| `00_starter` | Wake and configure the sensor |
| `01_raw_accel` | Read acceleration in g |
| `02_accel_angles` | Derive absolute roll/pitch tilt from gravity |
| `03_raw_gyro` | Read calibrated angular velocity in deg/s |
| `04_gyro_integration` | Integrate rates and observe drift |
| `05_complementary_filter` | Stabilize roll/pitch with gravity |
| `06_final_serial` | Stream `roll/pitch/yaw` at about 40 Hz |
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

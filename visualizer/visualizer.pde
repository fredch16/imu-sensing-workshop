import processing.serial.*;

Serial imuPort;
float roll, pitch, yaw;

// Set this to part of your ESP32 port name, for example "cu.usbserial" or "COM5".
// Leave it empty to use the only available port. If several exist, the sketch lists
// them and stops so you can make an explicit, reliable choice.
final String PORT_HINT = "";

void setup() {
  size(1000, 700, P3D);
  String portName = chooseSerialPort(PORT_HINT);
  if (portName == null) {
    println("Set PORT_HINT near the top of visualizer.pde, then run again.");
    exit();
    return;
  }
  println("Opening " + portName + " at 115200 baud");
  imuPort = new Serial(this, portName, 115200);
  imuPort.clear();
  imuPort.bufferUntil('\n');
}

String chooseSerialPort(String hint) {
  String[] ports = Serial.list();
  println("Available serial ports:");
  printArray(ports);
  if (hint.length() > 0) {
    for (String port : ports) if (port.indexOf(hint) >= 0) return port;
    println("No port contains PORT_HINT: " + hint);
    return null;
  }
  return ports.length == 1 ? ports[0] : null;
}

void draw() {
  background(30);
  directionalLight(255, 255, 255, -1, 1, -1);
  ambientLight(100, 100, 100);
  pushMatrix();
  translate(width / 2, height / 2, 0);
  rotateY(radians(yaw));
  rotateX(radians(pitch));
  rotateZ(radians(roll));
  drawPlane();
  popMatrix();
  drawHUD();
}

void serialEvent(Serial port) {
  String line = port.readStringUntil('\n');
  if (line == null) return;
  String[] fields = split(trim(line), '/');
  if (fields.length != 3) return;
  try {
    float nextRoll = Float.parseFloat(trim(fields[0]));
    float nextPitch = Float.parseFloat(trim(fields[1]));
    float nextYaw = Float.parseFloat(trim(fields[2]));
    if (Float.isNaN(nextRoll) || Float.isInfinite(nextRoll) ||
        Float.isNaN(nextPitch) || Float.isInfinite(nextPitch) ||
        Float.isNaN(nextYaw) || Float.isInfinite(nextYaw)) return;
    roll = nextRoll;
    pitch = nextPitch;
    yaw = nextYaw;
  } catch (NumberFormatException error) {
    // Startup messages and incomplete/malformed lines are intentionally ignored.
  }
}

void drawHUD() {
  camera();
  hint(DISABLE_DEPTH_TEST);
  noLights();
  fill(255);
  textSize(28);
  text("Roll:  " + nf(roll, 1, 2) + " deg", 35, 50);
  text("Pitch: " + nf(pitch, 1, 2) + " deg", 35, 90);
  text("Yaw:   " + nf(yaw, 1, 2) + " deg (gyro-only; drifts)", 35, 130);
  hint(ENABLE_DEPTH_TEST);
}

void drawPlane() {
  noStroke();
  fill(200, 80, 80);
  box(50, 30, 280);
  pushMatrix();
  box(420, 8, 90);
  popMatrix();
  pushMatrix();
  translate(0, 0, 115);
  box(150, 7, 55);
  popMatrix();
  pushMatrix();
  translate(0, -30, 115);
  box(8, 65, 65);
  popMatrix();
  fill(100, 180, 255);
  pushMatrix();
  translate(0, -18, -70);
  box(35, 16, 65);
  popMatrix();
}

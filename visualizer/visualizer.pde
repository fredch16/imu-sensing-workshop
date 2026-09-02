import processing.serial.*;

Serial myPort;
Serial myPort;
float roll, pitch, yaw;

void setup() {
  fullScreen(P3D); // Fullscreen mode
  
  println(Serial.list());
  
  // Make sure this index matches your ESP32 port, easy way to check is to see which one you selected on arduino IDE!
  String portName = Serial.list()[1]; 
  myPort = new Serial(this, portName, 115200);
  myPort.bufferUntil('\n');
  fullScreen(P3D); // Fullscreen mode
  
  println(Serial.list());
  
  // Make sure this index matches your ESP32 port, easy way to check is to see which one you selected on arduino IDE!
  String portName = Serial.list()[1]; 
  myPort = new Serial(this, portName, 115200);
  myPort.bufferUntil('\n');
}

void draw() {
  background(30);
  
  // ----- 3D SCENE -----
  // Turn on lights for the 3D model
  
  // ----- 3D SCENE -----
  // Turn on lights for the 3D model
  directionalLight(255, 255, 255, -1, 1, -1);
  ambientLight(100, 100, 100);

  pushMatrix(); // Save the grid state
  translate(width/2, height/2, 0);
  
  // Scale everything by 2.0 (Twice as large!)
  scale(2.0); 

  // Apply rotations (Plane faces away, matched to hardware)
  rotateY(radians(yaw));      
  rotateX(radians(pitch));    
  rotateZ(radians(roll));     
  

  pushMatrix(); // Save the grid state
  translate(width/2, height/2, 0);
  
  // Scale everything by 2.0 (Twice as large!)
  scale(2.0); 

  // Apply rotations (Plane faces away, matched to hardware)
  rotateY(radians(yaw));      
  rotateX(radians(pitch));    
  rotateZ(radians(roll));     
  
  drawPlane();
  popMatrix(); // Restore the grid state
  
  // ----- 2D HUD -----
  popMatrix(); // Restore the grid state
  
  // ----- 2D HUD -----
  drawHUD();
}

void drawHUD() {
  // Reset the camera to a flat 2D view and turn off 3D depth/lights
  camera(); 
  // Reset the camera to a flat 2D view and turn off 3D depth/lights
  camera(); 
  hint(DISABLE_DEPTH_TEST);
  noLights(); 
  
  textSize(40); // Large font for fullscreen
  fill(255);    // White text
  
  // nf() formats the numbers so they don't jitter (1 digit left of decimal, 2 right)
  text("Roll  (X): " + nf(roll, 1, 2) + "°", 50, 80);
  text("Pitch (Y): " + nf(pitch, 1, 2) + "°", 50, 140);
  text("Yaw   (Z): " + nf(yaw, 1, 2) + "°", 50, 200);
  
  // Turn 3D depth testing back on for the next frame
  hint(ENABLE_DEPTH_TEST); 
  noLights(); 
  
  textSize(40); // Large font for fullscreen
  fill(255);    // White text
  
  // nf() formats the numbers so they don't jitter (1 digit left of decimal, 2 right)
  text("Roll  (X): " + nf(roll, 1, 2) + "°", 50, 80);
  text("Pitch (Y): " + nf(pitch, 1, 2) + "°", 50, 140);
  text("Yaw   (Z): " + nf(yaw, 1, 2) + "°", 50, 200);
  
  // Turn 3D depth testing back on for the next frame
  hint(ENABLE_DEPTH_TEST); 
}

void drawPlane() {
  noStroke();

  // ----- FUSELAGE -----

  // ----- FUSELAGE -----
  fill(200, 80, 80);
  pushMatrix();
  box(60, 40, 400); 
  popMatrix();
  
  // ----- COCKPIT -----
  fill(100, 180, 255);
  pushMatrix();
  box(60, 40, 400); 
  popMatrix();
  
  // ----- COCKPIT -----
  fill(100, 180, 255);
  pushMatrix();
  translate(0, -20, -100); 
  box(40, 20, 80);
  translate(0, -20, -100); 
  box(40, 20, 80);
  popMatrix();

  // ----- WINGS -----
  fill(200, 80, 80);

  // ----- WINGS -----
  fill(200, 80, 80);
  pushMatrix();
  box(600, 10, 120);
  box(600, 10, 120);
  popMatrix();

  // ----- HORIZONTAL TAIL -----

  // ----- HORIZONTAL TAIL -----
  pushMatrix();
  translate(0, 0, 170); 
  box(200, 8, 80);
  translate(0, 0, 170); 
  box(200, 8, 80);
  popMatrix();

  // ----- VERTICAL TAIL -----

  // ----- VERTICAL TAIL -----
  pushMatrix();
  translate(0, -30, 170); 
  box(10, 80, 100);
  translate(0, -30, 170); 
  box(10, 80, 100);
  popMatrix();
}

void serialEvent(Serial myPort) {
  String data = myPort.readStringUntil('\n');
  if (data != null) {
    data = trim(data);
    
    // Parses both formats automatically
    String[] parts = splitTokens(data, " /:XYZ");
    
    if (parts.length >= 3) {
      roll = float(parts[0]);
      pitch = float(parts[1]);
      yaw = float(parts[2]);
    }
  }
}
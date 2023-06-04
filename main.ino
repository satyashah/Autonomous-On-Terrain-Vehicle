#include "Servo.h"
#include "math.h"
#include "Enes100.h"
#include "definitions.h"
#include "vision.h"
#include "auxiliary.h"
#include "colorSensor.h"
#include "movement.h"
#include "navigation.h"
#include "mission.h"

void setup() {

  Serial.begin(9600);

  // Connect to vision system
  connect();
  Enes100.println("connected");
  initializePins();
  Enes100.println("initialized pins");
  initializeOTV();
  Enes100.println("initialized otv");
  
}

void loop() {
  goToMissionSite();

  
  turnTo((getClosestAngle()+180)%360);
  moveForward();
  delay(8000);
  turnTo(0);
  
  navigateObstacles();
  
}

// Vision function implementation
// Function to connect OTV to vision system
bool connect() {
  delay(2000);
  bool connection = Enes100.begin("HELLO", CRASH_SITE, 216, TX, RX);
  delay(2000);
  Enes100.updateLocation();

  // Enes100.println("Check Sequence: Aruca Marker [" + String(getX() > 0) + "] Distance Sensor [" + String(readDistanceSensor() > 0) + "]" );
  Serial.println(connection);
  return connection;
}

// Function to get Y position of OTV
float getY() {
  Enes100.updateLocation();
  if (Enes100.location.x < 0 || Enes100.location.x > 9){
    getY();
  }
  return Enes100.location.x;
}

// Function to get X position of OTV
float getX() {
  Enes100.updateLocation();
  if (Enes100.location.y < 0 || Enes100.location.y > 2){
    getX();
  }
  return Enes100.location.y;
}

// Function to get angle of the OTV in degrees
int getTheta() {
  
  Enes100.updateLocation();

  float degrees = Enes100.location.theta;

  if (degrees <= 0  && degrees >= -PI){
    degrees += 2*PI;
  }

  degrees = int(degrees*180/PI)%360;

  return degrees;
}

// Auxiliary function implementation
float sensorProtocol() {
  digitalWrite(TRIG, LOW);
  delay(2);

  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);

  float duration = pulseIn(ECHO, HIGH);
  float distance = duration / 29.0 / 2;

  return distance;
}

// Function to read the distance sensor value. Returns value in meters.
float readDistanceSensor() {
  
  delay(50);
  return min(sensorProtocol(), sensorProtocol());

}

// Function to turn distance servo left, right, or forward
void turnDistServo(String direction) {

  // Attach servo
  distServo.attach(distServoPin);

  // Based on input direction, turn servo to that corresponding direction
  if (direction == "LEFT") {
    distServo.write(18);
  }
  else if (direction == "RIGHT") {
    distServo.write(175);
  }
  else if (direction == "FORWARD") {
    distServo.write(90);
  }
  else {
    while (true) {
      distServo.write(0);
      delay(1000);
      distServo.write(180);
      delay(1000);
    }    
  }

  // Detach servo
  delay(1000);
  distServo.detach();

}

// Function to drop Aruca marker for Crash Site mission
void dropMarker() {
  
  // Attach servo
  markServo.attach(markServoPin);

  // Move servo to drop marker
  markServo.write(180);
  delay(1000);

  // Move servo back to starting position
  markServo.write(60);

  // Detach servo
  delay(1000);
  markServo.detach();

}

void initializePins() {

  // === Distance Sensor Setup
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // === Motor Driver Setup ===
  // Set all the motor control pins to outputs
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // === Color Sensor Setup ===
  // Set S0 - S3 as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Set LED as output
  pinMode(LED, OUTPUT);

  // Set Sensor output as input
  pinMode(OUT, INPUT);

}

// Color Sensor function implementation
// Function to read Red Pulse Widths
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  // Read the output Pulse Width
  int PW = pulseIn(OUT, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  // Read the output Pulse Width
  int PW = pulseIn(OUT, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  // Read the output Pulse Width
  int PW = pulseIn(OUT, LOW);
  // Return the value
  return PW;
}

// Turns color sensor LEDs on or off
void toggleColorSensor(bool on) {
  if(on) {
    // Set Frequency scaling to 20%
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    // Turns LED on
    digitalWrite(LED, HIGH);
  } else {
    // Turns LED off
    digitalWrite(LED, LOW);
  }
}

// Movement function implementation
void setRightMotorPWM(int PWMval) {
  if (PWMval > 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else if (PWMval < 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  analogWrite(ENA, abs(PWMval));
}

void setLeftMotorPWM(int PWMval) {
  if (PWMval > 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else if (PWMval < 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  analogWrite(ENB, abs(PWMval));
}

void moveForward() {
  delay(250);
  setLeftMotorPWM(251);//Account for right turn tendancy
  setRightMotorPWM(255);
}

void moveForward(int speed) {
  delay(250);
  setLeftMotorPWM(speed+40);
  setRightMotorPWM(speed);
}

void moveBackward() {
  setLeftMotorPWM(-255);
  setRightMotorPWM(-255);
}

void moveBackward(int speed) {
  setLeftMotorPWM(-speed-40);
  setRightMotorPWM(-speed);
}

void turnRight() {
  delay(250);
  setRightMotorPWM(-100);
  setLeftMotorPWM(100+50);
}

void turnLeft() {
  delay(250);
  setRightMotorPWM(100);
  setLeftMotorPWM(-100-50);
}

void stopMotors() {
  setLeftMotorPWM(0);
  setRightMotorPWM(0);
  delay(250);
}

// Navigation function implementation
// Function to determine the general direction the OTV is pointing
int getClosestAngle() {
  int curTheta = getTheta();
  if (curTheta >= 315 || curTheta < 45) {
    curTheta = 0;
  }
  else if (curTheta >= 45 && curTheta < 135) {
    curTheta = 90;
  }
  else if (curTheta >= 135 && curTheta < 225) {
    curTheta = 180;
  }
  else if (curTheta >= 225 && curTheta < 315) {
    curTheta = 270;
  }
  else {
    curTheta = 0;
  }

  return curTheta;
}

int getTurn() {

  // If facing forward, determine which side of arena OTC is on
  // If OTV is on right side, turn to 90 degrees, If OTV is on left side, turn to 270 degrees
  if (getClosestAngle() == 0) {
    if (getX() > 1) {
      return 270;
    }
    else {
      return 90;
    }
  }
  
  // If facing anywhere but forward, turn forward
  return 0;
  
}

// Function to realign the OTV with the angle it should be facing
void precisionAdjust() {

  //turn direction boolean, true means go right, false means go left
  bool direction;

  stopMotors();

  int curAngle = getTheta();

  int newAngle = getClosestAngle();
  
  if (curAngle < 180) {
    if (newAngle > curAngle && newAngle < curAngle + 180) {
      direction = false;
    }
    else {
      direction = true;
    }
  }

  else {
    if (newAngle > curAngle || newAngle < curAngle - 180) {
      direction = false;
    }
    else {
      direction = true;
    }
  }

  // Calculate a delay time factor based on how far off the OTV is from the desired angle
  int delayTimeFactor = min(abs(newAngle - curAngle), abs(abs(360 - newAngle) - curAngle));
  
  // Reason for turning so late is to ensure accurate timing
  if (direction) {
    turnRight();
  }   
  else {
    turnLeft();
  } 

  // Multiply delayTimeFactor by 48.8 because it takes about 48.8 ms for the OTV to turn 1 degree at normal turn speed.
  delay(delayTimeFactor*25);
  stopMotors();
    
}

void turnTo(int newAngle) {

  stopMotors();
  
  float tol = 0.05;
  int curAngle = getTheta();

  
  // Draw a circle and use some test cases to understand this logic circuit
  if (curAngle < 180) {
    if (newAngle > curAngle && newAngle < curAngle + 180) {
      turnLeft();   
    }
    else {
      turnRight();     
    }
  }

  else {
    if (newAngle > curAngle || newAngle < curAngle - 180) {
      turnLeft();
    }
    else {
      turnRight();
    }
  }

  while (abs(curAngle - newAngle) > 180 * tol) {
    if (newAngle == 0 && curAngle >= 180){
      newAngle = 360;
    }
    if (newAngle == 360 && curAngle < 180) {
      newAngle = 0;
    }
    curAngle = getTheta();
  }

  stopMotors();

  precisionAdjust();  
  
}

// Function to realign the OTV if it begins to deviate from target angle
void safetyCheck() {
  int closestTheta = getClosestAngle();
  int diff = min(abs(closestTheta - getTheta()), abs(abs(360 - closestTheta) + getTheta()));
  if (abs(diff) > 15) {
    turnTo(closestTheta);
    moveForward();
  }
}

void checkCenter(bool& checkMid) {
  
  float tol = 0.05;
  
  if (checkMid) {
    // If middle has not been checked yet and the OTV is in the middle, check if there is an obstacle in the way
    if ((getX() < 1 + 2*tol) && (getX() > 1 - 2*tol)) {
      stopMotors();      
      turnTo(0);

      // If there is an obstacle within 25 cm, disregard it and turn away and continue path normally
      if (readDistanceSensor() < 25) {
        turnTo(getTurn());
      }
      else {
        checkMid = false;
      }
      moveForward();
      
    }    

  }
    
}

void initiateEndSequence() {
  
  // Only initate the end sequence if past the obstacles

  // If not centered on the limbo, position OTV to be centered on the limbo
  if (getX() < 1.5) {
    turnTo(90);
    moveForward();
    while (getX() < 1.5) {
      safetyCheck();
    }
    turnTo(0);
  }
  else if (getX() > 1.6) {
    turnTo(270);
    moveForward();
    while (getX() > 1.5) {
      safetyCheck();
    }
    turnTo(0);
  }

  // Move towards limbo and once OTV is past the limbo, stop
  moveForward();
  while (getY() < 3.6) {
    safetyCheck();
  }
  stopMotors();
  delay(1000);
  while (true) {
    turnRight();
    delay(250);
    turnLeft();
    delay(250);
  }
}

// Function to begin navigating the obstacles
void navigateObstacles() {

  // Face forward and begin moving
  turnTo(0);
  turnDistServo("FORWARD");
  moveForward();
  bool checkMid = true;
  bool complete = false;
  
  while (!complete) {

    safetyCheck();
    // If OTV runs into an obstacle within 20cm, turn away from it and move forward
    if (readDistanceSensor() < 15) {
      turnTo(getTurn());
      moveForward();
    }

    // If OTV has not yet checked the middle at any point, run the checkCenter function
    checkCenter(checkMid);

    // Once OTV is past the obstacles, break the loop
    if (getY() > 2.8) {
      complete = true;
    }

  }

  initiateEndSequence();

}

// Specialized turning function to pivot about the left wheel
void pivotTurnLeft() {

  stopMotors();
  int curAngle = getClosestAngle();
  int newAngle;
  
  // If OTV is to turn left, add 90 degrees to the current angle
  newAngle = (curAngle + 90) % (360);

  // Turn to calculated angle, then adjust to be centered
  stopMotors();
  
  float tol = 0.05;
  curAngle = getTheta();
  
  // Draw a circle and use some test cases to understand this logic circuit
  setRightMotorPWM(100);
  setLeftMotorPWM(0);

  int test = getTheta();
  while (abs(test - newAngle) > 180 * tol) {
    test = getTheta();
    delay(100);
  }

  stopMotors();

  precisionAdjust();  
  
}

// Specialized turning function to pivot about the right wheel
void pivotTurnRight() {

  stopMotors();
  int curAngle = getClosestAngle();
  int newAngle;
  
  // If OTV is to turn right, remove 90 degrees to the current angle
  newAngle = (curAngle + 270) % (360);

  // Turn to calculated angle, then adjust to be centered
  stopMotors();
  
  float tol = 0.05;
  curAngle = getTheta();
  
  // Draw a circle and use some test cases to understand this logic circuit
  setRightMotorPWM(0);
  setLeftMotorPWM(100);

  int test = getTheta();
  while (abs(test - newAngle) > 180 * tol) {
    test = getTheta();
    delay(100);
  }

  stopMotors();

  precisionAdjust();  
  
}

// Mission code implementation
float gatherRedStrength() {

  toggleColorSensor(true);

  float r = 0, g = 0, b = 0;
  for (int k = 0; k < 5; k++) {
    r += getRedPW();
    delay(100);
    g += getGreenPW();
    delay(100);
    b += getBluePW();
    delay(100);    
  }

  toggleColorSensor(false);
    Enes100.println( r);

  Enes100.println(( g + b ) / r);
  return (( g + b ) / r );
}

void goToMissionSite() {

  // Turn to site
  turnTo(getTurn());

  precisionAdjust();
  precisionAdjust();
  precisionAdjust();

  // Move towards site
  moveForward();

  // Keep moving torwards site until ~5cm away
  while(readDistanceSensor() >= 10) {}
  stopMotors();
  
}

int measureSide(int sideNum, bool& firstSide, float* sideLengths) {
  
  float pos1, pos2, clearDist = 0.28;
  int redIndex = -1;
  
  // Move OTV out of the box's way
  moveBackward();
  if (firstSide) {
    delay(3000);
    firstSide = false;
  }
  else {
    delay(750);
  }
  
  float testDist = readDistanceSensor();
  while (testDist < 50) {
    testDist = readDistanceSensor();
  }
  stopMotors();
  delay(250);

  // Move OTV until distance sensor detects the first edge of the side
  moveForward(100);
  while (readDistanceSensor() > 50) {}
  /*if (firstSide) {
    delay(1000);
    firstSide = false;
  }*/
  stopMotors();
  delay(250);
  precisionAdjust();
  

  // If OTV is facing forward or backward, measure Y, otherwise measure X
  if (getClosestAngle() == 180 || getClosestAngle() == 0) {
    pos1 = getY();
  }
  else {
    pos1 = getX();
  }

  // Move OTV forward until end of box is detected
  moveForward(100);
  while (readDistanceSensor() < 50) {}
  stopMotors();
  precisionAdjust();

  moveBackward(100);
  while (readDistanceSensor() > 50) {}
  stopMotors();
  precisionAdjust();

  // If OTV is facing forward or backward, measure Y, otherwise measure X
  if (getClosestAngle() == 180 || getClosestAngle() == 0) {
    pos2 = getY();
  }
  else {
    pos2 = getX();
  }

  sideLengths[sideNum] = abs(pos2-pos1);

  // Check color of side  

  if (sideLengths[sideNum] > 0.22) {
    moveBackward();
    delay(500);
    stopMotors();
  }

  if (gatherRedStrength() > 4.2) {
    redIndex = sideNum;
    dropMarker();
    int redAngle = getClosestAngle();    
    if (redAngle == 0) {
      Enes100.mission(DIRECTION, NEG_Y);
    }
    else if (redAngle == 90) {
      Enes100.mission(DIRECTION, POS_X);
    }
    else if (redAngle == 180) {
      Enes100.mission(DIRECTION, POS_Y);
    }
    else {
      Enes100.mission(DIRECTION, NEG_X);
    }
  }

  moveForward(100);

  // If OTV is facing forward or backward, measure Y, otherwise measure X
  if (getClosestAngle() == 180 || getClosestAngle() == 0) {
    while(abs(getY() - pos2) < clearDist) {}
  }
  else {
    while(abs(getX() - pos2) < clearDist) {}
  }

  stopMotors();
  precisionAdjust();
  delay(500);

  return redIndex;
  

}

void conductMission() {  
  
  // Turn OTV so color sensor faces crash site
  pivotTurnRight();
  delay(500);
  
  // Point distance sensor towards crash site
  turnDistServo("LEFT");
  delay(1000);
    
  // Store sideLengths in array
  float sideLengths[4] = {0, 0, 0, 0};

  // Traverse each side and store the length of each side
  bool firstSide = true;
  int redIndex = -1;  
  for (int i = 0; i < 4; i++) {
    redIndex = measureSide(i, firstSide, sideLengths);
    Enes100.println("Side " + String(i) + ": " + String(sideLengths[i]) + "m");
    if (i < 3) {
      pivotTurnLeft();
    }
  }

  getRedDims(redIndex, sideLengths);
  turnDistServo("FORWARD");
  moveForward();
  delay(8000);
  stopMotors();
  
}




// Everything from here and below is for the crash site object class


float* getRedDims(int redSideIndex, float* sideLengths) {
  float trueDims[3] = { 0.270, 0.180, 0.135 };

  float side13 = abs(sideLengths[0] - sideLengths[2])/2;
  float side24 = abs(sideLengths[1] - sideLengths[3])/2;
  

  if (side13 > .22){
    side13 = .270;
  } else if (side13 > .15) {
    side13 = .180;
  } else {
    side13 = .135;
  }

  if (side24 > .22){
    side24 = .270;
  } else if (side13 > .15) {
    side24 = .180;
  } else {
    side24 = .135;
  }

  float height = 0;

  if ((side13 == .270 && side24 == .180) || (side13 == .180 && side24 == .270)){
    height = .135;
  } else if ((side13 == .135 && side24 == .180) || (side13 == .180 && side24 == .135)){
    height = .270;
  } else {
    height = .180;
  }
  if (redSideIndex == 0 || redSideIndex == 2){
    Enes100.mission(LENGTH, side13*100);
    Enes100.mission(HEIGHT, height*100);
  } else {
    Enes100.mission(LENGTH, side24*100);
    Enes100.mission(HEIGHT, height*100);
  }

  
}

// Function to initialized OTV so it is ready to do the mission and navigate the obstacle course
void initializeOTV() {
  
  turnDistServo("FORWARD");
  markServo.attach(markServoPin);
  markServo.write(60);
  delay(1500);
  markServo.detach();
  turnTo(0);

}
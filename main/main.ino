// === Library Definitions ===
#include "Enes100.h"
#include "math.h"

#include <Servo.h>
Servo myservo;
// === Pin Definitions ===
// -- Motor Driver --
// Motor A connections
#define ENA 9
#define IN1 8
#define IN2 7
// Motor B connections
#define ENB 4
#define IN3 6
#define IN4 5

// -- Color Sensor --
#define S0 39
#define S1 37
#define S2 35
#define S3 33
#define OUT 31
#define LED 41

// --- Distance Sensor ---
#define TRIG 11
#define ECHO 12

// --- Communication ---
#define RX 52
#define TX 14

// === Variable Definitions ===
// -- RGB Pulse Width --
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// -- Navigation --
float d1;
float d2;
float minDistance = 300;

// -- Position and Angle --
float y;
float x;
float theta;

float tolarance = .05;
bool checkMiddle = true;

// -- Distance sensing variables --
long duration;
float distance;

void setup() {
  //Connect Distance Sensor

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

  // Set Frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  // Turns LED on
  digitalWrite(LED, HIGH);


  //Enes100.begin("CowPlowers", CRASH_SITE, 11, 52, 50);

  //Enes100.println("hello world");
  delay(2000);
  Enes100.begin("CowPlowers", CRASH_SITE, 10, TX, RX);
  delay(2000);
  Serial.begin(9600);
  Enes100.println("CONNECTED");
  myservo.attach(32);
}
void oloop() {
  //moveForward();

  // myservo.write(90);
  // moveForward();
  // Serial.println("X");
  // Serial.println(getX());
  // Serial.println("Theta");
  // Serial.println(getTheta());
  Serial.println(readDistanceSensor());
  delay(100);
  // Serial.print("TURNING 180");
  // turnTo(180);
  // // moveForward();
  // delay(1000);
  // Enes100.print("TURNING 0");
  // Serial.print("TURNING 0");
  // turnTo(0);
}
void loop() {

  delay(100);

  //Getting Locations
  // Enes100.println("Y: " + String(getY()));
  // Enes100.println("X: " + String(getX()));
  // Enes100.println("Theta: " + String(getTheta()));

  // int distSensor = readDistanceSensor();
  d1 = readDistanceSensor();
  d2 = readDistanceSensor();

  // Enes100.println("D1: " + String(d1));
  // Enes100.println("D2: " + String(d2));

  if (checkMiddle && getX() < 1 * (1 + tolarance) && getX() > 1 * (1 - tolarance)) {
    Enes100.println("Checking Middle...");
    float oldAngle = getTheta();
    turnTo(0);
    d1 = readDistanceSensor();
    if (d1 < minDistance) {
      Enes100.println("Turning Back: " + String(oldAngle));
      turnTo(oldAngle);
      moveForward();
    } else {
      Enes100.println("Moving Forward");
      moveForward();
    }

    checkMiddle = false;
    d1 = readDistanceSensor();

    Enes100.println("Check Middle Flag: " + checkMiddle);
    Enes100.println("End Checking Middle");
  }

  if (d1 < minDistance) {
    Enes100.println("Turning - D1: " + String(d1));
    stopMotors();
    delay(1000);

    float turnAngle = getTurn();
    turnTo(turnAngle);
    moveForward();
    Enes100.println("END TURNING");
  }

  if (getY() > 2.6) {
    Enes100.println("-------------XXX END SEQUENCE INITIATED XXX-------------");
    turnTo(PI / 2);
    moveForward();
    while (getX() < 1.5) {}

    turnTo(0);
    moveForward();
    while (getY() < 3.6) {}


    stopMotors();
    while (true) {}
  }
}

/* ==================================
 * Motor Driver Functions
 * ================================== */

void setBothMotorsPWM(int PWMval) {
  setRightMotorPWM(PWMval);
  setLeftMotorPWM(PWMval);
}

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

/* ==================================
 * Navigation Functions
 * ================================== */

float readDistanceSensor() {
  digitalWrite(TRIG, LOW);
  delay(2);

  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration / 29 / 2;
  ;

  return distance;
}

float getTurn() {
  float curAngle = getTheta();
  float curX = getX();

  if (abs(curAngle) < PI / 4) {
    // Facing Forward
    Enes100.print("FACING FORWARD: OTV ON");
    if (curX > 1) {
      Enes100.println(" LEFT SIDE");
      return -PI / 2;
    } else {
      Enes100.println(" RIGHT SIDE");
      return PI / 2;
    }
  } else if (curAngle > PI / 4 && curAngle < 3 * PI / 4) {
    //Facing Left
    Enes100.println("FACING LEFT");
    return 0;
  } else if (curAngle < -PI / 4 && curAngle > -3 * PI / 4) {
    //Facing Right
    Enes100.println("FACING RIGHT");
    return 0;
  } else {
    // Facing Backward
    Enes100.println("FACING BACKWARD");
    return 0;
  }
}

void turnTo(float theta) {
  float curAngle = getTheta();
  Serial.print("CurAngle: " + String(curAngle));
  Enes100.print("CurAngle: " + String(curAngle));
  stopMotors();

  if (theta - curAngle > 0) {
    //Turn Left
    setLeftMotorPWM(-100);
    setRightMotorPWM(100);
  } else {
    //Turn Right
    setLeftMotorPWM(100);
    setRightMotorPWM(-100);
  }

  Enes100.println("Current Angle: " + String(curAngle) + ", Difference: " + String(abs(theta - curAngle)) + ", Turning To: " + String(theta));

  while (abs(theta - curAngle) > PI * tolarance) {
    curAngle = getTheta();
    Enes100.println("       Current/Target: " + String(curAngle) + "/" + String(theta));
  }

  stopMotors();
}

void stopMotors() {
  setLeftMotorPWM(0);
  setRightMotorPWM(0);
}

void moveForward() {
  setLeftMotorPWM(255);
  setRightMotorPWM(255);
}

float getY() {
  Enes100.updateLocation();
  if (Enes100.location.x < 0 || Enes100.location.x > 9){
    getY();
  }
  return Enes100.location.x;
}

float getX() {
  Enes100.updateLocation();
  if (Enes100.location.y < 0 || Enes100.location.y > 2){
    getX();
  }
  return Enes100.location.y;
}

float getTheta() {
  Enes100.updateLocation();
  if (abs(Enes100.location.theta) > PI || abs(Enes100.location.theta) == 0){//CHEKC THIS LATER ==0 to account for errors
    getTheta();
  }
  return Enes100.location.theta;
}

/* ==================================
 * Color Sensor Functions
 * ================================== */
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

// Function to read RGB Pulse Widths
void getColors() {
  // Read Red value
  redPW = getRedPW();
  delay(200);

  // Read Green value
  greenPW = getGreenPW();
  delay(200);

  // Read Blue value
  bluePW = getBluePW();
  delay(200);
}

// Checks if Red for mission objective
bool isRed() {
  bool ret;

  // Get Red PW, Green PW, Blue PW
  getColors();

  // Conditions for isRed: if redPW is at least 70 PW greenPW and bluePW
  if (bluePW - redPW > 70 && greenPW - redPW > 70) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;
}

#include "Arduino.h"
#include "Servo.h"

// PIN DEFINITIONS
// Motor A connections
#define ENA 9
#define IN1 8
#define IN2 7
// Motor B connections
#define ENB 4
#define IN3 6
#define IN4 5

//Distance Sensor
#define TRIG 13
#define ECHO 12

//Dist Sensor Servo
#define distServoPin 11

//Marker Sensor Servo
#define markServoPin 10

// -- Color Sensor --
#define S0 31
#define S1 33
#define S2 35
#define S3 37
#define OUT 39
#define LED 47

// -- Connection --
#define RX 53
#define TX 52

// The only two objects needed to be global throughout the program -- servo objects for the distance sensor servo and the marker servo
Servo distServo;
Servo markServo;
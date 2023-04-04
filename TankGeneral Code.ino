#include "Tank.h"
#include "Enes100.h"
#include "math.h"

float d1;
float d2;
float minDistance = .3;

float y;
float x;
float theta;

float tolarance = .05;
bool checkMiddle = true;
// const float PI = 3.14159



void setup() {
  // put your setup code here, to run once:
  Tank.begin();

  Enes100.begin("CowPlowers", CRASH_SITE, 11, 52, 50);
  
  Enes100.println("hello world");

  turnTo(0);
  moveForward();
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(100);

  //Getting Locations
  // Enes100.println("Y: " + String(getY()));
  // Enes100.println("X: " + String(getX()));
  // Enes100.println("Theta: " + String(getTheta()));


  // int distSensor = Tank.readDistanceSensor(1);
  d1 = Tank.readDistanceSensor(1);
  d2 = Tank.readDistanceSensor(7);

  // Enes100.println("D1: " + String(d1));
  // Enes100.println("D2: " + String(d2));

  if (checkMiddle && getX() < 1*(1+tolarance) && getX() > 1*(1-tolarance)){
    Enes100.println("Checking Middle...");
    float oldAngle = getTheta();
    turnTo(0);
    d1 = Tank.readDistanceSensor(1);
    if (d1 < minDistance){
      Enes100.println("Turning Back: " + String(oldAngle));      
      turnTo(oldAngle);
      moveForward();   
    }
    else {
      Enes100.println("Moving Forward");  
      moveForward();      
    }

    checkMiddle = false;
    d1 = Tank.readDistanceSensor(1);

    Enes100.println("Check Middle Flag: " + checkMiddle);
    Enes100.println("End Checking Middle");
  }

  if (d1 < minDistance){
    Enes100.println("Turning - D1: " + String(d1));
    stopMotors();
    delay(1000);

    float turnAngle = getTurn();
    turnTo(turnAngle);
    moveForward();
    Enes100.println("END TURNING");

  }

  if (getY() > 2.6){
    Enes100.println("-------------XXX END SEQUENCE INITIATED XXX-------------");
    turnTo(PI/2);
    moveForward();
    while (getX() < 1.5){}

    turnTo(0);
    moveForward();
    while (getY() < 3.6){}


    stopMotors();
    while (true){}
  }
  
}


float getTurn(){
  float curAngle = getTheta();
  float curX = getX();

  if (abs(curAngle) < PI/4){
    // Facing Forward
    Enes100.print("FACING FORWARD: OTV ON");
    if (curX > 1){
      Enes100.println(" LEFT SIDE");
      return -PI/2;
    }
    else{
      Enes100.println(" RIGHT SIDE");
      return PI/2;
    }    
  }
  else if (curAngle > PI/4 && curAngle < 3*PI/4){
    //Facing Left
    Enes100.println("FACING LEFT");
    return 0;
  }
  else if (curAngle < -PI/4 && curAngle > -3*PI/4){
    //Facing Right
    Enes100.println("FACING RIGHT");
    return 0;
  }
  else{
    // Facing Backward
    Enes100.println("FACING BACKWARD");
    return 0;
  }
}

void turnTo(float theta){
  float curAngle = getTheta();
  stopMotors();

  if (theta - curAngle > 0){
    //Turn Left
    Tank.setLeftMotorPWM(-100);
    Tank.setRightMotorPWM(100);
  }
  else {
    //Turn Right
    Tank.setLeftMotorPWM(100);
    Tank.setRightMotorPWM(-100);
  }

  Enes100.println("Current Angle: " + String(curAngle) + ", Difference: " + String(abs(theta - curAngle)) + ", Turning To: " + String(theta));

  while (abs(theta - curAngle) > PI*tolarance){
    curAngle = getTheta();
    Enes100.println("       Current/Target: " + String(curAngle) + "/" + String(theta));
  }

  stopMotors();
}

void stopMotors(){
  Tank.setLeftMotorPWM(0);
  Tank.setRightMotorPWM(0);
}

void moveForward(){
  Tank.setLeftMotorPWM(255);
  Tank.setRightMotorPWM(255);
}

float getY(){
  Enes100.updateLocation();
  if (Enes100.location.x < 0 || Enes100.location.x > 9){
    getY();
  }
  return Enes100.location.x;
}

float getX(){
  Enes100.updateLocation();
  if (Enes100.location.y < 0 || Enes100.location.y > 2){
    getX();
  }
  return Enes100.location.y;
}

float getTheta(){
  Enes100.updateLocation();
  if (abs(Enes100.location.theta) > PI || abs(Enes100.location.theta) == 0){//CHEKC THIS LATER ==0 to account for errors
    getTheta();
  }
  return Enes100.location.theta;
}

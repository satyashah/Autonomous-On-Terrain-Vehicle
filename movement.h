// Movement
void setRightMotorPWM(int); // Arduino-communicating function to set right motor PWM
void setLeftMotorPWM(int); // Arduino-communicating function to set left motor PWM
void moveForward(void); // Move OTV forward at maximum speed
void moveForward(int); // Move OTC forward at a desired speed
void moveBackward(void); // Move OTV backward at maximum speed
void moveBackward(int); // Move OTV backward at desired speed
void stopMotors(void); // Stop both motors
void turnRight(void); // Turn OTV right
void turnLeft(void); // Turn OTV left
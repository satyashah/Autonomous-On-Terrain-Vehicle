#include "Enes100.h"
bool connection;
bool updated;
int xPos;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  connection = Enes100.begin("CowPlowers", CRASH_SITE, 10, 50, 52);
  Serial.println(connection);
  
  Enes100.println("hello world");

}

void loop() {
  // put your main code here, to run repeatedly:
  updated = Enes100.updateLocation();
  delay(1000);
  Enes100.println("Y: " + String(Enes100.location.y));
  Enes100.println("X: " + String(Enes100.location.x));
  Enes100.println("Theta: " + String(Enes100.location.theta));
}

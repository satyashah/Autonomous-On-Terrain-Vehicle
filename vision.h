// Vision system
bool connect(void); // Connect OTV to overhead vision system
float getY(void); // Grab OTV Y position
float getX(void); // Grab OTV X position
int getTheta(void); // Grab OTV ANGLE position
void transmit(String); // Send message to ENES100 vision system
void transmitSide(int, float, float); // Transmit the direction and dimensions of the red side to ENES100 vision system
// Navigation
int getClosestAngle(void); // Depending on the direction the OTV is facing, returns the nearest cardinal direction angle
int getTurn(void); // Grabs the most ideal turn for the OTV to make when navigating the obstacles
void precisionAdjust(void); // Adjusts the position of the OTV to be more aligned with the nearest cardinal direction angle
void turnTo(int); // Turns the OTV to a new desired angle
void safetyCheck(void); // Specialized function to realign the OTV when it begins to deviate from the desired angle
void navigateObstacles(void); // Function to navigate the main bulk of obstacles
void initiateEndSequence(void); // Function to navigate the final portion of the obstacle course since it is not randomized
void checkCenter(bool&); // Function to check if there are obstacles in the center path of the arena
void pivotTurnLeft(void); // Specialized turn to turn left by pivoting about the left wheel by 90 degrees
void pivotTurnRight(void); // Specialized turn to turn right by pivoting about the right wheel by 90 degrees

// Mission
void goToMissionSite(void); // Navigate to mission site
int measureSide(int, bool&, float*); // Perform all measurements of a side on the crash site (red strength, length, and the direction)
void conductMission(void); // Perform measure side on all four sides
float gatherRedStrength(void); // Gather and average the red strength of a particular side
float* getRedDims(int); // Returns an array of length 2 with the length and height of the red side

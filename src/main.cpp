#include "MicromouseSolver.h"

MicromouseSolver solver;

void setup() {
    // Initialize the solver
    solver.begin();
    
    // Uncomment these for testing individual components:
    // solver.testSensors();
    // solver.testMotors();
    // solver.calibrate();
}

void loop() {
    // Main solving loop
    solver.run();
}

#ifndef MICROMOUSE_SOLVER_H
#define MICROMOUSE_SOLVER_H

#include <Arduino.h>
#include "config.h"
#include "SensorManager.h"
#include "MotorController.h"
#include "Maze.h"

enum RobotState {
    IDLE,
    EXPLORING,
    RETURNING_TO_START,
    FAST_RUN,
    FINISHED
};

class MicromouseSolver {
private:
    SensorManager sensors;
    MotorController motors;
    Maze maze;
    
    RobotState state;
    unsigned long startTime;
    int explorationRuns;
    
    void exploreCell();
    void moveToNextCell(int direction);
    bool checkAndReturnToStart();
    void executeFastRun();

public:
    MicromouseSolver();
    void begin();
    void run();
    
    // Manual control for testing
    void testSensors();
    void testMotors();
    void calibrate();
};

#endif

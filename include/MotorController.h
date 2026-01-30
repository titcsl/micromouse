#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    STOP
};

class MotorController {
private:
    int currentLeftSpeed;
    int currentRightSpeed;
    bool isMoving;
    
    void setMotorA(int speed);
    void setMotorB(int speed);

public:
    MotorController();
    void begin();
    
    // Basic movement
    void move(Direction dir, int speed = BASE_SPEED);
    void setSpeed(int leftSpeed, int rightSpeed);
    void stop();
    void brake();
    
    // Timed movements
    void moveForward(int duration, int speed = BASE_SPEED);
    void turnLeft90();
    void turnRight90();
    void turnAround();
    
    // Wall following with correction
    void moveForwardWithCorrection(int leftDist, int rightDist);
    
    // Status
    bool getIsMoving() { return isMoving; }
};

#endif

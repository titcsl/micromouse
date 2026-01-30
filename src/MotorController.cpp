#include "MotorController.h"

MotorController::MotorController() {
    currentLeftSpeed = 0;
    currentRightSpeed = 0;
    isMoving = false;
}

void MotorController::begin() {
    // setup pins
    pinMode(MOTOR_AIN1, OUTPUT);
    pinMode(MOTOR_AIN2, OUTPUT);
    pinMode(MOTOR_BIN1, OUTPUT);
    pinMode(MOTOR_BIN2, OUTPUT);
    pinMode(MOTOR_STBY, OUTPUT);
    
    // configure PWM
    ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PWMA, PWM_CHANNEL_A);
    ledcAttachPin(MOTOR_PWMB, PWM_CHANNEL_B);
    
    // enable driver
    digitalWrite(MOTOR_STBY, HIGH);
    
    stop();
    
    #if DEBUG_MODE
    Serial.println("Motors initialized");
    #endif
}

void MotorController::setMotorA(int speed) {
    // left motor
    if (speed > 0) {
        digitalWrite(MOTOR_AIN1, HIGH);
        digitalWrite(MOTOR_AIN2, LOW);
        ledcWrite(PWM_CHANNEL_A, constrain(speed, 0, 255));
    } else if (speed < 0) {
        digitalWrite(MOTOR_AIN1, LOW);
        digitalWrite(MOTOR_AIN2, HIGH);
        ledcWrite(PWM_CHANNEL_A, constrain(-speed, 0, 255));
    } else {
        digitalWrite(MOTOR_AIN1, LOW);
        digitalWrite(MOTOR_AIN2, LOW);
        ledcWrite(PWM_CHANNEL_A, 0);
    }
}

void MotorController::setMotorB(int speed) {
    // right motor
    if (speed > 0) {
        digitalWrite(MOTOR_BIN1, HIGH);
        digitalWrite(MOTOR_BIN2, LOW);
        ledcWrite(PWM_CHANNEL_B, constrain(speed, 0, 255));
    } else if (speed < 0) {
        digitalWrite(MOTOR_BIN1, LOW);
        digitalWrite(MOTOR_BIN2, HIGH);
        ledcWrite(PWM_CHANNEL_B, constrain(-speed, 0, 255));
    } else {
        digitalWrite(MOTOR_BIN1, LOW);
        digitalWrite(MOTOR_BIN2, LOW);
        ledcWrite(PWM_CHANNEL_B, 0);
    }
}

void MotorController::setSpeed(int leftSpeed, int rightSpeed) {
    currentLeftSpeed = leftSpeed;
    currentRightSpeed = rightSpeed;
    setMotorA(leftSpeed);
    setMotorB(rightSpeed);
    isMoving = (leftSpeed != 0 || rightSpeed != 0);
}

void MotorController::move(Direction dir, int speed) {
    switch (dir) {
        case FORWARD:
            setSpeed(speed, speed);
            break;
        case BACKWARD:
            setSpeed(-speed, -speed);
            break;
        case LEFT:
            setSpeed(-speed, speed);
            break;
        case RIGHT:
            setSpeed(speed, -speed);
            break;
        case STOP:
            stop();
            break;
    }
    
    #if DEBUG_PRINT_MOVEMENT
    Serial.println("[MOTORS] Direction: " + String(dir) + ", Speed: " + String(speed));
    #endif
}

void MotorController::stop() {
    setSpeed(0, 0);
    isMoving = false;
}

void MotorController::brake() {
    // Short brake for quick stop
    digitalWrite(MOTOR_AIN1, HIGH);
    digitalWrite(MOTOR_AIN2, HIGH);
    digitalWrite(MOTOR_BIN1, HIGH);
    digitalWrite(MOTOR_BIN2, HIGH);
    delay(50);
    stop();
}

void MotorController::moveForward(int duration, int speed) {
    move(FORWARD, speed);
    delay(duration);
    stop();
}

void MotorController::turnLeft90() {
    #if DEBUG_MODE
    Serial.println("Turn left");
    #endif
    
    move(LEFT, TURN_SPEED);
    delay(TURN_90_TIME);
    stop();
    delay(100);
}

void MotorController::turnRight90() {
    #if DEBUG_MODE
    Serial.println("Turn right");
    #endif
    
    move(RIGHT, TURN_SPEED);
    delay(TURN_90_TIME);
    stop();
    delay(100);
}

void MotorController::turnAround() {
    #if DEBUG_MODE
    Serial.println("Turn 180");
    #endif
    
    move(LEFT, TURN_SPEED);
    delay(TURN_180_TIME);
    stop();
    delay(100);
}

void MotorController::moveForwardWithCorrection(int leftDist, int rightDist) {
    // basic wall following
    int error = 0;
    int correction = 0;
    
    if (leftDist < WALL_DETECTION_THRESHOLD && rightDist < WALL_DETECTION_THRESHOLD) {
        // both walls - center between them
        error = leftDist - rightDist;
        correction = error * KP_WALL;
    }
    else if (leftDist < WALL_DETECTION_THRESHOLD) {
        // only left wall
        error = SIDE_WALL_TARGET_DISTANCE - leftDist;
        correction = error * KP_WALL;
    }
    else if (rightDist < WALL_DETECTION_THRESHOLD) {
        // only right wall
        error = rightDist - SIDE_WALL_TARGET_DISTANCE;
        correction = error * KP_WALL;
    }
    
    int leftSpeed = SEARCH_SPEED - correction;
    int rightSpeed = SEARCH_SPEED + correction;
    
    leftSpeed = constrain(leftSpeed, 50, 200);
    rightSpeed = constrain(rightSpeed, 50, 200);
    
    setSpeed(leftSpeed, rightSpeed);
}

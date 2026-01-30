#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>
#include "config.h"

class SensorManager {
private:
    VL53L1X tofLeft;
    VL53L1X tofFront;
    VL53L1X tofRight;
    
    uint16_t leftDistance;
    uint16_t frontDistance;
    uint16_t rightDistance;
    
    unsigned long lastReadTime;
    bool initialized;

    void configureSensorAddress(VL53L1X &sensor, uint8_t xshutPin, uint8_t newAddress);

public:
    SensorManager();
    bool begin();
    void update();
    
    // Distance getters (in mm)
    uint16_t getLeftDistance() { return leftDistance; }
    uint16_t getFrontDistance() { return frontDistance; }
    uint16_t getRightDistance() { return rightDistance; }
    
    // Wall detection
    bool hasLeftWall() { return leftDistance < WALL_DETECTION_THRESHOLD; }
    bool hasFrontWall() { return frontDistance < WALL_DETECTION_THRESHOLD; }
    bool hasRightWall() { return rightDistance < WALL_DETECTION_THRESHOLD; }
    
    // Emergency detection
    bool isEmergencyStop() { return frontDistance < EMERGENCY_STOP_DISTANCE; }
    
    void printDistances();
};

#endif

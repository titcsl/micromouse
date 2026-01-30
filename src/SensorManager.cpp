#include "SensorManager.h"

SensorManager::SensorManager() {
    leftDistance = 0;
    frontDistance = 0;
    rightDistance = 0;
    lastReadTime = 0;
    initialized = false;
}

bool SensorManager::begin() {
    // init i2c bus
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);
    
    // setup shutdown pins
    pinMode(TOF_LEFT_XSHUT, OUTPUT);
    pinMode(TOF_FRONT_XSHUT, OUTPUT);
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);
    
    // turn off all sensors first
    digitalWrite(TOF_LEFT_XSHUT, LOW);
    digitalWrite(TOF_FRONT_XSHUT, LOW);
    digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(10);
    
    // configure each sensor one by one
    configureSensorAddress(tofLeft, TOF_LEFT_XSHUT, TOF_LEFT_ADDR);
    delay(10);
    configureSensorAddress(tofFront, TOF_FRONT_XSHUT, TOF_FRONT_ADDR);
    delay(10);
    configureSensorAddress(tofRight, TOF_RIGHT_XSHUT, TOF_RIGHT_ADDR);
    delay(10);
    
    // Set timing budget for all sensors
    tofLeft.setDistanceMode(VL53L1X::Short);
    tofLeft.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET * 1000);
    tofLeft.startContinuous(SENSOR_READ_INTERVAL);
    
    tofFront.setDistanceMode(VL53L1X::Short);
    tofFront.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET * 1000);
    tofFront.startContinuous(SENSOR_READ_INTERVAL);
    
    tofRight.setDistanceMode(VL53L1X::Short);
    tofRight.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET * 1000);
    tofRight.startContinuous(SENSOR_READ_INTERVAL);
    
    initialized = true;
    
    #if DEBUG_MODE
    Serial.println("ToF sensors initialized");
    #endif
    
    return true;
}

void SensorManager::configureSensorAddress(VL53L1X &sensor, uint8_t xshutPin, uint8_t newAddress) {
    // turn on this sensor
    digitalWrite(xshutPin, HIGH);
    delay(10);
    
    sensor.setTimeout(500);
    if (!sensor.init()) {
        #if DEBUG_MODE
        Serial.print("Failed to init sensor on pin ");
        Serial.println(xshutPin);
        #endif
        return;
    }
    
    // set new address
    sensor.setAddress(newAddress);
    
    #if DEBUG_MODE
    Serial.print("Configured sensor on pin ");
    Serial.print(xshutPin);
    Serial.print(" to address 0x");
    Serial.println(newAddress, HEX);
    #endif
}

void SensorManager::update() {
    if (!initialized) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime < SENSOR_READ_INTERVAL) return;
    
    lastReadTime = currentTime;
    
    // read all sensors
    leftDistance = tofLeft.read(false);
    frontDistance = tofFront.read(false);
    rightDistance = tofRight.read(false);
    
    // check for timeouts
    if (tofLeft.timeoutOccurred()) leftDistance = 8000;
    if (tofFront.timeoutOccurred()) frontDistance = 8000;
    if (tofRight.timeoutOccurred()) rightDistance = 8000;
    
    #if DEBUG_PRINT_SENSORS
    printDistances();
    #endif
}

void SensorManager::printDistances() {
    Serial.print("L: ");
    Serial.print(leftDistance);
    Serial.print(" | F: ");
    Serial.print(frontDistance);
    Serial.print(" | R: ");
    Serial.println(rightDistance);
}

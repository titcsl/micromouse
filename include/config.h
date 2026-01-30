#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions and config
// TODO: double check these pins before soldering

// I2C pins for all sensors
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// shutdown pins - needed to set different addresses
#define TOF_LEFT_XSHUT 25
#define TOF_FRONT_XSHUT 26
#define TOF_RIGHT_XSHUT 27

// TB6612 motor driver connections
#define MOTOR_AIN1 13
#define MOTOR_AIN2 12
#define MOTOR_PWMA 14
#define MOTOR_BIN1 32
#define MOTOR_BIN2 33
#define MOTOR_PWMB 15
#define MOTOR_STBY 4  // standby pin

// PWM settings
#define PWM_FREQ 20000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1

// ========================================
// SENSOR CONFIGURATION
// ========================================

// sensor addresses after init
#define TOF_LEFT_ADDR 0x30
#define TOF_FRONT_ADDR 0x31
#define TOF_RIGHT_ADDR 0x32

// wall detection thresholds - might need tuning
#define WALL_DETECTION_THRESHOLD 180  // mm
#define FRONT_WALL_STOP_DISTANCE 80   
#define SIDE_WALL_TARGET_DISTANCE 90

#define SENSOR_TIMING_BUDGET 50  
#define SENSOR_READ_INTERVAL 20

// ========================================
// MOTOR CONFIGURATION
// ========================================

// motor speeds (0-255) - found through testing
#define BASE_SPEED 120
#define TURN_SPEED 100
#define SEARCH_SPEED 110
#define FAST_RUN_SPEED 180  // use this for speed run

// PID values for wall following - still tuning these
#define KP_WALL 1.5
#define KI_WALL 0.0
#define KD_WALL 0.8

// movement timing in ms
#define CELL_MOVE_TIME 800   // time to move one cell
#define TURN_90_TIME 600     
#define TURN_180_TIME 1200

// ========================================
// MAZE CONFIGURATION
// ========================================

// maze size - will update when we get actual dimensions
#define MAZE_WIDTH 16
#define MAZE_HEIGHT 16

#define CELL_SIZE 180  // mm

// starting position
#define START_X 0
#define START_Y 0

// goal position - update this based on competition rules
#define GOAL_X_MIN 7
#define GOAL_X_MAX 8
#define GOAL_Y_MIN 7
#define GOAL_Y_MAX 8

// ========================================
// ALGORITHM CONFIGURATION
// ========================================

// algorithm settings
#define USE_FLOOD_FILL true
#define MAX_EXPLORATION_RUNS 3

#define ENABLE_FAST_RUN true
#define FAST_RUN_DELAY 2000

// debugging
#define DEBUG_MODE true
#define SERIAL_BAUD 115200
#define DEBUG_PRINT_MAZE true
#define DEBUG_PRINT_SENSORS true
#define DEBUG_PRINT_MOVEMENT false

// safety
#define MAX_RUNTIME 300000  // 5 min max
#define EMERGENCY_STOP_DISTANCE 30

#endif

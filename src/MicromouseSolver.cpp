#include "MicromouseSolver.h"

MicromouseSolver::MicromouseSolver() {
    state = IDLE;
    startTime = 0;
    explorationRuns = 0;
}

void MicromouseSolver::begin() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);
    
    Serial.println("\nMicromouse Solver Starting...\n");
    
    Serial.println("Init sensors...");
    if (!sensors.begin()) {
        Serial.println("ERROR: Sensor init failed!");
        while (1);
    }
    
    Serial.println("Init motors...");
    motors.begin();
    
    Serial.println("Init maze...");
    maze.reset();
    
    delay(1000);
    
    Serial.println("\nReady! Place at start and press any key\n");
    
    while (!Serial.available());
    while (Serial.available()) Serial.read();
    
    state = EXPLORING;
    startTime = millis();
    
    Serial.println("Starting exploration...\n");
}

void MicromouseSolver::run() {
    // safety timeout
    if (millis() - startTime > MAX_RUNTIME) {
        motors.stop();
        Serial.println("TIMEOUT - stopping");
        state = FINISHED;
        return;
    }
    
    sensors.update();
    
    // emergency check
    if (sensors.isEmergencyStop()) {
        motors.brake();
        delay(200);
    }
    
    switch (state) {
        case EXPLORING:
            exploreCell();
            break;
            
        case RETURNING_TO_START:
            if (checkAndReturnToStart()) {
                explorationRuns++;
                
                if (explorationRuns >= MAX_EXPLORATION_RUNS) {
                    Serial.println("\nExploration done, starting fast run\n");
                    delay(FAST_RUN_DELAY);
                    state = FAST_RUN;
                } else {
                    Serial.println("\nRun " + String(explorationRuns) + " done\n");
                    state = EXPLORING;
                }
            }
            break;
            
        case FAST_RUN:
            if (ENABLE_FAST_RUN) {
                executeFastRun();
            }
            state = FINISHED;
            break;
            
        case FINISHED:
            motors.stop();
            Serial.println("\nFINISHED!");
            Serial.print("Time: ");
            Serial.print((millis() - startTime) / 1000.0);
            Serial.println(" sec");
            while (1);
            break;
            
        default:
            break;
    }
}

void MicromouseSolver::exploreCell() {
    bool leftWall = sensors.hasLeftWall();
    bool frontWall = sensors.hasFrontWall();
    bool rightWall = sensors.hasRightWall();
    
    maze.updateWalls(leftWall, frontWall, rightWall);
    
    #if DEBUG_MODE
    Serial.print("Walls: L");
    Serial.print(leftWall ? "1" : "0");
    Serial.print(" F");
    Serial.print(frontWall ? "1" : "0");
    Serial.print(" R");
    Serial.println(rightWall ? "1" : "0");
    #endif
    
    if (maze.isAtGoal()) {
        Serial.println("\nReached goal!");
        maze.printMaze();
        motors.stop();
        delay(1000);
        state = RETURNING_TO_START;
        return;
    }
    
    int bestMove = maze.getBestMove();
    moveToNextCell(bestMove);
}

void MicromouseSolver::moveToNextCell(int direction) {
    // 0=forward, 1=right, 2=back, 3=left
    
    switch (direction) {
        case 0: // forward
            Serial.println("Move forward");
            
            unsigned long moveStart = millis();
            while (millis() - moveStart < CELL_MOVE_TIME) {
                sensors.update();
                motors.moveForwardWithCorrection(
                    sensors.getLeftDistance(),
                    sensors.getRightDistance()
                );
                
                if (sensors.isEmergencyStop()) {
                    motors.brake();
                    delay(100);
                    break;
                }
            }
            motors.stop();
            break;
            
        case 1:
            Serial.println("Turn right");
            motors.turnRight90();
            break;
            
        case 2:
            Serial.println("Turn around");
            motors.turnAround();
            break;
            
        case 3:
            Serial.println("Turn left");
            motors.turnLeft90();
            break;
    }
    
    maze.updatePosition(direction);
    delay(50);
}

bool MicromouseSolver::checkAndReturnToStart() {
    // Simple return: keep following flood fill until back at start
    sensors.update();
    
    bool leftWall = sensors.hasLeftWall();
    bool frontWall = sensors.hasFrontWall();
    bool rightWall = sensors.hasRightWall();
    
    maze.updateWalls(leftWall, frontWall, rightWall);
    
    int x, y, dir;
    maze.getPosition(x, y, dir);
    
    // Check if at start
    if (x == START_X && y == START_Y) {
        Serial.println("[RETURN] Back at start position!");
        motors.stop();
        return true;
    }
    
    // Continue moving toward start
    int bestMove = maze.getBestMove();
    moveToNextCell(bestMove);
    
    return false;
}

void MicromouseSolver::executeFastRun() {
    Serial.println("\n[FAST RUN] Executing optimized path...");
    
    // Reset position
    maze.setPosition(START_X, START_Y, NORTH);
    
    while (!maze.isAtGoal()) {
        sensors.update();
        
        // Emergency check
        if (sensors.isEmergencyStop()) {
            motors.brake();
            Serial.println("[FAST RUN] Emergency stop!");
            return;
        }
        
        // Get best move
        int bestMove = maze.getBestMove();
        
        // Execute move at higher speed
        switch (bestMove) {
            case 0: // Forward
                unsigned long moveStart = millis();
                while (millis() - moveStart < CELL_MOVE_TIME - 200) {
                    sensors.update();
                    motors.setSpeed(FAST_RUN_SPEED, FAST_RUN_SPEED);
                    
                    if (sensors.isEmergencyStop()) {
                        motors.brake();
                        return;
                    }
                }
                motors.stop();
                break;
                
            case 1:
                motors.turnRight90();
                break;
            case 2:
                motors.turnAround();
                break;
            case 3:
                motors.turnLeft90();
                break;
        }
        
        maze.updatePosition(bestMove);
        delay(30);
    }
    
    Serial.println("[FAST RUN] Reached goal!");
    motors.stop();
}

void MicromouseSolver::testSensors() {
    Serial.println("\n[TEST] Sensor test mode");
    Serial.println("Reading sensors every second. Press any key to stop.\n");
    
    while (!Serial.available()) {
        sensors.update();
        sensors.printDistances();
        delay(1000);
    }
    
    while (Serial.available()) Serial.read();
}

void MicromouseSolver::testMotors() {
    Serial.println("\n[TEST] Motor test mode");
    
    Serial.println("Forward...");
    motors.moveForward(1000, 100);
    delay(500);
    
    Serial.println("Left turn...");
    motors.turnLeft90();
    delay(500);
    
    Serial.println("Right turn...");
    motors.turnRight90();
    delay(500);
    
    Serial.println("Turn around...");
    motors.turnAround();
    delay(500);
    
    Serial.println("[TEST] Motor test complete");
}

void MicromouseSolver::calibrate() {
    Serial.println("\n[CALIBRATE] Calibration mode");
    Serial.println("Place robot in known position and check sensors...\n");
    
    for (int i = 0; i < 10; i++) {
        sensors.update();
        sensors.printDistances();
        delay(500);
    }
    
    Serial.println("\n[CALIBRATE] Adjust WALL_DETECTION_THRESHOLD and other");
    Serial.println("parameters in config.h based on these readings.");
}

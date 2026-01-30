#include "Maze.h"

Maze::Maze() {
    reset();
}

void Maze::reset() {
    // init all cells
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            cells[x][y].walls = 0;
            cells[x][y].distance = 0;
            
            // boundary walls
            if (x == 0) cells[x][y].walls |= WALL_WEST;
            if (x == MAZE_WIDTH - 1) cells[x][y].walls |= WALL_EAST;
            if (y == 0) cells[x][y].walls |= WALL_SOUTH;
            if (y == MAZE_HEIGHT - 1) cells[x][y].walls |= WALL_NORTH;
        }
    }
    
    currentX = START_X;
    currentY = START_Y;
    currentDir = NORTH;
    
    #if DEBUG_MODE
    Serial.println("Maze reset");
    #endif
}

void Maze::setPosition(int x, int y, int dir) {
    currentX = x;
    currentY = y;
    currentDir = dir;
}

void Maze::getPosition(int &x, int &y, int &dir) {
    x = currentX;
    y = currentY;
    dir = currentDir;
}

void Maze::updatePosition(int moveDir) {
    // update direction
    currentDir = (currentDir + moveDir) % 4;
    
    // move forward
    switch (currentDir) {
        case NORTH: currentY++; break;
        case EAST:  currentX++; break;
        case SOUTH: currentY--; break;
        case WEST:  currentX--; break;
    }
    
    cells[currentX][currentY].walls |= VISITED;
    
    #if DEBUG_MODE
    Serial.print("Pos: (");
    Serial.print(currentX);
    Serial.print(",");
    Serial.print(currentY);
    Serial.print(") Dir:");
    Serial.println(currentDir);
    #endif
}

void Maze::setWall(int x, int y, int dir, bool hasWall) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) return;
    
    uint8_t wallFlag = 0;
    switch (dir) {
        case NORTH: wallFlag = WALL_NORTH; break;
        case EAST:  wallFlag = WALL_EAST; break;
        case SOUTH: wallFlag = WALL_SOUTH; break;
        case WEST:  wallFlag = WALL_WEST; break;
    }
    
    if (hasWall) {
        cells[x][y].walls |= wallFlag;
        
        // Set corresponding wall in adjacent cell
        int adjX = x, adjY = y;
        int adjDir = (dir + 2) % 4;
        
        switch (dir) {
            case NORTH: adjY++; break;
            case EAST:  adjX++; break;
            case SOUTH: adjY--; break;
            case WEST:  adjX--; break;
        }
        
        if (adjX >= 0 && adjX < MAZE_WIDTH && adjY >= 0 && adjY < MAZE_HEIGHT) {
            uint8_t adjWallFlag = 0;
            switch (adjDir) {
                case NORTH: adjWallFlag = WALL_NORTH; break;
                case EAST:  adjWallFlag = WALL_EAST; break;
                case SOUTH: adjWallFlag = WALL_SOUTH; break;
                case WEST:  adjWallFlag = WALL_WEST; break;
            }
            cells[adjX][adjY].walls |= adjWallFlag;
        }
    }
}

bool Maze::hasWall(int x, int y, int dir) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) return true;
    
    switch (dir) {
        case NORTH: return cells[x][y].walls & WALL_NORTH;
        case EAST:  return cells[x][y].walls & WALL_EAST;
        case SOUTH: return cells[x][y].walls & WALL_SOUTH;
        case WEST:  return cells[x][y].walls & WALL_WEST;
    }
    return false;
}

void Maze::updateWalls(bool leftWall, bool frontWall, bool rightWall) {
    // Calculate absolute directions based on current orientation
    int leftDir = (currentDir + 3) % 4;   // Turn left
    int frontDir = currentDir;
    int rightDir = (currentDir + 1) % 4;  // Turn right
    
    setWall(currentX, currentY, leftDir, leftWall);
    setWall(currentX, currentY, frontDir, frontWall);
    setWall(currentX, currentY, rightDir, rightWall);
}

bool Maze::isGoal(int x, int y) {
    return (x >= GOAL_X_MIN && x <= GOAL_X_MAX && 
            y >= GOAL_Y_MIN && y <= GOAL_Y_MAX);
}

bool Maze::isAtGoal() {
    return isGoal(currentX, currentY);
}

void Maze::calculateDistances() {
    // flood fill algorithm
    // initialize
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            cells[x][y].distance = 9999;
        }
    }
    
    // goal = 0
    for (int x = GOAL_X_MIN; x <= GOAL_X_MAX; x++) {
        for (int y = GOAL_Y_MIN; y <= GOAL_Y_MAX; y++) {
            cells[x][y].distance = 0;
        }
    }
    
    // propagate distances
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (int x = 0; x < MAZE_WIDTH; x++) {
            for (int y = 0; y < MAZE_HEIGHT; y++) {
                if (cells[x][y].distance == 0) continue;
                
                uint16_t minDist = 9999;
                
                int dx[] = {0, 1, 0, -1};
                int dy[] = {1, 0, -1, 0};
                
                for (int dir = 0; dir < 4; dir++) {
                    if (!hasWall(x, y, dir)) {
                        int nx = x + dx[dir];
                        int ny = y + dy[dir];
                        
                        if (nx >= 0 && nx < MAZE_WIDTH && ny >= 0 && ny < MAZE_HEIGHT) {
                            if (cells[nx][ny].distance < minDist) {
                                minDist = cells[nx][ny].distance;
                            }
                        }
                    }
                }
                
                if (minDist + 1 < cells[x][y].distance) {
                    cells[x][y].distance = minDist + 1;
                    changed = true;
                }
            }
        }
    }
    
    #if DEBUG_MODE
    Serial.println("Distances updated");
    #endif
}

int Maze::getBestMove() {
    calculateDistances();
    
    uint16_t currentDist = cells[currentX][currentY].distance;
    int bestMove = 0; // 0=forward, 1=right, 2=back, 3=left
    uint16_t bestDist = currentDist;
    
    // Check all four relative directions
    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};
    
    for (int relDir = 0; relDir < 4; relDir++) {
        int absDir = (currentDir + relDir) % 4;
        
        if (!hasWall(currentX, currentY, absDir)) {
            int nx = currentX + dx[absDir];
            int ny = currentY + dy[absDir];
            
            if (nx >= 0 && nx < MAZE_WIDTH && ny >= 0 && ny < MAZE_HEIGHT) {
                if (cells[nx][ny].distance < bestDist) {
                    bestDist = cells[nx][ny].distance;
                    bestMove = relDir;
                }
            }
        }
    }
    
    return bestMove;
}

int Maze::getDistance(int x, int y) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) return 9999;
    return cells[x][y].distance;
}

void Maze::printMaze() {
    #if DEBUG_PRINT_MAZE
    Serial.println("\n[MAZE] Current maze state:");
    
    for (int y = MAZE_HEIGHT - 1; y >= 0; y--) {
        // Print top walls
        for (int x = 0; x < MAZE_WIDTH; x++) {
            Serial.print("+");
            Serial.print(hasWall(x, y, NORTH) ? "---" : "   ");
        }
        Serial.println("+");
        
        // Print side walls and distances
        for (int x = 0; x < MAZE_WIDTH; x++) {
            Serial.print(hasWall(x, y, WEST) ? "|" : " ");
            
            if (x == currentX && y == currentY) {
                Serial.print(" * ");
            } else {
                Serial.printf("%3d", cells[x][y].distance);
            }
        }
        Serial.println("|");
    }
    
    // Print bottom border
    for (int x = 0; x < MAZE_WIDTH; x++) {
        Serial.print("+---");
    }
    Serial.println("+\n");
    #endif
}

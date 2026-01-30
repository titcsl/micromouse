#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>
#include "config.h"

// Cell wall bit flags
#define WALL_NORTH 0x01
#define WALL_EAST  0x02
#define WALL_SOUTH 0x04
#define WALL_WEST  0x08
#define VISITED    0x10

// Directions
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

struct Cell {
    uint8_t walls;      // Wall flags
    uint16_t distance;  // Flood fill distance
};

class Maze {
private:
    Cell cells[MAZE_WIDTH][MAZE_HEIGHT];
    int currentX, currentY;
    int currentDir; // 0=North, 1=East, 2=South, 3=West
    
    void floodFill(int goalX, int goalY);
    int getNextDirection();
    bool isGoal(int x, int y);
    
public:
    Maze();
    void reset();
    
    // Position management
    void setPosition(int x, int y, int dir);
    void getPosition(int &x, int &y, int &dir);
    void updatePosition(int moveDir);
    
    // Wall management
    void setWall(int x, int y, int dir, bool hasWall);
    bool hasWall(int x, int y, int dir);
    void updateWalls(bool leftWall, bool frontWall, bool rightWall);
    
    // Pathfinding
    void calculateDistances();
    int getBestMove();
    bool isAtGoal();
    
    // Utilities
    void printMaze();
    int getDistance(int x, int y);
};

#endif

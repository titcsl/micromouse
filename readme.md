# Micromouse

Firmware for a maze-solving micromouse robot, built for the **ESP32** (Arduino framework). The robot explores an unknown 16×16 maze using three VL53L1X time-of-flight distance sensors, builds a wall map as it goes, computes the shortest path with a **flood-fill algorithm**, and finishes with an optimized high-speed run once the maze has been fully explored.

## How It Works

1. **Explore** — the robot reads its three ToF sensors (left/front/right) each cell, records which walls it sees into a 16×16 grid, and uses flood-fill distances from the goal to always move toward the nearest unexplored/shortest path.
2. **Return to start** — once the goal region is reached, the robot flood-fills back toward the start cell using everything it has learned.
3. **Repeat** — this explore/return cycle runs up to `MAX_EXPLORATION_RUNS` times, refining the wall map each pass.
4. **Fast run** — with the maze now known, the robot re-runs the shortest path at higher motor speed (`FAST_RUN_SPEED`).

Wall-following during movement uses a proportional (`KP_WALL`) correction between the left/right ToF readings to stay centered in each cell.

## Hardware Assumed

- ESP32 microcontroller
- 3× VL53L1X time-of-flight distance sensors (left, front, right), each on I2C with a dedicated XSHUT pin for address assignment
- TB6612 dual motor driver (2 DC motors, PWM speed control)

## Project Structure — File Index

```
micromouse/
├── readme.md                    (this file)
│
├── include/
│   ├── config.h                  All tunable constants in one place: pin assignments (I2C, ToF
│   │                             XSHUT, motor driver), PWM settings, sensor addresses/thresholds,
│   │                             motor speeds, PID gains for wall following, movement timings,
│   │                             maze dimensions (16×16) and goal region, algorithm toggles
│   │                             (flood-fill, fast run), debug flags, and safety limits
│   │                             (max runtime, emergency stop distance).
│   │
│   ├── Maze.h                    Declares Maze: a 16×16 grid of Cell structs (wall bitflags +
│   │                             flood-fill distance). Tracks the robot's current position/
│   │                             orientation, records discovered walls, runs the flood-fill
│   │                             distance calculation, and reports the best next move.
│   │
│   ├── MotorController.h         Declares MotorController: drives two motors via the TB6612
│   │                             (PWM + direction pins), with helpers for 90°/180° turns, timed
│   │                             forward movement, hard braking, and wall-following speed
│   │                             correction.
│   │
│   ├── SensorManager.h           Declares SensorManager: wraps three VL53L1X sensors (assigning
│   │                             each a unique I2C address at startup), exposes left/front/right
│   │                             distance readings, and derives wall-present / emergency-stop
│   │                             booleans from configurable thresholds.
│   │
│   └── MicromouseSolver.h        Declares MicromouseSolver: the top-level state machine
│                                 (IDLE → EXPLORING → RETURNING_TO_START → FAST_RUN → FINISHED)
│                                 that owns the sensors, motors, and maze, and coordinates them
│                                 into the explore/return/fast-run behavior above. Also exposes
│                                 manual test/calibration routines.
│
└── src/
    ├── main.cpp                   Arduino entry point — Arduino's setup()/loop() simply
    │                              delegate to MicromouseSolver::begin() and ::run(). Commented-
    │                              out lines show how to run sensor/motor test modes instead.
    │
    ├── Maze.cpp                   Implements Maze: wall storage with shared walls between
    │                              adjacent cells, the flood-fill distance propagation
    │                              (BFS-like relaxation from the goal outward), best-move
    │                              selection based on neighboring distances, and an ASCII maze
    │                              printout for debugging over Serial.
    │
    ├── MotorController.cpp        Implements MotorController: raw PWM/direction control for
    │                              each motor (setMotorA/B), composed into move(), timed turns
    │                              (turnLeft90/turnRight90/turnAround), a hard brake() (all
    │                              driver pins high), and moveForwardWithCorrection(), a simple
    │                              proportional wall-following controller.
    │
    ├── SensorManager.cpp          Implements SensorManager: brings each VL53L1X sensor up one
    │                              at a time via its XSHUT pin so each can be assigned a unique
    │                              I2C address, configures short-range/fast timing budgets,
    │                              polls all three on an interval, and handles sensor timeouts.
    │
    └── MicromouseSolver.cpp       Implements MicromouseSolver: begin() initializes sensors/
                                   motors/maze and waits for a Serial keypress to start; run()
                                   is the per-loop state machine tick, including a safety runtime
                                   timeout and emergency-stop braking; exploreCell(),
                                   moveToNextCell(), checkAndReturnToStart(), and
                                   executeFastRun() implement each state's behavior; testSensors(),
                                   testMotors(), and calibrate() are manual diagnostic routines.
```

## Building / Flashing

This is a PlatformIO/Arduino-style project (uses `Arduino.h`, `Wire.h`, and the `VL53L1X` library). To build:

1. Open the project in PlatformIO (or the Arduino IDE with an ESP32 board package installed).
2. Install the `VL53L1X` library dependency (e.g. Pololu's VL53L1X Arduino library).
3. Double-check all pin numbers in `include/config.h` against your actual wiring before flashing — the file has a standing `// TODO: double check these pins before soldering` note.
4. Flash to the ESP32, open Serial Monitor at `115200` baud, place the robot at the maze start, and press any key to begin exploration.

## Known Rough Edges

- `config.h` notes several values as still being tuned (PID gains, movement timings, maze dimensions/goal region) — expect to recalibrate for your specific maze and hardware.
- No unit tests; this is hardware-in-the-loop firmware, tested via the built-in `testSensors()`/`testMotors()`/`calibrate()` modes.

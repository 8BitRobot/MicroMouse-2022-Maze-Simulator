#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "API.h"

int idleCount = 0;
int currentGoalX = 0;
int currentGoalY = 0;

int main(int argc, char* argv[]) {
    // debug_log("Running...");
    API_clearAllText();
    // fillManhattanDists();
    floodFillDistToPoint(0, 0);
    while (1) {
        Action nextMove = solver();
        switch (nextMove) {
            case FORWARD:
                API_moveForward();
                idleCount = 0;
                break;
            case LEFT:
                API_turnLeft();
                idleCount = 0;
                break;
            case RIGHT:
                API_turnRight();
                idleCount = 0;
                break;
            case IDLE:
                idleCount++;
                if (idleCount == 10) {
                    idleCount = 0;
                    if (currentGoalX == 0 && currentGoalY == 0) {
                        currentGoalX = 15;
                    } else if (currentGoalX == 15 && currentGoalY == 0) {
                        currentGoalY = 15;
                    } else if (currentGoalX == 15 && currentGoalY == 15) {
                        currentGoalX = 0;
                    } else if (currentGoalX == 0 && currentGoalY == 15) {
                        currentGoalX = 7;
                        currentGoalY = 7;
                    } else if (currentGoalX == 7 && currentGoalY == 7) return 0;
                    floodFillDistToPoint(currentGoalX, currentGoalY);
                    // fprintf(stderr, "floodfilled dist to %d %d\n", currentGoalX, currentGoalY);
                    // fflush(stderr);
                }
                break;
        }
    }
}
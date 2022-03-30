#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "API.h"

void fillManhattanDists() {
    // quadrant 1
    int startNum = 7;
    for (int i = 0; i <= 7; i++) {
        int cNum = startNum;
        for (int j = 8; j <= 15; j++) {
            distances[i][j] = cNum;
            char* str = malloc(3);
            API_setText(j, 15 - i, itoa(cNum++, str, 10));
            free(str);
        }
        startNum--;
    }
    // quadrant 2
    startNum = 14;
    for (int i = 0; i <= 7; i++) {
        int cNum = startNum;
        for (int j = 0; j <= 7; j++) {
            distances[i][j] = cNum;
            char* str = malloc(3);
            API_setText(j, 15 - i, itoa(cNum--, str, 10));
            free(str);
        }
        startNum--;
    }
    // quadrant 3
    startNum = 7;
    for (int i = 8; i <= 15; i++) {
        int cNum = startNum;
        for (int j = 0; j <= 7; j++) {
            distances[i][j] = cNum;
            char* str = malloc(3);
            API_setText(j, 15 - i, itoa(cNum--, str, 10));
            free(str);
        }
        startNum++;
    }
    // quadrant 3
    startNum = 0;
    for (int i = 8; i <= 15; i++) {
        int cNum = startNum;
        for (int j = 8; j <= 15; j++) {
            distances[i][j] = cNum;
            char* str = malloc(3);
            API_setText(j, 15 - i, itoa(cNum++, str, 10));
            free(str);
        }
        startNum++;
    }
}

int main(int argc, char* argv[]) {
    debug_log("Running...");
    API_clearAllText();
    fillManhattanDists();
    while (1) {
        Action nextMove = solver();
        switch(nextMove){
            case FORWARD:
                API_moveForward();
                break;
            case LEFT:
                API_turnLeft();
                break;
            case RIGHT:
                API_turnRight();
                break;
            case IDLE:
                break;
        }
    }
}
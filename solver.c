#include "solver.h"
#include "API.h"
#include <stdio.h>

int cx = 0;
int cy = 15;

void enqueue(struct Queue* q, struct Point* p) {
    q->queue[q->end++ % 512] = p;
};

struct Point* dequeue(struct Queue* q) {
    struct Point* p = q->queue[q->start % 512];
    q->queue[q->start++ % 512] = 0;
    return p;
};

Heading direction = NORTH;

Action solver() {
    return manhattanFollower();
}

Action leftWallFollower() {
    if(API_wallFront()) {
        if(API_wallLeft()){
            return RIGHT;
        }
        return LEFT;
    }
    return FORWARD;
}

Action manhattanFollower() {
    int distCellLeft = 1000;
    int distCellRight = 1000;
    int distCellFront = 1000;

    if (API_wallLeft()) {
        API_setWall(cx, 15 - cy, headingToWallDirection(direction, 'l'));
    }
    if (API_wallRight()) {
        API_setWall(cx, 15 - cy, headingToWallDirection(direction, 'r'));
    }
    if (API_wallFront()) {
        API_setWall(cx, 15 - cy, headingToWallDirection(direction, 'f'));
    }

    return FORWARD;
}

Action floodFill() {

}
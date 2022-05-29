#ifndef SOLVER_H
#define SOLVER_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

struct Point {
    int x;
    int y;
};

struct Queue {
    int start;
    int end;
    struct Point* queue[512];
};

void enqueue(struct Queue* q, struct Point* p);
struct Point* dequeue(struct Queue* q);
int queueIsEmpty(struct Queue* q);

int hwalls[16][17]; // walls blocking horizontal movement
int vwalls[17][16]; // walls blocking vertical movement
int distances[16][16];

Action solver();
Action leftWallFollower();

Action dirToAction();
Action manhattanFollower();
void localFloodFill();

void fillManhattanDists();
void emptyDistances();
void floodFillDistToCenter();
void floodFillDistToPoint(int spointx, int spointy);

#endif
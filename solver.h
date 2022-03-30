#ifndef SOLVER_H
#define SOLVER_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

struct Point {
    int x;
    int y;
    int mdist;
};

struct Queue {
    int start;
    int end;
    struct Point* queue[512];
};

void enqueue(struct Queue* q, struct Point* p);

struct Point* dequeue(struct Queue* q);

int hwalls[17][16];
int vwalls[16][17];
int distances[16][16];

Action solver();
Action leftWallFollower();
Action manhattanFollower();
Action floodFill();

#endif
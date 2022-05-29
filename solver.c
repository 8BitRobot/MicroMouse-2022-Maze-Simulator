#include "solver.h"
#include "API.h"
#include <stdio.h>
#include <stdlib.h>

int cx = 0;
int cy = 15;
int justFloodFilled = 0;

void enqueue(struct Queue* q, struct Point* p) {
    q->queue[q->end++ % 512] = p;
};

struct Point* dequeue(struct Queue* q) {
    struct Point* p = q->queue[q->start % 512];
    q->queue[q->start++ % 512] = 0;
    return p;
};

int queueIsEmpty(struct Queue* q) {
    return q->start == q->end;
}

// void push(struct Stack* s, struct Point* p) {
//     s->stack[s->count++] = p;
// }

// struct Point* pop(struct Stack* s) {
//     struct Point* p = s->stack[s->count--];
//     return p;
// }

// int stackIsEmpty(struct Stack* s) {
//     return s->count == 0;
// }

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

Action dirToAction(Heading goal) {
    if (goal == -1) {
        debug_log("bad!");
        return IDLE;
    }
    if (direction == NORTH) {
        if (goal == WEST) return LEFT;
        else if (goal == NORTH) return FORWARD;
        else if (goal == EAST) return RIGHT;
        return IDLE;
    } else if (direction == SOUTH) {
        if (goal == EAST) return LEFT;
        else if (goal == SOUTH) return FORWARD;
        else if (goal == WEST) return RIGHT;
        return IDLE;
    } else if (direction == EAST) {
        if (goal == NORTH) return LEFT;
        else if (goal == EAST) return FORWARD;
        else if (goal == SOUTH) return RIGHT;
        return IDLE;
    } else {
        if (goal == SOUTH) return LEFT;
        else if (goal == WEST) return FORWARD;
        else if (goal == NORTH) return RIGHT;
        return IDLE;
    }
}

void dirToWallArrayUpdate(char side) {
    if (side == 'l') {
        if (direction == NORTH) {
            hwalls[cy][cx] = 1;
        } else if (direction == SOUTH) {
            hwalls[cy][cx + 1] = 1;
        } else if (direction == EAST) {
            vwalls[cy][cx] = 1;
        } else {
            vwalls[cy + 1][cx] = 1;
        }
    } else if (side == 'r') {
        if (direction == SOUTH) {
            hwalls[cy][cx] = 1;
        } else if (direction == NORTH) {
            hwalls[cy][cx + 1] = 1;
        } else if (direction == WEST) {
            vwalls[cy][cx] = 1;
        } else {
            vwalls[cy + 1][cx] = 1;
        }
    } else if (side == 'f') {
        if (direction == NORTH) {
            vwalls[cy][cx] = 1;
        } else if (direction == SOUTH) {
            vwalls[cy + 1][cx] = 1;
        } else if (direction == EAST) {
            hwalls[cy][cx + 1] = 1;
        } else {
            hwalls[cy][cx] = 1;
        }
    }
}

int dirToWallXDelta(char side) {
    if (side == 'l') {
        if (direction == NORTH) return -1;
        if (direction == SOUTH) return 1;
        return 0;
    } else if (side == 'r') {
        if (direction == NORTH) return 1;
        if (direction == SOUTH) return -1;
        return 0;
    } else if (side == 'f') {
        if (direction == EAST) return 1;
        if (direction == WEST) return -1;
        return 0;
    }
}

int dirToWallYDelta(char side) {
    if (side == 'l') {
        if (direction == EAST) return -1;
        if (direction == WEST) return 1;
        return 0;
    } else if (side == 'r') {
        if (direction == EAST) return 1;
        if (direction == WEST) return -1;
        return 0;
    } else if (side == 'f') {
        if (direction == SOUTH) return 1;
        if (direction == NORTH) return -1;
        return 0;
    }
}

Action manhattanFollower() {
    if (distances[cy][cx] == 0) return IDLE;
    int canReachDirection[4] = { 1, 1, 1, 1 };
    canReachDirection[(direction + 2) % 4] = 0;
    if (API_wallLeft()) {
        canReachDirection[(direction - 1) % 4] = 0;
        dirToWallArrayUpdate('l');
        API_setWall(cx, 15 - cy, headingToWallDirection(direction, 'l'));
        localFloodFill(cx + dirToWallXDelta('l'), cy + dirToWallYDelta('l'));
    }
    if (API_wallRight()) {
        canReachDirection[(direction + 1) % 4] = 0;
        dirToWallArrayUpdate('r');
        API_setWall(cx, 15 - cy, headingToWallDirection(direction, 'r'));
        localFloodFill(cx + dirToWallXDelta('r'), cy + dirToWallYDelta('r'));
    }
    if (API_wallFront()) {
        canReachDirection[direction] = 0;
        dirToWallArrayUpdate('f');
        API_setWall(cx, 15 - cy, headingToWallDirection(direction, 'f'));
        localFloodFill(cx + dirToWallXDelta('f'), cy + dirToWallYDelta('f'));
    }

    int distCellsAround[4] = {600, 600, 600, 600};
    if (cy > 0) distCellsAround[0] = distances[cy - 1][cx]; // north
    if (cx < 15) distCellsAround[1] = distances[cy][cx + 1]; // east
    if (cy < 15) distCellsAround[2] = distances[cy + 1][cx]; // south
    if (cx > 0) distCellsAround[3] = distances[cy][cx - 1]; // west

    int hasOpenPath = 0;

    int minDist = distances[cy][cx];
    int goal = -1;
    int minDistCannotReach = distances[cy][cx];
    int goalCannotReach = -1;
    for (int i = 0; i < 4; i++) {
        if (canReachDirection[i] && distCellsAround[i] < minDist) {
            goal = i;
            minDist = distCellsAround[i];
            hasOpenPath = 1;
        } else if (!canReachDirection[i] && distCellsAround[i] < minDistCannotReach) {
            goalCannotReach = i;
            minDistCannotReach = distCellsAround[i];
        }
    }
    // fprintf(stderr, "justFloodfilled: %d\n", justFloodFilled);
    if (goalCannotReach == -1 || (hasOpenPath && goal != -1 && minDist <= minDistCannotReach) || justFloodFilled) {
        Action act = dirToAction(goal);
        // fprintf(stderr, "dir: %d\n", direction);
        // fprintf(stderr, "goal: %d\n", goal);
        // fprintf(stderr, "act: %d\n", act);
        // fprintf(stderr, "hasOpenPath: %d\n", hasOpenPath);
        // fflush(stderr);
        if (goal == -1) {
            direction = (direction + 1) % 4;
            return RIGHT;
        }
        if (act == RIGHT) {
            direction = (direction + 1) % 4;
        } else if (act == LEFT) {
            direction = (direction - 1) % 4;
        } else if (act == FORWARD) {
            if (direction == NORTH) cy--;
            else if (direction == SOUTH) cy++;
            else if (direction == EAST) cx++;
            else cx--;
            justFloodFilled = 0;
        }
        return act;
    } else if (goalCannotReach != -1 && minDistCannotReach < minDist) {
        Action act = dirToAction(goalCannotReach);
        // fprintf(stderr, "my way is blocked!\n");
        // fflush(stderr);
        localFloodFill(cx, cy);
        // for (int i = 0; i < 16; i++) {
        //     for (int j = 0; j < 16; j++) {
        //         fprintf(stderr, "%d ", distances[i][j]);
        //     }
        //     fprintf(stderr, "\n");
        // }
        // fflush(stderr);
        justFloodFilled = 1;
        return IDLE;
    }
    // fprintf(stderr, "uh oh!\n");
    exit(1);
}

void localFloodFill(int currentX, int currentY) {
    if (currentX < 0 || currentX > 15 || currentY < 0 || currentY > 15) return;
    if (distances[currentY][currentX] == 0) return;
    int floodFillcx = currentX;
    int floodFillcy = currentY;
    // fprintf(stderr, "floodfill!\n");
    // fflush(stderr);
    struct Queue* q = malloc(sizeof(struct Queue));
    q->start = 0;
    q->end = 0;
    struct Point* start = malloc(sizeof(struct Point));
    start->x = floodFillcx;
    start->y = floodFillcy;
    enqueue(q, start);
    int firstRun = 1;
    while (!queueIsEmpty(q)) {
        struct Point* current = dequeue(q);
        floodFillcx = current->x;
        floodFillcy = current->y;
        int currentDist = distances[current->y][current->x];
        // fprintf(stderr, "cx: %d, cy: %d\n", current->x, current->y);
        // fprintf(stderr, "currentDist: %d\n", currentDist);
        // fflush(stderr);
        int minDistOfAccessibleNeighbors = 600;
        int northAccessible = 0, southAccessible = 0, eastAccessible = 0, westAccessible = 0;
        if (floodFillcy > 0 && !vwalls[floodFillcy][floodFillcx]) {
            minDistOfAccessibleNeighbors = (minDistOfAccessibleNeighbors > distances[floodFillcy - 1][floodFillcx])
                ? distances[floodFillcy - 1][floodFillcx]
                : minDistOfAccessibleNeighbors;
            northAccessible = 1;
            // fprintf(stderr, "north was open\n");
        }
        if (floodFillcy < 15 && !vwalls[floodFillcy + 1][floodFillcx]) {
            minDistOfAccessibleNeighbors = (minDistOfAccessibleNeighbors > distances[floodFillcy + 1][floodFillcx])
                ? distances[floodFillcy + 1][floodFillcx]
                : minDistOfAccessibleNeighbors;
            southAccessible = 1;
            // fprintf(stderr, "south was open\n");
        }
        if (floodFillcx > 0 && !hwalls[floodFillcy][floodFillcx]) {
            minDistOfAccessibleNeighbors = (minDistOfAccessibleNeighbors > distances[floodFillcy][floodFillcx - 1])
                ? distances[floodFillcy][floodFillcx - 1]
                : minDistOfAccessibleNeighbors;
            westAccessible = 1;
            // fprintf(stderr, "west was open\n");
        }
        if (floodFillcx < 15 && !hwalls[floodFillcy][floodFillcx + 1]) {
            minDistOfAccessibleNeighbors = (minDistOfAccessibleNeighbors > distances[floodFillcy][floodFillcx + 1])
                ? distances[floodFillcy][floodFillcx + 1]
                : minDistOfAccessibleNeighbors;
            eastAccessible = 1;
            // fprintf(stderr, "east was open\n");
        }
        // fprintf(stderr, "minDistOAN: %d\n", minDistOfAccessibleNeighbors);
        // fflush(stderr);
        if (minDistOfAccessibleNeighbors == 600) {
            // fprintf(stderr, "what the fuck\n");
            // fflush(stderr);
            exit(1);
        } else if (minDistOfAccessibleNeighbors >= currentDist) {
            distances[floodFillcy][floodFillcx] = minDistOfAccessibleNeighbors + 1;
            char* str = malloc(3);
            API_setText(current->x, 15 - current->y, itoa(minDistOfAccessibleNeighbors + 1, str, 10));
            free(str);
            //
            // fprintf(stderr, "new current distance: %d\n", distances[current->y][current->x]);
            // fflush(stderr);
            // 
            if (northAccessible) {
                struct Point* northPoint = malloc(sizeof(struct Point));
                northPoint->x = floodFillcx;
                northPoint->y = floodFillcy - 1;
                enqueue(q, northPoint);
                // fprintf(stderr, "enqueueing: %d %d\n", floodFillcx, floodFillcy - 1);
            }
            if (southAccessible) {
                struct Point* southPoint = malloc(sizeof(struct Point));
                southPoint->x = floodFillcx;
                southPoint->y = floodFillcy + 1;
                enqueue(q, southPoint);
                // fprintf(stderr, "enqueueing: %d %d\n", floodFillcx, floodFillcy + 1);
            }
            if (westAccessible) {
                struct Point* westPoint = malloc(sizeof(struct Point));
                westPoint->x = floodFillcx - 1;
                westPoint->y = floodFillcy;
                enqueue(q, westPoint);
                // fprintf(stderr, "enqueueing: %d %d\n", floodFillcx - 1, floodFillcy);
            }
            if (eastAccessible) {
                struct Point* eastPoint = malloc(sizeof(struct Point));
                eastPoint->x = floodFillcx + 1;
                eastPoint->y = floodFillcy;
                enqueue(q, eastPoint);
                // fprintf(stderr, "enqueueing: %d %d\n", floodFillcx + 1, floodFillcy);
            }
        }
        free(current);
    }
    free(q);
}


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

void emptyDistances() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            distances[i][j] = -1;
        }
    }
}

void floodFillDistToCenter() {
    distances[7][7] = 0;
    distances[7][8] = 0;
    distances[8][7] = 0;
    distances[8][8] = 0;
    struct Queue* queue = malloc(sizeof(struct Queue));
    queue->start = 0;
    queue->end = 0;
    struct Point* start1 = malloc(sizeof(struct Point));
    start1->x = 7;
    start1->y = 7;
    enqueue(queue, start1);
    // fprintf(stderr, "%d %d\n", start1->x, start1->y);
    struct Point* start2 = malloc(sizeof(struct Point));
    start2->x = 7;
    start2->y = 8;
    enqueue(queue, start2);
    // fprintf(stderr, "%d %d\n", start2->x, start2->y);
    struct Point* start3 = malloc(sizeof(struct Point));
    start3->x = 8;
    start3->y = 7;
    enqueue(queue, start3);
    // fprintf(stderr, "%d %d\n", start3->x, start3->y);
    struct Point* start4 = malloc(sizeof(struct Point));
    start4->x = 8;
    start4->y = 8;
    enqueue(queue, start4);
    // fprintf(stderr, "%d %d\n", start4->x, start4->y);
    // fflush(stderr);

    while (!queueIsEmpty(queue)) {
        struct Point* current = dequeue(queue);
        int ffy = current->y;
        int ffx = current->x;
        // fprintf(stderr, "cx: %d, cy: %d\n", current->x, current->y);
        // fflush(stderr);
        if (ffy > 0 && !vwalls[ffy][ffx] && distances[ffy - 1][ffx] == -1) {
            struct Point* northPoint = malloc(sizeof(struct Point));
            northPoint->y = ffy - 1;
            northPoint->x = ffx;
            enqueue(queue, northPoint);
            distances[ffy - 1][ffx] = distances[ffy][ffx] + 1;
        }
        if (ffy < 15 && !vwalls[ffy + 1][ffx] && distances[ffy + 1][ffx] == -1) {
            struct Point* southPoint = malloc(sizeof(struct Point));
            southPoint->y = ffy + 1;
            southPoint->x = ffx;
            enqueue(queue, southPoint);
            distances[ffy + 1][ffx] = distances[ffy][ffx] + 1;
        }
        if (ffx > 0 && !hwalls[ffy][ffx] && distances[ffy][ffx - 1] == -1) {
            struct Point* westPoint = malloc(sizeof(struct Point));
            westPoint->y = ffy;
            westPoint->x = ffx - 1;
            enqueue(queue, westPoint);
            distances[ffy][ffx - 1] = distances[ffy][ffx] + 1;
        }
        if (ffx < 15 && !hwalls[ffy][ffx + 1] && distances[ffy][ffx + 1] == -1) {
            struct Point* eastPoint = malloc(sizeof(struct Point));
            eastPoint->y = ffy;
            eastPoint->x = ffx + 1;
            enqueue(queue, eastPoint);
            distances[ffy][ffx + 1] = distances[ffy][ffx] + 1;
        }
        free(current);
    }
    free(queue);
}

void floodFillDistToPoint(int spointx, int spointy) {
    emptyDistances();
    distances[spointy][spointx] = 0;
    struct Queue* queue = malloc(sizeof(struct Queue));
    queue->start = 0;
    queue->end = 0;
    struct Point* spoint = malloc(sizeof(struct Point));
    spoint->x = spointx;
    spoint->y = spointy;
    enqueue(queue, spoint);
    // fprintf(stderr, "%d %d\n", start1->x, start1->y);
    // struct Point* start2 = malloc(sizeof(struct Point));
    // start2->x = 7;
    // start2->y = 8;
    // enqueue(queue, start2);
    // fprintf(stderr, "%d %d\n", start2->x, start2->y);
    // struct Point* start3 = malloc(sizeof(struct Point));
    // start3->x = 8;
    // start3->y = 7;
    // enqueue(queue, start3);
    // fprintf(stderr, "%d %d\n", start3->x, start3->y);
    // struct Point* start4 = malloc(sizeof(struct Point));
    // start4->x = 8;
    // start4->y = 8;
    // enqueue(queue, start4);
    // fprintf(stderr, "%d %d\n", start4->x, start4->y);
    // fflush(stderr);

    while (!queueIsEmpty(queue)) {
        struct Point* current = dequeue(queue);
        int ffy = current->y;
        int ffx = current->x;
        // fprintf(stderr, "cx: %d, cy: %d\n", current->x, current->y);
        // fflush(stderr);
        if (ffy > 0 && !vwalls[ffy][ffx] && distances[ffy - 1][ffx] == -1) {
            struct Point* northPoint = malloc(sizeof(struct Point));
            northPoint->y = ffy - 1;
            northPoint->x = ffx;
            enqueue(queue, northPoint);
            distances[ffy - 1][ffx] = distances[ffy][ffx] + 1;
        }
        if (ffy < 15 && !vwalls[ffy + 1][ffx] && distances[ffy + 1][ffx] == -1) {
            struct Point* southPoint = malloc(sizeof(struct Point));
            southPoint->y = ffy + 1;
            southPoint->x = ffx;
            enqueue(queue, southPoint);
            distances[ffy + 1][ffx] = distances[ffy][ffx] + 1;
        }
        if (ffx > 0 && !hwalls[ffy][ffx] && distances[ffy][ffx - 1] == -1) {
            struct Point* westPoint = malloc(sizeof(struct Point));
            westPoint->y = ffy;
            westPoint->x = ffx - 1;
            enqueue(queue, westPoint);
            distances[ffy][ffx - 1] = distances[ffy][ffx] + 1;
        }
        if (ffx < 15 && !hwalls[ffy][ffx + 1] && distances[ffy][ffx + 1] == -1) {
            struct Point* eastPoint = malloc(sizeof(struct Point));
            eastPoint->y = ffy;
            eastPoint->x = ffx + 1;
            enqueue(queue, eastPoint);
            distances[ffy][ffx + 1] = distances[ffy][ffx] + 1;
        }
        free(current);
    }
    free(queue);
    // fprintf(stderr, "rewriting maze numbers\n");
    // fflush(stderr);
    char* str = malloc(3);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            API_setText(j, 15 - i, itoa(distances[i][j], str, 10));
            // fprintf(stderr, "%d ", distances[i][j]);
        }
        // fprintf(stderr, "\n");
    }
    free(str);
}
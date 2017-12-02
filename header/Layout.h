//
// display warehouse, separate process and shared memory
//

#ifndef AMAZOOM_LAYOUT_H
#define AMAZOOM_LAYOUT_H

#define MAZE_MEMORY_NAME "shared_memory"
#define MAZE_MUTEX_NAME "shared_memory_mutex"

#define WALL_CHAR 'X'
#define EMPTY_CHAR ' '
#define EXIT_CHAR 'E'

#define COL_IDX 0
#define ROW_IDX 1

#define ROBOT_HOME_COL = A
#define ROBOT_HOME_ROW = 1

#define MAX_LAYOUT_COL 10
#define MAX_LAYOUT_ROW 10
#define MAX_ROBOTS   50

#define MAGIC_NUMBER 1373858591

#include "Cell.h"
#include "Coordinate.h"
#include <map>

struct LayoutInfo {
    int rows;           // rows in warehouse
    int cols;           // columns in warehouse
    char layout[MAX_LAYOUT_COL][MAX_LAYOUT_ROW];  // warehouse storage
};

struct RobotInfo {
    int nrobots;      // number robots
    int rloc[MAX_ROBOTS][2];   // robot locations [col][row]
};

struct SharedData {
    LayoutInfo linfo;    // layout info
    RobotInfo rinfo;  // robot info
    bool quit;         // tell everyone to quit
    int  magic;        // magic number for detecting initialization
};

class Layout{
    protected:
        std::map<std::string, Cell> coordinate2cell;        // map coordinate to storage cells
};

#endif //AMAZOOM_LAYOUT_H

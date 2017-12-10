//
// Layout.h
// display warehouse, separate process and shared memory
//

#ifndef AMAZOOM_LAYOUT_H
#define AMAZOOM_LAYOUT_H

#include "Amazoom.h"

struct LayoutInfo {
    int rows;           // rows in warehouse
    int cols;           // columns in warehouse
    char layout[MAX_LAYOUT_COL][MAX_LAYOUT_ROW];  // warehouse storage
};

struct RobotInfo {
    int nrobots;      // number robots
    int rloc[MAX_ROBOT][2];   // robot locations [col][row]
};

struct SharedData {
    LayoutInfo linfo;    // layout info
    RobotInfo rinfo;  // robot info
    bool quit;         // tell everyone to quit
    int  magic = 0;        // magic number for detecting initialization
};

#endif /*AMAZOOM_LAYOUT_H*/

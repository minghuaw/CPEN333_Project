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
	int loading[2];		// loading bay location
	int unloading[2];   // loading bay location
	int home[2];		// robot home location
};

struct RobotInfo {
    int nrobots;      // number robots
    int rloc[MAX_ROBOTS][2];   // robot locations [col][row]
};

struct TruckInfo {
	int ndtrucks;		// number delivery trucks
	int nrtrucks;		// number restock trucks
	char dtruckStatus[MAX_TRUCKS];		// status of truck, 'A' for arrival and 'D' for departure
	char rtruckStatus[MAX_TRUCKS];
	double dcapcity[MAX_TRUCKS];		// capacity of trucks
	double rcapcity[MAX_TRUCKS];
	double dweight[MAX_TRUCKS];			// weight of trucks
	double rweight[MAX_TRUCKS];
};

struct SharedData {
    LayoutInfo linfo;   // layout info
    RobotInfo rinfo;	// robot info
	TruckInfo tinfo;	// truck info
    bool quit;			// tell everyone to quit
    int  magic = 0;     // magic number for detecting initialization
};

#endif /*AMAZOOM_LAYOUT_H*/

//
//  Amazoom.h
//  common definitions
//

#ifndef AMAZOOM_H
#define AMAZOOM_H

#define DB_MUTEX_ID "DB_MUTEX"
#define SHARED_LAYOUT_ID "LAYOUT"
#define LAYOUT_MAGIC_KEY 29785699
#define LOADING_BAY_SEM_RESOURCE 1 // only one loading dock is available
#define LOADING_BAY_NAME "loading"
#define UNLOADING_BAY_NAME "unloading"
#define LAYOUT_0 "../data/layout0.txt"
#define WAREHOUSE_NUM 2
#define MAX_ROBOT 50
#define LAYOUT_MEMORY_NAME "amazoom_layout"
#define LAYOUT_MEMORY_MUTEX_NAME "amazoom_layout_mutex"

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

#endif // AMAZOOM_H
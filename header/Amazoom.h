//
//  Amazoom.h
//  common definitions
//

#ifndef AMAZOOM_H
#define AMAZOOM_H

#define DB_MUTEX_NAME "DB_MUTEX_NAME"
#define SHARED_LAYOUT_ID "LAYOUT"
#define LAYOUT_MAGIC_KEY 29785699
#define LOADING_BAY_SEM_RESOURCE 1 // only one loading dock is available
#define LOADING_BAY_NAME "loading"
#define UNLOADING_BAY_NAME "unloading"
#define WAREHOUSE_NUM 2

#define LOCAL_HOST "127.0.0.1"
#define CLIENT_PORT 52102
#define WAREHOUSE_PORT 51102

#define WAREHOUSE_MEMORY_NAME "amazoom_layout"
#define WAREHOUSE_MEMORY_MUTEX_NAME "amazoom_layout_mutex"

#define WALL_CHAR 'X'
#define EMPTY_CHAR ' '
#define HOME_CHAR 'H'
#define LOADINGBAY_CHAR 'L'
#define UNLOADINGBAY_CHAR 'U'
#define SHELF_CHAR '|'

#define COL_IDX 0
#define ROW_IDX 1
#define SHELF_LEVEL 3 // each shelf has 3 levels
#define CELL_CAPACITY 100000 // each cell can hold 75kg items

#define MAX_LAYOUT_COL 80
#define MAX_LAYOUT_ROW 80

#define MAX_ROBOTS 50
#define ROBOT_CAPACITY 15 // each robot can carry 15kg items

#define MAX_TRUCKS 5
#define TRUCK_ARRIVAL 'A'
#define TRUCK_DEPARTURE 'D'
#define NO_DTRUCK_DOCKED '-1'
#define TRUCK_CAPACITY 450		// 450Kg accodring to federal regislation
#define TRUCK_CAPACITY_VARIATION 50	// 100 kg variation
#define FULL_RATIO 0.8

#define POISION_ID "-1"
#define LOW_STOCK_ALERT 5

#define ITEM_LIST "../data/itemList.txt"

#endif // AMAZOOM_H
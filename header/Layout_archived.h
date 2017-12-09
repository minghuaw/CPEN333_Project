//
//  Cell.h
//

#ifndef LAYOUT_H
#define LAYOUT_H

#define MAX_LAYOUT_ROW 10
#define MAX_LAYOUT_COL 10
#define ROBOT_HOME_COL = A
#define ROBOT_HOME_ROW = 1

#include "Cell.h"
#include <map>

class Layout{
    protected:
        char layoutInfo[MAX_LAYOUT_ROW][MAX_LAYOUT_COL];
        std::map<std::string, Cell> coordinate2cell;
    public:
    
};
#endif


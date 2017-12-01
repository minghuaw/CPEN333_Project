#ifndef COORDINATE_H
#define COORDINATE_H

#include <string>

class Coordinate {
private:
    char col;               // column number "A, B, C ..."
    int row;                // row number "1, 2, 3 ....."
    char side;              // side on the shelf "L, R"
    int shelf;              // shelf level "1, 2, 3 ...."
public:
    /**
     * Constructor for Coordinate class
     * @param col column number, captitalized char from 'A' to 'Z'
     * @param row row number, positive integer
     * @param side side, char of either 'L' or 'R'
     * @param shelf shelf level, positive integer
     */
    Coordinate(char col, int row, char side, int shelf) {}

    /**
     * return the col number
     * @return col number, char from 'A' to 'Z'
     */
    char getCol(){}

    /**
     * return row number
     * @return row number
     */
    int getRow(){}

    /**
     * return side
     * @return side, char 'L' or 'R'
     */
    char getSide(){}

    /**
     * return shelf level number
     * @return shelf level
     */
    int getShelf(){}

    /**
     * convert Coordinate to string
     * @return std::string in format "[col][row][side][shelf]"
     */
    std::string toString(){}

};

#endif
#ifndef COORDINATE_H
#define COORDINATE_H

#include <string>

class Coordinate {
private:
    char col;               // col number "A, B, C ..."
    int row;                // row number "0, 1, 2 ....."
    char side;              // side on the shelf "L, R"
    int shelf;              // shelf level "0, 1, 2..."

	
public:
	
	/** 
	* default constructor
	*/
	Coordinate() {};
    /**
     * Constructor for Coordinate class
     * @param col column number, captitalized char from 'A' to 'Z'
     * @param row row number, positive integer
     * @param side side, char of either 'L' or 'R'
     * @param shelf shelf level, positive integer
     */
    Coordinate(int col_num, int row, char side, int shelf):row(row),side(side),shelf(shelf) {
		col = 'A' + col_num;
	}

    /**
     * return the col number
     * @return col number, char from 'A' to 'Z'
     */
    char getCol(){
		return col;
	}

    /**
     * return row number
     * @return row number
     */
    int getRow(){
		return row;
	}

    /**
     * return side
     * @return side, char 'L' or 'R'
     */
    char getSide(){
		return side;
	}

    /**
     * return shelf level number
     * @return shelf level
     */
    int getShelf(){
		return shelf;
	}

    /**
     * convert Coordinate to string
     * @return std::string in format "[col][row][side][shelf]"
     */
    std::string toString(){
		std::string s;
		s += col;
		s += std::to_string(row);
		s += side;
		s += std::to_string(shelf);

		return s;
	}

};

#endif
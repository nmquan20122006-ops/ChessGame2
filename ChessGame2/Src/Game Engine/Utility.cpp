#include"Utility.h"

Position::Position(int row, int col) :row(row), col(col) {};

bool Position::operator==(const Position other)const {

	return row == other.row && col == other.col;

}

bool Position::operator!=(const Position other)const {

	return !(*this == other);

}



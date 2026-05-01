#include"Utility.h"
#include"Board.h"
#include"State/GameState.hpp"

Position::Position(int row, int col) :row(row), col(col) {};

bool Position::operator==(const Position other)const {

	return row == other.row && col == other.col;

}

bool Position::operator!=(const Position other)const {

	return !(*this == other);

}


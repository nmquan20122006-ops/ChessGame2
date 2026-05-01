#pragma once
#include"Type.hpp"
#include"Move.hpp"
#include"Board.h"
#include<memory>
#include<string>

struct UndoEntry {

	std::shared_ptr<Board> boardBefore;
	Move moveBefore;
	Color turnBefore = Color::none;
	std::string fenBefore;
	int halfMoveClockCountBefore = 0;
	int fullMoveNumberCountBefore = 1;
};
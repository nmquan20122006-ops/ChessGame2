#pragma once
#include"State/GameState.hpp"

class Board;
class MoveExecutor;
class MoveService;

class PromotionController {
public:
	PromotionController(std::shared_ptr<Board> b, std::shared_ptr<GameState> g,
		std::shared_ptr<MoveService>m, MoveExecutor& r);

	bool prepare(Position from,Position to);

	Move execute(Piece selectedPiece);

	bool isPending()const;
private:

	std::shared_ptr<Board> board;
	std::shared_ptr<GameState> gameState;
	std::shared_ptr<MoveService> moveService;
	MoveExecutor& moveExecutor;
};
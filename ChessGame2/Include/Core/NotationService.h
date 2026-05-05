#pragma once
#include<string>
#include<memory>
#include"Utility.h"
#include"Piece.h"

class GameState;
class MoveService;
class Board;

class ChessNotation {
private:
	std::string		posToSAN(const Position pos);
	char			pieceToSAN(const Piece piece);

public:
	ChessNotation();
	
	std::string fullSAN(Move& move, std::shared_ptr<GameState> gameState, 
		std::shared_ptr<Board> board, std::shared_ptr<MoveService> moveService);

	std::string disAmbiguation(Move& move, std::shared_ptr<Board> board, std::shared_ptr<MoveService> moveService);
};
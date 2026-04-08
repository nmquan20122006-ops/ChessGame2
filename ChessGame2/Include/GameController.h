#pragma once

#include"Board.h"
#include"MoveValidator.h"
#include<vector>
#include"AllMoveValid.h"
#include"Define.h"

struct DragState {

	bool isDragging = false;
	Position fromPos = { -1,-1 };
	Piece draggingPiece = Piece::Empty;
	sf::Vector2f mousePosition;
	std::vector<Position> activeValidMoves;
	Position currentHover = { -1,-1 };
};

class GameController {

private:
	Board& board;

	DragState drag;

	bool isSelecting;

	bool isCheck, isCheckMate;

	Position isSelectingPos;

	Position checkPosition;

	MoveValidator moveValidator;

	color currentTurn = color::white;

	std::vector<Position> getAllValidMoves;

	AllValidMove allValidMove;

public:
	GameController(Board& board);

	moveType handleSquareClick(Position pos);

	moveType makeMove(Position fromPos, Position toPos);

	void updateGameState();

	color getColor(Piece p);

	bool getIsSelecting()const { return isSelecting; }

	bool getIsCheck()const { return isCheck; }

	Position getIsSelectingPosition()const { return isSelectingPos; }

	std::vector<Position>getCurrentValidMoves()const { return getAllValidMoves; }

	color getTurn()const { return currentTurn; }

	Position getCheckPosition()const { return checkPosition; }
	

	bool canStartDrag(Position pos);

	void resetDrag();

	void startDrag(Position pos, sf::Vector2f worldPos);
	
	void updateDrag(sf::Vector2f worldPos,Position pos);

	moveType endDrag(Position toPos);

	const DragState& getDragState()const { return drag; }
	
	bool CheckMate(bool isWhiteTurn, const Board& board);

	Position getStartPos()const { return drag.fromPos; }
};

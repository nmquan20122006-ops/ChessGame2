#pragma once
#include"Utility.h"
#include"Define.h"

struct DragState {

	bool isActive = false;
	bool isDragging = false;
	
	Position fromPos = { -1,-1 };

	Piece draggingPiece = Piece::Empty;

	sf::Vector2f mousePosition;

	void reset() {
		isActive = false;
		fromPos = { -1,-1 };
		draggingPiece = Piece::Empty;
	}
};

struct GameState {

	color currentTurn = color::white;

	bool isCheck = false;
	bool isCheckMate = false;

	Position selectedPos = { -1,-1 };
	Position checkPos;

	std::vector<Position>validMoves;

	void clearSelection() {
		selectedPos = { -1,-1 };
		validMoves.clear();
	}

	bool hasSelection()const {
		return selectedPos.row != -1;
	}

	color getColor(Piece p) const {

		if ((int)p >= 1 && (int)p <= 6)return color::white;

		return color::black;
	}

	DragState drag;
};


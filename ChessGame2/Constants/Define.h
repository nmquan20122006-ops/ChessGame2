#pragma once
// Defines.h
#pragma once

enum class color { black, white, none };

enum class moveType {
    none, select, move, capture, check, checkMate, unValid
};

enum class MoveType {
	normal, castle, enPassant, promotion, none
};

struct Move {

	Position fromPos;
	Position toPos;

	MoveType type;

	Piece promotionPiece;
	Piece movedPiece;
	Piece capturedPiece;

};

struct BotMove {

	Position fromPos, toPos;
};

struct isOnProcess {
	
	bool isProcessing = false;
	bool isAnimating = false;

	bool canAcceptInput() {

		return !isProcessing && !isAnimating;
	}
};

struct PendingMove {

	Position from = { -1,-1 }, to = { -1,-1 };
	Piece piece = Piece::Empty;
	bool isActive = false;
};

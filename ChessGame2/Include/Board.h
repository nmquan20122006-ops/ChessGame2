#pragma once

#include"Piece.h"
#include"Utility.h"
#include"../Constants/Constants.h"

struct LastMove {

	Position from = { -1,-1 };
	Position to = { -1,-1 };

};

struct CastleState {

	bool whiteKingMoved = false;
	bool blackKingMoved = false;
	
	bool whiteKingRookMoved = false;
	bool blackKingRookMoved = false;

	bool whiteQueenRookMoved = false;
	bool blackQueenRookMoved = false;

	bool canWhiteCastleKingSide()const {
		return !whiteKingRookMoved && !whiteKingMoved;
	}

	bool canBlackCastleKingSide()const {
		return !blackKingRookMoved && !blackKingMoved;
	}

	bool canWhiteCastleQueenSide()const {
		return !whiteQueenRookMoved && !whiteKingMoved;
	}

	bool canBlackCastleQueenSide()const {
		return !blackQueenRookMoved && !blackKingMoved;
	}

};

class Board {
private:

	Piece board[8][8] = {};

	Position whiteKingPos, blackKingPos;

	CastleState castleState;

	std::vector<Position> w_PieceList;
	std::vector<Position> b_PieceList;

public:

	Board();
	void initBoard();
	void setPiece(Position pos,Piece p);

	Piece getPiece(Position pos) const;
	bool isEmpty(Position pos) const;
	bool isInside(Position pos) const;

	void deletePiece(Position pos);
	void resetBoard();

	void movePiece(Position fromPos, Position toPos);

	LastMove lastMove;
	void updateLastMove(LastMove move);
	LastMove getLastMove() const { return lastMove; }

	Position findKing(bool isWhiteKing) const ;
	void updateKingPosition(Position toPos,Piece& p);

	void updateCastleState(Piece p,Position pos);
	CastleState getCastleState()const { return castleState; }

	const std::vector<Position>& getPositionExist(bool white) const;
	void updatePosInVector(Position p, Position p1, bool isWhite);
	void removeFromVector(std::vector<Position>& v, Position p);

};



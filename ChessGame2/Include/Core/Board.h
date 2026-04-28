#pragma once
#include"Piece.h"
#include"Utility.h"
#include"../Constants/Constants.h"
#include"GameState.h"

class Board_DEBUG;
class BoardToFEN_DEBUG;

struct LastMove {

	Position from = { -1,-1 };
	Position to = { -1,-1 };

	void reset() { from = { -1,-1 };to = { -1,-1 }; }

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

	bool noMoreCastle()const {
		return !canWhiteCastleKingSide() && !canWhiteCastleQueenSide() &&
			!canBlackCastleKingSide() && !canBlackCastleQueenSide();
	}

	void reset() {
		whiteKingMoved = false;
		blackKingMoved = false;
		whiteKingRookMoved = false;
		blackKingRookMoved = false;
		whiteQueenRookMoved = false;
		blackQueenRookMoved = false;
	}

	void resetKingSide() {
		whiteKingRookMoved = false;
		whiteKingMoved = false;
	}

};

class Board {

	friend class Board_DEBUG;
	friend class BoardToFEN_DEBUG;

private:

	Piece board[8][8] = {};

	Position whiteKingPos, blackKingPos;

	CastleState castleState;
	
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
	void updateKingPosition(Position toPos,const Piece& p);

	void updateCastleState(Piece p,Position pos);
	void undoCastleState(Piece p, Position pos);
	void updateCastleStateOnCapture(Piece capturedPiece, Position capturePos);
	void undoCastleStateOnCapture(Piece capturedPiece, Position capturePos);
	CastleState getCastleState()const { return castleState; }

	void setLastMove(Position from,Position to);
	void resetLastMove() { lastMove.reset(); }

	auto resetC_KingSide() { return castleState.resetKingSide(); }

};

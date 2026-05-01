/**
* @file Board.h
* @brief This file defines the Board class, which represents the chessboard and manages the placement and movement of pieces on the board.
* @brief The Board class includes methods for initializing the board, setting and getting pieces, checking if a position is empty or inside the board, deleting pieces, resetting the board, moving pieces, and managing the last move and castling state.
* @author [Nguyen Minh Quan]
* @date [13/2/2026]
* @version [1.0]
* copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
*/
#pragma once
#include"Piece.h"
#include"Utility.h"
#include"Constants.h"

class Board_DEBUG;
class BoardToFEN_DEBUG;

struct LastMove {

		Position from = { -1,-1 };
		Position to = { -1,-1 };

		void reset() { from = { -1,-1 }; to = { -1,-1 }; }

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

		void resetWhiteKingSide() {
			whiteKingRookMoved = false;
			whiteKingMoved = false;
		}

		void resetWhiteQueenSide() {
			whiteQueenRookMoved = false;
			whiteKingMoved = false;
		}

		void resetBlackKingSide() {
			blackKingRookMoved = false;
			blackKingMoved = false;
		}

		void resetBlackQueenSide() {
			blackQueenRookMoved = false;
			blackKingMoved = false;
		}

	};

class Board {

	friend class Board_DEBUG;
	friend class BoardToFEN_DEBUG;

private:
	Piece		 board[8][8] = {};
	Position	 whiteKingPos, blackKingPos;
	CastleState	 castleState;

public:
	// =========================================================
	// Constructors & copy contructor
	// =========================================================
	Board();
	Board(const Board& other);
	// =========================================================
	// Board setup
	// =========================================================
	void		 initBoard();
	void		 setPiece(Position pos, Piece p);
	void		 resetBoard();
	// =========================================================
	// Board queries and actions
	// =========================================================
	Piece		 getPiece(Position pos)	const;
	bool		 isEmpty(Position pos) const;
	bool		 isInside(Position pos) const;
	void		 deletePiece(Position pos);
	void		 movePiece(Position fromPos, Position toPos);
	// =========================================================
	// last move statement management
	// =========================================================
	LastMove	 lastMove;
	void		 updateLastMove(LastMove move);
	LastMove	 getLastMove() const		{ return lastMove; }
	void		 setLastMove(Position from, Position to);
	void		 resetLastMove()			{ lastMove.reset(); }
	// =========================================================
	// king position and king move management
	// =========================================================
	Position	 findKing(bool isWhiteKing) const;
	void		 updateKingPosition(Position toPos, const Piece& p);
	// =========================================================
	// castling state management
	// =========================================================
	CastleState	 getCastleState()const		{ return castleState; }
	void		 updateCastleState(Piece p, Position from, Position to, Piece capturedPiece);
	auto		 resetWhiteKingSide()		{ return castleState.resetWhiteKingSide(); }
	auto		 resetWhiteQueenSide()		{ return castleState.resetWhiteQueenSide(); }
	auto		 resetBlackQueenSide()		{ return castleState.resetBlackQueenSide(); }
	auto		 resetBlackKingSide()		{ return castleState.resetBlackKingSide(); }
};
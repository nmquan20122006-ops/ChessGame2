/**
* @file MoveExecutor.h
* @brief This file defines the MoveExecutor class, which is responsible for applying and undoing moves on the chess board. The MoveExecutor class includes methods to execute different types of moves, such as normal moves, castling, en passant, and promotion.
* @brief It also provides functionality to undo moves, including special moves like castling. The class interacts with the Board class to update the game state based on the moves executed.
* @author [Nguyen Minh Quan]
* @date [29/4/2026]
* @version [1.0]
* @copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
*/
#pragma once
#include"State/GameState.hpp"
#include<memory.h>

class Board;

class MoveExecutor {
private:
	std::shared_ptr<Board> board;
	std::shared_ptr<GameState>gameState;

	MoveType moveType = MoveType::none;

	MoveType	implementCastle(Move& move, Piece& piece);
	MoveType	implementPromotion(Move& move, Piece& piece);
	MoveType	implementEnPassant(Move& move, Piece& piece);

public:
	MoveExecutor(std::shared_ptr<Board> b, std::shared_ptr<GameState> g);

	void		applyMove(Move& move);
	void		recordPrevBoard(const Move& moveBefore);
	void		syncAfterUndo(const UndoEntry& undoEntry);

	int			halfMoveClockProcess(int prevClock, const Move& move);
	int			fullMoveNumberProcess(int prevClock, const Color currentTurn);

	void		updateEventBus();
};
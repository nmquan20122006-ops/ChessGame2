#pragma once
#include<iostream>
#include<memory>
#include<vector>
#include<string>

#include"State/Move.hpp"
class Board;
class GameState;
class ChessNotation;
class MoveService;

struct UndoEntry {

	std::shared_ptr<Board>		boardBefore;
	Move						moveBefore;
	Color						turnBefore = Color::none;
	std::string					fenBefore;
	int							halfMoveClockCountBefore = 0;
	int							fullMoveNumberCountBefore = 1;
	Position					checkPosBefore = { -1, -1 };
	bool						isCheckBefore = false;
};

struct MoveRecord {

	Move		move;
	std::string fen;
	std::string san;
	Color		turn;
	int			moveNumber;
};

class MoveLog {

	friend class MoveExecutor;

private:
	std::vector<UndoEntry>		undoStack;
	std::vector<MoveRecord>		moveHistoryStack;

	std::shared_ptr<Board>		board;
	std::shared_ptr<GameState>	gameState;
	ChessNotation&				chessNotation;
	std::shared_ptr<MoveService>moveService;
public:
	MoveLog(std::shared_ptr<Board> b, std::shared_ptr<GameState> s, ChessNotation& c, std::shared_ptr<MoveService>m);

	void		savePrevBoard(Move& move);
	void		syncGame(UndoEntry& undoEntry);
	void		record(Move& move);
	void		undoHistory();
	UndoEntry	popUndo();

	std::string	toPGN();

	const auto& getMoveHistoryStack() const { return moveHistoryStack; }
	const auto& getUndoStack() const { return undoStack; }
};
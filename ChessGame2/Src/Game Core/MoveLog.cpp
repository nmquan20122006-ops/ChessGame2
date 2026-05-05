#include"MoveLog.h"
#include"Board.h"
#include"State/GameState.hpp"
#include"NotationService.h"

MoveLog::MoveLog(std::shared_ptr<Board> b, std::shared_ptr<GameState> s, ChessNotation& c, std::shared_ptr<MoveService>m):
    board(b), gameState(s), chessNotation(c), moveService(m){}

void MoveLog::savePrevBoard(Move& prevMove) {
	
    std::shared_ptr<Board> prevBoard = std::make_shared<Board>(*board);

    UndoEntry undoEntry;

    undoEntry.boardBefore = prevBoard;
    undoEntry.moveBefore = prevMove;
    undoEntry.turnBefore = gameState->getCurrentTurn();
    undoEntry.fenBefore = gameState->getCurrentFEN();
    undoEntry.halfMoveClockCountBefore = gameState->getHalfMoveClockCount();
    undoEntry.fullMoveNumberCountBefore = gameState->getFullMoveNumberCount();
    undoEntry.checkPosBefore = gameState->getCheckPos();
    undoEntry.isCheckBefore = gameState->getIsCheck();

    undoStack.push_back(undoEntry);
}

void MoveLog::syncGame(UndoEntry& undoEntry) {

    *this->board = *undoEntry.boardBefore;
    gameState->setCurrentTurn(undoEntry.turnBefore);
    gameState->setCurrentFEN(undoEntry.fenBefore);
    gameState->setHalfMoveClockCount(undoEntry.halfMoveClockCountBefore);
    gameState->setFullMoveNumberCount(undoEntry.fullMoveNumberCountBefore);
    gameState->setCheckPos(undoEntry.checkPosBefore);
    gameState->setIsCheck(undoEntry.isCheckBefore);
    gameState->setIsCheckMate(false);

}

void MoveLog::record(Move& move) {

    MoveRecord record;
    
    record.move = move;
    record.san = chessNotation.fullSAN(move, gameState, board, moveService);
    record.fen = gameState->getCurrentFEN();
    record.turn = gameState->getCurrentTurn();
    record.moveNumber = (moveHistoryStack.size() / 2) + 1;

    moveHistoryStack.push_back(record);
}

void MoveLog::undoHistory() {

    if (!moveHistoryStack.empty()) moveHistoryStack.pop_back();
}

UndoEntry MoveLog::popUndo() {

    UndoEntry undoEntry = undoStack.back();
    undoStack.pop_back();

    return undoEntry;
}

std::string MoveLog::toPGN() {

    std::string PGN;
    for (int i = 0; i < moveHistoryStack.size(); i++) {
        if (i % 2 == 0)
            PGN += std::to_string(i / 2 + 1) + ". ";
        PGN += moveHistoryStack[i].san + " ";
    }
    return PGN;
}
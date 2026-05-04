#include "MoveExecutor.h"
#include "MoveValidator.h"
#include "Board.h"
#include "EventBus.h"

MoveExecutor::MoveExecutor(std::shared_ptr<Board> b, std::shared_ptr<GameState> g) : board(b), gameState(g) {}

MoveType MoveExecutor::implementEnPassant(Move& move, Piece& piece) {

    if (!MoveValidator::isCanEnPassant(move.fromPos, move.toPos, *board))return MoveType::none;

    LastMove lastMove = board->getLastMove();
    Piece lastPiece = board->getPiece(lastMove.to);

    move.enPassantCapturedPos = lastMove.to;
    move.capturedPiece = lastPiece;
    move.moveType = MoveType::enPassant;

    board->deletePiece(lastMove.to);

    return MoveType::enPassant;
}

MoveType MoveExecutor::implementCastle(Move& move, Piece& piece) {
    
    if (!MoveValidator::isCanCastle(move.fromPos, move.toPos, *board))return MoveType::none;

    int row = move.fromPos.row;

    board->setPiece(move.toPos, piece);
    board->deletePiece(move.fromPos);
    board->updateKingPosition(move.toPos, piece);

    if (move.toPos.col == 6) {                     
        Position oldRookPos = { row, 7 };
        Position newRookPos = { row, 5 };

        Piece rook = board->getPiece(oldRookPos);

        board->setPiece(newRookPos, rook);
        board->deletePiece(oldRookPos);
    }
    else {                                          
        Position oldRookPos = { row, 0 };
        Position newRookPos = { row, 3 };

        Piece rook = board->getPiece(oldRookPos);

        board->setPiece(newRookPos, rook);
        board->deletePiece(oldRookPos);
    }

    board->updateLastMove({ move.fromPos,move.toPos });

    return MoveType::castle;
}

MoveType MoveExecutor::implementPromotion(Move& move, Piece& piece) {

    bool isWhitePromotion = (piece == Piece::W_Pawn && move.toPos.row == 0);
    bool isBlackPromotion = (piece == Piece::B_Pawn && move.toPos.row == 7);

    if (isWhitePromotion || isBlackPromotion) {
      
        if (move.promotionPiece != Piece::Empty) {
            piece = move.promotionPiece;
        }
        else {
            piece = (isWhitePromotion) ? Piece::W_Queen : Piece::B_Queen;
        }

        return MoveType::promotion;
    }

    return MoveType::none;
}

void MoveExecutor::applyMove(Move& move) {

    Piece piece = move.movedPiece;

    if (implementEnPassant(move, piece) == MoveType::enPassant) {

        recordPrevBoard(move);

        board->movePiece(move.fromPos, move.toPos);

        gameState->setHalfMoveClockCount(halfMoveClockProcess(gameState->getHalfMoveClockCount(), move));
        gameState->setFullMoveNumberCount(fullMoveNumberProcess(gameState->getFullMoveNumberCount(), gameState->getCurrentTurn()));

        board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
        return;
    }
    
    if (implementCastle(move, piece) == MoveType::castle) {
        board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
        return;
    }

    recordPrevBoard(move);

    board->movePiece(move.fromPos, move.toPos);

    if (implementPromotion(move, piece) == MoveType::promotion) {
        board->setPiece(move.toPos, piece);
        board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
        return;
    }

    gameState->setHalfMoveClockCount(halfMoveClockProcess(gameState->getHalfMoveClockCount(), move));
    gameState->setFullMoveNumberCount(fullMoveNumberProcess(gameState->getFullMoveNumberCount(), gameState->getCurrentTurn()));

    board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
}

void MoveExecutor::recordPrevBoard(const Move& moveBefore) {

    std::shared_ptr<Board> prevBoard = std::make_shared<Board>(*board);

    UndoEntry undoEntry;
    undoEntry.boardBefore = prevBoard;
    undoEntry.moveBefore = moveBefore;
    undoEntry.turnBefore = gameState->getCurrentTurn();
    undoEntry.fenBefore = gameState->getCurrentFEN();
    undoEntry.halfMoveClockCountBefore = gameState->getHalfMoveClockCount();
    undoEntry.fullMoveNumberCountBefore = gameState->getFullMoveNumberCount();
    gameState->pushToUndoStack(undoEntry);
}

void MoveExecutor::syncAfterUndo(const UndoEntry& undoEntry) {

    *this->board = *undoEntry.boardBefore;
    gameState->setCurrentTurn(undoEntry.turnBefore);
    gameState->setCurrentFEN(undoEntry.fenBefore);
    gameState->setHalfMoveClockCount(undoEntry.halfMoveClockCountBefore);
    gameState->setFullMoveNumberCount(undoEntry.fullMoveNumberCountBefore);
}

int MoveExecutor::halfMoveClockProcess(int prevClock, const Move& move) {

    if (move.movedPiece == Piece::B_Pawn || move.movedPiece == Piece::W_Pawn || move.capturedPiece != Piece::Empty) {
        return 0;
    }
    return prevClock + 1;
}

int MoveExecutor::fullMoveNumberProcess(int prevClock, const Color currentTurn) {

    if (currentTurn == Color::black)return prevClock + 1;
    return prevClock;
}
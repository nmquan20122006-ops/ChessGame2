#include "MoveExecutor.h"
#include "MoveValidator.h"
#include "Board.h"

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
        board->movePiece(move.fromPos, move.toPos);
        move.moveType = MoveType::enPassant;
        board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
        return;
    }
    
    if (implementCastle(move, piece) == MoveType::castle) {
        move.moveType = MoveType::castle;
        board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
        return;
    }

    recordPrevBoard(move);

    board->movePiece(move.fromPos, move.toPos);

    if (implementPromotion(move, piece) == MoveType::promotion) {
        board->setPiece(move.toPos, piece);
        move.moveType = MoveType::promotion;
        board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
        return;
    }

    gameState->halfMoveClockCount = halfMoveClockProcess(gameState->halfMoveClockCount, move);
    gameState->fullMoveNumberCount = fullMoveNumberProcess(gameState->fullMoveNumberCount, gameState->getCurrentTurn());

    board->updateCastleState(piece, move.fromPos, move.toPos, move.capturedPiece);
    move.moveType = MoveType::normal;
}

void MoveExecutor::recordPrevBoard(const Move& moveBefore) {
    //create a previous board
    std::shared_ptr<Board> prevBoard = std::make_shared<Board>(*board);
    //create a undoEntry
    UndoEntry undoEntry;
    undoEntry.boardBefore = prevBoard;
    undoEntry.moveBefore = moveBefore;
    undoEntry.turnBefore = gameState->currentTurn;
    undoEntry.fenBefore = gameState->currentFEN;
    undoEntry.halfMoveClockCountBefore = gameState->halfMoveClockCount;
    undoEntry.fullMoveNumberCountBefore = gameState->fullMoveNumberCount;
    gameState->pushToUndoStack(undoEntry);
}

void MoveExecutor::syncAfterUndo(const UndoEntry& undoEntry) {

    *this->board = *undoEntry.boardBefore;
    gameState->currentTurn = undoEntry.turnBefore;
    gameState->currentFEN = undoEntry.fenBefore;
    gameState->halfMoveClockCount = undoEntry.halfMoveClockCountBefore;
    gameState->fullMoveNumberCount = undoEntry.fullMoveNumberCountBefore;
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

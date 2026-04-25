#include "MoveExecutor.h"

MoveExecutor::MoveExecutor(Board& b) : board(b) {}

MoveType MoveExecutor::implementEnPassant(Move& move, Piece& piece) {

    if (getType(piece) != Piece::W_Pawn && getType(piece) != Piece::B_Pawn)
        return MoveType::none;

    if (move.fromPos.col == move.toPos.col)
        return MoveType::none;

    if (!board.isEmpty(move.toPos))
        return MoveType::none;

    LastMove lastMove = board.getLastMove();

    if (lastMove.from.row == -1)
        return MoveType::none;

    Piece lastPiece = board.getPiece(lastMove.to);

    if (getType(lastPiece) != Piece::W_Pawn && getType(lastPiece) != Piece::B_Pawn)
        return MoveType::none;
    if (std::abs(lastMove.from.row - lastMove.to.row) != 2)
        return MoveType::none;
    if (lastMove.to.row != move.fromPos.row)
        return MoveType::none;
    if (std::abs(lastMove.to.col - move.fromPos.col) != 1)
        return MoveType::none;

    int direction = isWhite(piece) ? -1 : 1;
    if (move.toPos.row != lastMove.to.row + direction)
        return MoveType::none;
    if (move.toPos.col != lastMove.to.col)
        return MoveType::none;

    move.enPassantCapturedPos = lastMove.to;
    move.capturedPiece = lastPiece;

    board.setPiece(lastMove.to, Piece::Empty);
    return MoveType::enPassant;
}

MoveType MoveExecutor::implementCastle(Move& move, Piece& piece) {

    if (piece != Piece::W_King && piece != Piece::B_King)
        return MoveType::none;
    if (std::abs(move.toPos.col - move.fromPos.col) != 2)
        return MoveType::none;

    int row = move.fromPos.row;

    if (move.toPos.col == 6) {                        // Kingside
        Position oldRookPos = { row, 7 };
        Position newRookPos = { row, 5 };
        Piece rook = board.getPiece(oldRookPos);
        board.setPiece(newRookPos, rook);
        board.setPiece(oldRookPos, Piece::Empty);
        board.updateCastleState(piece, move.fromPos);
        board.updateCastleState(rook, oldRookPos);
    }
    else {                                           // Queenside
        Position oldRookPos = { row, 0 };
        Position newRookPos = { row, 3 };
        Piece rook = board.getPiece(oldRookPos);
        board.setPiece(newRookPos, rook);
        board.setPiece(oldRookPos, Piece::Empty);
        board.updateCastleState(piece, move.fromPos);
        board.updateCastleState(rook, oldRookPos);
    }

    return MoveType::castle;
}

MoveType MoveExecutor::implementPromotion(Move& move, Piece& piece) {\

    if (piece == Piece::W_Pawn && move.toPos.row == 0) {
        piece = Piece::W_Queen;
    }
    else if (piece == Piece::B_Pawn && move.toPos.row == 7) {
        piece = Piece::B_Queen;
    }
    else {
        return MoveType::none;   
    }
    return MoveType::promotion;
}
#include<iostream>
void MoveExecutor::applyMove(Move& move) {

    move.movedPiece = board.getPiece(move.fromPos);
    move.capturedPiece = board.getPiece(move.toPos);

    std::cout << "MovedPiece: " << (int)move.movedPiece << std::endl;
    std::cout << "CapturedPiece: " << (int)move.capturedPiece << std::endl;

    Piece piece = move.movedPiece;

    
    if (implementEnPassant(move, piece) == MoveType::enPassant) {
        board.movePiece(move.fromPos, move.toPos);
        move.type = MoveType::enPassant;
        return;
    }

    board.movePiece(move.fromPos, move.toPos);

    
    if (implementCastle(move, piece) == MoveType::castle) {
        board.updateKingPosition(move.toPos, piece);
        move.type = MoveType::castle;
        return;
    }

    if (implementPromotion(move, piece) == MoveType::promotion) {
        board.setPiece(move.toPos, piece); 
        move.type = MoveType::promotion;
        return;
    }

    move.type = MoveType::normal;
}

void MoveExecutor::undoCastle(const Move& move) {
    int row = move.fromPos.row;

    Piece king = board.getPiece(move.toPos);
    board.setPiece(move.fromPos, king);
    board.deletePiece(move.toPos);
    board.updateKingPosition(move.fromPos, king);

    if (move.toPos.col == 6) {                       
        Piece rook = board.getPiece({ row, 5 });
        board.setPiece({ row, 7 }, rook);
        board.deletePiece({ row, 5 });
        board.resetC_KingSide();
    }
    else {                                           
        Piece rook = board.getPiece({ row, 3 });
        board.setPiece({ row, 0 }, rook);
        board.deletePiece({ row, 3 });
    }
}

void MoveExecutor::undoMove(const Move& move) {

    switch (move.type) {

    case MoveType::castle:
        undoCastle(move);
        break;

    case MoveType::enPassant:
        board.setPiece(move.fromPos, move.movedPiece);
        board.setPiece(move.toPos, Piece::Empty);

        board.setPiece(move.enPassantCapturedPos, move.capturedPiece);
        break;

    case MoveType::promotion:

        board.setPiece(move.fromPos, move.movedPiece);
        board.setPiece(move.toPos, move.capturedPiece);
        break;

    default:
        board.setPiece(move.fromPos, move.movedPiece);
        board.setPiece(move.toPos, move.capturedPiece);
       

        break;
    }
}


void MoveExecutor::redoMove(const Move& move) {
    
    Piece piece = move.movedPiece;

    switch (move.type) {
    case MoveType::castle:
        board.movePiece(move.fromPos, move.toPos);

        board.updateKingPosition(move.toPos, piece);
        break;

    case MoveType::enPassant:
        board.setPiece(move.enPassantCapturedPos, Piece::Empty);
        board.movePiece(move.fromPos, move.toPos);
        break;

    case MoveType::promotion:
        board.movePiece(move.fromPos, move.toPos);
        board.setPiece(move.toPos, Piece::W_Queen);
        break;

    default:
        board.movePiece(move.fromPos, move.toPos);
        board.updateCastleState(piece, move.fromPos);
        board.updateKingPosition(move.toPos, piece);
        break;
    }
}
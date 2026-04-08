#include"MoveService.h"
#include<iostream>

bool MoveService::isUnderAttack(Position kingPos, color targetColor, const Board& board) {

    bool isWhiteTarget = (targetColor == color::white);

    // ===== KNIGHT =====
    const int knightDir[8][2] = {
        {-2,-1},{-2,1},{-1,-2},{-1,2},
        {1,-2},{1,2},{2,-1},{2,1}
    };

    for (auto& d : knightDir) {
        Position pos = { kingPos.row + d[0], kingPos.col + d[1] };

        if (!board.isInside(pos)) continue;

        Piece p = board.getPiece(pos);

        if (p != Piece::Empty && isWhite(p) != isWhiteTarget) {
            if (getType(p) == Piece::B_Knight||getType(p)==Piece::W_Knight) return true;
        }
    }

    // ===== ROOK / QUEEN =====
    const int rookDir[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    for (auto& d : rookDir) {
        int r = kingPos.row + d[0];
        int c = kingPos.col + d[1];

        while (board.isInside({ r, c })) {
            Piece p = board.getPiece({ r, c });

            if (p != Piece::Empty) {

                if (isWhite(p) != isWhiteTarget) {
                    Piece type = getType(p);
                    if (type == Piece::W_Rook || type == Piece::W_Queen
                        || type == Piece::B_Rook || type == Piece::B_Queen)
                        return true;
                }
                break;
            }

            r += d[0];
            c += d[1];
        }
    }

    // ===== BISHOP / QUEEN =====
    const int diagDir[4][2] = { {1,1},{-1,1},{-1,-1},{1,-1} };

    for (auto& d : diagDir) {
        int r = kingPos.row + d[0];
        int c = kingPos.col + d[1];

        while (board.isInside({ r, c })) {
            Piece p = board.getPiece({ r, c });

            if (p != Piece::Empty) {

                if (isWhite(p) != isWhiteTarget) {
                    Piece type = getType(p);
                    if (type == Piece::W_Bishop || type == Piece::W_Queen
                        || type == Piece::B_Bishop || type == Piece::B_Queen)
                        return true;
                }
                break;
            }

            r += d[0];
            c += d[1];
        }
    }

    // ===== KING =====
    const int kingDir[8][2] = {
        {1,1},{-1,-1},{-1,1},{1,-1},
        {0,1},{0,-1},{1,0},{-1,0}
    };

    for (auto& d : kingDir) {
        Position pos = { kingPos.row + d[0], kingPos.col + d[1] };

        if (!board.isInside(pos)) continue;

        Piece p = board.getPiece(pos);
        Piece type = getType(p);

        if (p != Piece::Empty && isWhite(p) != isWhiteTarget) {
            if (type == Piece::W_King || type == Piece::B_King) return true;
        }
    }

    // ===== PAWN =====
    int dir = isWhiteTarget ? -1 : 1;

    Position left = { kingPos.row + dir, kingPos.col - 1 };
    Position right = { kingPos.row + dir, kingPos.col + 1 };

    if (board.isInside(left)) {
        Piece p = board.getPiece(left);
        if (isWhiteTarget && p == Piece::B_Pawn) return true;
        if (!isWhiteTarget && p == Piece::W_Pawn) return true;
    }

    if (board.isInside(right)) {
        Piece p = board.getPiece(right);
        if (isWhiteTarget && p == Piece::B_Pawn) return true;
        if (!isWhiteTarget && p == Piece::W_Pawn) return true;
    }

    return false;
}

bool MoveService::Check(const Board& board, color turn) {

    Position kingPos = board.findKing(turn==color::white);

    return isUnderAttack(kingPos, turn, board);
}

bool MoveService::CheckMate(const Board& board, color turn) {

    Position kingPos = board.findKing(turn == color::white);

    Piece king = board.getPiece(kingPos);

    bool isWhiteKing = isWhite(king);

    if (!Check(board, turn))return false;

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {

            Position currentPos = { r,c };
            Piece currentPiece = board.getPiece(currentPos);

            if (isSameColor(currentPiece, king)) {

                auto moves = generator.getAllValidMoves(currentPos, board);

                for (auto& toPos : moves) {

                    Board tempBoard = board;
                    tempBoard.movePiece(currentPos, toPos);

                    if (!Check(tempBoard, turn)) {

                        return false;

                    }

                }
            }
        }
    }
   

    return true;
}

Move MoveService::createMove(Position fromPos, Position toPos, Board& board) {

    Move move;

    move.fromPos = fromPos;
    move.toPos = toPos;

    move.movedPiece = board.getPiece(fromPos);
    move.capturedPiece = board.getPiece(toPos);

    if (move.movedPiece == Piece::W_King && std::abs(toPos.col - fromPos.col) == 2) {
        move.type = MoveType::castle;
    }

    else if (move.movedPiece == Piece::W_Pawn && fromPos.col != toPos.col && board.getPiece(toPos) == Piece::Empty) {
        move.type = MoveType::enPassant;
    }

    else if (move.movedPiece == Piece::W_Pawn && (toPos.row == 7 || toPos.row == 0)) {
        move.type = MoveType::promotion;
    }
    else {
        move.type = MoveType::normal;
    }

    return move;
}

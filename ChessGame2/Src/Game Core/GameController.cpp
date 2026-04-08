#include"GameController.h"
#include<iostream>

GameController::GameController(Board& board): board(board),isSelecting(false),isSelectingPos(-1,-1),isCheck(false),isCheckMate(false){}

moveType GameController::handleSquareClick(Position position) {

    if (!board.isInside(position)) return moveType::none;
    Piece clickedPiece = board.getPiece(position);

    if (!isSelecting) {

        if (clickedPiece != Piece::Empty && getColor(clickedPiece) == currentTurn) {

            isSelectingPos = position;
            isSelecting = true;
            getAllValidMoves = allValidMove.getAllValidMoves(position, board);

            return moveType::select;
        }

        return moveType::none;
    }
    //isSelecting

    else {

        if (position.row == isSelectingPos.row && position.col == isSelectingPos.col) {

            isSelecting = false;
            return moveType::none;

        }

        if (clickedPiece != Piece::Empty && getColor(clickedPiece) == currentTurn) {

            isSelectingPos = position;
            isSelecting = true;
            getAllValidMoves = allValidMove.getAllValidMoves(isSelectingPos, board);

            return moveType::select;
        }

        else {

            bool isValidTarget = false;

            for (const auto& validPos : getAllValidMoves) {

                if (validPos.row == position.row && validPos.col == position.col) {

                    isValidTarget = true;
                    break;
                }
            }

            if (isValidTarget) {

                moveType result = makeMove(isSelectingPos, position);

                isSelecting = false;
                getAllValidMoves.clear();
                return result;
            }

            isSelecting = false;
            getAllValidMoves.clear();

            return moveType::unValid;
        }
    }
}

color GameController::getColor(Piece p) {

	if ((int)p >= 1 && (int)p <= 6)return color::white;

	return color::black;
}

moveType GameController::makeMove(Position fromPos,Position toPos) {

    moveType result = moveType::move;

    Piece fromPiece = board.getPiece(fromPos);
    Piece targetPiece = board.getPiece(toPos);

    bool isWhitePiece = isWhite(fromPiece);

    if (targetPiece != Piece::Empty) {
        result = moveType::capture;
    }

    if (getType(fromPiece) == Piece::W_Pawn && fromPos.col != toPos.col && board.isEmpty(toPos)) {
        Position victimPos = { fromPos.row, toPos.col };
        board.setPiece(victimPos, Piece::Empty);
        result = moveType::capture;
    }

    board.movePiece(fromPos, toPos);

    getAllValidMoves.clear();

    updateGameState();

    if (this->isCheck)result = moveType::check;
    if (this->isCheckMate)result = moveType::checkMate;

    return result;
}

void GameController::updateGameState() {

	bool isWhiteCheck = (currentTurn == color::white);
    
    bool isWhiteOpponent = !isWhiteCheck;

    if (moveValidator.isInCheck(isWhiteOpponent, board)) {

        isCheck = true;

        checkPosition = board.findKing(isWhiteOpponent);

        if (CheckMate(isWhiteOpponent, board)) {
            isCheckMate = true;
            //
        }
        else {
            isCheckMate = false;
        }
    }
    else {
        checkPosition = { -1,-1 };
        isCheck = false;
    }

    currentTurn = (currentTurn == color::white) ? color::black : color::white;

}

bool GameController::canStartDrag(Position pos) {
       
    Piece p = board.getPiece(pos);

    if (drag.isDragging) return false;

    if (p == Piece::Empty || getColor(p) != currentTurn) return false;

    return true;
}

void GameController::resetDrag() {

    drag.draggingPiece = Piece::Empty;
    drag.isDragging = false;
    drag.fromPos = { -1,-1 };
    drag.activeValidMoves.clear();
}

void GameController::startDrag(Position pos, sf::Vector2f worldPos) {

    if (drag.isDragging && drag.fromPos == pos) return;
    
    if (drag.isDragging && drag.fromPos != pos) {
        resetDrag();
    }

    if (!canStartDrag(pos)) return;

    auto moves = allValidMove.getAllValidMoves(pos, board);

    if (moves.empty()) return;

    drag.isDragging = true;
    drag.fromPos = pos;
    drag.draggingPiece = board.getPiece(pos);
    drag.activeValidMoves = std::move(moves);
    drag.mousePosition = worldPos;
}

void GameController::updateDrag(sf::Vector2f worldPos,Position pixelToCoordinate) {

    if (!drag.isDragging)return;

    drag.mousePosition = worldPos;

    Position mouseGridPos = pixelToCoordinate;

}

moveType GameController::endDrag(Position toPos) {

    if (!drag.isDragging) return moveType::none;
    
    for (const auto& move : drag.activeValidMoves) {
        if (move == toPos) {

            moveType result = makeMove(drag.fromPos, toPos);
            resetDrag();
            return result;
            
        }
    }

    resetDrag();
    return moveType::none;
}

bool GameController::CheckMate(bool isWhiteTurn, const Board& board) {

    Position kingPos = board.findKing(isWhiteTurn);

    Piece king = board.getPiece(kingPos);

    bool isWhiteKing = isWhite(king);

    if (!moveValidator.isInCheck(isWhiteTurn, board))return false;

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {

            Position currentPos = { r,c };
            Piece currentPiece = board.getPiece(currentPos);

            if (isSameColor(currentPiece, king)) {

                auto moves = allValidMove.getRawMoves(currentPos, board);

                for (auto& toPos : moves) {

                    Board tempBoard = board;
                    tempBoard.movePiece(currentPos, toPos);

                    if (!moveValidator.isInCheck(isWhiteKing, tempBoard)) {
                        return false;

                    }

                }
            }
        }
    }
    isCheckMate = true;
    return true;
}


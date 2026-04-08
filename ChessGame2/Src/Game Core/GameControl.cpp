#include "GameControl.h"
#include"StockfishEngine.h"

GameControl::GameControl(Board& b, MoveService& m, GameState& g, MoveExecutor& e)
    : board(b), moveService(m), gameState(g), moveExecutor(e)
    , lastmoveType(moveType::none) {
}

GameControl::~GameControl() {
}

moveType GameControl::selectPiece(Position pos) {
    Piece piece = board.getPiece(pos);

    if (piece == Piece::Empty) return moveType::none;
    if (gameState.getColor(piece) != gameState.currentTurn) return moveType::none;

    gameState.selectedPos = pos;
    gameState.validMoves = moveService.getValidMoves(pos, board);

    return moveType::select;
}

moveType GameControl::tryMove(Position from, Position to) {

    if (!moveService.isValidMove(from, to, board)) {

        lastmoveType = moveType::unValid;

        return lastmoveType;
    }



    Move move = moveService.createMove(from, to, board);
    Piece target = board.getPiece(to);
    bool isCapture = target != Piece::Empty;

    moveExecutor.applyMove(move);

    std::string uciMove;
    uciMove += char('a' + from.col);
    uciMove += char('8' - from.row);
    uciMove += char('a' + to.col);
    uciMove += char('8' - to.row);
    moveHistory.push_back(uciMove);

    gameState.currentTurn = (gameState.currentTurn == color::white) ? color::black : color::white;

    
    updateGameState();
    gameState.clearSelection();

    
    if (gameState.isCheckMate) lastmoveType = moveType::checkMate;

    else if (gameState.isCheck) lastmoveType = moveType::check;

    else if (isCapture) lastmoveType = moveType::capture;

    else lastmoveType = moveType::move;

    return lastmoveType;
}

void GameControl::updateGameState() {

    gameState.isCheck = moveService.Check(board, gameState.currentTurn);

    if (gameState.isCheck) {
        gameState.checkPos = board.findKing(gameState.currentTurn == color::white);
    }

    gameState.isCheckMate = moveService.CheckMate(board, gameState.currentTurn);
}

void GameControl::handlePress(Position pos, sf::Vector2f mousePos) {
    auto& dragState = gameState.drag;

    if (!board.isInside(pos)) return;

    Piece p = board.getPiece(pos);

    dragState.isActive = true;
    dragState.fromPos = pos;
    dragState.mousePosition = mousePos;

    if (p != Piece::Empty && gameState.getColor(p) == gameState.currentTurn) {
        dragState.isDragging = true;
        dragState.draggingPiece = p;
        gameState.validMoves = moveService.getValidMoves(pos, board);
    }
    else {
        dragState.isDragging = false;
    }
}

void GameControl::handleMove(sf::Vector2f mousePos) {
    auto& dragState = gameState.drag;

    if (dragState.isActive) {
        dragState.mousePosition = mousePos;
    }
}

moveType GameControl::handleRelease(Position toPos) {

    if (isGameProcess.isAnimating || isGameProcess.isProcessing) {

        return moveType::none;
    }

    auto& dragState = gameState.drag;

    if (!dragState.isActive) return moveType::none;

    Position from = dragState.fromPos;

    dragState.isActive = false;

    Piece p = board.getPiece(from);

    
    if (from == toPos) {
        
        if (gameState.hasSelection() && from == gameState.selectedPos) {

            gameState.clearSelection();
            dragState.reset();
            lastmoveType = moveType::none;
            return lastmoveType;
        }

        if (gameState.hasSelection() && from != gameState.selectedPos) {

            Piece selectedPiece = board.getPiece(gameState.selectedPos);

            if (gameState.getColor(selectedPiece) == gameState.currentTurn) {

                lastmoveType = tryMove(gameState.selectedPos, toPos);

                if (lastmoveType != moveType::unValid) {

                    dragState.reset();
                    gameState.clearSelection();
                    return lastmoveType;
                }
            }
        }

        if (p != Piece::Empty && gameState.getColor(p) == gameState.currentTurn) {
            gameState.selectedPos = from;
            gameState.validMoves = moveService.getValidMoves(from, board);
            dragState.reset();
            lastmoveType = moveType::select;
            return lastmoveType;
        }

        dragState.reset();
        lastmoveType = moveType::none;
        return lastmoveType;
    }

    if (p != Piece::Empty && gameState.getColor(p) == gameState.currentTurn) {
        lastmoveType = tryMove(from, toPos);

        if (lastmoveType != moveType::unValid) {
            dragState.reset();
            gameState.clearSelection();
            return lastmoveType;
        }
        else {
            dragState.reset();
            return lastmoveType;
        }
    }

    dragState.reset();
    lastmoveType = moveType::none;
    return lastmoveType;
}

moveType GameControl::consumeMoveType() {

    moveType temp = lastmoveType;
    lastmoveType = moveType::none;
    return temp;
}

void GameControl::reset() {
    moveHistory.clear();
    lastmoveType = moveType::none;
    gameState.clearSelection();
    gameState.drag.reset();
}

moveType GameControl::executeAIMove(const Position& from, const Position& to) {

    Move move = moveService.createMove(from, to, board);

    Piece target = board.getPiece(to);
    bool isCapture = (target != Piece::Empty);

    moveExecutor.applyMove(move);

    std::string uciMove = StockfishGame::moveToUCI(from, to);

    moveHistory.push_back(uciMove);

    gameState.currentTurn = (gameState.currentTurn == color::white) ? color::black : color::white;

    updateGameState();

    gameState.clearSelection();

    if (gameState.isCheckMate) lastmoveType = moveType::checkMate;
    else if (gameState.isCheck) lastmoveType = moveType::check;
    else if (isCapture) lastmoveType = moveType::capture;
    else lastmoveType = moveType::move;

    return lastmoveType;
}

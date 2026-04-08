#pragma once

#include"SFML/Graphics.hpp"
#include "Board.h"
#include "MoveService.h"
#include "GameState.h"
#include "MoveExecutor.h"
#include"Define.h"
#include"StockfishPlay.h"
#include <vector>


class GameControl {
private:
    Board& board;

    MoveService& moveService;

    GameState& gameState;

    MoveExecutor& moveExecutor;

    StockfishGame stockfishGame;

    std::vector<std::string> moveHistory;

    moveType lastmoveType;

    isOnProcess isGameProcess;

    void updateGameState();

public:
    GameControl(Board& b, MoveService& m, GameState& g, MoveExecutor& e);

    ~GameControl();

    moveType selectPiece(Position pos);
    moveType tryMove(Position from, Position to);
    void handlePress(Position pos, sf::Vector2f mousePos);
    void handleMove(sf::Vector2f mousePos);
    moveType handleRelease(Position toPos);
    moveType executeAIMove(const Position& from, const Position& to);

    moveType consumeMoveType();

    moveType peakMoveType()const { return lastmoveType; }

  
    const std::vector<std::string>& getMoveHistory() const { return moveHistory; }

    void reset();

    GameState& getState()const { return gameState; }
};

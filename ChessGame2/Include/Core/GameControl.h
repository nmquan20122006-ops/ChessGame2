/**
* @file GameControl.h
* @brief This file defines the GameControl class, which serves as the central controller for managing the chess game logic, including handling player interactions, executing moves, updating game state, and interfacing with the AI engine.
* @brief the various components of the game, such as the board, game state, move executor, move service, and AI engine, to ensure a cohesive and responsive gaming experience.
* @author [Nguyen Minh Quan]
* @date [21/4/2026]
* @version [1.0]
* copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
*/
#pragma once
#include <functional>
#include <vector>
#include <memory>

#include"GameState.h"
#include"Utility.h"
#include"Bot.h"

class Board;
class MoveExecutor;
class MoveService;
class StockfishGame;
class DumpBot;

class GameControl {
public:

    using MoveEventCallback = std::function<void(const Move& move)>;
    using StateChangeCallback = std::function<void(GameStateEnum newState)>;
    using AnimRequest = std::function<void(Position, Position,Piece, std::function<void()>) >;
    
private:

    std::shared_ptr<Board>board;
    std::unique_ptr<MoveExecutor>moveExecutor;
    std::unique_ptr<MoveService>moveService;
    std::shared_ptr<GameState>gameState;
    std::unique_ptr<StockfishGame>stockfishGame;
    std::unique_ptr<DumpBot>dumpBot;

    AnimRequest animationProvider = nullptr;
    
    std::vector<MoveEventCallback> m_onMoveExecutedListeners;
    std::vector<StateChangeCallback> m_onGameStateChangedListeners;

    void deselect() { gameState->clearSelection(); }

    void updateGameState();
    void notifyMoveExecuted(const Move& move);
    void notifyStateChanged(const GameStateEnum& newState);

    bool m_isAnimating = false;
    bool m_isUndoing = false;

public:
    GameControl(std::shared_ptr<Board> b, std::shared_ptr<GameState> s,
        std::unique_ptr<MoveService>& ms, std::unique_ptr<MoveExecutor>& me);
    ~GameControl() = default;

    MoveExecutor* getMoveExecutor_ptr() { return moveExecutor.get(); }
    StockfishGame* getStockfishGame_ptr() { return stockfishGame.get(); }

    void initStockfishGame();

    bool requestMove(Position from, Position to);

    void handleSquareSelection(Position pos);
    void handlePress(Position, sf::Vector2f mousePos);
    void handleMove(sf::Vector2f mousePos);
    void handleRelease(Position pos);

    void subscribeToMove(MoveEventCallback callback) {
        m_onMoveExecutedListeners.push_back(callback);
    }
    void subscribeToStateChange(StateChangeCallback callback) {
        m_onGameStateChangedListeners.push_back(callback);
    }

    void setAnimationProvider(AnimRequest provider) { animationProvider = provider; }

    bool executeAiMove(std::string& uciMove);
    bool executePlayerMove(Position from, Position to);
    void updateAiMove();

    void syncAfterUndo(const UndoEntry& undoEntry);
    bool executeUndoMove();

    int halfMoveClockProcess(int prevClock, const Move& move);
    int fullMoveNumberProcess(int prevClock, const color currentTurn);

	void preparePromotion(Position fromPos, Position toPos);
    void executePromotionMove(Piece selectedPiece);

    void finalizeMove(const Move& move);
    bool requestAiMove(Position from,Position to,char promoition);

    void resetGame();

    GameState& getState() { return *gameState; }

    bool isBlocking()const;

};
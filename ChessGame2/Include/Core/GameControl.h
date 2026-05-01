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

#include"State/GameState.hpp"
#include"Utility.h"
#include"Bot.h"

class Board;
class MoveExecutor;
class MoveService;
class StockfishGame;

class GameControl {
public:

    using MoveEventCallback          = std::function<void(const Move& move)>;
    using StateChangeCallback        = std::function<void(GameStatus newState)>;
    using AnimRequest                = std::function<void(Position, Position, Piece, std::function<void()>)>;

private:

    std::shared_ptr<Board>              board;
    std::unique_ptr<MoveExecutor>       moveExecutor;
    std::shared_ptr<MoveService>        moveService;
    std::shared_ptr<GameState>          gameState;
    std::unique_ptr<StockfishGame>      stockfishGame;
    std::unique_ptr<DumpBot>            dumpBot;

    AnimRequest                         animationProvider = nullptr;

    std::vector<MoveEventCallback>      m_onMoveExecutedListeners;
    std::vector<StateChangeCallback>    m_onGameStateChangedListeners;

    void            updateGameState();
    void            notifyMoveExecuted(const Move& move);
    void            notifyStateChanged(const GameStatus& newState);

public:
    GameControl(std::shared_ptr<Board> b, std::shared_ptr<GameState> s,
        std::shared_ptr<MoveService>& ms, std::unique_ptr<MoveExecutor>& me);
    ~GameControl();
    //====================================================================
    //request
    //====================================================================
    bool             requestAiMove(Position from, Position to, char promoition);
    bool             requestMove(Position from, Position to);
    //====================================================================
    //execute
    //====================================================================
    bool             executeAiMove(std::string& uciMove);
    bool             executePlayerMove(Position from, Position to);
    bool             executeUndoMove();
    void             executePromotionMove(Piece selectedPiece);
    //====================================================================
    //subscribe listeners
    //====================================================================
    void             subscribeToMove(MoveEventCallback callback) { m_onMoveExecutedListeners.push_back(callback); }
    void             subscribeToStateChange(StateChangeCallback callback) { m_onGameStateChangedListeners.push_back(callback); }
    void             setAnimationProvider(AnimRequest provider) { animationProvider = provider; }

    //====================================================================
    //sync
    //====================================================================
    void             syncAfterUndo(const UndoEntry& undoEntry);
    void             finalizeMove(const Move& move);

    void             initStockfishGame();
    void             updateAiMove();
    void             preparePromotion(Position fromPos, Position toPos);
    void             resetGame();
    GameState&       getState()                                            { return *gameState; }
    bool             isBlocking()const;

};
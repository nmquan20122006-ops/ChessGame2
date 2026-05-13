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
#include"MoveLog.h"

class Board;
class MoveExecutor;
class MoveService;
class IChessEngine;
class ChessNotation;
class PromotionController;
class ChessEngineController;
struct EngineConfig;

class GameControl {
public:
    using MoveEventCallback                 = std::function<void(const Move& move)>;
    using StateChangeCallback               = std::function<void(GameStatus newState)>;
    using AnimRequest                       = std::function<void(Position, Position, Piece, std::function<void()>)>;
private:
    std::shared_ptr<Board>                  m_board;
    std::unique_ptr<MoveExecutor>           m_moveExecutor;
    std::shared_ptr<MoveService>            m_moveService;
    std::shared_ptr<GameState>              m_gameState;
    std::unique_ptr<DumpBot>                m_dumpBot;
    std::unique_ptr<PromotionController>    m_promotionController;
    std::unique_ptr<ChessEngineController>  m_chessEngineController;
    MoveLog&                                m_moveLog;
    AnimRequest                             m_animationProvider = nullptr;
    std::vector<MoveEventCallback>          m_onMoveExecutedListeners;
    std::vector<StateChangeCallback>        m_onGameStateChangedListeners;

    void                                    updateGameState();
    void                                    notifyMoveExecuted(const Move& move);
    void                                    notifyStateChanged(const GameStatus& newState);
public:
    GameControl(std::shared_ptr<Board> b,
        std::shared_ptr<GameState> s,
        std::shared_ptr<MoveService>& ms,
        std::unique_ptr<MoveExecutor>& me,
        MoveLog& l);
                                            ~GameControl();
    bool                                    requestMove(Position from, Position to);
    bool                                    executePlayerMove(Position from, Position to);
    bool                                    executeUndoMove();
    void                                    executePromotionMove(Piece selectedPiece);
    void                                    subscribeToMove(MoveEventCallback callback) { m_onMoveExecutedListeners.push_back(callback); }
    void                                    subscribeToStateChange(StateChangeCallback callback) { m_onGameStateChangedListeners.push_back(callback); }
    void                                    setAnimationProvider(AnimRequest provider) { m_animationProvider = provider; }
    void                                    publishMoveEvent(Move& move);
    void                                    syncAfterUndo(UndoEntry undoEntry);
    void                                    finalizeMove(Move& move);
    void                                    initChessEngine(EngineConfig& engineConfig);
    void                                    updateChessEngineMove();
    void                                    executeEngineMove();
    void                                    executeHint();
    void                                    executeEnemyBestMove();
    void                                    resetGame();
    bool                                    isBlocking() const ;
    GameState&                              getState() { return *m_gameState; }
    MoveExecutor&                           getMoveExecutor() { return *m_moveExecutor; }
    PromotionController&                    getPromotionController() { return *m_promotionController; }
};